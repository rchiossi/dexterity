#!/usr/bin/python

from ctypes import cdll
from ctypes import Structure
from ctypes import POINTER, pointer

from ctypes import c_int, c_uint, c_uint8, c_uint16, c_uint32
from ctypes import c_char_p

from ctypes import create_string_buffer

#Bytestream
class _ByteStream(Structure):
    _fields_ = [
        ('filename',c_char_p),
        ('size',c_uint32),
        ('data',POINTER(c_uint8)),
        ('offset',c_uint32),
        ('exhausted',c_int),
        ]

class ByteStream(object):
    def __init__(self,fname=None,size=None):
        if fname != None:
            self._bs = dxlib.bsmap(fname)
        elif size != None:
            self._bs = dxlib.bsalloc(size)
        else:
            raise(Exception("Not enough parameters for ByteStream"))

    def __del__(self):
        dxlib.bsfree(self._bs)

    def read(self,size):
        buf = create_string_buffer('\x00' * size)
        ret = dxlib.bsread(self._bs,buf,size)
        return buf.value

    def read_offset(self,size,offset):
        buf = create_string_buffer('\x00' * size)
        ret = dxlib.bsread_offset(self._bs,buf,size,offset)
        return buf.value

    def seek(self,offset):
        dxlib.bsseek(self._bs,offset)

    def reset(self):
        dxlib.bsreset(self._bs)

    def save(self,filename):
        dxlib.bssave(self._bs,filename)

    def exhausted(self):
        return (self._bs.contents.exhausted != 0)

#LEB128
class Leb128(Structure):
    _fields_ = [
        ('data',c_uint8 * 5),
        ('size',c_uint),
        ]

class ULeb128(Leb128):
    def __init__(self,val=None):
        if val != None:
            dxlib.uitoul128(pointer(self),val)

    def __int__(self):
        return dxlib.ul128toui(self)

class SLeb128(Leb128):
    def __init__(self,val=None):
        if val != None:
            dxlib.itosl128(pointer(self),val)

    def __int__(self):
        return dxlib.sl128toi(self)

class ULeb128p1(Leb128):
    def __init__(self,val=None):
        if val != None:
            dxlib.itoul128p1(pointer(self),val)

    def __int__(self):
        return dxlib.ul128p1toi(self)

#Dex
class Metadata(Structure):
    _fields_ = [
        ('corrupted',c_uint),
        ('offset', c_uint32),
        ]

class DexHeaderItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('magic', c_uint8 * 8),
        ('checksum', c_uint32),
        ('signature', c_uint8 * 20),
        ('file_size', c_uint32),
        ('header_size', c_uint32),
        ('endian_tag', c_uint32),
        ('link_size', c_uint32),
        ('link_off', c_uint32),
        ('map_off', c_uint32),
        ('string_ids_size', c_uint32),
        ('string_ids_off', c_uint32),
        ('type_ids_size', c_uint32),
        ('type_ids_off', c_uint32),
        ('proto_ids_size', c_uint32),
        ('proto_ids_off', c_uint32),
        ('field_ids_size', c_uint32),
        ('field_ids_off', c_uint32),
        ('method_ids_size', c_uint32),
        ('method_ids_off', c_uint32),
        ('class_defs_size', c_uint32),
        ('class_defs_off', c_uint32),
        ('data_size', c_uint32),
        ('data_off', c_uint32),
        ]

class DexStringIdItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('string_data_off', c_uint32),
        ]

class DexStringDataItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', ULeb128),
        ('data', POINTER(c_uint8)),
        ]

class DexTypeIdItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('descriptor_idx', c_uint32),
        ]

class DexProtoIdItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('shorty_idx', c_uint32),
        ('return_type_idx', c_uint32),
        ('parameters_off', c_uint32),
        ]

class DexFieldIdItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('class_idx', c_uint16),
        ('type_idx', c_uint16),
        ('name_idx', c_uint32),
        ]
    
class DexMethodIdItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('class_idx', c_uint16),
        ('proto_idx', c_uint16),
        ('name_idx', c_uint32),
        ]
    
class DexClassDefItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('class_idx', c_uint32),
        ('access_flags', c_uint32),
        ('superclass_idx', c_uint32),
        ('interfaces_off', c_uint32),
        ('source_file_idx', c_uint32),
        ('annotations_off', c_uint32),
        ('class_data_off', c_uint32),
        ('static_values_off', c_uint32),
        ]

class DexEncodedFieldItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('field_idx_diff',ULeb128),
        ('access_flags',ULeb128),
        ]

class DexEncodedMethodItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('method_idx_diff',ULeb128),
        ('access_flags',ULeb128),
        ('code_off',ULeb128),
        ]

class DexClassDataItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('static_fields_size',ULeb128),
        ('instance_fields_size',ULeb128),
        ('direct_methods_size',ULeb128),
        ('virtual_methods_size',ULeb128),
        ('static_fields',POINTER(POINTER(DexEncodedFieldItem))),
        ('instance_fields',POINTER(POINTER(DexEncodedFieldItem))),
        ('direct_methods',POINTER(POINTER(DexEncodedMethodItem))),
        ('virtual_methods',POINTER(POINTER(DexEncodedMethodItem))),
        ]

class DexTypeItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('type_idx', c_uint16),
        ]

class DexTypeList(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', c_uint32),
        ('list', POINTER(POINTER(DexTypeItem))),
        ]

class DexTryItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('start_addr', c_uint32),
        ('insns_count', c_uint16),
        ('handler_off', c_uint16),
        ]

class DexEncodedTypeAddrPair(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('type_idx', ULeb128),
        ('addr', ULeb128),
        ]

class DexEncodedCatchHandler(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', SLeb128),
        ('handlers', POINTER(POINTER(DexEncodedTypeAddrPair))),
        ('catch_all_addr', ULeb128)
        ]

class DexEncodedCatchHandlerList(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size',ULeb128),
        ('list',POINTER(POINTER(DexEncodedCatchHandler))),
        ]

class DexCodeItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('registers_size',c_uint16),
        ('ins_size',c_uint16),
        ('outs_size',c_uint16),
        ('tries_size',c_uint16),
        ('debug_info_off',c_uint32),
        ('insns_size',c_uint32),
        ('insns',POINTER(c_uint16)),
        ('padding',c_uint16),
        ('tries',POINTER(POINTER(DexTryItem))),
        ('handlers',POINTER(DexEncodedCatchHandlerList)),
        ]

class DexDebugInfo(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('line_start', ULeb128),
        ('parameters_size', ULeb128),
        ('parameter_names', POINTER(ULeb128p1)), 
        ('state_machine', POINTER(c_uint8)), 
        ]

class DexMapItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('type',c_uint16),
        ('unused',c_uint16),
        ('size',c_uint32),
        ('offset',c_uint32),
        ]

class DexMapList(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', c_uint32),
        ('list', POINTER(POINTER(DexMapItem))),
        ]

class DexEncodedValue(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('argtype', c_uint8),
        ('value', POINTER(c_uint8)),
        ]

class DexEncodedArray(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', ULeb128),
        ('values', POINTER(POINTER(DexEncodedValue))),
        ]

class DexAnnotationElement(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('name_idx', ULeb128),
        ('value', POINTER(DexEncodedValue)),
        ]

class DexEncodedAnnotation(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('type_idx', ULeb128),
        ('size', ULeb128),
        ('elements', POINTER(POINTER(DexAnnotationElement))),
        ]

class DexFieldAnnotation(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('field_idx', c_uint32),
        ('annotations_off', c_uint32),
        ]

class DexMethodAnnotation(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('method_idx', c_uint32),
        ('annotations_off', c_uint32),
        ]

class DexParameterAnnotation(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('method_idx', c_uint32),
        ('annotations_off', c_uint32),
        ]

class DexAnnotationDirectoryItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('class_annotations_off', c_uint32),
        ('fields_size', c_uint32),
        ('annotated_methods_size', c_uint32),
        ('annotated_parameters_size', c_uint32),
        ('field_annotations', POINTER(POINTER(DexFieldAnnotation))),
        ('method_annotations', POINTER(POINTER(DexMethodAnnotation))),
        ('parameter_annotations', POINTER(POINTER(DexParameterAnnotation))),
        ]

class DexAnnotationSetRefItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('annotations_off', c_uint32),
        ]

class DexAnnotationSetRefList(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', c_uint32),
        ('list', POINTER(POINTER(DexAnnotationSetRefItem))),
        ]

class DexAnnotationOffItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('annotation_off', c_uint32),
        ]

class DexAnnotationSetItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', c_uint32),
        ('entries', POINTER(POINTER(DexAnnotationOffItem))),
        ]

class DexAnnotationItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('visibility', c_uint8),
        ('annotation', POINTER(DexEncodedAnnotation)),
        ]

class DexEncodedArrayItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('value', POINTER(DexEncodedArray)),
        ]

#Load Library
dxlib = cdll.LoadLibrary("lib/libdexterity.so")
# ByteStream prototypes
dxlib.bsalloc.argtypes = (c_uint,)
dxlib.bsalloc.restype = POINTER(_ByteStream)

dxlib.bsmap.argtypes = (c_char_p,)
dxlib.bsmap.restype = POINTER(_ByteStream)

dxlib.bsfree.argtypes = (POINTER(_ByteStream),)
dxlib.bsfree.restype = c_int

dxlib.bsread.argtypes = (POINTER(_ByteStream),c_char_p)
dxlib.bsread.restype = c_uint

dxlib.bsread_offset.argtypes = (POINTER(_ByteStream),c_char_p,c_uint)
dxlib.bsread_offset.restype = c_uint

dxlib.bsseek.argtypes = (POINTER(_ByteStream),c_uint)
dxlib.bsseek.restype = None

dxlib.bsreset.argtypes = (POINTER(_ByteStream),)
dxlib.bsreset.restype = None

dxlib.bssave.argtypes = (POINTER(_ByteStream),c_char_p)
dxlib.bssave.restype = None

# Leb128 Prototypes
dxlib.ul128toui.argtypes = (Leb128,)
dxlib.ul128toui.restype = c_uint

dxlib.ul128p1toi.argtypes = (Leb128,)
dxlib.ul128p1toi.restype = c_int

dxlib.sl128toi.argtypes = (Leb128,)
dxlib.sl128toi.restype = c_int

#Dex prototypes
def DXPARSE(name,res):
    global dxlib
    getattr(dxlib,name).argtypes = (POINTER(_ByteStream),c_uint32)
    getattr(dxlib,name).restype  = POINTER(res) 

DXPARSE('dx_header',DexHeaderItem)
DXPARSE('dx_stringid',DexStringIdItem)
DXPARSE('dx_typeid',DexTypeIdItem)
DXPARSE('dx_protoid',DexProtoIdItem)
DXPARSE('dx_fieldid',DexFieldIdItem)
DXPARSE('dx_methodid',DexMethodIdItem)
DXPARSE('dx_classdef',DexClassDefItem)
DXPARSE('dx_stringdata',DexStringDataItem)
DXPARSE('dx_encodedfield',DexEncodedFieldItem)
DXPARSE('dx_encodedmethod',DexEncodedMethodItem)
DXPARSE('dx_classdata',DexClassDataItem)
DXPARSE('dx_typelist',DexTypeList)
DXPARSE('dx_tryitem',DexTryItem)
DXPARSE('dx_encodedtypeaddrpair',DexEncodedTypeAddrPair)
DXPARSE('dx_encodedcatchhandler',DexEncodedCatchHandler)
DXPARSE('dx_encodedcatchhandlerlist',DexEncodedCatchHandlerList)
DXPARSE('dx_codeitem',DexCodeItem)
DXPARSE('dx_debuginfo',DexDebugInfo)
DXPARSE('dx_mapitem',DexMapItem)
DXPARSE('dx_maplist',DexMapList)

