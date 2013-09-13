from dxlib import _ByteStream
from dxlib import dxlib

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

