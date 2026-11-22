/* Automatically generated from vmufile_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry vmufileInstanceTableEntries[] = {
   { "longDesc", VMUFileInstanceImp::E_longDesc, DontDelete, 0, &vmufileInstanceTableEntries[5] },
   { 0, 0, 0, 0, 0 },
   { "appID", VMUFileInstanceImp::E_appID, DontDelete, 0, 0 },
   { "shortDesc", VMUFileInstanceImp::E_shortDesc, DontDelete, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "data", VMUFileInstanceImp::E_data, DontDelete, 0, 0 }
};

const struct HashTable vmufileInstanceTable = { 2, 6, vmufileInstanceTableEntries, 5 };

} // namespace

namespace KJS {

const struct HashEntry vmufileProtoTableEntries[] = {
   { "load", VMUFileProtoFuncImp::E_load, DontEnum|Function, 1, 0 },
   { 0, 0, 0, 0, 0 },
   { "toString", VMUFileProtoFuncImp::E_toString, DontEnum|Function, 0, 0 },
   { "save", VMUFileProtoFuncImp::E_save, DontEnum|Function, 1, 0 }
};

const struct HashTable vmufileProtoTable = { 2, 4, vmufileProtoTableEntries, 4 };

} // namespace
