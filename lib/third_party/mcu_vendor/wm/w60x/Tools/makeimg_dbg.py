# -*- coding:utf-8 -*_
#
# W600 make img script
# Copyright (c) 2018 Winner Micro Electronic Design Co., Ltd.
# All rights reserved.
#
#                                   image header structure
# --------------------------------------------------------------------------------------------
# | magic number(4B)  | image type(2B)| zip type(2B) | run image addr(4B) | run image len(4B) |
# --------------------------------------------------------------------------------------------
# | run image crc(4B) |  update image addr(4B)  | update image len(4B) | update image crc(4B) |
# ---------------------------------------------------------------------------------------------
# | update no(4B)     |                     ver(16B)                                          |
# ---------------------------------------------------------------------------------------------
# 
#
#               openocd debug image structure
#    ----------------------------------------------
#    |  application image header (image header)     |
#    ------------------------------------
#    |      256-sizeof(image heager)-pad 0xFF       |
#    ------------------------------------
#    |              application image               |
#    ------------------------------------------------
#
#

import sys
import os
import struct

debug_img_head_len = 0x100
magic_no = 0xA0FFFF9F
image_head_len = 56

def main(argv):
    if(len(argv) != 3):
        print("param cnt error")
        print("param 0: .py")
        print("param 1: input sec_image file")
        print("param 2: output debug image")
        raise Exception("param cnt error!")


    # app image
    try:
        f_app=open(argv[1], "rb+")
    except IOError:
        print("not find %s file" % argv[1])
        raise
    else:
        magic_word=f_app.read(4)
        magic, = struct.unpack('<I', magic_word)
        if magic != magic_no:
            f_app.close()
            raise Exception("input %s file error" % argv[1])

    appimg_len=os.path.getsize(argv[1]) - image_head_len

    # create outfile
    try:
        f_out=open(argv[2], 'wb+')
    except IOError:
        f_app.close()
        print("create %s file error" % argv[2])
        raise
    else:
        final_len=debug_img_head_len + appimg_len
        data = struct.pack('<B', 0xff) * final_len
        f_out.write(data)

        # Write SECBOOT header to output file
        f_out.seek(0)
        f_app.seek(0)
        f_out.write(f_app.read(image_head_len))

        # Write SEC image to output file
        f_out.seek(debug_img_head_len)

        # Write run img header to output file
        f_out.write(f_app.read())

        f_app.close()
        f_out.close()
        print("create %s file success！" % argv[2])
        print("%s size:%dKB. %s size:%dKB" % (argv[1], appimg_len / 1024.0 , argv[2], final_len / 1024.0))


if __name__ == '__main__':
    main(sys.argv)
