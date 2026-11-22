#include "file_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"

#include "file_object.lut.h"

using namespace KJS;

Object g_FilePrototype;
Object g_File;

void InitFileClass(ExecState* exec, Object global)
{
	FilePrototypeImp* fileProto = new FilePrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_FilePrototype = Object(fileProto);
	g_File = Object(new FileObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), fileProto));
	global.put(exec, "File", g_File, DontEnum|ReadOnly);
}

//INSTANCE

const ClassInfo FileInstanceImp::info = {"File", 0, 0, 0};

FileInstanceImp::FileInstanceImp(ObjectImp* proto) : ObjectImp(proto)
{
	fp = NULL;
}

FileInstanceImp::~FileInstanceImp()
{
	if (fp)
		fclose(fp);
}

CString FileInstanceImp::toString()
{
	CString temp = "File object: ";
	temp += filename;
	return temp;
}

bool FileInstanceImp::open(const char* fn, const char* mode)
{
	if (fp)
		fclose(fp);
	fp = fopen(fn, mode);
	return fp != NULL;
}

void FileInstanceImp::close()
{
	if (fp)
		fclose(fp);
	fp = NULL;
}

bool FileInstanceImp::seek(int offset, int origin)
{
	if (!fp)
		return false;
	return fseek(fp, offset, origin) == 0;
}

bool FileInstanceImp::tell(int& result)
{
	if (!fp)
		return false;
	result = ftell(fp);
	return true;
}

bool FileInstanceImp::eof()
{
	return (!fp || feof(fp));
}

bool FileInstanceImp::readNumber(float& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(float), 1, fp) == 1;
}

bool FileInstanceImp::writeNumber(float n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(float), 1, fp) == 1;
}

bool FileInstanceImp::readInt8(int8& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(int8), 1, fp) == 1;
}

bool FileInstanceImp::readInt16(int16& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(int16), 1, fp) == 1;
}

bool FileInstanceImp::readInt32(int32& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(int32), 1, fp) == 1;
}

bool FileInstanceImp::writeInt8(int8 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(int8), 1, fp) == 1;
}

bool FileInstanceImp::writeInt16(int16 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(int16), 1, fp) == 1;
}

bool FileInstanceImp::writeInt32(int32 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(int32), 1, fp) == 1;
}

bool FileInstanceImp::readUInt8(uint8& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(uint8), 1, fp) == 1;
}

bool FileInstanceImp::readUInt16(uint16& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(uint16), 1, fp) == 1;
}

bool FileInstanceImp::readUInt32(uint32& n)
{
	if (!fp)
		return false;
	return fread(&n, sizeof(uint32), 1, fp) == 1;
}

bool FileInstanceImp::writeUInt8(uint8 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(uint8), 1, fp) == 1;
}

bool FileInstanceImp::writeUInt16(uint16 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(uint16), 1, fp) == 1;
}

bool FileInstanceImp::writeUInt32(uint32 n)
{
	if (!fp)
		return false;
	return fwrite(&n, sizeof(uint32), 1, fp) == 1;
}

bool FileInstanceImp::readString(CString& result, uint32 size)
{
	if (!fp)
		return false;
	char* temp = new char[size+1];
	if (fgets(temp, size+1, fp) == NULL)
	{
		delete[] temp;
		return false;
	}
	result = temp;
	delete[] temp;
	return true;
}

bool FileInstanceImp::readAll(CString& result)
{
	if (!fp)
		return false;
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* sztemp = new char[fsize+1];
	fread(sztemp, sizeof(char), fsize, fp);
	sztemp[fsize] = '\0';
	result = sztemp;
	delete[] sztemp;
	return true;
}

bool FileInstanceImp::writeString(const char* str)
{
	if (!fp)
		return false;
	int len = strlen(str);
	fwrite(str, sizeof(char), len, fp);
	return true;
}

bool FileInstanceImp::readChar(CString& result)
{
	if (!fp)
		return false;
	char temp[2];
	temp[0] = fgetc(fp);
	temp[1] = '\0';
	result = temp;
	return true;
}

// PROTOTYPE

const ClassInfo FilePrototypeImp::info = {"File", 0, &fileProtoTable, 0};

