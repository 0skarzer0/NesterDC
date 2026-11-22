#ifndef _DREAMSCRIPT_H_
#define _DREAMSCRIPT_H_

#include "kjs/value.h"
#include "kjs/object.h"
#include "kjs/types.h"
#include "kjs/interpreter.h"
#include "core/dc_utils.h"

bool DreamScript_Init();
void DreamScript_Shutdown();
bool DreamScript_Evaluate(const char* str, const char* codename = "");
bool DreamScript_EvaluateFile(const char* filename);
KJS::Object* DreamScript_GetGlobalObject();
KJS::Interpreter* DreamScript_GetInterpreter();

#endif
