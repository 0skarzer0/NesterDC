/* Automatically generated from image_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry imageProtoTableEntries[] = {
   { "draw", ImageProtoFuncImp::E_draw, DontEnum|Function, 0, 0 },
   { "getFilename", ImageProtoFuncImp::E_getFilename, DontEnum|Function, 0, &imageProtoTableEntries[12] },
   { "getWidth", ImageProtoFuncImp::E_getWidth, DontEnum|Function, 0, 0 },
   { "unload", ImageProtoFuncImp::E_unload, DontEnum|Function, 0, &imageProtoTableEntries[11] },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "load", ImageProtoFuncImp::E_load, DontEnum|Function, 1, &imageProtoTableEntries[10] },
   { 0, 0, 0, 0, 0 },
   { "toString", ImageProtoFuncImp::E_toString, DontEnum|Function, 0, 0 },
   { "drawWH", ImageProtoFuncImp::E_drawWH, DontEnum|Function, 0, 0 },
   { "drawTrue", ImageProtoFuncImp::E_drawTrue, DontEnum|Function, 0, 0 },
   { "drawRotated", ImageProtoFuncImp::E_drawRotated, DontEnum|Function, 0, 0 },
   { "getHeight", ImageProtoFuncImp::E_getHeight, DontEnum|Function, 0, 0 }
};

const struct HashTable imageProtoTable = { 2, 13, imageProtoTableEntries, 10 };

} // namespace
