/* Automatically generated from mouse_object.cpp using ./create_hash_table. DO NOT EDIT ! */

const struct HashEntry mouseTableEntries[] = {
   { "lbutton", MouseImp::E_lbutton, DontDelete|ReadOnly, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "dx", MouseImp::E_dx, DontDelete|ReadOnly, 0, 0 },
   { "dy", MouseImp::E_dy, DontDelete|ReadOnly, 0, 0 },
   { "dz", MouseImp::E_dz, DontDelete|ReadOnly, 0, &mouseTableEntries[8] },
   { "poll", MouseImp::E_poll, DontDelete|ReadOnly|Function, 0, &mouseTableEntries[9] },
   { "rbutton", MouseImp::E_rbutton, DontDelete|ReadOnly, 0, 0 },
   { "sbutton", MouseImp::E_sbutton, DontDelete|ReadOnly, 0, 0 }
};

const struct HashTable mouseTable = { 2, 10, mouseTableEntries, 8 };
