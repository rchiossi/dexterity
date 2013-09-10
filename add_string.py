#!/usr/bin/python

import argparse
import struct

from ctypes import cast, c_char_p, c_uint8
from ctypes import sizeof, create_string_buffer, pointer, POINTER

from dx.dex import Dex
from dx.dex import DexStringIdItem, DexStringDataItem, ULeb128, Metadata

from adler32 import update_checksum
from sha1 import update_signature

def find_index(dex,string):
    for i,item in enumerate(dex.string_data_list):
        str_data = str(cast(item.data,c_char_p).value)[:int(item.size)]
        
        if str_data == string:
            return -1

        if str_data > string: 
            return i

    return len(dex.string_data_list)

def add_string_id(dex,index):
    n_id = DexStringIdItem()

    n_id.meta.corrupted = 0

    if index == len(dex.string_ids):
        n_id.meta.offset = dex.string_ids[index-1].meta.offset
        n_id.meta.offset += sizeof(DexStringIdItem) - sizeof(Metadata)

        n_id.string_data_off = dex.string_ids[index-1].string_data_off
        n_id.string_data_off += dex.string_data_list[index-1].size.size
        n_id.string_data_off += int(dex.string_data_list[index-1].size)+1
    else:
        n_id.meta.offset = dex.string_ids[index].meta.offset
        n_id.string_data_off = dex.string_ids[index].string_data_off

    dex.string_ids.insert(index,n_id)

    dex.header.string_ids_size += 1

    for item in dex.map_list.list:
        if item.contents.type == 0x0001:
            item.contents.size += 1
            break

    dex.size += sizeof(DexStringIdItem) - sizeof(Metadata)

def add_string_data(dex,index,string):
    n_data = DexStringDataItem()

    n_data.meta.offset = dex.string_ids[index].string_data_off
    n_data.meta.corrupted = 0

    n_data.size = ULeb128(len(string))

    n_data.data = cast(pointer(create_string_buffer(string + '\x00')),POINTER(c_uint8))

    dex.string_data_list.insert(index,n_data)

    for item in dex.map_list.list:
        if item.contents.type == 0x2002:
            item.contents.size += 1
            break

    size = n_data.size.size + len(string) + 1

    dex.header.data_size += size

    dex.size += size

def add_string(dex,string):
    index = find_index(dex,string)
    
    if index == -1: return False

    add_string_id(dex,index)
    dex.shift_stringids(index,1)
    
    offset = dex.string_ids[index].meta.offset
    delta = sizeof(DexStringIdItem) - sizeof(Metadata)

    dex.shift_offsets(offset,delta)

    dex.string_ids[index].meta.offset = offset

    if index == 0:
        dex.header.string_ids_off = offset

    add_string_data(dex,index,string)

    offset = dex.string_data_list[index].meta.offset
    delta   = dex.string_data_list[index].size.size
    delta  += int(dex.string_data_list[index].size)+1

    dex.shift_offsets(offset,delta)   

    dex.string_data_list[index].meta.offset = offset
    dex.string_ids[index].string_data_off = offset

    return True

def main():
    parser = argparse.ArgumentParser(description="Parse and reconstruct dex file")

    parser.add_argument('target',help='Target DEX file')
    parser.add_argument('string',help='String to be added')

    args = parser.parse_args() 

    dex = Dex(args.target)
    dex.parse()

    add_string(dex,args.string)

    dex.save("out2.dex")

    update_signature("out2.dex")
    update_checksum("out2.dex")

    print "Done"

if __name__ == '__main__':
    main()
