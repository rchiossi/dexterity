#!/usr/bin/python

import argparse
import hashlib
import zlib
import struct

def update_signature(filename):
    f = open(filename,'r+')

    data = f.read()[32:]

    m = hashlib.sha1()
    m.update(data)
    
    signature = m.digest()

    f.seek(12)

    f.write(signature)

    f.close()

    return m.hexdigest()

def update_checksum(filename):
    f = open(filename,'r+')

    data = f.read()[12:]

    checksum = zlib.adler32(data)

    f.seek(8)

    f.write(struct.pack('<I',checksum & 0xffffffff))

    f.close()

    return checksum
