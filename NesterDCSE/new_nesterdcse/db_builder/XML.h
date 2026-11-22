#if !defined(AFX_XML_H__D9F1340C_4C0C_4F5C_A646_46D6D74C3489__INCLUDED_)
#define AFX_XML_H__D9F1340C_4C0C_4F5C_A646_46D6D74C3489__INCLUDED_

#include "main.h"
#include "punk_array.h"

START_EXTERN_C

typedef struct SXML
{
	char* name;
	char* text;
	DECLARE_PUNK_ARRAY(children, struct SXML*);
	DECLARE_PUNK_ARRAY(attribute_names, char*);
	DECLARE_PUNK_ARRAY(attribute_values, char*);
	struct SXML* parent;
	char parent_delete;
} XML;

extern uint32 XML_resize_count;

void XML_Init(XML* p, const char* szName /*= NULL*/);
void XML_Destroy(XML* p);

void XML_SetText(XML* p, const char* szText);
void XML_AppendText(XML* p, const char* szText);
void XML_AddChildNode(XML* p, XML* node);
XML* XML_AddChild(XML* p, const char* szName);
void XML_SetAttribute(XML* p, const char* name, const char* value);
const char* XML_GetAttribute(XML* p, const char* name);
const char* XML_GetAttributeString(XML* p, const char* name, const char* szDefault);
int32 XML_GetAttributeInt(XML* p, const char* name, int32 iDefault);
uint32 XML_GetAttributeHex(XML* p, const char* name, uint32 uiDefault);
XML* XML_Find(XML* p, const char* tagname, const char* attr_name /*= NULL*/, const char* attr_val /*= NULL*/);

XML* XML_Parse(const char* szDoc);

XML* XML_OpenFile(const char* filename);

END_EXTERN_C

#endif
