
// Don't modify comment 
#include "MyDlg.h"
//[inc]add your include files here

#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/thread.h>
#include "XML.h"
#include "xml_serialize.h"
#include <wx/process.h>

//[inc]end your include
 

IMPLEMENT_CLASS(MyDlg,wxDialog)


MyDlg::MyDlg(){
}

MyDlg::MyDlg(wxWindow* parent,wxWindowID id,const wxString& title,const wxPoint& pos,const wxSize& size,long style,const wxString& name)
 VwX_INIT_OBJECTS_MyDlg
{
 OnPreCreate();
 Create(parent,id,title,pos,size,style,name);

 if((pos==wxDefaultPosition)&&(size==wxDefaultSize)){
     SetSize(0,0,564,480);
 }

 if((pos!=wxDefaultPosition)&&(size==wxDefaultSize)){
     SetSize(564,480);
 }
 initBefore();
 fileImgBuf[0].LoadFile(_T("images/nesterdcselogo05.gif"),wxBITMAP_TYPE_GIF);
 bitmapTitleImg0=&fileImgBuf[0];
 VwXinit();initAfter();
}
MyDlg::~MyDlg()
{
  txtMasterLibrary->PopEventHandler(true);
  txtROMs->PopEventHandler(true);
  DMyDlg();
}
void MyDlg::VwXinit()
{
 SetTitle(wxT("NesterDC SE CD Builder"));
 bitmapTitle=new wxStaticBitmap(this,-1,*bitmapTitleImg0,wxPoint(6,7),wxSize(543,125));
 st4c=new wxStaticText(this,-1,wxT(""),wxPoint(30,146),wxSize(141,16),wxST_NO_AUTORESIZE);
   st4c->SetLabel(wxT("ROMs Path:"));
   st4c->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 st5c=new wxStaticText(this,-1,wxT(""),wxPoint(31,172),wxSize(141,21),wxST_NO_AUTORESIZE);
   st5c->SetLabel(wxT("Master Library URL:"));
   st5c->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 st6c=new wxStaticText(this,-1,wxT(""),wxPoint(30,198),wxSize(141,21),wxST_NO_AUTORESIZE);
   st6c->SetLabel(wxT("CD Image Format:"));
   st6c->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 st7c=new wxStaticText(this,-1,wxT(""),wxPoint(32,224),wxSize(141,21),wxST_NO_AUTORESIZE);
   st7c->SetLabel(wxT("Features:"));
   st7c->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 txtROMs=new wxTextCtrl(this,-1,wxT(""),wxPoint(181,146),wxSize(300,21),wxTE_PROCESS_ENTER);
   txtROMs->PushEventHandler(new MyDlgEvt(this));
 txtMasterLibrary=new wxTextCtrl(this,-1,wxT(""),wxPoint(181,172),wxSize(300,21),wxTE_PROCESS_ENTER);
   txtMasterLibrary->SetLabel(wxT("http://www.sch3rz0.com/nesterdcse/master_library"));
   txtMasterLibrary->PushEventHandler(new MyDlgEvt(this));
 chkCartridge=new wxCheckBox(this,-1,wxT(""),wxPoint(181,224),wxSize(141,21));
   chkCartridge->SetValue(true);
   chkCartridge->SetTitle(wxT("Cartridge Scans"));
   chkCartridge->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkInstructions=new wxCheckBox(this,-1,wxT(""),wxPoint(327,224),wxSize(166,21));
   chkInstructions->SetValue(true);
   chkInstructions->SetTitle(wxT("Instruction Manuals"));
   chkInstructions->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkFAQs=new wxCheckBox(this,-1,wxT(""),wxPoint(181,246),wxSize(141,21));
   chkFAQs->SetValue(true);
   chkFAQs->SetTitle(wxT("FAQs"));
   chkFAQs->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkTitles=new wxCheckBox(this,-1,wxT(""),wxPoint(327,246),wxSize(141,21));
   chkTitles->SetValue(true);
   chkTitles->SetTitle(wxT("Title Shots"));
   chkTitles->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkScreens=new wxCheckBox(this,-1,wxT(""),wxPoint(327,268),wxSize(191,21));
   chkScreens->SetValue(true);
   chkScreens->SetTitle(wxT("Screen Shots (in-game)"));
   chkScreens->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkBoxes=new wxCheckBox(this,-1,wxT(""),wxPoint(181,268),wxSize(141,21));
   chkBoxes->SetValue(true);
   chkBoxes->SetTitle(wxT("Box Scans"));
   chkBoxes->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkNSFs=new wxCheckBox(this,-1,wxT(""),wxPoint(181,290),wxSize(141,21));
   chkNSFs->SetValue(true);
   chkNSFs->SetTitle(wxT("NSFs"));
   chkNSFs->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 chkGameGenie=new wxCheckBox(this,-1,wxT(""),wxPoint(327,290),wxSize(189,21));
   chkGameGenie->SetValue(true);
   chkGameGenie->SetTitle(wxT("Game Genie Codes"));
   chkGameGenie->SetFont(wxFont(12,74,90,90,0,wxT("Tahoma")));
 wxString dropImageFormatItem[]={wxT("Nero"),wxT("DiscJuggler")};
 dropImageFormat=new wxChoice(this,-1,wxPoint(181,198),wxSize(300,21),2,dropImageFormatItem);
 txtOutput=new wxTextCtrl(this,-1,wxT(""),wxPoint(3,316),wxSize(552,130),wxTE_MULTILINE);
 buttonStart=new wxButton(this,-1,wxT(""),wxPoint(18,254),wxSize(147,54));
   buttonStart->SetLabel(wxT("Start Build"));
   buttonStart->SetFont(wxFont(14,74,90,90,0,wxT("Tahoma")));
 buttonROMs=new wxButton(this,-1,wxT(""),wxPoint(485,146),wxSize(20,21));
   buttonROMs->SetLabel(wxT("..."));
 chkLibraryDefault=new wxCheckBox(this,-1,wxT(""),wxPoint(483,172),wxSize(73,21));
   chkLibraryDefault->SetTitle(wxT("Default"));
 Refresh();
}
 
