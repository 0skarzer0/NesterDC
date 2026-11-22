/* Automatically generated from video_object.cpp using ./create_hash_table. DO NOT EDIT ! */

namespace KJS {

const struct HashEntry videoTableEntries[] = {
   { "x", VideoImp::E_x, DontDelete, 0, 0 },
   { "y", VideoImp::E_y, DontDelete, 0, &videoTableEntries[12] },
   { "frameFinish", VideoImp::E_frameFinish, DontDelete|ReadOnly|Function, 0, 0 },
   { "height", VideoImp::E_height, DontDelete, 0, 0 },
   { "width", VideoImp::E_width, DontDelete, 0, 0 },
   { "takeScreenshot", VideoImp::E_takeScreenshot, DontDelete|ReadOnly|Function, 1, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "toString", VideoImp::E_toString, DontDelete|ReadOnly|Function, 0, &videoTableEntries[10] },
   { 0, 0, 0, 0, 0 },
   { "frameBegin", VideoImp::E_frameBegin, DontDelete|ReadOnly|Function, 0, &videoTableEntries[11] },
   { "drawRect", VideoImp::E_drawRect, DontDelete|ReadOnly|Function, 9, 0 },
   { "isPAL", VideoImp::E_isPAL, DontDelete|ReadOnly, 0, 0 }
};

const struct HashTable videoTable = { 2, 13, videoTableEntries, 10 };

} // namespace
