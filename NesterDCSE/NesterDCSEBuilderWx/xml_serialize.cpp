#include "xml_serialize.h"

wxString XML_Serialize(XML* root, uint32 depth/*=0*/)
{
    wxString ret;
    wxString tabstr;
    uint32 k;
    for (k = 0; k < depth; ++k)
        tabstr += "\t";
    ret += tabstr+"<";
    ret += root->name;
    for (k = 0; k < root->attribute_names.count; ++k)
    {
        if (root->children.count ==0)
            ret += "\n"+tabstr+"\t";
        else
            ret += " ";
        ret += root->attribute_names.data[k];
        ret += "=\"";
        ret += root->attribute_values.data[k];
        ret += "\"";
    }
    if (root->children.count > 0)
    {
        for (k = 0; k < root->children.count; ++k)
            ret += XML_Serialize(root->children.data[k], depth+1);
        ret += "</";
        ret += root->name;
        ret += ">";
    }
    else
    {
        if (root->attribute_names.count > 0)
            ret += "\n"+tabstr;
        ret += "/>";
    }
    ret += "\n";
    return ret;
}
