#!/usr/bin/python

import argparse
import hashlib

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

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('target',help='Target DEX file')

    args = parser.parse_args() 

    signature = update_signature(args.target)

    print "Calculated signature: %s" % signature

if __name__ == '__main__':
    main()
