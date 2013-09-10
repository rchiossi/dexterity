#!/usr/bin/python

import argparse
import zlib
import struct

def update_checksum(filename):
    f = open(filename,'r+')

    data = f.read()[12:]

    checksum = zlib.adler32(data)

    f.seek(8)

    f.write(struct.pack('<I',checksum & 0xffffffff))

    f.close()

    return checksum

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('target',help='Target DEX file')

    args = parser.parse_args() 

    checksum = update_checksum(args.target)

    print "Calculated checksum: %s" % hex(checksum)

if __name__ == '__main__':
    main()
