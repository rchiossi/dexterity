from dx.dex import ByteStream
from dx.dex import dxlib

#DexParser
class DexParser(object):
    def __init__(self,filename):
        if filename == None: raise(Exception("Null File Name."))        
        self.bs = ByteStream(filename)
    
    def seek(self,offset):
        self.bs.seek(offset)

    def item(self,item,offset=None):
        if offset != None: self.seek(offset)

        f_parse = getattr(dxlib,'dx_' + item)
        if f_parse == None:
            raise(Exception("Invalid parse item: %s" % item))

        obj = f_parse(self.bs._bs,self.bs._bs.contents.offset)

        return obj.contents

    def list(self,item,amount,offset=None):
        if offset != None: self.seek(offset)

        return [self.item(item) for i in xrange(amount)]

    def table(self,item,dlist,doff):
        res = []
        
        for ditem in dlist:
            if getattr(ditem,doff) != 0:
                res.append(self.item(item,getattr(ditem,doff)))

        return res

    def raw(self,offset=None,size=0):
        if offset != None: self.seek(offset)

        return self.bs.read(size)

    def debug_state_machine(self,offset=None):
        if offset != None: self.seek(offset)

        return dxlib.dx_debug_state_machine(self.bs._bs,self.bs._bs.contents.offset)

