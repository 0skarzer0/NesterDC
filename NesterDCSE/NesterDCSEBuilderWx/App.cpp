#include "app.h"
#include "MyDlg.h"
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/socket.h>

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
    VwXinitApp();
    wxSocketBase::Initialize();
    wxFileSystem::AddHandler(new wxInternetFSHandler);
    dlg = new MyDlg(NULL,-1,"");
    dlg->ShowModal();
    return FALSE;
}

int MyApp::OnExit()
{
    if (dlg->pBuildThread && dlg->pBuildThread->IsRunning())
    {
        dlg->buttonStart->SetLabel("Stopping Build");
        dlg->buttonStart->Enable(false);
        dlg->pBuildThread->Delete();
    }
    return wxApp::OnExit();
}
