#include "XML.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

uint32 XML_resize_count = 0x10;

void XML_Init(XML* p, const char* szName)
{
    if (szName)
    {
        uint32 len = strlen(szName);
        p->name = (char*) malloc(len + 1);
        memcpy(p->name, szName, len);
        p->name[len] = '\0';
    }
    else
        p->name = NULL;
    p->text = NULL;
    p->parent = NULL;
    p->parent_delete = 1;
    punk_array_init(p->children);
    punk_array_init(p->attribute_names);
    punk_array_init(p->attribute_values);
}

void XML_Destroy(XML* p)
{
    if (p->name)
        free(p->name);
    if (p->text)
        free(p->text);
    uint32 k = 0;
    for (; k < p->attribute_names.count; ++k)
        free(p->attribute_names.data[k]);
    for (k = 0; k < p->attribute_values.count; ++k)
        free(p->attribute_values.data[k]);
    for (k = 0; k < p->children.count; ++k)
    {
        if (p->children.data[k]->parent_delete)
            free(p->children.data[k]);
    }
}

void XML_SetText(XML* p, const char* szText)
{
    if (p->text)
        free(p->text);
    p->text = (char*) malloc(strlen(szText)+1);
    strcpy(p->text, szText);
}

void XML_AppendText(XML* p, const char* szText)
{
    if (!p->text)
        XML_SetText(p, szText);
    else
    {
        char* sztemp = (char*) malloc(strlen(p->text)+strlen(szText)+1);
        sprintf(sztemp, "%s%s", p->text, szText);
        free(p->text);
        p->text = sztemp;
    }
}

void XML_AddChildNode(XML* p, XML* node)
{
    node->parent_delete = 0;
    punk_array_push(p->children, node);
    node->parent = p;
}

XML* XML_AddChild(XML* p, const char* szName)
{
    XML* temp = (XML*) malloc(sizeof(XML));
    XML_Init(temp, szName);
    XML_AddChildNode(p, temp);
    return temp;
}

void XML_SetAttribute(XML* p, const char* name, const char* value)
{
    uint32 k;
    for (k = 0; k < p->attribute_names.count; ++k)
    {
        if (strcmp(p->attribute_names.data[k], name) == 0)
        {
            free(p->attribute_values.data[k]);
            int32 len = strlen(value);
            char* temp = (char*) malloc(len + 1);
            memcpy(temp, value, len);
            temp[len] = '\0';
            p->attribute_values.data[k] = temp;
            return;
        }
    }
    int32 len = strlen(name);
    char* temp = (char*) malloc(len + 1);
    memcpy(temp, name, len);
    temp[len] = '\0';
    punk_array_push(p->attribute_names, temp);

    len = strlen(value);
    temp = (char*) malloc(len + 1);
    memcpy(temp, value, len);
    temp[len] = '\0';
    punk_array_push(p->attribute_values, temp);
}

const char* XML_GetAttribute(XML* p, const char* name)
{
    uint32 k;
    for (k = 0; k < p->attribute_names.count; ++k)
    {
        if (stricmp(p->attribute_names.data[k], name) == 0)
            return p->attribute_values.data[k];
    }
    return NULL;
}

const char* XML_GetAttributeString(XML* p, const char* name, const char* szDefault)
{
    uint32 k;
    for (k = 0; k < p->attribute_names.count; ++k)
    {
        if (stricmp(p->attribute_names.data[k], name) == 0)
            return p->attribute_values.data[k];
    }
    return szDefault;
}

int32 XML_GetAttributeInt(XML* p, const char* name, int32 iDefault)
{
    const char* sztemp = XML_GetAttribute(p, name);
    if (!sztemp)
        return iDefault;
    return atoi(sztemp);
}

uint32 XML_GetAttributeHex(XML* p, const char* name, uint32 uiDefault)
{
    const char* sztemp = XML_GetAttribute(p, name);
    if (!sztemp)
        return uiDefault;
    hexstr_to_uint32(sztemp, &uiDefault);
    return uiDefault;
}

XML* XML_Find(XML* p, const char* tagname, const char* attr_name, const char* attr_val)
{
    XML* temp = NULL;
    uint32 k;
    for (k = 0; k < p->children.count; ++k)
    {
        temp = p->children.data[k];
        if (stricmp(temp->name, tagname) == 0)
        {
            if (attr_name)
            {
                uint32 n;
                for (n = 0; n < temp->attribute_names.count; ++n)
                {
                    if (stricmp(temp->attribute_names.data[n], attr_name) == 0)
                    {
                        if (!attr_val || stricmp(temp->attribute_values.data[n], attr_val) == 0)
                            return temp;
                    }
                }
            }
            else
                return temp;
        }
        if ((temp = XML_Find(temp, tagname, attr_name, attr_val)))
            return temp;
    }
    return NULL;
}

