#!/usr/bin/python

import argparse

from dx.dex import Dex
from dx.bytestream import ByteStream
from dx.dxlib import dxlib

from dx.printer import DexPrinter

def print_dump(args):
    printer = DexPrinter(args.meta)

    bs = ByteStream(args.dex_file)
    dex = dxlib.dx_parse(bs._bs).contents

    if args.H:
        printer.header(dex.header.contents)
    if args.X:
        printer.maplist(dex.map_list.contents)   
    if args.S:
        for i in xrange(dex.header.contents.string_ids_size):
            printer.stringid(dex.string_ids[i].contents)
    if args.T:
        for i in xrange(dex.header.contents.type_ids_size):
            printer.typeid(dex.type_ids[i].contents)
    if args.P:
        for i in xrange(dex.header.contents.proto_ids_size):
            printer.protoid(dex.proto_ids[i].contents)
    if args.F:
        for i in xrange(dex.header.contents.field_ids_size):
            printer.fieldid(dex.field_ids[i].contents)
    if args.M:
        for i in xrange(dex.header.contents.method_ids_size):
            printer.methodid(dex.method_ids[i].contents)
    if args.C:
        for i in xrange(dex.header.contents.class_defs_size):
            printer.classdef(dex.class_defs[i].contents)
    if args.t:
        for i in xrange(dex.meta.type_lists_size):
            printer.typelist(dex.type_lists[i].contents)
    if args.s:
        for i in xrange(dex.header.contents.string_ids_size):
            printer.stringdata(dex.string_data_list[i].contents)
    if args.c:
        for i in xrange(dex.meta.class_data_size):
            printer.classdata(dex.class_data[i].contents)
    if args.b:
        for i in xrange(dex.meta.code_list_size):
            printer.codeitem(dex.code_list[i].contents)
    if args.d:
        for i in xrange(dex.meta.debug_info_list_size):
            printer.debuginfo(dex.debug_info_list[i].contents)
    if args.i:
        for i in xrange(dex.meta.encoded_arrays_size):
            printer.encodedarray(dex.encoded_arrays[i].contents)
    if args.n:
        for i in xrange(dex.meta.an_directories_size):
            printer.annotationdirectoryitem(dex.an_directories[i].contents)
    if args.l:
        for i in xrange(dex.meta.an_set_ref_lists_size):
            printer.annotationsetreflist(dex.an_set_ref_lists[i].contents)
    if args.e:
        for i in xrange(dex.meta.an_set_size):
            printer.annotationsetitem(dex.an_set[i].contents)
    if args.r:
        for i in xrange(dex.meta.annotations_size):
            printer.annotationitem(dex.annotations[i].contents)

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
