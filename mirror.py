#!/usr/bin/python

import argparse

from dx.dex import Dex
from dx.dex import ByteStream
from dx.dex import dxlib

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('source',help='Source DEX file')
    parser.add_argument('target',help='Target DEX file')

    args = parser.parse_args()

    dex = Dex(args.source)
    dex.parse()

    bs = ByteStream(size=dex.dxp.bs._bs.contents.size)

    dxlib.dxb_header(bs._bs,dex.header)
    dxlib.dxb_maplist(bs._bs,dex.map_list)

    for item in dex.string_ids: dxlib.dxb_stringid(bs._bs,item)
    for item in dex.type_ids: dxlib.dxb_typeid(bs._bs,item)
    for item in dex.proto_ids: dxlib.dxb_protoid(bs._bs,item)
    for item in dex.field_ids: dxlib.dxb_fieldid(bs._bs,item)
    for item in dex.method_ids: dxlib.dxb_methodid(bs._bs,item)
    for item in dex.class_defs: dxlib.dxb_classdef(bs._bs,item)

    for item in dex.string_data_list: dxlib.dxb_stringdata(bs._bs,item)
    for item in dex.type_lists: dxlib.dxb_typelist(bs._bs,item)
    for item in dex.class_annotations: dxlib.dxb_annotationdirectoryitem(
        bs._bs, item)
    for item in dex.class_data_list: dxlib.dxb_classdata(bs._bs,item)
    for item in dex.class_statics: dxlib.dxb_encodedarray(bs._bs,item)
    for item in dex.code_list: dxlib.dxb_codeitem(bs._bs,item)
    for item in dex.debug_info_list: dxlib.dxb_debuginfo(bs._bs,item)

    for item in dex.annotation_sets: dxlib.dxb_annotationsetitem(bs._bs,item)
    for item in dex.annotation_set_ref_lists: dxlib.dxb_annotationsetreflist(
        bs._bs,item)
    for item in dex.annotations: dxlib.dxb_annotationitem(bs._bs,item)
    
    bs.save(args.target)

if __name__ == "__main__":
    main()