/*

enum { E_toString, E_open, E_close, E_seek, E_tell, E_eof, E_readInt8, E_readInt16, E_readInt32, E_writeInt8
	   E_writeInt16, E_writeInt32, E_readUInt8, E_readUInt16, E_readUInt32, E_writeUInt8, E_writeUInt16
	   E_writeUInt32, E_readString, E_writeString, E_readChar };

@begin fileProtoTable 21
	toString		FileProtoFuncImp::E_toString		DontEnum|Function	0
	open			FileProtoFuncImp::E_open			DontEnum|Function	0
	close			FileProtoFuncImp::E_close			DontEnum|Function	0
	seek			FileProtoFuncImp::E_seek			DontEnum|Function	0
	tell			FileProtoFuncImp::E_tell			DontEnum|Function	0
	eof				FileProtoFuncImp::E_eof				DontEnum|Function	0
	readNumber		FileProtoFuncImp::E_readNumber		DontEnum|Function	0
	writeNumber		FileProtoFuncImp::E_writeNumber		DontEnum|Function	0
	readInt8		FileProtoFuncImp::E_readInt8		DontEnum|Function	0
	readInt16		FileProtoFuncImp::E_readInt16		DontEnum|Function	0
	readInt32		FileProtoFuncImp::E_readInt32		DontEnum|Function	0
	writeInt8		FileProtoFuncImp::E_writeInt8		DontEnum|Function	0
	writeInt16		FileProtoFuncImp::E_writeInt16		DontEnum|Function	0
	writeInt32		FileProtoFuncImp::E_writeInt32		DontEnum|Function	0
	readUInt8		FileProtoFuncImp::E_readUInt8		DontEnum|Function	0
	readUInt16		FileProtoFuncImp::E_readUInt16		DontEnum|Function	0
	readUInt32		FileProtoFuncImp::E_readUInt32		DontEnum|Function	0
	writeUInt8		FileProtoFuncImp::E_writeUInt8		DontEnum|Function	0
	writeUInt16		FileProtoFuncImp::E_writeUInt16		DontEnum|Function	0
	writeUInt32		FileProtoFuncImp::E_writeUInt32		DontEnum|Function	0
	readAll			FileProtoFuncImp::E_readAll			DontEnum|Function	0
	readString		FileProtoFuncImp::E_readString		DontEnum|Function	0
	writeString		FileProtoFuncImp::E_writeString		DontEnum|Function	0
	readChar		FileProtoFuncImp::E_readChar		DontEnum|Function	0
@end
*/

FilePrototypeImp::FilePrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : FileInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value FilePrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<FileProtoFuncImp, ObjectImp>(exec, propertyName, &fileProtoTable, this);
}

FileProtoFuncImp::FileProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value FileProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "File")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	FileInstanceImp* f = (FileInstanceImp*) thisObj.imp();
	switch (id)
	{
		case FileProtoFuncImp::E_toString:
			return String(f->toString().c_str());
		case FileProtoFuncImp::E_open:
		{
			if (args.size() > 1)
			{
				CString fn = args[0].toString(exec).cstring();
				CString mode = args[1].toString(exec).cstring();
				return Boolean(f->open(fn.c_str(), mode.c_str()));
			}
			break;
		}
		case FileProtoFuncImp::E_close:
		{
			f->close();
			break;
		}
		case FileProtoFuncImp::E_seek:
		{
			if (args.size() > 1)
				return Boolean(f->seek(args[0].toInteger(exec), args[1].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_tell:
		{
			int result;
			if (!f->tell(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_eof:
		{
			return Boolean(f->eof());
		}
		case FileProtoFuncImp::E_readNumber:
		{
			float result;
			if (!f->readNumber(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_writeNumber:
		{
			if (args.size() > 0)
				return Boolean(f->writeNumber(args[0].toNumber(exec)));
			break;
		}
		case FileProtoFuncImp::E_readInt8:
		{
			int8 result;
			if (!f->readInt8(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_readInt16:
		{
			int16 result;
			if (!f->readInt16(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_readInt32:
		{
			int32 result;
			if (!f->readInt32(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_writeInt8:
		{
			if (args.size() > 0)
				return Boolean(f->writeInt8(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_writeInt16:
		{
			if (args.size() > 0)
				return Boolean(f->writeInt16(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_writeInt32:
		{
			if (args.size() > 0)
				return Boolean(f->writeInt32(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_readUInt8:
		{
			uint8 result;
			if (!f->readUInt8(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_readUInt16:
		{
			uint16 result;
			if (!f->readUInt16(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_readUInt32:
		{
			uint32 result;
			if (!f->readUInt32(result))
				return Null();
			return Number(result);
		}
		case FileProtoFuncImp::E_writeUInt8:
		{
			if (args.size() > 0)
				return Boolean(f->writeUInt8(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_writeUInt16:
		{
			if (args.size() > 0)
				return Boolean(f->writeUInt16(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_writeUInt32:
		{
			if (args.size() > 0)
				return Boolean(f->writeUInt32(args[0].toInteger(exec)));
			break;
		}
		case FileProtoFuncImp::E_readAll:
		{
			CString str;
			if (f->readAll(str))
				return String(str.c_str());
			else
				return Null();
			break;
		}
		case FileProtoFuncImp::E_readString:
		{
			if (args.size() > 0)
			{
				CString str;
				if (f->readString(str, args[0].toInteger(exec)))
					return String(str.c_str());
				else
					return Null();
			}
			break;
		}
		case FileProtoFuncImp::E_writeString:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				return Boolean(f->writeString(str.c_str()));
			}
			break;
		}
		case FileProtoFuncImp::E_readChar:
		{
			CString temp;
			if (f->readChar(temp))
				return String(temp.c_str());
			else
				return Null();
			break;
		}
	}
	return Undefined();
}

//OBJECT

FileObjectImp::FileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, FilePrototypeImp* fileProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, fileProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object FileObjectImp::construct(ExecState* exec, const List& args)
{
	FileInstanceImp* temp = new FileInstanceImp(g_FilePrototype.imp());
	if (args.size() == 2)
		temp->open(args[0].toString(exec).cstring().c_str(), args[1].toString(exec).cstring().c_str());
	else if (args.size() == 1)
		temp->open(args[0].toString(exec).cstring().c_str(), "rb");
	return Object(temp);
}

