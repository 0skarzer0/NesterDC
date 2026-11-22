
// Don't modify comment 
#ifndef __VwX_MyDlg_H__
#define __VwX_MyDlg_H__

 #include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
 
//[inc]add your include files here

#include "cdbuildthread.h"

//[inc]end your include
 
class MyDlg;
class MyDlgEvt:public wxEvtHandler
{
public:
 MyDlgEvt(MyDlg *parent){ptr_winPan=parent;}
protected:  
 MyDlg *ptr_winPan;
 DECLARE_EVENT_TABLE()     
 void VwXEvOnKillFocus(wxFocusEvent& event);
//[evt]add your code here



//[evt]end your code
};

class MyDlg:public wxDialog
{
 DECLARE_CLASS(MyDlg)
 friend class MyDlgEvt;
public:
 MyDlg();
 MyDlg(wxWindow* parent, wxWindowID id = -1, const wxString& title = wxT(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_NO_PARENT|wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX, const wxString& name = wxT("dialogBox"));
// Pointer control
 wxStaticBitmap *bitmapTitle;
 wxStaticText *st4c;
 wxStaticText *st5c;
 wxStaticText *st6c;
 wxStaticText *st7c;
 wxTextCtrl *txtROMs;
 wxTextCtrl *txtMasterLibrary;
 wxCheckBox *chkCartridge;
 wxCheckBox *chkInstructions;
 wxCheckBox *chkFAQs;
 wxCheckBox *chkTitles;
 wxCheckBox *chkScreens;
 wxCheckBox *chkBoxes;
 wxCheckBox *chkNSFs;
 wxCheckBox *chkGameGenie;
 wxChoice *dropImageFormat;
 wxTextCtrl *txtOutput;
 wxButton *buttonStart;
 wxButton *buttonROMs;
 wxCheckBox *chkLibraryDefault;
 virtual ~MyDlg();
 void DMyDlg();
 void initBefore();
 void OnPreCreate();
 void VwXinit();
 wxBitmap *bitmapTitleImg0;
 wxBitmap fileImgBuf[1];
 void initAfter();
protected:
 void VwXVwXEvOnButtonClick(wxCommandEvent& event);
 void VwXVwXEvOnCheckBox(wxCommandEvent& event);
 void VwXVwXEvOnChoiceSelect(wxCommandEvent& event);
 void VwXVwXEvOnEraseBackground(wxEraseEvent& event);
 void VwXVwXEvOnInitDialog(wxInitDialogEvent& event);
 void VwXVwXEvOnKillFocus(wxFocusEvent& event);
 void VwXVwXEvOnTextEnter(wxCommandEvent& event);
 void VwXEvOnInitDialog(wxInitDialogEvent& event);
 void VwXEvOnEraseBackground(wxEraseEvent& event);
 void txtROMs_VwXEvOnTextEnter(wxCommandEvent& event,int index=-1);
 void txtROMs_VwXEvOnKillFocus(wxFocusEvent& event,int index=-1);
 void txtMasterLibrary_VwXEvOnTextEnter(wxCommandEvent& event,int index=-1);
 void txtMasterLibrary_VwXEvOnKillFocus(wxFocusEvent& event,int index=-1);
 void dropImageFormat_VwXEvOnChoiceSelect(wxCommandEvent& event,int index=-1);
 void chkTitles_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkScreens_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkNSFs_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkLibraryDefault_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkInstructions_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkGameGenie_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkFAQs_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkCartridge_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void chkBoxes_VwXEvOnCheckBox(wxCommandEvent& event,int index=-1);
 void buttonStart_VwXEvOnButtonClick(wxCommandEvent& event,int index=-1);
 void buttonROMs_VwXEvOnButtonClick(wxCommandEvent& event,int index=-1);
 DECLARE_EVENT_TABLE()  
//[win]add your code here

public:
    void EnableConfigControls(bool b);

    CDBuildThread* pBuildThread;

    void UpdateCFG();
    void ReadCFG();
    
    void OnBuildComplete();

 #define VwX_INIT_OBJECTS_MyDlg
//[win]end your code 
};
// end MyDlg

#endif
