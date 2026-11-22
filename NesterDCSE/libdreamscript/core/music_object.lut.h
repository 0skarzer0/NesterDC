/* Automatically generated from music_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry musicTableEntries[] = {
   { "init", MusicImp::E_init, DontDelete|ReadOnly|Function, 0, &musicTableEntries[4] },
   { 0, 0, 0, 0, 0 },
   { "toString", MusicImp::E_toString, DontDelete|ReadOnly|Function, 0, &musicTableEntries[5] },
   { 0, 0, 0, 0, 0 },
   { "shutdown", MusicImp::E_shutdown, DontDelete|ReadOnly|Function, 0, 0 },
   { "play", MusicImp::E_play, DontDelete|ReadOnly|Function, 1, &musicTableEntries[6] },
   { "stop", MusicImp::E_stop, DontDelete|ReadOnly|Function, 0, 0 }
};

const struct HashTable musicTable = { 2, 7, musicTableEntries, 4 };

} // namespace
