#!/usr/bin/python

import argparse

from dx.dex import Dex

def main():
    parser = argparse.ArgumentParser(description="Check for corrupted structures")

    parser.add_argument('dex_file',help='Target DEX file')

    args = parser.parse_args()

    dex = Dex(args.dex_file)
    dex.parse()

    corrupted  = dex.header.meta.corrupted
    corrupted |= dex.map_list.meta.corrupted

    for item in dex.string_ids: corrupted |= item.meta.corrupted
    for item in dex.type_ids: corrupted |= item.meta.corrupted
    for item in dex.proto_ids: corrupted |= item.meta.corrupted
    for item in dex.field_ids: corrupted |= item.meta.corrupted
    for item in dex.method_ids: corrupted |= item.meta.corrupted
    for item in dex.class_defs: corrupted |= item.meta.corrupted

    for item in dex.string_data_list: corrupted |= item.meta.corrupted
    for item in dex.type_lists: corrupted |= item.meta.corrupted
    for item in dex.class_annotations: corrupted |= item.meta.corrupted
    for item in dex.class_data_list: corrupted |= item.meta.corrupted
    for item in dex.class_statics: corrupted |= item.meta.corrupted
    for item in dex.code_list: corrupted |= item.meta.corrupted
    for item in dex.debug_info_list: corrupted |= item.meta.corrupted

    for item in dex.annotation_sets: corrupted |= item.meta.corrupted
    for item in dex.annotation_set_ref_lists: corrupted |= item.meta.corrupted

    for item in dex.annotations: corrupted |= item.meta.corrupted

    print "Corrupted: " + str(corrupted)

if __name__ == "__main__":
    main()
