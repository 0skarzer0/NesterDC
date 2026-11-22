#ifndef _ZIPFILE_OBJECT_H_
#define _ZIPFILE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"

#include "../kjs/port_kjs.h"
#include "unzip.h"

using namespace KJS;

class ZipFileInstanceImp : public ObjectImp
{
public:
	ZipFileInstanceImp(ObjectImp* proto);
	~ZipFileInstanceImp();

	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
	
	// Start JS Functions
	CString toString();
	bool open(const char* fn);
	void close();
	
	bool goToFirstFile();
	bool goToNextFile();
	bool getCurrentFileInfo(unz_file_info *pfile_info, char *szFileName, uLong fileNameBufferSize);
	char* readCurrentFileIntoString(); 

	// End JS Functions
	
	unzFile GetHandle() { return fp; }
	
private:
	CString filename;
	unzFile fp;
};

class ZipFilePrototypeImp : public ZipFileInstanceImp
{
public:
	ZipFilePrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class ZipFileProtoFuncImp : public InternalFunctionImp
{
public:
	ZipFileProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);

	enum { E_toString, E_open, E_close, E_goToFirstFile, E_goToNextFile, E_getCurrentFileInfo, E_readCurrentFileIntoString };
private:
	int id;
};

class ZipFileObjectImp : public InternalFunctionImp
{
public:
	ZipFileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, ZipFilePrototypeImp* zipFileProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitZipFileClass(ExecState* exec, Object globobj);

extern Object g_ZipFilePrototype;
extern Object g_ZipFile;

#endif
