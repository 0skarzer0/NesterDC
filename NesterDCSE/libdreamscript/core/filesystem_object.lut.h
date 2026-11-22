/* Automatically generated from filesystem_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry filesystemTableEntries[] = {
   { "getFileList", FileSystemImp::E_getFileList, DontDelete|ReadOnly|Function, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "getCWD", FileSystemImp::E_getCWD, DontDelete|ReadOnly|Function, 0, &filesystemTableEntries[9] },
   { "toString", FileSystemImp::E_toString, DontDelete|ReadOnly|Function, 0, 0 },
   { "mkdir", FileSystemImp::E_mkdir, DontDelete|ReadOnly|Function, 1, 0 },
   { "setCWD", FileSystemImp::E_setCWD, DontDelete|ReadOnly|Function, 0, 0 },
   { "remove", FileSystemImp::E_remove, DontDelete|ReadOnly|Function, 1, 0 },
   { "resetCD", FileSystemImp::E_resetCD, DontDelete|ReadOnly|Function, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "getFile", FileSystemImp::E_getFile, DontDelete|ReadOnly|Function, 0, &filesystemTableEntries[10] },
   { "copy", FileSystemImp::E_copy, DontDelete|ReadOnly|Function, 2, 0 }
};

const struct HashTable filesystemTable = { 2, 11, filesystemTableEntries, 9 };

} // namespace
