#!/bin/sh
make cdimg
dc-tool -i cd.iso -t 192.168.0.4 -x nesterdcse.elf