#define MODE_SCANNING    0
#define MODE_TAG        1
#define MODE_TEXT        2

char *ne_shave(char *str, const char *whitespace)
{
    char *pnt, *ret = str;

    while (*ret != '\0' && strchr(whitespace, *ret) != NULL) {
    ret++;
    }

    /* pnt points at the NUL terminator. */
    pnt = &ret[strlen(ret)];
    
    while (pnt > ret && strchr(whitespace, *(pnt-1)) != NULL) {
    pnt--;
    }

    *pnt = '\0';
    return ret;
}

XML* XML_Parse(const char* szDoc)
{
    XML* root = NULL;
    XML* curnode = NULL;
    int32 mode = MODE_SCANNING;
    uint32 doclen = strlen(szDoc);
    char* doc = (char*) malloc(doclen + 1);
    memcpy(doc, szDoc, doclen);
    doc[doclen] = '\0';
    char* curTextBegin = NULL;
    char* pos = doc;
    char* end = doc + doclen;

    while (pos < end)
    {
        //printf("pos = %d char = %c mode = %d\n", pos-doc, *pos, mode);
        if (mode == MODE_SCANNING || mode == MODE_TEXT)
        {
            if (*pos == '<')
            {
                if (pos[1] == '/')
                {
                    if (!curnode)
                        goto parse_failed;
                    // Make Text
                    if (mode == MODE_TEXT && curTextBegin && curnode)
                    {
                        *pos = '\0';
                        XML_AppendText(curnode, ne_shave(curTextBegin, " \t\r\n"));
                    }
                    pos += 2;
                    char* base = pos;
                    while (*pos != '>')
                    {
                        ++pos;
                        if (pos == end)
                            goto parse_failed;
                    }
                    *pos = '\0';

                    //printf("end tag name = %s curnode = %s\n", base, curnode->name);
                    if (strcmp(base, curnode->name) != 0)
                        goto parse_failed;
                    curnode = curnode->parent;
                    curTextBegin = pos+1;
                    mode = MODE_TEXT;
                }
                else
                {
                    ++pos;
                    char* base = pos;
                    while (!isspace(*pos) && *pos != '>')
                    {
                        ++pos;
                        if (pos == end)
                            goto parse_failed;
                    }
                    char ctemp = *pos;
                    *pos = '\0';
                    //printf("new tag name = %s\n", base);
                    //++pos;
                    XML* temp = (XML*) malloc(sizeof(XML));
                    XML_Init(temp, base);
                    if (!root)
                        root = temp;
                    else
                        XML_AddChildNode(curnode, temp);
                    curnode = temp;
                    *pos = ctemp;
                    pos--;
                    mode = MODE_TAG;
                }
            }
        }
        else if (mode == MODE_TAG)
        {
            if (!isspace(*pos))
            {
                switch (*pos)
                {
                case '>':
                    {
                        if (pos[-1] == '/')
                        {
                            //printf("self close start = %s\n", curnode->name);
                            curnode = curnode->parent;
                            //printf("self close end = %s\n", curnode->name);
                        }
                        curTextBegin = pos+1;
                        mode = MODE_TEXT;
                        break;
                    }
                case '/':
                    {
                        break;
                    }
                default: // attribute name
                    {
                        char* name_base = pos;
                        while (*pos != '=')
                        {
                            ++pos;
                            if (pos == end)
                                goto parse_failed;
                        }
                        *pos = '\0';
                        ++pos;
                        if (*pos != '"')
                            goto parse_failed;
                        ++pos;
                        char* value_base = pos;
                        while (*pos != '"')
                        {
                            ++pos;
                            if (pos == end)
                                goto parse_failed;
                        }
                        *pos = '\0';
                        //printf("attribute tag:%s %s=%s\n", curnode->name, name_base, value_base);
                        XML_SetAttribute(curnode, name_base, value_base);
                        break;
                    }
                };
            }
        }
        ++pos;
    }
    free(doc);
    return root;
parse_failed:
    //printf("what?!\n");
    if (root)
        XML_Destroy(root);
    free(doc);
    return NULL;
}

XML* XML_OpenFile(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        int32 filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char* szTemp = (char*) malloc(filesize+1);
        fread(szTemp, 1, filesize, fp);
        szTemp[filesize] = '\0';
        fclose(fp);
        XML* tempXML = XML_Parse(szTemp);
        free(szTemp);
        return tempXML;
    }
    return NULL;
}

uint32 XML_SaveFile(XML* p, const char* filename)
{
    return 1;
}