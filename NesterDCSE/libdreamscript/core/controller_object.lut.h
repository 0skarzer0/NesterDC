/* Automatically generated from controller_object.cpp using ./create_hash_table. DO NOT EDIT ! */

const struct HashEntry controllerTableEntries[] = {
   { "x", ControllerImp::X, DontDelete|ReadOnly, 0, 0 },
   { "y", ControllerImp::Y, DontDelete|ReadOnly, 0, 0 },
   { "right", ControllerImp::Right, DontDelete|ReadOnly, 0, 0 },
   { "start", ControllerImp::Start, DontDelete|ReadOnly, 0, 0 },
   { "poll", ControllerImp::Poll, DontDelete|ReadOnly|Function, 1, &controllerTableEntries[16] },
   { "down", ControllerImp::Down, DontDelete|ReadOnly, 0, 0 },
   { "ltrig", ControllerImp::Ltrig, DontDelete|ReadOnly, 0, 0 },
   { "a", ControllerImp::A, DontDelete|ReadOnly, 0, &controllerTableEntries[17] },
   { "joyx", ControllerImp::Joyx, DontDelete|ReadOnly, 0, &controllerTableEntries[15] },
   { "joyy", ControllerImp::Joyy, DontDelete|ReadOnly, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "rtrig", ControllerImp::Rtrig, DontDelete|ReadOnly, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "b", ControllerImp::B, DontDelete|ReadOnly, 0, 0 },
   { "up", ControllerImp::Up, DontDelete|ReadOnly, 0, 0 },
   { "left", ControllerImp::Left, DontDelete|ReadOnly, 0, 0 }
};

const struct HashTable controllerTable = { 2, 18, controllerTableEntries, 15 };
