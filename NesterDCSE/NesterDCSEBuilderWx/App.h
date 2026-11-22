#ifndef __MyApp_H__
#define __MyApp_H__

#include <wx/app.h>

class MyDlg;

class MyApp : public wxApp
{
public:
    bool VwXinitApp();
    virtual bool OnInit();
    virtual int OnExit();
    
    MyDlg *dlg;
};

DECLARE_APP(MyApp)

#endif

