/* Automatically generated from zipfile_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry zipFileProtoTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "goToFirstFile", ZipFileProtoFuncImp::E_goToFirstFile, DontEnum|Function, 0, 0 },
   { "open", ZipFileProtoFuncImp::E_open, DontEnum|Function, 1, &zipFileProtoTableEntries[10] },
   { 0, 0, 0, 0, 0 },
   { "readCurrentFileIntoString", ZipFileProtoFuncImp::E_readCurrentFileIntoString, DontEnum|Function, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "toString", ZipFileProtoFuncImp::E_toString, DontEnum|Function, 0, &zipFileProtoTableEntries[11] },
   { "getCurrentFileInfo", ZipFileProtoFuncImp::E_getCurrentFileInfo, DontEnum|Function, 0, 0 },
   { "close", ZipFileProtoFuncImp::E_close, DontEnum|Function, 0, 0 },
   { "goToNextFile", ZipFileProtoFuncImp::E_goToNextFile, DontEnum|Function, 0, 0 }
};

const struct HashTable zipFileProtoTable = { 2, 12, zipFileProtoTableEntries, 10 };

} // namespace
