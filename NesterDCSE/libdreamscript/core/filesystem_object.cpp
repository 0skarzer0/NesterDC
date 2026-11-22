#include "filesystem_object.h"
#include "../kjs/lookup.h"
#include "filesystem_object.lut.h"

#include "dc_utils.h"

const ClassInfo FileSystemImp::info = { "FileSystem", 0, &filesystemTable, 0 };

FileSystemImp* g_filesystemImp = NULL;

/* Source for filesystem_object.lut.h

enum { E_toString, E_resetCD, E_getCWD, E_setCWD, E_getFileList, E_getFile, E_copy, E_mkdir, E_remove };

@begin filesystemTable 9
	toString		FileSystemImp::E_toString			DontDelete|ReadOnly|Function	0
	resetCD			FileSystemImp::E_resetCD			DontDelete|ReadOnly|Function	0
	getCWD			FileSystemImp::E_getCWD				DontDelete|ReadOnly|Function	0
	setCWD			FileSystemImp::E_setCWD				DontDelete|ReadOnly|Function	0
	getFileList		FileSystemImp::E_getFileList		DontDelete|ReadOnly|Function	0
	getFile			FileSystemImp::E_getFile			DontDelete|ReadOnly|Function	0
	copy			FileSystemImp::E_copy				DontDelete|ReadOnly|Function	2
	mkdir			FileSystemImp::E_mkdir				DontDelete|ReadOnly|Function	1
	remove			FileSystemImp::E_remove				DontDelete|ReadOnly|Function	1
@end
*/

FileSystemImp::FileSystemImp() : ObjectImp()
{
	m_cwd = "/";
}

Value FileSystemImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<FileSystemFunctionImp, FileSystemImp, ObjectImp>(exec, propertyName, &filesystemTable, this);
}

Value FileSystemImp::getValueProperty(ExecState *exec, int token) const
{
	return Undefined();
}

FileSystemFunctionImp::FileSystemFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value FileSystemFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "FileSystem")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case FileSystemImp::E_toString:
		{
			return String("FileSystem Object");
			break;
		}
		case FileSystemImp::E_resetCD:
		{
			iso_reset();
			break;
		}
		case FileSystemImp::E_getCWD:
		{
			FileSystemImp* pFS = (FileSystemImp*) thisObj.imp();
			return String(pFS->m_cwd.c_str());
			break;
		}
		case FileSystemImp::E_setCWD:
		{
			if (args.size() > 0)
			{
				FileSystemImp* pFS = (FileSystemImp*) thisObj.imp();
				CString temp = args[0].toString(exec).cstring();
				if (temp.size() > 0)
				{
					CString& cwd = pFS->m_cwd;
					if (temp.c_str()[0] == '/')
						cwd = temp;
					else
						cwd += temp;
					if (cwd.c_str()[cwd.size() - 1] != '/')
						cwd += "/";
				}
			}
			break;
		}
		case FileSystemImp::E_getFileList:
		{
			FileSystemImp* pFS = (FileSystemImp*) thisObj.imp();
			List locargs;
			CString tempdir = pFS->m_cwd;
			if (args.size() > 0)
			{
				CString arg0 = args[0].toString(exec).cstring();
				if (arg0.c_str()[0] == '/')
					tempdir = arg0;
				else
					tempdir += arg0;
			}
			if (tempdir.c_str()[tempdir.size()-1] != '/')
				tempdir += "/";
			file_t fpd = fs_open(tempdir.c_str(), O_DIR | O_RDONLY);
			dirent_t* dirinfo;
			while ((dirinfo = fs_readdir(fpd)))
			{
				if (strcmp(dirinfo->name, ".") == 0 || strcmp(dirinfo->name, "..") == 0)
					continue;
				Object tempobj = exec->interpreter()->builtinObject().construct(exec, List());
				char tempFullPath[0x100];
				sprintf(tempFullPath, "%s%s", tempdir.c_str(), dirinfo->name);
				tempobj.put(exec, Identifier("fullPath"), String(tempFullPath));
				tempobj.put(exec, Identifier("name"), String(dirinfo->name));
				tempobj.put(exec, Identifier("size"), Number(dirinfo->size));
				tempobj.put(exec, Identifier("isDir"), Boolean(dirinfo->size == -1));
				locargs.append(tempobj);
			}
			return exec->interpreter()->builtinArray().construct(exec, locargs);
			break;
		}
		case FileSystemImp::E_getFile:
		{
			
			break;
		}
		case FileSystemImp::E_copy:
		{
			if (args.size() > 1)
			{
				CString srcFilename = args[0].toString(exec).cstring();
				CString destFilename = args[1].toString(exec).cstring();
				//dbgio_printf("copying %s to %s\n", srcFilename.c_str(), destFilename.c_str());
				FILE* fpIN = fopen(srcFilename.c_str(), "rb");
				if (!fpIN)
					return Boolean(false);
				FILE* fpOUT = fopen(destFilename.c_str(), "wb");
				if (!fpOUT)
				{
					fclose(fpIN);
					return Boolean(false);
				}
				char buf[0x1000];
				int iread;
				while (true)
				{
					iread = fread(buf, 1, 0x1000, fpIN);
					if (iread)
						fwrite(buf, 1, iread, fpOUT);
					if (iread < 0x1000)
						break;
				}
				fclose(fpIN);
				fclose(fpOUT);
				return Boolean(true);
			}
			break;
		}
		case FileSystemImp::E_mkdir:
		{
			if (args.size() > 0)
			{
				CString dirName = args[0].toString(exec).cstring();
				dbgio_printf("%s fs_mkdir = %d\n", dirName.c_str(), fs_mkdir(dirName.c_str()));
			}
			break;
		}
		case FileSystemImp::E_remove:
		{
			if (args.size() > 0)
			{
				CString Name = args[0].toString(exec).cstring();
				fs_unlink(Name.c_str());
			}
			break;
		}
	}
	return Undefined();
}

void InitFileSystemObject(ExecState* exec, Object obj)
{
	g_filesystemImp = new FileSystemImp();
	obj.put(exec, Identifier("FileSystem"), Object(g_filesystemImp));
}
