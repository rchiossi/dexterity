#!/usr/bin/python

import argparse

from dx.dex import Dex
from dx.hash import update_signature
from dx.hash import update_checksum

def main():
    parser = argparse.ArgumentParser(description="Add a string to a DEX file.")

    parser.add_argument('source',help='Source DEX file')
    parser.add_argument('target',help='Target DEX file')
    parser.add_argument('string',help='String to be added')

    args = parser.parse_args() 

    dex = Dex(args.source)

    dex.add_string(args.string)

    dex.save(args.target)

    update_signature(args.target)
    update_checksum(args.target)

    print "Done"

if __name__ == '__main__':
    main()
