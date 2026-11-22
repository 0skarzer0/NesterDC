#ifndef _FILE_OBJECT_H_
#define _FILE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"

#include "../kjs/port_kjs.h"

using namespace KJS;

class FileInstanceImp : public ObjectImp
{
public:
	FileInstanceImp(ObjectImp* proto);
	~FileInstanceImp();

	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
	
	// Start JS Functions
	CString toString();
	bool open(const char* fn, const char* mode);
	void close();

	bool seek(int offset, int origin);
	bool tell(int& result);
	bool eof();
	
	bool readNumber(float& n);
	bool writeNumber(float n);

	bool readInt8(int8& n);
	bool readInt16(int16& n);
	bool readInt32(int32& n);
	bool writeInt8(int8 n);
	bool writeInt16(int16 n);
	bool writeInt32(int32 n);
	
	bool readUInt8(uint8& n);
	bool readUInt16(uint16& n);
	bool readUInt32(uint32& n);
	bool writeUInt8(uint8 n);
	bool writeUInt16(uint16 n);
	bool writeUInt32(uint32 n);
	
	bool readString(CString& result, uint32 size);
	bool readAll(CString& result);
	bool writeString(const char* str);
	bool readChar(CString& result);
	// End JS Functions
	
	FILE* GetFILE() { return fp; }
	
private:
	CString filename;
	FILE* fp;
};

class FilePrototypeImp : public FileInstanceImp
{
public:
	FilePrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class FileProtoFuncImp : public InternalFunctionImp
{
public:
	FileProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);

	enum { E_toString, E_open, E_close, E_seek, E_tell, E_eof, E_readNumber, E_writeNumber, E_readInt8, E_readInt16,
		   E_readInt32, E_writeInt8, E_writeInt16, E_writeInt32, E_readUInt8, E_readUInt16, E_readUInt32, E_writeUInt8,
		   E_writeUInt16, E_writeUInt32, E_readAll, E_readString, E_writeString, E_readChar };
private:
	int id;
};

class FileObjectImp : public InternalFunctionImp
{
public:
	FileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, FilePrototypeImp* fileProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitFileClass(ExecState* exec, Object globobj);

extern Object g_FilePrototype;
extern Object g_File;

#endif
