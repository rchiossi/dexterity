#!/usr/bin/python

import argparse

from dx.dex import Dex
from dx.printer import DexPrinter

def print_dump(args):
    printer = DexPrinter(args.meta)

    dex = Dex(args.dex_file)
    dex.parse()

    if args.H:
        printer.header(dex.header)
    if args.X:
        printer.maplist(dex.map_list)   
    if args.S:
        for item in dex.string_ids:
            printer.stringid(item)
    if args.T:
        for item in dex.type_ids:
            printer.typeid(item)
    if args.P:
        for item in dex.proto_ids:
            printer.protoid(item)
    if args.F:
        for item in dex.field_ids:
            printer.fieldid(item)
    if args.M:
        for item in dex.method_ids:
            printer.methodid(item)
    if args.C:
        for item in dex.class_defs:
            printer.classdef(item)
    if args.t:
        for item in dex.type_lists:
            printer.typelist(item)
    if args.s:
        for item in dex.string_data_list:
            printer.stringdata(item)
    if args.c:
        for item in dex.class_data_list:
            printer.classdata(item)
    if args.b:
        for item in dex.code_list:
            printer.codeitem(item)
    if args.d:
        for item in dex.debug_info_list:
            printer.debuginfo(item)
    if args.i:
        for item in dex.class_statics:
            printer.encodedarray(item)
    if args.n:
        for item in dex.class_annotations:
            printer.annotationdirectoryitem(item)
    if args.l:
        for item in dex.annotation_set_ref_lists:
            printer.annotationsetreflist(item)
    if args.e:
        for item in dex.annotation_sets:
            printer.annotationsetitem(item)
    if args.r:
        for item in dex.annotations:
            printer.annotationitem(item)

def main():
    parser = argparse.ArgumentParser(description="Dump dex structures")

    parser.add_argument('dex_file',help='Target DEX file')

    parser.add_argument('--meta', action='store_true',help='print meta info')
    parser.add_argument('-H', action='store_true',help='header_item')
    parser.add_argument('-X', action='store_true',help='map_list')

    parser.add_argument('-S', action='store_true',help='string_id')
    parser.add_argument('-T', action='store_true',help='type_id')
    parser.add_argument('-P', action='store_true',help='proto_id')
    parser.add_argument('-F', action='store_true',help='field_id')
    parser.add_argument('-M', action='store_true',help='method_id')
    parser.add_argument('-C', action='store_true',help='class_def')

    parser.add_argument('-t', action='store_true',help='type_list')
    parser.add_argument('-s', action='store_true',help='string_data')
    parser.add_argument('-c', action='store_true',help='class_data')
    parser.add_argument('-b', action='store_true',help='code_item')
    parser.add_argument('-d', action='store_true',help='debug_info')
    parser.add_argument('-i', action='store_true',help='class_static_fields')
    parser.add_argument('-n', action='store_true',help='class_annotations')
    parser.add_argument('-l', action='store_true',help='annotation_set_ref_list')
    parser.add_argument('-e', action='store_true',help='annotation_set_item')
    parser.add_argument('-r', action='store_true',help='annotation_item')

    args = parser.parse_args()

    print_dump(args)

if __name__ == "__main__":
    main()
