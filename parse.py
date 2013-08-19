#!/usr/bin/python

import ctypes

from dex import DexParser

from dxprinter import DexPrinter

def main():
    printer = DexPrinter(True)

    dxp = DexParser("./tests/classes.dex")

    #header
    header = dxp.item('header')

    #data from header
    link_data = dxp.raw(header.link_off,header.link_size)
    map_list = dxp.item('maplist',header.map_off)
    string_ids = dxp.list('stringid' ,header.string_ids_size ,header.string_ids_off)
    type_ids   = dxp.list('typeid'   ,header.type_ids_size   ,header.type_ids_off)
    proto_ids  = dxp.list('protoid'  ,header.proto_ids_size  ,header.proto_ids_off)
    field_ids  = dxp.list('fieldid'  ,header.field_ids_size  ,header.field_ids_off)
    method_ids = dxp.list('methodid' ,header.method_ids_size ,header.method_ids_off)
    class_defs = dxp.list('classdef' ,header.class_defs_size ,header.class_defs_off)
      
    #data from string id
    string_data_list = dxp.table('stringdata',string_ids,'string_data_off')

    #data from proto id
    type_lists = dxp.table('typelist',proto_ids,'parameters_off')

    #data from class def
    type_lists += dxp.table('typelist',class_defs,'interfaces_off')
    #class_annotations = dxp.table('annotationsdirectory',classdefs,'annotations_off')
    class_data_list = dxp.table('classdata',class_defs,'class_data_off')
    #class_statics = dxp.table('encodedarray',class_defs,'static_values_off')

    #data from class data
    code_list = []
    
    for class_data in class_data_list:
#        printer.classdata(class_data)
        for i in xrange(class_data.direct_methods_size.uleb()):
            method = class_data.direct_methods[i].contents
            if method.code_off.uleb() != 0:
                code_list.append(dxp.item('codeitem',method.code_off.uleb()))
        
        for i in xrange(class_data.virtual_methods_size.uleb()):
            method = class_data.virtual_methods[i].contents
            if method.code_off.uleb() != 0:
                code_list.append(dxp.item('codeitem',method.code_off.uleb()))

    for item in code_list:
        printer.codeitem(item)
    

#    printer.header(header)

#    printer.maplist(map_list)

#    for item in string_ids:
#        printer.stringid(item)

#    for item in type_ids:
#        printer.typeid(item)

#    for item in proto_ids:
#        printer.protoid(item)

#    for item in field_ids:
#        printer.fieldid(item)

#    for item in method_ids:
#        printer.methodid(item)

#    for item in class_defs:
#        printer.classdef(item)

#    for item in type_lists:
#        printer.typelist(item)

#    for item in string_data_list:
#        printer.stringdata(item)

#    for item in class_data_list:
#        printer.classdata(item)



if __name__ == '__main__':
    main()
