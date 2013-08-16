#!/usr/bin/python

import ctypes

from dex import DexParser
from dex import dxprint

def main():
    dxp = DexParser("./tests/classes.dex")

    header = dxp.item('header')

    map_list = dxp.item('maplist',header.map_off)

    string_ids = dxp.list('stringid' ,header.string_ids_size ,header.string_ids_off)
    type_ids   = dxp.list('typeid'   ,header.type_ids_size   ,header.type_ids_off)
    proto_ids  = dxp.list('protoid'  ,header.proto_ids_size  ,header.proto_ids_off)
    field_ids  = dxp.list('fieldid'  ,header.field_ids_size  ,header.field_ids_off)
    method_ids = dxp.list('methodid' ,header.method_ids_size ,header.method_ids_off)
    class_defs = dxp.list('classdef' ,header.class_defs_size ,header.class_defs_off)
        
    string_data_list = dxp.table('stringdata',string_ids,'string_data_off')

    type_lists = dxp.table('typelist',proto_ids,'parameters_off')

    for class_def in class_defs:
        dxp.class_data(class_def.class_data_off)

    class_data_list = dxp.table('classdata',class_defs,'class_data_off')

#    dxprint(header)

#    dxprint(map_list)

#    for item in string_ids:
#        dxprint(item)

#    for item in proto_ids:
#        dxprint(item) 

#    for item in class_defs:
#        dxprint(item)

#    for item in type_lists:
#        dxprint(item) 

#    for item in string_data_list:
#        print str(item.data)[:item.size.uleb()]

    for item in class_data_list:
        dxprint(item) 



if __name__ == '__main__':
    main()
