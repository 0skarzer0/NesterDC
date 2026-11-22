#!/bin/sh
cd ../libdreamscript
make
cd ../new_nesterdcse
make
make cdimg
dc-tool -i cd.iso -t 192.168.0.4 -x nesterdcse.elf

