#!/usr/bin/python

import ctypes

from dex import DexParser
from dex import dxprint

def main():
    dxp = DexParser("./tests/classes.dex")

    header = dxp.item('header')

    string_ids = dxp.list('stringid',header.string_ids_size)
    type_ids   = dxp.list('typeid'  ,header.type_ids_size)
    proto_ids  = dxp.list('protoid' ,header.proto_ids_size)
    field_ids  = dxp.list('fieldid' ,header.field_ids_size)
    method_ids = dxp.list('methodid',header.method_ids_size)
    class_defs = dxp.list('classdef',header.class_defs_size)

    map_list = dxp.item('maplist',header.map_off)

    string_data_list = dxp.table('stringdata',string_ids,'string_data_off')

#    dxprint(header)

#    for item in string_ids:
#        print dxprint(item)

#    for item in string_data_list:
#        print str(item.data)[:item.size.uleb()]

#    for item in class_defs:
#        dxprint(item)
#    print map_list.list.contents.meta.offset

    dxprint(map_list)

if __name__ == '__main__':
    main()
