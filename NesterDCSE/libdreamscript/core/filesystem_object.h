#ifndef _FILESYSTEM_OBJECT_H_
#define _FILESYSTEM_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "punk_array.h"

using namespace KJS;

class FileSystemFunctionImp;

class FileSystemImp : public ObjectImp
{
public:
	FileSystemImp();
	
	enum { E_toString, E_resetCD, E_getCWD, E_setCWD, E_getFileList, E_getFile, E_copy, E_mkdir, E_remove };

	virtual UString className() const { return "FileSystem"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	
	static const ClassInfo info;
	
	CString m_cwd;
	
private:
};

class FileSystemFunctionImp : public ObjectImp
{
public:
	FileSystemFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitFileSystemObject(ExecState* exec, Object obj);

extern FileSystemImp* g_filesystemImp;

#endif
