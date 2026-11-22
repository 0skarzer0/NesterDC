#include "zipfile_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"

#include "zipfile_object.lut.h"

using namespace KJS;

Object g_ZipFilePrototype;
Object g_ZipFile;

void InitZipFileClass(ExecState* exec, Object global)
{
	ZipFilePrototypeImp* zipFileProto = new ZipFilePrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_ZipFilePrototype = Object(zipFileProto);
	g_ZipFile = Object(new ZipFileObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), zipFileProto));
	global.put(exec, "ZipFile", g_ZipFile, DontEnum|ReadOnly);
}

//INSTANCE

const ClassInfo ZipFileInstanceImp::info = {"File", 0, 0, 0};

ZipFileInstanceImp::ZipFileInstanceImp(ObjectImp* proto) : ObjectImp(proto)
{
	fp = NULL;
}

ZipFileInstanceImp::~ZipFileInstanceImp()
{
	if (fp)
		unzClose(fp);
}

CString ZipFileInstanceImp::toString()
{
	CString temp = "ZipFile object: ";
	temp += filename;
	return temp;
}

bool ZipFileInstanceImp::open(const char* fn)
{
	if (fp)
		unzClose(fp);
	fp = unzOpen(fn);
	return fp != NULL;
}

void ZipFileInstanceImp::close()
{
	if (fp)
		unzClose(fp);
	fp = NULL;
}

bool ZipFileInstanceImp::goToFirstFile()
{
	if (!fp)
		return false;
	return unzGoToFirstFile(fp) == UNZ_OK;
}

bool ZipFileInstanceImp::goToNextFile()
{
	if (!fp)
		return false;
	return unzGoToNextFile(fp) == UNZ_OK;
}

bool ZipFileInstanceImp::getCurrentFileInfo(unz_file_info *pfile_info, char *szFileName, uLong fileNameBufferSize)
{
	if (!fp)
		return false;
	return unzGetCurrentFileInfo(fp, pfile_info, szFileName, fileNameBufferSize, NULL, 0, NULL, 0) == UNZ_OK;
}

char* ZipFileInstanceImp::readCurrentFileIntoString()
{
	if (!fp)
		return NULL;
	unz_file_info info;
	if (unzGetCurrentFileInfo(fp, &info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
		return NULL;
	if (unzOpenCurrentFile(fp) != UNZ_OK)
		return NULL;
	char* szBuf = new char[info.uncompressed_size+1];
	if (unzReadCurrentFile(fp, szBuf, info.uncompressed_size) != info.uncompressed_size)
	{
		unzCloseCurrentFile(fp);
		delete[] szBuf;
		return NULL;
	}
	szBuf[info.uncompressed_size] = NULL;
	unzCloseCurrentFile(fp);
	return szBuf;
}

// PROTOTYPE

const ClassInfo ZipFilePrototypeImp::info = {"ZipFile", 0, &zipFileProtoTable, 0};

/*

enum { E_toString, E_open, E_close, E_goToFirstFile, E_goToNextFile, E_getCurrentFileInfo, E_readCurrentFileIntoString };

@begin zipFileProtoTable 10
	toString					ZipFileProtoFuncImp::E_toString						DontEnum|Function	0
	open						ZipFileProtoFuncImp::E_open							DontEnum|Function	1
	close						ZipFileProtoFuncImp::E_close						DontEnum|Function	0
	goToFirstFile				ZipFileProtoFuncImp::E_goToFirstFile				DontEnum|Function	0
	goToNextFile				ZipFileProtoFuncImp::E_goToNextFile					DontEnum|Function	0
	getCurrentFileInfo			ZipFileProtoFuncImp::E_getCurrentFileInfo			DontEnum|Function	0
	readCurrentFileIntoString	ZipFileProtoFuncImp::E_readCurrentFileIntoString	DontEnum|Function	0
@end
*/

ZipFilePrototypeImp::ZipFilePrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : ZipFileInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value ZipFilePrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<ZipFileProtoFuncImp, ObjectImp>(exec, propertyName, &zipFileProtoTable, this);
}

ZipFileProtoFuncImp::ZipFileProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value ZipFileProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "File")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	ZipFileInstanceImp* f = (ZipFileInstanceImp*) thisObj.imp();
	switch (id)
	{
		case ZipFileProtoFuncImp::E_toString:
			return String(f->toString().c_str());
		case ZipFileProtoFuncImp::E_open:
		{
			if (args.size() == 1)
			{
				CString fn = args[0].toString(exec).cstring();
				return Boolean(f->open(fn.c_str()));
			}
			break;
		}
		case ZipFileProtoFuncImp::E_close:
		{
			f->close();
			break;
		}
		case ZipFileProtoFuncImp::E_goToFirstFile:
		{
			return Boolean(f->goToFirstFile());
		}
		case ZipFileProtoFuncImp::E_goToNextFile:
		{
			return Boolean(f->goToNextFile());
		}
		/*
		uLong version;              // version made by                 2 bytes
		uLong version_needed;       // version needed to extract       2 bytes
		uLong flag;                 // general purpose bit flag        2 bytes
		uLong compression_method;   // compression method              2 bytes
		uLong dosDate;              // last mod file date in Dos fmt   4 bytes
		uLong crc;                  // crc-32                          4 bytes
		uLong compressed_size;      // compressed size                 4 bytes 
		uLong uncompressed_size;    // uncompressed size               4 bytes 
		uLong size_filename;        // filename length                 2 bytes
		uLong size_file_extra;      // extra field length              2 bytes
		uLong size_file_comment;    // file comment length             2 bytes
		uLong disk_num_start;       // disk number start               2 bytes
		uLong internal_fa;          // internal file attributes        2 bytes
		uLong external_fa;          // external file attributes        4 bytes
		tm_unz tmu_date;
		*/
		case ZipFileProtoFuncImp::E_getCurrentFileInfo:
		{
			char filename[0x100];
			unz_file_info info;
			bool result = f->getCurrentFileInfo(&info, filename, 0x100);
			if (!result)
				return Null();
			Object tempobj = exec->interpreter()->builtinObject().construct(exec, List());
			tempobj.put(exec, Identifier("compressed_size"), Number(info.compressed_size));
			tempobj.put(exec, Identifier("uncompressed_size"), Number(info.uncompressed_size));
			tempobj.put(exec, Identifier("filename"), String(filename));
			return tempobj;
		}
		case ZipFileProtoFuncImp::E_readCurrentFileIntoString:
		{
			char* szStr = f->readCurrentFileIntoString();
			if (szStr)
			{
				String temp(szStr);
				delete[] szStr;
				return temp;
			}
			else
				return Null();
		}
	}
	return Undefined();
}

//OBJECT

ZipFileObjectImp::ZipFileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, ZipFilePrototypeImp* zipFileProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, zipFileProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object ZipFileObjectImp::construct(ExecState* exec, const List& args)
{
	ZipFileInstanceImp* temp = new ZipFileInstanceImp(g_ZipFilePrototype.imp());
	if (args.size() == 1)
		temp->open(args[0].toString(exec).cstring().c_str());
	return Object(temp);
}

