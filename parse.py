#!/usr/bin/python

import sys

from dex import DexParser
from dxprinter import DexPrinter

def main():    
    target = sys.argv[-1]
    print 'Parsing Dex: %s' % target

    dxp = DexParser(target)

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
        for i in xrange(class_data.direct_methods_size.uleb()):
            method = class_data.direct_methods[i].contents
            if method.code_off.uleb() != 0:
                code_list.append(dxp.item('codeitem',method.code_off.uleb()))
        
        for i in xrange(class_data.virtual_methods_size.uleb()):
            method = class_data.virtual_methods[i].contents
            if method.code_off.uleb() != 0:
                code_list.append(dxp.item('codeitem',method.code_off.uleb()))




    opts = ' '.join(sys.argv[1:-1]).split('-')
    args = {}

    printer = DexPrinter('debug' in opts)

    for i,opt in enumerate(opts):
        if opt in ['','debug']: continue

        if opt in ['H','X']:
            args[opt] = True

        elif opt.split(' ')[0] in ['S','T','P','F','M','C','t','s','c','B']:
            if len(opt.split()) == 1:
                args[opt] = -1
            elif opt.split(' ')[1].isdigit():
                args[opt.split(' ')[0]] = int(opt.split(' ')[1])
            else:
                print "Invalid Argument for %s : %s" % opt.split(' ')
                sys.exit(-1)
        else:
            print 'Unknown Option: -%s' % opt.split(' ')[0]
            sys.exit(-1)


    if 'H' in args.keys():
        printer.header(header)

    elif 'X' in args.keys():
        printer.maplist(map_list)            

    elif 'S' in args.keys():
        if args.get('S') < 0:
            for item in string_ids:
                printer.stringid(item)
        else:
            printer.stringid(string_ids[args.get('S')])

    elif 'T' in args.keys():
        if args.get('T') < 0:
            for item in type_ids:
                printer.typeid(item)
        else:
            printer.typeid(type_ids[args.get('T')])

    elif 'P' in args.keys():
        if args.get('P') < 0:
            for item in proto_ids:
                printer.protoid(item)
        else:
            printer.protoid(proto_ids[args.get('P')])

    elif 'F' in args.keys():
        if args.get('F') < 0:
            for item in field_ids:
                printer.fieldid(item)
        else:
            printer.fieldid(field_ids[args.get('F')])

    elif 'M' in args.keys():
        if args.get('M') < 0:
            for item in method_ids:
                printer.methodid(item)
        else:
            printer.methodid(method_ids[args.get('M')])

    elif 'C' in args.keys():
        if args.get('C') < 0:
            for item in class_defs:
                printer.classdef(item)
        else:
            printer.classdef(class_defs[args.get('C')])

    elif 't' in args.keys():
        if args.get('t') < 0:
            for item in type_lists:
                printer.typelist(item)
        else:
            printer.typelist(type_lists[args.get('t')])

    elif 's' in args.keys():
        if args.get('s') < 0:
            for item in string_data_list:
                printer.stringdata(item)
        else:
            printer.stringdata(string_data_list[args.get('s')])

    elif 'c' in args.keys():
        if args.get('c') < 0:
            for item in class_data_list:
                printer.classdata(item)
        else:
            printer.classdata(class_data_list[args.get('c')])

    elif 'B' in args.keys():
        if args.get('B') < 0:
            for item in code_list:
                printer.codeitem(item)
        else:
            printer.codeitem(code_list[args.get('B')])

    else:
        print 'Unknown Options.'

if __name__ == '__main__':
    main()
