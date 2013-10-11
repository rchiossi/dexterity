# dexterity

### Description

**dexterity** is a C library intended for manipulation and analysis of **DEX** files. It has python bindings for all basic DEX structures and most of the manipulation functions.

WARNING: This library is still in it's early stages of development, use at your own risk! 

### Examples

##### Mirror
The following example parses a DEX file using the built-in parser and then writes a new DEX file from the parsed structures in memory.
```python
#!/usr/bin/python

from dx.dex import Dex

dex = Dex("classes.dex")
dex.save("mirror.dex")
```

###### Add String
The following example parses a DEX file, add a string to it, creates a new DEX file with the modifications and fix the signature and checksum of the new file.
```python
#!/usr/bin/python

from dx.dex import Dex
from dx.hash import update_signature
from dx.hash import update_checksum

dex = Dex("classes.dex")
dex.add_string("Hello World")
dex.save("hello.dex")

update_signature("hello.dex")
update_checksum("hello.dex")
```

For more examples of other usages of the library, check the examples folder.

### License
**dexterity** is released under BSD 3-clause license. Please check LICENSE for more details.

### Support
For questions and/or suggestions, join **#droidsec** on **Freenode**.
