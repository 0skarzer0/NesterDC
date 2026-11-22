/* Automatically generated from system_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry systemTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "toString", SystemImp::E_toString, DontDelete|ReadOnly|Function, 0, &systemTableEntries[4] },
   { "garbageCollect", SystemImp::E_garbageCollect, DontDelete|ReadOnly|Function, 0, &systemTableEntries[5] },
   { "availableMemory", SystemImp::E_availableMemory, DontDelete|ReadOnly|Function, 0, 0 },
   { "startTimer", SystemImp::E_startTimer, DontDelete|ReadOnly|Function, 2, &systemTableEntries[6] },
   { "stopTimer", SystemImp::E_stopTimer, DontDelete|ReadOnly|Function, 0, 0 }
};

const struct HashTable systemTable = { 2, 7, systemTableEntries, 4 };

} // namespace
