from dxlib import DexEncodedArray
from dxlib import DexEncodedAnnotation
from ctypes import cast
from ctypes import POINTER, c_char_p, c_uint32

class DexPrinter (object):
    def __init__(self,meta_verbose=False):
        self.meta_verbose = meta_verbose

    def max_attr(self,obj):
        return max([len(i[0]) for i in obj._fields_])
            
    def print_attr(self,attr,val,pad,size):
        msg = ' '*pad
        msg += attr + ':'
        msg += ' '* (size-len(msg)+pad+4)
        msg += str(val)

        print msg
        
    def print_label(self,msg,pad):
        print ' '*pad + msg + ':'
        
    def meta(self,obj,pad=0):
        if not self.meta_verbose: return

        self.print_label('Metadata',pad)
        
        size = self.max_attr(obj)
        pad += 2

        self.print_attr('corrupted',obj.corrupted,pad,size)
        self.print_attr('offset',hex(obj.offset),pad,size)

    def header(self,obj,pad=0):
        self.print_label("Header",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('magic',''.join(['%02x'%x for x in obj.magic]),pad,size)
        self.print_attr('checksum',hex(obj.checksum),pad,size)
        self.print_attr('signature',''.join(['%02x'%x for x in obj.signature])
                        ,pad,size)
        self.print_attr('file_size',obj.file_size,pad,size)
        self.print_attr('header_size',obj.header_size,pad,size)
        self.print_attr('endian_tag',obj.endian_tag,pad,size)
        self.print_attr('link_size',obj.link_size,pad,size)
        self.print_attr('link_off',hex(obj.link_off),pad,size)
        self.print_attr('map_off',hex(obj.map_off),pad,size)
        self.print_attr('string_ids_size',obj.string_ids_size,pad,size)
        self.print_attr('string_ids_off',hex(obj.string_ids_off),pad,size)
        self.print_attr('type_ids_size',obj.type_ids_size,pad,size)
        self.print_attr('type_ids_off',hex(obj.type_ids_off),pad,size)
        self.print_attr('proto_ids_size',obj.proto_ids_size,pad,size)
        self.print_attr('proto_ids_off',hex(obj.proto_ids_off),pad,size)
        self.print_attr('field_ids_size',obj.field_ids_size,pad,size)
        self.print_attr('field_ids_off',hex(obj.field_ids_off),pad,size)
        self.print_attr('method_ids_size',obj.method_ids_size,pad,size)
        self.print_attr('method_ids_off',hex(obj.method_ids_off),pad,size)
        self.print_attr('class_defs_size',obj.class_defs_size,pad,size)
        self.print_attr('class_defs_off',hex(obj.class_defs_off),pad,size)
        self.print_attr('data_size',obj.data_size,pad,size)
        self.print_attr('data_off',hex(obj.data_off),pad,size)

    def stringid(self,obj,pad=0):
        self.print_label("StringId",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('string_data_off',hex(obj.string_data_off),pad,size)

    def stringdata(self,obj,pad=0):
        self.print_label("StringData",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('size',int(obj.size),pad,size)
        self.print_attr('data',str(cast(obj.data,c_char_p).value)[:int(obj.size)],
                        pad,size)

    def typeid(self,obj,pad=0):
        self.print_label("TypeId",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('descriptor_idx',obj.descriptor_idx,pad,size)
    

    def protoid(self,obj,pad=0):
        self.print_label("ProtoId",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('shorty_idx',obj.shorty_idx,pad,size)
        self.print_attr('return_type_idx',obj.return_type_idx,pad,size)
        self.print_attr('parameters_off',obj.parameters_off,pad,size)


    def fieldid(self,obj,pad=0):
        self.print_label("FieldId",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('class_idx',obj.class_idx,pad,size)
        self.print_attr('type_idx',obj.type_idx,pad,size)
        self.print_attr('name_idx',obj.name_idx,pad,size)

    def methodid(self,obj,pad=0):
        self.print_label("MethodId",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('class_idx',obj.class_idx,pad,size)
        self.print_attr('proto_idx',obj.proto_idx,pad,size)
        self.print_attr('name_idx',obj.name_idx,pad,size)

    def classdef(self,obj,pad=0):
        self.print_label("ClassDef",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('class_idx',obj.class_idx,pad,size)
        self.print_attr('access_flags',obj.access_flags,pad,size)
        self.print_attr('superclass_idx',obj.superclass_idx,pad,size)
        self.print_attr('interfaces_off',hex(obj.interfaces_off),pad,size)
        self.print_attr('source_file_idx',obj.source_file_idx,pad,size)
        self.print_attr('annotations_off',hex(obj.annotations_off),pad,size)
        self.print_attr('class_data_off',hex(obj.class_data_off),pad,size)
        self.print_attr('static_values_off',hex(obj.static_values_off),pad,size)

    def encodedfield(self,obj,pad=0):
        self.print_label("EncodedField",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('field_idx_diff',int(obj.field_idx_diff),pad,size)
        self.print_attr('access_flags',int(obj.access_flags),pad,size)

    def encodedmethod(self,obj,pad=0):
        self.print_label("EncodedMethod",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('method_idx_diff',int(obj.method_idx_diff),pad,size)
        self.print_attr('access_flags',int(obj.access_flags),pad,size)
        self.print_attr('code_off',hex(int(obj.code_off)),pad,size)

    def classdata(self,obj,pad=0):
        self.print_label("ClassData",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('static_fields_size',int(obj.static_fields_size),pad,size)
        self.print_attr('instance_fields_size',int(obj.instance_fields_size),
                        pad,size)
        self.print_attr('direct_methods_size',int(obj.direct_methods_size),pad,size)
        self.print_attr('virtual_methods_size',int(obj.virtual_methods_size),
                        pad,size)

        self.print_label("static_fields",pad)
        for i in xrange(int(obj.static_fields_size)):
            self.encodedfield(obj.static_fields[i].contents,pad+2)

        self.print_label("instance_fields",pad)
        for i in xrange(int(obj.instance_fields_size)):
            self.encodedfield(obj.instance_fields[i].contents,pad+2)

        self.print_label("direct_methods",pad)
        for i in xrange(int(obj.direct_methods_size)):
            self.encodedmethod(obj.direct_methods[i].contents,pad+2)

        self.print_label("virtual_methods",pad)
        for i in xrange(int(obj.virtual_methods_size)):
            self.encodedmethod(obj.virtual_methods[i].contents,pad+2)

    def typeitem(self,obj,pad=0):
        self.print_label("TypeItem",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('type_idx',obj.type_idx,pad,size)

    def typelist(self,obj,pad=0):
        self.print_label("TypeList",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('size',obj.size,pad,size)

        for i in xrange(obj.size):
            self.typeitem(obj.list[i].contents,pad)

    def tryitem(self,obj,pad=0):
        self.print_label("TryItem",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('start_addr',hex(obj.start_addr),pad,size)
        self.print_attr('insns_count',obj.insns_count,pad,size)
        self.print_attr('handler_off',obj.handler_off,pad,size)

    def encodedtypeaddrpair(self,obj,pad=0):
        self.print_label("EncodedTypeAddrPair",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('type_idx',int(obj.type_idx),pad,size)
        self.print_attr('addr',hex(int(obj.addr)),pad,size)

    def encodedcatchhandler(self,obj,pad=0):
        self.print_label("EncodedCatchHandler",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('size',int(obj.size),pad,size)

        self.print_label('handlers',pad)
        for i in xrange(abs(int(obj.size))):
            self.encodedtypeaddrpair(obj.handlers[i].contents,pad)

        if int(obj.size) <= 0:
            self.print_attr('catch_all_addr',hex(int(obj.catch_all_addr)),pad,size)

    def encodedcatchhandlerlist(self,obj,pad=0):
        self.print_label("EncodedCatchHandlerList",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('size',int(obj.size),pad,size)

        self.print_label('list',pad)
        for i in xrange(int(obj.size)):
            self.encodedcatchhandler(obj.list[i].contents,pad+2)

    def codeitem(self,obj,pad=0):
        self.print_label("CodeItem",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('registers_size',obj.registers_size,pad,size)
        self.print_attr('ins_size',obj.ins_size,pad,size)
        self.print_attr('outs_size',obj.outs_size,pad,size)
        self.print_attr('tries_size',obj.tries_size,pad,size)
        self.print_attr('debug_info_off',hex(obj.debug_info_off),pad,size)
        self.print_attr('insns_size',obj.insns_size,pad,size)

        self.print_label('insns',pad)
        line = ' '*(pad+2)
        for i in xrange(obj.insns_size):            
            line += '%04x' % obj.insns[i]

            if (len(line) % 60) == 0:
                print line
                line =' '*(pad+2)

        print line

        if obj.tries_size > 0 and obj.tries_size % 2 != 0:
            self.print_attr('padding',hex(obj.padding),pad,size)
        
        self.print_label('tries',pad)
        for i in xrange(obj.tries_size):
            self.tryitem(obj.tries[i].contents,pad+2)

        self.print_label('handlers',pad)
        if obj.tries_size > 0:
            self.encodedcatchhandlerlist(obj.handlers.contents,pad+2)

    def debuginfo(self,obj,pad=0):
        self.print_label("DebugInfo",pad)

        size = self.max_attr(obj)
        pad +=2

        self.meta(obj.meta,pad)

        self.print_attr('line_start',int(obj.line_start),pad,size)
        self.print_attr('parameters_size',int(obj.parameters_size),pad,size)

        self.print_label('parameter_names',pad)
        for i in xrange(int(obj.parameters_size)):
            print ' '*(pad+2) + '%d' % int(obj.parameter_names[i])

        self.print_label('state_machine',pad)
        line = ' '*(pad+2)
        i = 0
        while obj.state_machine[i] != 0x0:
            line += '%02x' % obj.state_machine[i]

            if (len(line) % 60) == 0:
                print line
                line =' '*(pad+2)

            i+=1

        line += '00'
        print line

    def mapitem(self,obj,pad=0):
        self.print_label("MapItem",pad)
        
        pad += 2

        self.meta(obj.meta,pad)
        
        size = self.max_attr(obj)

        type_map = {
            0x0000:"header_item",
            0x0001:"string_id_item",
            0x0002:"type_id_item",
            0x0003:"proto_id_item",
            0x0004:"field_id_item",
            0x0005:"method_id_item",
            0x0006:"class_def_item",
            0x1000:"map_list",
            0x1001:"type_list",
            0x1002:"annotation_set_ref_list",
            0x1003:"annotation_set_item",
            0x2000:"class_data_item",
            0x2001:"code_item",
            0x2002:"string_data_item",
            0x2003:"debug_info_item",
            0x2004:"annotation_item",
            0x2005:"encoded_array_item",
            0x2006:"annotations_directory_item",
            }
                
        if obj.type not in type_map.keys():
            label = 'UNKNOWN'
        else:
            label = type_map.get(obj.type)

        self.print_attr('type','%s (%s)' % (hex(obj.type),label),pad,size)

        self.print_attr('unused',obj.unused,pad,size)
        self.print_attr('size',obj.size,pad,size)
        self.print_attr('offset',hex(obj.offset),pad,size)
        
    def maplist(self,obj,pad=0):
        self.print_label("MapList",pad)

        pad += 2

        self.meta(obj.meta,pad)

        self.print_attr("size",obj.size,pad,4)

        self.print_label('list',pad)
        for i in xrange(obj.size):
            self.mapitem(obj.list[i].contents,pad+2)

    def encodedvalue(self,obj,pad=0):
        self.print_label("EncodedValue",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        obj_type = (obj.argtype & 0x1f)        
        obj_arg = ((obj.argtype >> 5) & 0x7)
        
        self.print_attr('argtype',hex(obj.argtype),pad,size)

        self.print_attr('type',hex(obj_type),pad,size)
        self.print_attr('arg',obj_arg,pad,size)

        if obj_type == 0x0:
            self.print_attr('value',obj.value.contents,pad,size)
        elif obj_type in [0x2,0x3,0x4,0x6,0x10,0x11,0x17,0x18,0x19,0x1a,0x1b]:
            self.print_label('value',pad)
            data = ' '*(pad+2)
            for i in xrange(obj_arg+1):
                data += "%02x" % obj.value[i]
            print data
        elif obj_type == 0x1c:
            self.print_label('value',pad)
            self.encodedarray(cast(obj.value,POINTER(DexEncodedArray)).contents,pad+2)
        elif obj_type == 0x1d:
            self.print_label('value',pad)
            self.print_label(obj.value.contents._type_,pad)            
            self.encodedannotation(cast(obj.value,
                                        POINTER(DexEncodedAnnotation)).contents,pad+2)
        elif obj_type in [0x1e,0x1f]:
            self.print_label('value',pad)
        else:
            self.print_label('value',pad)
            print ' '*(pad+2) + 'Unknown'
        
    def encodedarray(self,obj,pad=0):
        self.print_label("EncodedArray",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('size',int(obj.size),pad,size)

        self.print_label('values',pad)
        for i in xrange(int(obj.size)):
            self.encodedvalue(obj.values[i].contents,pad+2)
        
    def annotationelement(self,obj,pad=0):
        self.print_label("AnnotationElement",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('name_idx',int(obj.name_idx),pad,size)
        self.print_label('value',pad)
        self.encodedvalue(obj.value.contents,pad+2)

    def encodedannotation(self,obj,pad=0):
        self.print_label("EncodedAnnotation",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('type_idx',int(obj.type_idx),pad,size)
        self.print_attr('size',int(obj.size),pad,size)

        self.print_label('elements',pad)
        for i in xrange(int(obj.size)):
            self.annotationelement(obj.elements[i].contents,pad+2)

    def fieldannotation(self,obj,pad=0):
        self.print_label("FieldAnnotation",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('field_idx',obj.field_idx,pad,size)
        self.print_attr('annotations_off',hex(obj.annotations_off),pad,size)

    def methodannotation(self,obj,pad=0):
        self.print_label("MethodAnnotation",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('method_idx',obj.method_idx,pad,size)
        self.print_attr('annotations_off',hex(obj.annotations_off),pad,size)

    def parameterannotation(self,obj,pad=0):
        self.print_label("ParameterAnnotation",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('method_idx',obj.method_idx,pad,size)
        self.print_attr('annotations_off',hex(obj.annotations_off),pad,size)

    def annotationdirectoryitem(self,obj,pad=0):
        self.print_label("AnnotationDirectoryItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('class_annotations_off',hex(obj.class_annotations_off),
                        pad,size)
        self.print_attr('fields_size',obj.fields_size,pad,size)
        self.print_attr('annotated_methods_size',obj.annotated_methods_size,pad,size)
        self.print_attr('annotated_parameters_size',obj.annotated_parameters_size,
                        pad,size)

        self.print_label('field_annotations',pad)
        for i in xrange(obj.fields_size):
            self.fieldannotation(obj.field_annotations[i].contents,pad+2)

        self.print_label('method_annotations',pad)
        for i in xrange(obj.annotated_methods_size):
            self.methodannotation(obj.method_annotations[i].contents,pad+2)

        self.print_label('parameter_annotations',pad)
        for i in xrange(obj.annotated_parameters_size):
            self.parameterannotation(obj.parameter_annotations[i].contents,pad+2)

    def annotationsetrefitem(self,obj,pad=0):
        self.print_label("AnnotationSetRefItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('annotations_off',hex(obj.annotations_off),pad,size)
        
    def annotationsetreflist(self,obj,pad=0):
        self.print_label("AnnotationSetRefList",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('size',hex(obj.size),pad,size)

        self.print_label('list',pad)
        for i in xrange(obj.size):
            self.annotationsetrefitem(obj.list[i].contents,pad+2)

    def annotationoffitem(self,obj,pad=0):
        self.print_label("AnnotationOffItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('annotation_off',hex(obj.annotation_off),pad,size)

    def annotationsetitem(self,obj,pad=0):
        self.print_label("AnnotationSetItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('size',hex(obj.size),pad,size)

        self.print_label('entries',pad)
        for i in xrange(obj.size):
            self.annotationoffitem(obj.entries[i].contents,pad+2)

    def annotationitem(self,obj,pad=0):
        self.print_label("AnnotationItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_attr('visibility',hex(obj.visibility),pad,size)

        self.print_label('annotation',pad)
        self.encodedannotation(obj.annotation.contents,pad+2)

    def encodedarrayitem(self,obj,pad=0):
        self.print_label("AnnotationItem",pad)

        pad += 2

        self.meta(obj.meta,pad)

        size = self.max_attr(obj)

        self.print_label('value',pad)
        self.encodedarray(obj.value.contents,pad+2)
