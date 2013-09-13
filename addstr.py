#!/usr/bin/python

import argparse

from dx.dex import Dex

from sha1 import update_signature
from adler32 import update_checksum

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('target',help='Target DEX file')
    parser.add_argument('string',help='String to be added')

    args = parser.parse_args() 

    dex = Dex(args.target)

    dex.add_string(args.string)

    dex.save("out2.dex")

    update_signature("out2.dex")
    update_checksum("out2.dex")

    print "Done"

if __name__ == '__main__':
    main()
