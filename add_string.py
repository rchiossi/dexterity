#!/usr/bin/python

import argparse
import struct

from ctypes import cast, c_char_p, pointer, c_uint8
from ctypes import sizeof, create_string_buffer, pointer, POINTER

from dx.dex import Dex
from dx.dex import DexStringIdItem, DexStringDataItem, Leb128, Metadata

from adler32 import update_checksum

from dx.printer import DexPrinter
dxp = DexPrinter(True)

def find_index(dex,string):
    for i,item in enumerate(dex.string_data_list):
        str_data = str(cast(item.data,c_char_p).value)[:item.size.uleb()]
        
        if str_data == string:
            return -1

        if str_data > string: 
            return i

    return len(dex.string_data_list)

def add_string_id(dex,index):
    n_id = DexStringIdItem()

    n_id.meta.offset = dex.string_ids[index].meta.offset
    n_id.meta.corrupted = 0

    n_id.string_data_off = dex.string_ids[index].string_data_off

    dex.string_ids.insert(index,n_id)

    dex.header.string_ids_size += 1

    dex.size += sizeof(DexStringIdItem) - sizeof(Metadata)

def shift_string_ids(dex,index):
    for item in dex.type_ids:
        if item.descriptor_idx >= index:
            item.descriptor_idx += 1

    for item in dex.proto_ids:
        if item.shorty_idx >= index:
            item.shorty_idx += 1

    for item in dex.field_ids:
        if item.name_idx >= index:
            item.name_idx += 1

    for item in dex.method_ids:
        if item.name_idx >= index:
            item.name_idx += 1

    for item in dex.class_defs:
        if item.source_file_idx >= index:
            item.source_file_idx += 1

    #TODO: Update annotations

def add_string_data(dex,index,string):
    n_data = DexStringDataItem()

    n_data.meta.offset = dex.string_data_list[index].meta.offset
    n_data.meta.corrupted = 0

    n_data.size = Leb128()    
    #TODO: convert int to uleb
    for i,b in enumerate(bytearray(struct.pack('<I',len(string)))):
        n_data.size.data[i] = b
    n_data.size.size = 1
    #-------

    n_data.data = cast(pointer(create_string_buffer(string + '\x00')),POINTER(c_uint8))

#    dex.string_data_list.insert(index,n_data)
    dex.string_data_list[index] = n_data

    size = n_data.size.size + len(string) + 1

    dex.size += size

    return size,n_data.meta.offset

def shift_offsets(dex,offset,delta):
    pass
    

def add_string(dex,string):
    index = find_index(dex,string)
    
    if index == -1: return False

    add_string_id(dex,index)
    shift_string_ids(dex,index)

    size_delta,offset = add_string_data(dex,index,string)
    shift_offsets(dex,offset,size_delta)   

    return True

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('target',help='Target DEX file')
    parser.add_argument('string',help='String to be added')

    args = parser.parse_args() 

    args.string = 'p1ra'

    dex = Dex(args.target)
    dex.parse()

    add_string(dex,args.string)

    dex.save("out2.dex")

    update_checksum("out2.dex")

    print "Done"

if __name__ == '__main__':
    main()
