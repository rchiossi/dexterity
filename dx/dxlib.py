from ctypes import cdll
from ctypes import Structure
from ctypes import POINTER, pointer

from ctypes import c_int, c_uint, c_uint8, c_uint16, c_uint32, c_int32
from ctypes import c_char_p
from ctypes import c_size_t

from ctypes import create_string_buffer

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

#ByteStream
class _ByteStream(Structure):
    _fields_ = [
        ('filename',c_char_p),
        ('size',c_uint32),
        ('data',POINTER(c_uint8)),
        ('offset',c_uint32),
        ('exhausted',c_int),
        ]

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

# C dex object
class DexMeta(Structure):
    _fields_ = [
        ('type_lists_size',c_size_t),
        ('type_lists_alloc',c_size_t),
        ('an_directories_size',c_size_t),
        ('an_directories_alloc',c_size_t),
        ('class_data_size',c_size_t),
        ('class_data_alloc',c_size_t),
        ('encoded_arrays_size',c_size_t),
        ('encoded_arrays_alloc',c_size_t),
        ('code_list_size',c_size_t),
        ('code_list_alloc',c_size_t),
        ('debug_info_list_size',c_size_t),
        ('debug_info_list_alloc',c_size_t),
        ('an_set_size',c_size_t),
        ('an_set_alloc',c_size_t),
        ('an_set_ref_lists_size',c_size_t),
        ('an_set_ref_lists_alloc',c_size_t),
        ('annotations_size',c_size_t),
        ('annotations_alloc',c_size_t),
        ]

class _Dex(Structure):
    _fields_ = [
        ('header',POINTER(DexHeaderItem)),
        ('map_list',POINTER(DexMapList)),
        ('string_ids',POINTER(POINTER(DexStringIdItem))),
        ('type_ids',POINTER(POINTER(DexTypeIdItem))),
        ('proto_ids',POINTER(POINTER(DexProtoIdItem))),
        ('field_ids',POINTER(POINTER(DexFieldIdItem))),
        ('method_ids',POINTER(POINTER(DexMethodIdItem))),
        ('class_defs',POINTER(POINTER(DexClassDefItem))),
        ('string_data_list',POINTER(POINTER(DexStringDataItem))),
        ('type_lists',POINTER(POINTER(DexTypeList))),
        ('an_directories',POINTER(POINTER(DexAnnotationDirectoryItem))),
        ('class_data',POINTER(POINTER(DexClassDataItem))),
        ('encoded_arrays',POINTER(POINTER(DexEncodedArray))),
        ('code_list',POINTER(POINTER(DexCodeItem))),
        ('debug_info_list',POINTER(POINTER(DexDebugInfo))),
        ('an_set',POINTER(POINTER(DexAnnotationSetItem))),
        ('an_set_ref_lists',POINTER(POINTER(DexAnnotationSetRefList))),
        ('annotations',POINTER(POINTER(DexAnnotationItem))),
        ('link_data',POINTER(c_uint8)),
        ('meta', DexMeta),
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
dxlib.dx_parse.argtypes = (POINTER(_ByteStream),)
dxlib.dx_parse.restype  = POINTER(_Dex)

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

dxlib.dx_build.argtypes = (POINTER(_Dex),c_char_p)
dxlib.dx_build.restype  = None

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

def DXOFFSET(name,obj):
    global dxlib
    getattr(dxlib,name).argtypes = (POINTER(obj),c_uint32,c_int32)
    getattr(dxlib,name).restype  = None

DXOFFSET('dxo_header',DexHeaderItem);
DXOFFSET('dxo_stringid',DexStringIdItem);
DXOFFSET('dxo_typeid',DexTypeIdItem);
DXOFFSET('dxo_protoid',DexProtoIdItem);
DXOFFSET('dxo_fieldid',DexFieldIdItem);
DXOFFSET('dxo_methodid',DexMethodIdItem);
DXOFFSET('dxo_classdef',DexClassDefItem);
DXOFFSET('dxo_stringdata',DexStringDataItem);
DXOFFSET('dxo_encodedfield',DexEncodedFieldItem);
DXOFFSET('dxo_encodedmethod',DexEncodedMethodItem);
DXOFFSET('dxo_classdata',DexClassDataItem);
DXOFFSET('dxo_typeitem',DexTypeItem);
DXOFFSET('dxo_typelist',DexTypeList);
DXOFFSET('dxo_tryitem',DexTryItem);
DXOFFSET('dxo_encodedtypeaddrpair',DexEncodedTypeAddrPair);
DXOFFSET('dxo_encodedcatchhandler',DexEncodedCatchHandler);
DXOFFSET('dxo_encodedcatchhandlerlist',DexEncodedCatchHandlerList);
DXOFFSET('dxo_codeitem',DexCodeItem);
DXOFFSET('dxo_debuginfo',DexDebugInfo);
DXOFFSET('dxo_mapitem',DexMapItem);
DXOFFSET('dxo_maplist',DexMapList);

DXOFFSET('dxo_encodedvalue',DexEncodedValue);
DXOFFSET('dxo_encodedarray',DexEncodedArray);
DXOFFSET('dxo_annotationelement',DexAnnotationElement);
DXOFFSET('dxo_encodedannotation',DexEncodedAnnotation);

DXOFFSET('dxo_fieldannotation',DexFieldAnnotation);
DXOFFSET('dxo_methodannotation',DexMethodAnnotation);
DXOFFSET('dxo_parameterannotation',DexParameterAnnotation);
DXOFFSET('dxo_annotationdirectoryitem',DexAnnotationDirectoryItem);
DXOFFSET('dxo_annotationsetrefitem',DexAnnotationSetRefItem);
DXOFFSET('dxo_annotationsetreflist',DexAnnotationSetRefList);
DXOFFSET('dxo_annotationoffitem',DexAnnotationOffItem);
DXOFFSET('dxo_annotationsetitem',DexAnnotationSetItem);
DXOFFSET('dxo_annotationitem',DexAnnotationItem);
DXOFFSET('dxo_encodedarrayitem',DexEncodedArrayItem);

def DXSTRINGID(name,obj):
    global dxlib
    getattr(dxlib,name).argtypes = (POINTER(obj),c_uint32,c_int32)
    getattr(dxlib,name).restype  = None

DXSTRINGID('dxsi_typeid',DexTypeIdItem);
DXSTRINGID('dxsi_protoid',DexProtoIdItem);
DXSTRINGID('dxsi_fieldid',DexFieldIdItem);
DXSTRINGID('dxsi_methodid',DexMethodIdItem);
DXSTRINGID('dxsi_classdef',DexClassDefItem);
DXSTRINGID('dxsi_debuginfo',DexDebugInfo);
DXSTRINGID('dxsi_encodedvalue',DexEncodedValue);
DXSTRINGID('dxsi_encodedarray',DexEncodedArray);
DXSTRINGID('dxsi_annotationelement',DexAnnotationElement);
DXSTRINGID('dxsi_encodedannotation',DexEncodedAnnotation);
DXSTRINGID('dxsi_annotationitem',DexAnnotationItem);