BEGIN_EVENT_TABLE(MyDlgEvt,wxEvtHandler)
  EVT_KILL_FOCUS(MyDlgEvt::VwXEvOnKillFocus)
//[evtEvt]add your code here


    

//[evtEvt]end your code
END_EVENT_TABLE()
void MyDlgEvt::VwXEvOnKillFocus(wxFocusEvent& event){
 ptr_winPan->VwXVwXEvOnKillFocus(event);
}
 
BEGIN_EVENT_TABLE( MyDlg,wxDialog)
  EVT_BUTTON(-1,MyDlg::VwXVwXEvOnButtonClick)
  EVT_CHECKBOX(-1,MyDlg::VwXVwXEvOnCheckBox)
  EVT_CHOICE(-1,MyDlg::VwXVwXEvOnChoiceSelect)
  EVT_ERASE_BACKGROUND(MyDlg::VwXVwXEvOnEraseBackground)
  EVT_INIT_DIALOG(MyDlg::VwXVwXEvOnInitDialog)
  EVT_KILL_FOCUS(MyDlg::VwXVwXEvOnKillFocus)
  EVT_TEXT_ENTER(-1,MyDlg::VwXVwXEvOnTextEnter)
//[evtwin]add your code here

//[evtwin]end your code
END_EVENT_TABLE()
void MyDlg::VwXVwXEvOnTextEnter(wxCommandEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==txtROMs){txtROMs_VwXEvOnTextEnter(event,-1);return;}
 if(m_wxWin==txtMasterLibrary){txtMasterLibrary_VwXEvOnTextEnter(event,-1);return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnKillFocus(wxFocusEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==txtROMs){event.Skip(true);txtROMs_VwXEvOnKillFocus(event,-1);return;}
 if(m_wxWin==txtMasterLibrary){event.Skip(true);txtMasterLibrary_VwXEvOnKillFocus(event,-1);return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnInitDialog(wxInitDialogEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==this){event.Skip(true);VwXEvOnInitDialog(event) ;return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnEraseBackground(wxEraseEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==this){event.Skip(true);VwXEvOnEraseBackground(event) ;return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnChoiceSelect(wxCommandEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==dropImageFormat){dropImageFormat_VwXEvOnChoiceSelect(event,-1);return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnCheckBox(wxCommandEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==chkFAQs){chkFAQs_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkTitles){chkTitles_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkScreens){chkScreens_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkBoxes){chkBoxes_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkNSFs){chkNSFs_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkGameGenie){chkGameGenie_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkInstructions){chkInstructions_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkCartridge){chkCartridge_VwXEvOnCheckBox(event,-1);return;}
 if(m_wxWin==chkLibraryDefault){chkLibraryDefault_VwXEvOnCheckBox(event,-1);return;}
 event.Skip(true);
}
void MyDlg::VwXVwXEvOnButtonClick(wxCommandEvent& event){
 wxObject *m_wxWin = event.m_eventObject ;
 if(m_wxWin==buttonROMs){buttonROMs_VwXEvOnButtonClick(event,-1);return;}
 if(m_wxWin==buttonStart){buttonStart_VwXEvOnButtonClick(event,-1);return;}
 event.Skip(true);
}

//[evtFunc]add your code here

void MyDlg::txtROMs_VwXEvOnKillFocus(wxFocusEvent& event,int index){ //init function
 //[5fb]Code event VwX...Don't modify[5fb]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::txtMasterLibrary_VwXEvOnKillFocus(wxFocusEvent& event,int index){ //init function
 //[5fd]Code event VwX...Don't modify[5fd]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::txtROMs_VwXEvOnTextEnter(wxCommandEvent& event,int index){ //init function
 //[194]Code event VwX...Don't modify[194]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::txtMasterLibrary_VwXEvOnTextEnter(wxCommandEvent& event,int index){ //init function
 //[195]Code event VwX...Don't modify[195]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::EnableConfigControls(bool b)
{
    txtROMs->Enable(b);
    txtMasterLibrary->Enable(b);
    chkCartridge->Enable(b);
    chkInstructions->Enable(b);
    chkFAQs->Enable(b);
    chkTitles->Enable(b);
    chkScreens->Enable(b);
    chkBoxes->Enable(b);
    chkNSFs->Enable(b);
    chkGameGenie->Enable(b);
    dropImageFormat->Enable(b);
    buttonROMs->Enable(b);
    chkLibraryDefault->Enable(b);
}

void MyDlg::OnBuildComplete()
{
    buttonStart->Enable(true);
    buttonStart->SetLabel("Start Build");
    EnableConfigControls(true);
}

void MyDlg::buttonStart_VwXEvOnButtonClick(wxCommandEvent& event,int index){ //init function
 //[2e8]Code event VwX...Don't modify[2e8]//
 //add your code here
    if (pBuildThread)
    {
        if (pBuildThread->IsRunning())
        {
            buttonStart->SetLabel("Stopping Build");
            buttonStart->Enable(false);
            pBuildThread->Delete();
            OnBuildComplete();
            txtOutput->AppendText("BUILD PROCESS ABORTED BY USER!\n\n");
            return;
        }
        delete pBuildThread;
    }
    txtOutput->SetValue("");
    EnableConfigControls(false);
    pBuildThread = new CDBuildThread("buildcfg.xml", txtOutput, this);
    pBuildThread->Create();
    pBuildThread->Run();
    buttonStart->SetLabel("Stop Build");
    
} //end function

void MyDlg::chkTitles_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[18e]Code event VwX...Don't modify[18e]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkScreens_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[18f]Code event VwX...Don't modify[18f]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkNSFs_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[191]Code event VwX...Don't modify[191]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkInstructions_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[18c]Code event VwX...Don't modify[18c]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkGameGenie_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[192]Code event VwX...Don't modify[192]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkFAQs_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[18d]Code event VwX...Don't modify[18d]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkCartridge_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[18b]Code event VwX...Don't modify[18b]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::chkBoxes_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[190]Code event VwX...Don't modify[190]//
 //add your code here
    UpdateCFG();
} //end function

void MyDlg::dropImageFormat_VwXEvOnChoiceSelect(wxCommandEvent& event,int index){ //init function
 //[2ac]Code event VwX...Don't modify[2ac]//
 //add your code here
    UpdateCFG();
} //end function

wxString defaultLibraryURL = "http://www.sch3rz0.com/nesterdcse/master_library";

void MyDlg::chkLibraryDefault_VwXEvOnCheckBox(wxCommandEvent& event,int index){ //init function
 //[53c]Code event VwX...Don't modify[53c]//
 //add your code here
    if (chkLibraryDefault->IsChecked())
    {
        txtMasterLibrary->SetValue(defaultLibraryURL);
        txtMasterLibrary->SetEditable(false);
    }
    else
    {
        txtMasterLibrary->SetValue("");
        txtMasterLibrary->SetEditable(true);
    }
    UpdateCFG();
} //end function

void MyDlg::buttonROMs_VwXEvOnButtonClick(wxCommandEvent& event,int index){ //init function
 //[ a8]Code event VwX...Don't modify[ a8]//
 //add your code here
    wxDirDialog dlg(this);
    dlg.SetPath(txtROMs->GetValue());
    if (dlg.ShowModal() == wxID_OK)
    {
        txtROMs->SetValue(wxFileName(dlg.GetPath()).GetFullPath());
        UpdateCFG();
    }
} //end function

void MyDlg::VwXEvOnInitDialog(wxInitDialogEvent& event){ //init function
 //[7aa]Code event VwX...Don't modify[7aa]//
 //add your code here

} //end function

void MyDlg::VwXEvOnEraseBackground(wxEraseEvent& event){ //init function
 //[6eb]Code event VwX...Don't modify[6eb]//
 //add your code here

} //end function

void MyDlg::initBefore(){
 //add your code here
    pBuildThread = NULL;
}

void MyDlg::initAfter(){
    //add your code here
    ReadCFG();
    dropImageFormat->SetSelection(0);
    Centre();
}

void MyDlg::OnPreCreate(){
 //add your code here

}

void MyDlg::DMyDlg(){
 //add your code here

}

static volatile bool inUpdateCFG = false;

void MyDlg::UpdateCFG()
{
    if (inUpdateCFG)
        return;
    inUpdateCFG = true;
    XML* root = XML_OpenFile("buildcfg.xml");
    if (!root)
        return;
    XML_SetAttribute(root, "roms_path", txtROMs->GetValue());
    XML_SetAttribute(root, "library_url", txtMasterLibrary->GetValue());
    char sztemp[0x10];
    sprintf(sztemp, "%d", dropImageFormat->GetSelection());
    XML_SetAttribute(root, "image_format", sztemp);
    
    //Features
    XML_SetAttribute(root, "get_carts", chkCartridge->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_manuals", chkInstructions->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_faqs", chkFAQs->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_titles", chkTitles->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_screenshots", chkScreens->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_boxes", chkBoxes->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_nsfs", chkNSFs->IsChecked()?"true":"false");
    XML_SetAttribute(root, "get_gamegenie", chkGameGenie->IsChecked()?"true":"false");
    
    wxString strdoc = XML_Serialize(root);
    //wxMessageDialog dlg(this, strdoc); dlg.ShowModal();
    FILE* fp = fopen("buildcfg.xml", "wb");
    if (fp)
    {
        fprintf(fp, strdoc.c_str());
        fclose(fp);
    }
    XML_Destroy(root);
    inUpdateCFG = false;
}

void MyDlg::ReadCFG()
{
    XML* root = XML_OpenFile("buildcfg.xml");
    if (!root)
        return;
    const char* sztemp;
    if (sztemp = XML_GetAttribute(root, "roms_path"))
        txtROMs->SetValue(sztemp);
    //defaultLibraryURL
    if (sztemp = XML_GetAttribute(root, "library_url"))
    {
        txtMasterLibrary->SetValue(sztemp);
        chkLibraryDefault->SetValue(defaultLibraryURL == sztemp);
        txtMasterLibrary->SetEditable(defaultLibraryURL != sztemp);
    }
    if (sztemp = XML_GetAttribute(root, "image_format"))
    {
        unsigned long val;
        if (wxString(sztemp).ToULong(&val))
            dropImageFormat->SetSelection(val);
    }
    if (sztemp = XML_GetAttribute(root, "get_carts"))
        chkCartridge->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_manuals"))
        chkInstructions->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_faqs"))
        chkFAQs->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_titles"))
        chkTitles->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_screenshots"))
        chkScreens->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_boxes"))
        chkBoxes->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_nsfs"))
        chkNSFs->SetValue((wxString(sztemp)=="true")?true:false);
    if (sztemp = XML_GetAttribute(root, "get_gamegenie"))
        chkGameGenie->SetValue((wxString(sztemp)=="true")?true:false);
    
    XML_Destroy(root);
}

//[evtFunc]end your code
