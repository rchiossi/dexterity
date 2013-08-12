#!/usr/bin/python

from ctypes import cdll
from ctypes import Structure
from ctypes import POINTER, pointer

from ctypes import c_int, c_uint, c_uint8, c_uint32
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
    def __init__(self,fname):
        self._bs = dxlib.bsmap(fname)

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
class _Leb128(Structure):
    _fields_ = [
        ('data',c_uint8 * 5),
        ('size',c_uint),
        ]

#Dex
class _Metadata(Structure):
    _fields_ = [
        ('corrupted',c_uint),
        ('offset', c_uint),
        ]

class _DexHeaderItem(Structure):
    _fields_ = [
        ('meta',_Metadata),
        ('magic',c_uint8 * 8),
        ('checksum',c_uint32),
        ('signature',c_uint8 * 20),
        ('file_size',c_uint32),
        ('header_size',c_uint32),
        ('endian_tag',c_uint32),
        ('link_size',c_uint32),
        ('link_off',c_uint32),
        ('map_off',c_uint32),
        ('string_ids_size',c_uint32),
        ('string_ids_off',c_uint32),
        ('type_ids_size',c_uint32),
        ('type_ids_off',c_uint32),
        ('proto_ids_size',c_uint32),
        ('proto_ids_off',c_uint32),
        ('field_ids_size',c_uint32),
        ('field_ids_off',c_uint32),
        ('method_ids_size',c_uint32),
        ('method_ids_off',c_uint32),
        ('class_defs_size',c_uint32),
        ('class_defs_off',c_uint32),
        ('data_size',c_uint32),
        ('data_off',c_uint32),
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

#Dex prototypes
def DXPARSE(name,res):
    global dxlib
    getattr(dxlib,name).argtypes = (POINTER(_ByteStream),c_uint32)
    getattr(dxlib,name).restype  = POINTER(res) 

DXPARSE('dx_header',_DexHeaderItem)




# Legacy code
class _Dex(Structure):
    _fields_ = [
        ('header',POINTER(_DexHeaderItem)),
        ]

class Dex(object):
    def __init__(self,bs,offset=0):
        self._dex = dxlib.dxdex(bs._bs,offset)

        self.header = self._dex.contents.header.contents

    def __del__(self):
        dxlib.dxfree(self._dex)

#dxlib.dxdex.argtypes = (POINTER(_ByteStream),c_uint)
#dxlib.dxdex.restype = POINTER(_Dex)

dxlib.dxfree.argtypes = (POINTER(_Dex),)
dxlib.dxfree.restype = None
