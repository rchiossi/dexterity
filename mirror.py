#!/usr/bin/python

import argparse

from dx.dex import Dex
from dx.dex import ByteStream
from dx.dex import dxlib

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('source',help='Source DEX file')
    parser.add_argument('target',help='Target DEX file')

    args = parser.parse_args()

    dex = Dex(args.source)
    dex.parse()

    dex.save(args.target)

if __name__ == "__main__":
    main()
