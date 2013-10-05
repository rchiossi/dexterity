# dexterity

### Description

**dexterity** is a C library intended for manipulation and analysis of **DEX** files. It has python bindings for all basic DEX structures and most of the manipulation functions.

WARNING: This library is still in it's early stages of development, use at your own risk! 

### Examples

###### mirror.py
The following example parses a DEX file using the built-in parser and then writes a new DEX file from the parsed structures in memory.
```python
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
```

###### addstr.py
The following example parses a DEX file, add a string to it and then creates a new DEX file with the modifications.
```python
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
```

For more examples of other usages of the library, check the examples folder.

### License
**dexterity** is released under BSD 3-clause license. Please check LICENSE for more details.

### Support
For questions and/or suggestions, join #dexterity on freenode.
