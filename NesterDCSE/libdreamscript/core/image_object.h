// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _IMAGE_OBJECT_H_
#define _IMAGE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"
#include "image.h"
#include "video_object.h"

//#include <sys/time.h>
#include "../kjs/port_kjs.h"

using namespace KJS;

class ImageInstanceImp : public ObjectImp, public DrawTarget
{
public:
	ImageInstanceImp(ObjectImp* proto);
	virtual ~ImageInstanceImp()
	{
		if (loaded)
			Image_Destroy(&img);
	}

	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
	static int loadCount;
	
	// Start JS Functions
	CString toString();
	bool load(const char* fn, pvr_list_t list, int filtering, IMG_ALPHA alpha);
	void unload();
	void draw(float x, float y, float scale, uint32 color, float z);
	void drawTrue(float x, float y, float scale, uint32 color, float z);
	void drawWH(float x, float y, float width, float height, uint32 color, float z);
	void drawRotated(float x, float y, float scale, uint32 color, float z,
					   float pivotu, float pivotv, float angle);
	const char* getFilename();
	uint32 getWidth();
	uint32 getHeight();
	// End JS Functions
	
	Image* GetImage() { return &img; }
	
	//DrawTarget
	virtual void DoDraw(struct SDrawCommand& sCmd);
	
private:
	CString filename;
	Image img;
	bool loaded;
};

class ImagePrototypeImp : public ImageInstanceImp
{
public:
	ImagePrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class ImageProtoFuncImp : public InternalFunctionImp
{
public:
	ImageProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);

	enum { E_toString, E_load, E_unload, E_draw, E_drawTrue, E_drawWH, E_drawRotated, E_getFilename, E_getWidth, E_getHeight };
private:
	int id;
};

class ImageObjectImp : public InternalFunctionImp
{
public:
	ImageObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, ImagePrototypeImp* dateProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitImageClass(ExecState* exec, Object globobj);

extern Object g_ImagePrototype;
extern Object g_Image;

#endif