DXPARSE('dx_encodedvalue',DexEncodedValue);
DXPARSE('dx_encodedarray',DexEncodedArray);
DXPARSE('dx_annotationelement',DexAnnotationElement);
DXPARSE('dx_encodedannotation',DexEncodedAnnotation);

dxlib.dx_debug_state_machine.argtypes = (POINTER(_ByteStream),c_uint32)
dxlib.dx_debug_state_machine.restype = POINTER(c_uint8)

DXPARSE('dx_fieldannotation',DexFieldAnnotation);
DXPARSE('dx_methodannotation',DexMethodAnnotation);
DXPARSE('dx_parameterannotation',DexParameterAnnotation);
DXPARSE('dx_annotationdirectoryitem',DexAnnotationDirectoryItem);
DXPARSE('dx_annotationsetrefitem',DexAnnotationSetRefItem);
DXPARSE('dx_annotationsetreflist',DexAnnotationSetRefList);
DXPARSE('dx_annotationoffitem',DexAnnotationOffItem);
DXPARSE('dx_annotationsetitem',DexAnnotationSetItem);
DXPARSE('dx_annotationitem',DexAnnotationItem);
DXPARSE('dx_encodedarrayitem',DexEncodedArrayItem);

def DXBUILD(name,obj):
    global dxlib
    getattr(dxlib,name).argtypes = (POINTER(_ByteStream),POINTER(obj))
    getattr(dxlib,name).restype  = None 

DXBUILD('dxb_header',DexHeaderItem);
DXBUILD('dxb_stringid',DexStringIdItem);
DXBUILD('dxb_typeid',DexTypeIdItem);
DXBUILD('dxb_protoid',DexProtoIdItem);
DXBUILD('dxb_fieldid',DexFieldIdItem);
DXBUILD('dxb_methodid',DexMethodIdItem);
DXBUILD('dxb_classdef',DexClassDefItem);
DXBUILD('dxb_stringdata',DexStringDataItem);
DXBUILD('dxb_encodedfield',DexEncodedFieldItem);
DXBUILD('dxb_encodedmethod',DexEncodedMethodItem);
DXBUILD('dxb_classdata',DexClassDataItem);
DXBUILD('dxb_typeitem',DexTypeItem);
DXBUILD('dxb_typelist',DexTypeList);
DXBUILD('dxb_tryitem',DexTryItem);
DXBUILD('dxb_encodedtypeaddrpair',DexEncodedTypeAddrPair);
DXBUILD('dxb_encodedcatchhandler',DexEncodedCatchHandler);
DXBUILD('dxb_encodedcatchhandlerlist',DexEncodedCatchHandlerList);
DXBUILD('dxb_codeitem',DexCodeItem);
DXBUILD('dxb_debuginfo',DexDebugInfo);
DXBUILD('dxb_mapitem',DexMapItem);
DXBUILD('dxb_maplist',DexMapList);

DXBUILD('dxb_encodedvalue',DexEncodedValue);
DXBUILD('dxb_encodedarray',DexEncodedArray);
DXBUILD('dxb_annotationelement',DexAnnotationElement);
DXBUILD('dxb_encodedannotation',DexEncodedAnnotation);

dxlib.dxb_debug_state_machine.argtypes = (POINTER(_ByteStream),POINTER(c_uint8))
dxlib.dxb_debug_state_machine.restype = None

DXBUILD('dxb_fieldannotation',DexFieldAnnotation);
DXBUILD('dxb_methodannotation',DexMethodAnnotation);
DXBUILD('dxb_parameterannotation',DexParameterAnnotation);
DXBUILD('dxb_annotationdirectoryitem',DexAnnotationDirectoryItem);
DXBUILD('dxb_annotationsetrefitem',DexAnnotationSetRefItem);
DXBUILD('dxb_annotationsetreflist',DexAnnotationSetRefList);
DXBUILD('dxb_annotationoffitem',DexAnnotationOffItem);
DXBUILD('dxb_annotationsetitem',DexAnnotationSetItem);
DXBUILD('dxb_annotationitem',DexAnnotationItem);
DXBUILD('dxb_encodedarrayitem',DexEncodedArrayItem);

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

#DexObj
class Dex(object):
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
