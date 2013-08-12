#!/usr/bin/python

import ctypes

from dex import DexParser
from dex import dxprint

def main():
    dx = DexParser("./tests/classes.dex")

    header = dx.header()   

    string_ids = [dx.stringid() for i in xrange(header.string_ids_size)]
    type_ids = [dx.typeid() for i in xrange(header.type_ids_size)]
    proto_ids = [dx.protoid() for i in xrange(header.proto_ids_size)]
    field_ids = [dx.fieldid() for i in xrange(header.field_ids_size)]
    method_ids = [dx.methodid() for i in xrange(header.method_ids_size)]
    class_defs = [dx.classdef() for i in xrange(header.class_defs_size)]

    string_data_list = [dx.stringdata(sid.string_data_off) for sid in string_ids]

#    dxprint(header)

#    for item in string_ids:
#        print dxprint(item)

#    for item in string_data_list:
#        print str(item.data)[:item.size.uleb()]

    for item in class_defs:
        dxprint(item)

    

if __name__ == '__main__':
    main()
