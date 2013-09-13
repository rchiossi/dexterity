#!/usr/bin/python

import argparse

from dx.dex import Dex

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('source',help='Source DEX file')
    parser.add_argument('target',help='Target DEX file')

    args = parser.parse_args()

    dex = Dex(args.source)
    dex.save(args.target)

if __name__ == "__main__":
    main()
