# -*- coding: utf-8 -*-

import os
import sys
import json
import shutil

out_json = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 4,
    "section": [
        {
            "firmware": "bk7231n_bootloader_enc.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "68K"
        },
        {
            "firmware": "bk7231_common_1.0.1_enc.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "1156K"
        }
    ]
}

bk7251_out_json = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader_bk7251_uart2_v1.0.8.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "65280"
        },
        {
            "firmware": "rtthread.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "1292K"
        }

    ]
}

def generate_beken_packager_json(beken_str, bootloader_str, firmware_str, out_json_str):
    global out_json
    global bk7251_out_json

    if beken_str == "bk7251":
        bk7251_out_json["section"][0]["firmware"] = bootloader_str
        bk7251_out_json["section"][1]["firmware"] = firmware_str
        bk7251_out_json = json.dumps(bk7251_out_json, sort_keys=True, indent=4)
        print(bk7251_out_json)
    else:
        out_json["section"][0]["firmware"] = bootloader_str
        out_json["section"][1]["firmware"] = firmware_str
        out_json = json.dumps(out_json, sort_keys=True, indent=4)
        print(out_json)

    with open(str(out_json_str), "w") as f:
	if beken_str == "bk7251":
            f.write(bk7251_out_json)
	else:
            f.write(out_json)

def gather_out_files(bootloader_str):
    if sys.platform == 'win32':
        cmd_cp = "copy "
        cmd_mv = "move "
        cmd_rm = "rd/s/q "
    else:
        cmd_cp = "cp "
        cmd_mv = "mv "
        cmd_rm = "rm -rf "
    out_folder = " out"
    os.system(cmd_rm + out_folder)
    os.system("mkdir" + out_folder)
    os.system(cmd_mv + "rtthread.bin" + out_folder)
    os.system(cmd_mv + "rtthread.elf" + out_folder)
    os.system(cmd_mv + "rtthread.map" + out_folder)
    os.system(cmd_mv + "all_2M.1220.bin" + out_folder)
    os.system(cmd_mv + "rtthread_uart_2M.1220.bin" + out_folder)
    os.system(cmd_cp + bootloader_str + out_folder)
    print 'rtthread.bin and other generated files were moved to folder %s' % out_folder

if __name__=='__main__':
    # from --beken=xxx
    beken_str = sys.argv[1]

    bootloader_str = ""
    firmware_str = "rtthread.bin"
    out_json_str = "config.json"
    if beken_str == "bk7231u":
        bootloader_str = "tools/beken_packager/bootloader_bk7231u_uart2_v1.0.8.bin"
    elif beken_str == "bk7231n":
        bootloader_str = "tools/beken_packager/bootloader_bk7231n_uart2_v1.0.8.bin"
    elif beken_str == "bk7236":
        bootloader_str = "tools/beken_packager/bootloader_bk7236_uart2_v1.0.8.bin"
    elif beken_str == "bk7271":
        bootloader_str = "tools/beken_packager/bootloader_bk7271_uart2_v1.0.8.bin"
    else:
        bootloader_str = "tools/beken_packager/bootloader_bk7251_uart2_v1.0.8.bin"

    # copy or generate json file
    generate_beken_packager_json(beken_str, bootloader_str, firmware_str, out_json_str)

    # run beken packager and gather out files
    if sys.platform == 'win32':
        os.system("tools\\beken_packager\\beken_packager")
    else:
        os.system("./tools/beken_packager/beken_packager")

    gather_out_files(bootloader_str)
