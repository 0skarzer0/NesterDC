#ifndef __CDBUILDTHREAD_H__
#define __CDBUILDTHREAD_H__

#include <wx/thread.h>
#include <wx/wx.h>

class MyDlg;

class CDBuildThread : public wxThread
{
public:
    CDBuildThread(const char* cfgFile, wxTextCtrl* txtOut, MyDlg* pDlg);
    
    virtual wxThread::ExitCode Entry();
    
private:
    wxString configFile;
    wxTextCtrl* txtOutput;
    MyDlg* dlg;
};

#endif
