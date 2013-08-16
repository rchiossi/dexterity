#!/usr/bin/python

from ctypes import cdll
from ctypes import Structure
from ctypes import POINTER, pointer

from ctypes import c_int, c_uint, c_uint8, c_uint16, c_uint32
from ctypes import c_char_p

from ctypes import create_string_buffer

from ctypes import Array
from _ctypes import _Pointer

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

    def exhausted(self):
        return (self._bs.contents.exhausted != 0)

#LEB128
class Leb128(Structure):
    _fields_ = [
        ('data',c_uint8 * 5),
        ('size',c_uint),
        ]

    def uleb(self):
        return dxlib.ul128toui(self)

    def ulebp1(self):
        return dxlib.ul128p1toui(self)

    def sleb(self):
        return dxlib.sl128toui(self)

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
        ('size', Leb128),
        ('data', c_char_p),
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
        ('field_idx_diff',Leb128),
        ('access_flags',Leb128),
        ]

class DexEncodedMethodItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('method_idx_diff',Leb128),
        ('access_flags',Leb128),
        ('code_off',Leb128),
        ]

class DexClassDataItem(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('static_fields_size',Leb128),
        ('instance_fields_size',Leb128),
        ('direct_methods_size',Leb128),
        ('virtual_methods_size',Leb128),
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
        ('type_idx', Leb128),
        ('addr', Leb128),
        ]

class DexEncodedCatchHandler(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size', Leb128),
        ('handlers', POINTER(POINTER(DexEncodedTypeAddrPair))),
        ('catch_all_addr', Leb128)
        ]

class DexEncodedCatchHandlerList(Structure):
    _fields_ = [
        ('meta', Metadata),
        ('size',Leb128),
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
        ('tries',POINTER(POINTER(DexTryItem))),
        ('handlers',DexEncodedCatchHandlerList),
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

#Load Library
dxlib = cdll.LoadLibrary("./dexterity.so")
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

# Leb128 Prototypes
dxlib.ul128toui.argtypes = (Leb128,)
dxlib.ul128toui.restype = c_uint

dxlib.ul128p1toui.argtypes = (Leb128,)
dxlib.ul128p1toui.restype = c_uint

dxlib.sl128toui.argtypes = (Leb128,)
dxlib.sl128toui.restype = c_uint

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
DXPARSE('dx_mapitem',DexMapItem)
DXPARSE('dx_maplist',DexMapList)

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

    def class_data(self,offset=None):
        if offset != None: self.seek(offset)

        obj = dxlib.dx_classdata(self.bs._bs,self.bs._bs.contents.offset);

        return obj

def dxprint(obj,pad=0):
    print ' '*pad + "%s:" % obj.__class__.__name__

    for name,a_class in obj._fields_:
        val = getattr(obj,name)
 
        if bool(val) == 0:
            print ' '*(pad+2) + "%s: None" % name
        elif issubclass(a_class,(Leb128,)):       
#            print ' '*(pad+2) + "%s: %d" % (name,dxlib.ul128toui(val))
            dxprint(val,pad+2)
        elif issubclass(a_class,(Structure,)):
            dxprint(val,pad+2)
        elif issubclass(a_class,(Array,)):
            data = ''.join(['%02x' % x for x in val])
            print ' '* pad + "  %s: %s" % (name,data)
        elif issubclass(a_class,(_Pointer,)):
            print ' '*(pad+2) + '>>> List - Fist item:'
            dxprint(val[0],pad+4)
        elif issubclass(a_class,(c_char_p,)):
            print ' '* pad + "  %s: %s" % (name,val)
        elif name.find('size') != -1:
            print ' '*pad + "  %s: %d" % (name,val)
        elif name.find('idx') != -1:
            print ' '*pad + "  %s: %d" % (name,val)
        else:
            print ' '*pad + "  %s: 0x%x" % (name,val)
