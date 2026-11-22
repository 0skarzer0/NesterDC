# libkjs Makefile

TARGET = libdreamscript.a

# KJS
OBJS =  kjs/array_object.o kjs/bool_object.o kjs/collector.o kjs/date_object.o kjs/debugger.o kjs/dtoa.o kjs/error_object.o
OBJS += kjs/function.o kjs/function_object.o kjs/grammar.o kjs/identifier.o kjs/internal.o kjs/interpreter.o kjs/lexer.o
OBJS += kjs/list.o kjs/lookup.o kjs/math_object.o kjs/nodes.o kjs/nodes2string.o kjs/number_object.o kjs/object.o
OBJS += kjs/object_object.o kjs/operations.o kjs/property_map.o kjs/reference.o kjs/reference_list.o kjs/regexp.o
OBJS += kjs/regexp_object.o kjs/scope_chain.o kjs/string_object.o kjs/ustring.o kjs/value.o kjs/snprintf.o
OBJS += kjs/pcre/pcre.o

OBJS += dreamscript.o

# DreamScript Core
OBJS += core/dc_controller.o core/dc_utils.o core/image.o
OBJS += core/dc_vmu.o core/image_object.o core/unzip.o core/unshrink.o core/explode.o core/unreduce.o
OBJS += core/video_object.o core/global_object.o core/dc_mouse.o core/input_object.o
OBJS += core/system_object.o core/filesystem_object.o core/file_object.o core/controller_object.o core/mouse_object.o
OBJS += core/XML.o core/music_object.o core/sprite_object.o core/makesplitpath.o
OBJS += core/allocator.o core/hashmap.o core/font_object.o core/soundeffect_object.o core/zipfile_object.o core/xml_object.o
OBJS += core/vmufile_object.o

KOS_CFLAGS += -g -Os

include $(KOS_BASE)/addons/Makefile.prefab