#Dex Obj
class DexPy(object):
    def __init__(self,filename):
        self.dxp = DexParser(filename)

        self.size = self.dxp.bs._bs.contents.size

    def parse(self):        
        #header
        self.header = self.dxp.item('header')
        
        #data from header
        self.link_data = self.dxp.raw(self.header.link_off,self.header.link_size)
        self.map_list = self.dxp.item('maplist',self.header.map_off)

        self.string_ids = self.dxp.list('stringid' ,self.header.string_ids_size,
                                   self.header.string_ids_off)
        self.type_ids   = self.dxp.list('typeid'   ,self.header.type_ids_size,
                                   self.header.type_ids_off)
        self.proto_ids  = self.dxp.list('protoid'  ,self.header.proto_ids_size,
                                   self.header.proto_ids_off)
        self.field_ids  = self.dxp.list('fieldid'  ,self.header.field_ids_size,
                                   self.header.field_ids_off)
        self.method_ids = self.dxp.list('methodid' ,self.header.method_ids_size,
                                   self.header.method_ids_off)
        self.class_defs = self.dxp.list('classdef' ,self.header.class_defs_size,
                                   self.header.class_defs_off)
      
        #data from string id
        self.string_data_list = self.dxp.table('stringdata',self.string_ids,
                                               'string_data_off')
        
        #data from proto id
        self.type_lists = self.dxp.table('typelist',self.proto_ids,'parameters_off')
        
        #data from class def
        self.type_lists += self.dxp.table('typelist',self.class_defs,'interfaces_off')
        self.class_annotations = self.dxp.table('annotationdirectoryitem',
                                      self.class_defs,'annotations_off')
        self.class_data_list = self.dxp.table('classdata',self.class_defs,
                                              'class_data_off')
        self.class_statics = self.dxp.table('encodedarray',self.class_defs,
                                            'static_values_off')
        
        #data from class data    
        self.code_list = []  
        
        for class_data in self.class_data_list:
            if class_data.meta.corrupted == True: continue
            
            for i in xrange(int(class_data.direct_methods_size)):
                method = class_data.direct_methods[i].contents
                if int(method.code_off) != 0:
                    self.code_list.append(self.dxp.item('codeitem',
                                                        int(method.code_off)))
        
            for i in xrange(int(class_data.virtual_methods_size)):
                method = class_data.virtual_methods[i].contents
                if int(method.code_off) != 0:
                    self.code_list.append(self.dxp.item('codeitem',
                                                        int(method.code_off)))

        #data from code item
        self.debug_info_list = self.dxp.table('debuginfo',self.code_list,
                                              'debug_info_off')

        #data from class annotations    
        self.annotation_sets = self.dxp.table(
            'annotationsetitem', self.class_annotations,'class_annotations_off')

        self.annotation_set_ref_lists = []

        for item in self.class_annotations:
            if item.meta.corrupted == True: continue

            for i in xrange(item.fields_size):            
                off = item.field_annotations[i].contents.annotations_off
                self.annotation_sets.append(self.dxp.item('annotationsetitem',off))

            for i in xrange(item.annotated_methods_size):            
                off = item.method_annotations[i].contents.annotations_off
                self.annotation_sets.append(self.dxp.item('annotationsetitem',off))

            for i in xrange(item.annotated_parameters_size):            
                off = item.parameter_annotations[i].contents.annotations_off
                self.annotation_set_ref_lists.append(
                    self.dxp.item('annotationsetreflist', off))
            
        #data from annotation set ref lists
        for item in self.annotation_set_ref_lists:
            if item.meta.corrupted == True: continue

            for i in xrange(item.size):            
                off = item.list[i].contents.annotations_off
                self.annotation_sets.append(self.dxp.item('annotationsetitem',off)) 

        #data from annotation set item
        self.annotations = []
            
        for item in self.annotation_sets:
            if item.meta.corrupted == True: continue

            for i in xrange(item.size):
                off = item.entries[i].contents.annotation_off
                self.annotations.append(self.dxp.item('annotationitem',off))

    def save(self,filename):
        bs = ByteStream(size=self.size)

        self.header.file_size = self.size

        dxlib.dxb_header(bs._bs,self.header)
        dxlib.dxb_maplist(bs._bs,self.map_list)

        for item in self.string_ids: dxlib.dxb_stringid(bs._bs,item)
        for item in self.type_ids: dxlib.dxb_typeid(bs._bs,item)
        for item in self.proto_ids: dxlib.dxb_protoid(bs._bs,item)
        for item in self.field_ids: dxlib.dxb_fieldid(bs._bs,item)
        for item in self.method_ids: dxlib.dxb_methodid(bs._bs,item)
        for item in self.class_defs: dxlib.dxb_classdef(bs._bs,item)

        for item in self.string_data_list: dxlib.dxb_stringdata(bs._bs,item)
        for item in self.type_lists: dxlib.dxb_typelist(bs._bs,item)
        for item in self.class_annotations: dxlib.dxb_annotationdirectoryitem(
            bs._bs, item)
        for item in self.class_data_list: dxlib.dxb_classdata(bs._bs,item)
        for item in self.class_statics: dxlib.dxb_encodedarray(bs._bs,item)
        for item in self.code_list: dxlib.dxb_codeitem(bs._bs,item)
        for item in self.debug_info_list: dxlib.dxb_debuginfo(bs._bs,item)

        for item in self.annotation_sets: dxlib.dxb_annotationsetitem(bs._bs,item)
        for item in self.annotation_set_ref_lists: dxlib.dxb_annotationsetreflist(
            bs._bs,item)
        for item in self.annotations: dxlib.dxb_annotationitem(bs._bs,item)
    
        bs.save(filename)

    def shift_offsets(self,base,delta):
        dxlib.dxo_header(self.header,base,delta)
        dxlib.dxo_maplist(self.map_list,base,delta)

        for item in self.string_ids: dxlib.dxo_stringid(item,base,delta)
        for item in self.type_ids: dxlib.dxo_typeid(item,base,delta)
        for item in self.proto_ids: dxlib.dxo_protoid(item,base,delta)
        for item in self.field_ids: dxlib.dxo_fieldid(item,base,delta)
        for item in self.method_ids: dxlib.dxo_methodid(item,base,delta)
        for item in self.class_defs: dxlib.dxo_classdef(item,base,delta)

        for item in self.string_data_list: dxlib.dxo_stringdata(item,base,delta)
        for item in self.type_lists: dxlib.dxo_typelist(item,base,delta)
        for item in self.class_annotations: dxlib.dxo_annotationdirectoryitem(
            item,base,delta)
        for item in self.class_data_list: dxlib.dxo_classdata(item,base,delta)
        for item in self.class_statics: dxlib.dxo_encodedarray(item,base,delta)
        for item in self.code_list: dxlib.dxo_codeitem(item,base,delta)
        for item in self.debug_info_list: dxlib.dxo_debuginfo(item,base,delta)

        for item in self.annotation_sets: dxlib.dxo_annotationsetitem(item,base,delta)
        for item in self.annotation_set_ref_lists: dxlib.dxo_annotationsetreflist(
            item,base,delta)
        for item in self.annotations: dxlib.dxo_annotationitem(item,base,delta)
        
    def shift_stringids(self,base,delta):
        for item in self.type_ids: dxlib.dxsi_typeid(item,base,delta)
        for item in self.proto_ids: dxlib.dxsi_protoid(item,base,delta)
        for item in self.field_ids: dxlib.dxsi_fieldid(item,base,delta)
        for item in self.method_ids: dxlib.dxsi_methodid(item,base,delta)
        for item in self.class_defs: dxlib.dxsi_classdef(item,base,delta)

        #for item in self.class_statics: dxlib.dxsi_encodedarray(item,base,delta)
        for item in self.debug_info_list: dxlib.dxsi_debuginfo(item,base,delta)

        #for item in self.annotations: dxlib.dxsi_annotationitem(item,base,delta)
