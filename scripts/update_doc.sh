#!/bin/bash

#parameter is ip address of rpi

#run build_rpi.sh
./scripts/build_native.sh

./build/native/install/filesystem/home/root/script-doc-generator SMBR.Utilities/script-doc-generator/template.md > SMBR.Scheduler/README.md

