#!/bin/sh -e

# APIVI usb package script
mkdir -p usb_root
mkdir -p usb_root/APIVS
mkdir -p usb_root/APIVS/bin
mkdir -p usb_root/APIVS/lib
mkdir -p usb_root/run
cp bin/* usb_root/APIVS/bin/
cp lib/* usb_root/APIVS/lib/
cp tests/* usb_root/run/
cp support/VS_config_1.txt usb_root/run/VS_config_1.txt
cp support/C2001d_vd_00 usb_root/run/C2001d_vd_00
cp support/C2001d_vd_clr usb_root/run/C2001d_vd_clr
cp support/Cxxxx_key0.txt usb_root/run/Cxxxx_key0.txt
cp support/startup usb_root/startup
cp support/runAPIVS usb_root/runAPIVS