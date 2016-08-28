#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/minifram.h>
#include <iostream>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeCommandBarFrame.hpp"
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeMiniFrame.hpp"

#include "FormIcon.xpm"

#ifdef WIN32
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"rpcrt4.lib")
#endif

enum MyControlID{
	IDM_ABOUT=wxID_ABOUT,
	IDM_EXIT=wxID_EXIT,
	IDM_NEW=wxID_NEW,
	IDM_OPEN=wxID_OPEN,
	IDM_SAVE=wxID_SAVE,
	IDM_SAVEAS=wxID_SAVEAS,
	IDM_CUT=wxID_CUT,
	IDM_COPY=wxID_COPY,
	IDM_PASTE=wxID_PASTE,
	IDM_DELETE=wxID_DELETE,
	IDM_SELECTALL=wxID_SELECTALL,

	IDM_LOWEST=wxID_HIGHEST,

	IDM_FORMAT,
	IDM_BOLD,
	IDM_ITALIC,
	IDM_UNDERLINE,
	IDM_LEFT,
	IDM_CENTER,
	IDM_RIGHT,
	IDM_COLOR,
};

#define MyPopupWindow wxFakeMenuPopupWindow

class MyFrame:public wxFakeCommandBarFrame{
private:
public:
	MyFrame(const wxString& s):wxFakeCommandBarFrame(NULL,wxID_ANY,s,wxDefaultPosition,wxSize(640,480)){
		//set icon
		SetIcon(wxIcon(FormIcon_xpm));

		//load bitmap
		wxBitmap bmIcon;
		wxImage::AddHandler(new wxPNGHandler);
		if(!bmIcon.LoadFile("icons.png",wxBITMAP_TYPE_PNG)){
			if(!bmIcon.LoadFile("../icons.png",wxBITMAP_TYPE_PNG)){
				std::cout<<"Error: can't load toolbar bitmap 'Strip-32a.png'\n";
				bmIcon.Create(64,64,32);
			}
		}

		//command bar test
		wxFakeCommandBarRefData *cb;

		AddButton(wxID_SEPARATOR)->Type(fbttSeparator).Flags(fbtfStartNewRow);

		cb=AddBarData(101);
		cb->Caption("Recent Files").Flags(fcbfDragToMakeThisMenuFloat|fcbfMultiLineToolBar);
		cb->Buttons().push_back(&AddButton()->Caption("&1 1.txt"));
		cb->Buttons().push_back(&AddButton()->Caption("&2 2.txt").Enabled(false));
		cb->Buttons().push_back(&AddButton()->Caption("&3 3.txt"));
		cb->Buttons().push_back(&AddButton()->Caption("&4 4.txt"));

		cb=AddBarData(wxID_FILE);
		cb->Caption("File").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(&AddButton(IDM_NEW)
			->Caption("&New\tCtrl+N").ToolTipText("New")
			.Bitmap(wxArtProvider::GetBitmap(wxART_NEW))
			.Description("Create a new document.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_OPEN)
			->Caption("&Open\tCtrl+O").ToolTipText("Open")
			.Bitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN))
			.Description("Open a file.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton(IDM_SAVE)
			->Caption("&Save\tCtrl+S").ToolTipText("Save")
			.Bitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE))
			.Description("Save current file.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_SAVEAS)
			->Caption("Save &As\tF12").ToolTipText("Save as")
			.Bitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS))
			.Description("Save current file to another file.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton()->Caption("&Recent files").SubMenu(FindBarData(101)).Flags(fbtfShowDropdown));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton(IDM_EXIT)
			->Caption("E&xit\tAlt+F4").ToolTipText("Exit")
			.Bitmap(wxArtProvider::GetBitmap(wxART_QUIT))
			.Description("Exit the program.")
			.Flags(fbtfHideCationInToolBar));

		unsigned int clrs[8][5]={
			0x0U, 0x80U, 0xFFU, 0xFF00FFU, 0xCC99FFU,
			0x3399U, 0x66FFU, 0x99FFU, 0xCCFFU, 0x99CCFFU,
			0x3333U, 0x8080U, 0xCC99U, 0xFFFFU, 0x99FFFFU,
			0x3300U, 0x8000U, 0x669933U, 0xFF00U, 0xCCFFCCU,
			0x663300U, 0x808000U, 0xCCCC33U, 0xFFFF00U, 0xFFFFCCU,
			0x800000U, 0xFF0000U, 0xFF6633U, 0xFFCC00U, 0xFFCC99U,
			0x993333U, 0x996666U, 0x800080U, 0x663399U, 0xFF99CCU,
			0x333333U, 0x808080U, 0x999999U, 0xC0C0C0U, 0xFFFFFF
		};

		cb=AddBarData(IDM_COLOR);
		cb->Caption("Color").Flags(fcbfDragToMakeThisMenuFloat|fcbfToolBarMode|fcbfMultiLineToolBar);
		cb->Buttons().push_back(&AddButton()->Caption("Automatic").Type(fbttOption).Bitmap(wxColour(0UL))
			.Flags(fbtfFullRow|fbtfAlignCenter));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		for(int i=0;i<5;i++){
			for(int j=0;j<8;j++){
				cb->Buttons().push_back(&AddButton()->Type(fbttOption).Bitmap(wxColour(clrs[j][i]))
					.Flags(fbtfHideCationInToolBar|(j==0?fbtfStartNewRow:0)));
			}
		}
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton()->Caption("Other colors...").Type(fbttOption)
			.Flags(fbtfFullRow|fbtfAlignCenter));

		cb=AddBarData(IDM_FORMAT);
		cb->Caption("Format").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(&AddButton(IDM_BOLD)
			->Caption("&Bold\tCtrl+B").ToolTipText("Bold").Type(fbttCheck).Bitmap(bmIcon.GetSubBitmap(wxRect(0,16,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_ITALIC)
			->Caption("&Italic\tCtrl+I").ToolTipText("Italic").Type(fbttCheck).Bitmap(bmIcon.GetSubBitmap(wxRect(16,16,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_UNDERLINE)
			->Caption("&Underline").ToolTipText("Underline").Type(fbttCheck).Bitmap(bmIcon.GetSubBitmap(wxRect(32,16,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton(IDM_LEFT)
			->Caption("Align &Left").ToolTipText("Align left").Type(fbttOptionNullable).Bitmap(bmIcon.GetSubBitmap(wxRect(0,0,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_CENTER)
			->Caption("Align &Center").ToolTipText("Align center").Type(fbttOptionNullable).Bitmap(bmIcon.GetSubBitmap(wxRect(16,0,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_RIGHT)
			->Caption("Align &Right").ToolTipText("Align right").Type(fbttOptionNullable).Bitmap(bmIcon.GetSubBitmap(wxRect(32,0,16,16)))
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton()
			->Caption("Color").ToolTipText("Color").Type(fbttSplit).Bitmap(bmIcon.GetSubBitmap(wxRect(0,32,16,16)))
			.Flags(fbtfHideCationInToolBar).SubMenu(FindBarData(IDM_COLOR)));

		cb=AddBarData(wxID_EDIT);
		cb->Caption("Edit").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(&AddButton(IDM_CUT)
			->Caption("Cu&t\tCtrl+X").ToolTipText("Cut")
			.Bitmap(wxArtProvider::GetBitmap(wxART_CUT))
			.Description("Cut the selection to clipboard.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_COPY)
			->Caption("&Copy\tCtrl+C").ToolTipText("Copy")
			.Bitmap(wxArtProvider::GetBitmap(wxART_COPY))
			.Description("Copy the selection to clipboard.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_PASTE)
			->Caption("&Paste\tCtrl+V").ToolTipText("Paste")
			.Bitmap(wxArtProvider::GetBitmap(wxART_PASTE))
			.Description("Paste the selection from clipboard.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(&AddButton(IDM_DELETE)
			->Caption("&Delete\tDel").ToolTipText("Delete")
			.Bitmap(wxArtProvider::GetBitmap(wxART_DELETE))
			.Description("Delete the selected objects.")
			.Flags(fbtfHideCationInToolBar));
		cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
		cb->Buttons().push_back(&AddButton(IDM_SELECTALL)
			->Caption("Select &All\tCtrl+A").ToolTipText("Select all")
			.Description("Select all objects."));

		cb=AddBarData(wxID_HELP);
		cb->Caption("Help").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(&AddButton(IDM_ABOUT)
			->Caption("&About\tF1").ToolTipText("About")
			.Bitmap(wxArtProvider::GetBitmap(wxART_INFORMATION))
			.Description("About the program.")
			.Flags(fbtfHideCationInToolBar));

		cb=AddBarData(102);
		cb->Caption("Save").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(FindButton(IDM_SAVE));
		cb->Buttons().push_back(FindButton(IDM_SAVEAS));

		cb=AddBarData(1);
		cb->Caption("Main Menu").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(&AddButton(wxID_FILE)->Caption("&File").SubMenu(FindBarData(wxID_FILE)));
		cb->Buttons().push_back(&AddButton(wxID_EDIT)->Caption("&Edit").SubMenu(FindBarData(wxID_EDIT)));
		cb->Buttons().push_back(&AddButton(IDM_FORMAT)->Caption("F&ormat").SubMenu(FindBarData(IDM_FORMAT)));
		cb->Buttons().push_back(&AddButton(wxID_HELP)->Caption("&Help").SubMenu(FindBarData(wxID_HELP)));

		cb=AddBarData(2);
		cb->Caption("Main Toolbar").Flags(fcbfDragToMakeThisMenuFloat);
		cb->Buttons().push_back(FindButton(IDM_NEW));
		cb->Buttons().push_back(AddButton(&((new wxFakeButtonData(*FindButton(IDM_OPEN)))
			->Type(fbttSplit).Shortcut(wxEmptyString)
			.SubMenu(FindBarData(101))
			)));
		cb->Buttons().push_back(AddButton(&((new wxFakeButtonData(*FindButton(IDM_SAVE)))
			->Type(fbttSplit).Shortcut(wxEmptyString)
			.SubMenu(FindBarData(102))
			)));
		for(int i=0;i<1;i++){
			cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
			cb->Buttons().push_back(FindButton(IDM_CUT));
			cb->Buttons().push_back(FindButton(IDM_COPY));
			cb->Buttons().push_back(FindButton(IDM_PASTE));
			cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
			cb->Buttons().push_back(FindButton(IDM_DELETE));
			cb->Buttons().push_back(FindButton(wxID_SEPARATOR));
			cb->Buttons().push_back(FindButton(IDM_ABOUT));
		}

		/*//set menu
		wxMenu *mnu1;
		wxMenu *mnuFile=new wxMenu,*mnuEdit=new wxMenu,*mnuHelp=new wxMenu;

		mnuFile->Append(IDM_NEW,"&New\tCtrl+N","Create a new document.");
		mnuFile->Append(IDM_OPEN,"&Open\tCtrl+O","Open a file.");
		mnuFile->AppendSeparator();
		mnuFile->Append(IDM_SAVE,"&Save\tCtrl+S","Save current file.");
		mnuFile->Append(IDM_SAVEAS,"Save &As\tF12","Save current file to another file.");
		mnuFile->AppendSeparator();
		mnuFile->Append(wxID_ANY,"&1 1.txt");
		mnuFile->Append(wxID_ANY,"&2 2.txt");
		mnuFile->Append(wxID_ANY,"&3 3.txt");
		mnuFile->Append(wxID_ANY,"&4 4.txt");
		mnuFile->AppendSeparator();
		mnuFile->Append(IDM_EXIT,"E&xit\tAlt+F4","Exit the program.");

		mnuEdit->Append(IDM_CUT,"Cu&t\tCtrl+X","Cut the selection to clipboard.");
		mnuEdit->Append(IDM_COPY,"&Copy\tCtrl+C","Copy the selection to clipboard.");
		mnuEdit->Append(IDM_PASTE,"&Paste\tCtrl+V","Paste the selection from clipboard.");
		mnuEdit->Append(IDM_DELETE,"&Delete\tDel","Delete the selected objects.");
		mnuEdit->AppendSeparator();
		mnuEdit->Append(IDM_SELECTALL,"Select &All\tCtrl+A","Select all objects.");

		mnuHelp->Append(IDM_ABOUT,"&About\tF1","About the program.");

		wxMenuBar *mnu=new wxMenuBar();
		mnu->Append(mnuFile,"&File");
		mnu->Append(mnuEdit,"&Edit");
		mnu->Append(mnuHelp,"&Help");

		SetMenuBar(mnu);

		//set toolbar
		wxToolBar *tb1=CreateToolBar();
		tb1->AddTool(IDM_NEW,"",wxArtProvider::GetBitmap(wxART_NEW,wxART_MENU),wxNullBitmap,wxITEM_NORMAL,"New","Create a new document.",NULL);
		tb1->AddTool(IDM_OPEN,"",wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_MENU),wxNullBitmap,wxITEM_DROPDOWN,"Open","Open a file.",NULL);
		mnu1=new wxMenu;
		mnu1->Append(wxID_ANY,"&1 1.txt");
		mnu1->Append(wxID_ANY,"&2 2.txt");
		mnu1->Append(wxID_ANY,"&3 3.txt");
		mnu1->Append(wxID_ANY,"&4 4.txt");
		tb1->SetDropdownMenu(IDM_OPEN,mnu1);
		tb1->AddTool(IDM_SAVE,"",wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_MENU),wxNullBitmap,wxITEM_DROPDOWN,"Save","Save current file.",NULL);
		mnu1=new wxMenu;
		mnu1->Append(IDM_SAVE,"&Save\tCtrl+S","Save current file.");
		mnu1->Append(IDM_SAVEAS,"Save &As\tF12","Save current file to another file.");
		tb1->SetDropdownMenu(IDM_SAVE,mnu1);
		tb1->AddSeparator();
		tb1->AddTool(IDM_CUT,"",wxArtProvider::GetBitmap(wxART_CUT,wxART_MENU),wxNullBitmap,wxITEM_NORMAL,"Cut","Cut the selection to clipboard.",NULL);
		tb1->AddTool(IDM_COPY,"",wxArtProvider::GetBitmap(wxART_COPY,wxART_MENU),wxNullBitmap,wxITEM_NORMAL,"Copy","Copy the selection to clipboard.",NULL);
		tb1->AddTool(IDM_PASTE,"",wxArtProvider::GetBitmap(wxART_PASTE,wxART_MENU),wxNullBitmap,wxITEM_NORMAL,"Paste","Paste the selection from clipboard.",NULL);
		tb1->AddSeparator();
		tb1->AddTool(IDM_DELETE,"Delete",wxArtProvider::GetBitmap(wxART_DELETE,wxART_MENU),wxNullBitmap,wxITEM_NORMAL,"Delete","Delete the selected objects.",NULL);
		tb1->Realize();

		//set status bar
		wxStatusBar *sb1=CreateStatusBar(2);
		SetStatusText("Ready");*/

		//add command bar
		AddDockableCommandBar(1,0,-1,-1,fdcbMainMenu|fdcbAlwaysDocked);
		AddDockableCommandBar(2,0,-1,-1);
		AddDockableCommandBar(wxID_FILE,fcbdHidden);
		AddDockableCommandBar(wxID_EDIT,fcbdHidden);
		AddDockableCommandBar(IDM_FORMAT,fcbdHidden);
		AddDockableCommandBar(wxID_HELP,fcbdHidden);
		AddDockableCommandBar(101,fcbdHidden);
		AddDockableCommandBar(102,fcbdHidden);
		AddDockableCommandBar(IDM_COLOR,fcbdHidden,-1,-1,fdcbClosable|fdcbAlwaysFloat);

		//add dockable panes
		AddDockablePane(new wxButton(this,wxID_ANY,"Command1"),wxEmptyString,NULL,0,0,0);
		AddDockablePane(NULL,wxEmptyString,NULL,0,0,0);
		AddDockablePane(new wxButton(this,wxID_ANY,"Command2"),wxEmptyString,NULL,0,0,0);
		AddDockablePane(NULL,wxEmptyString,NULL,0,fcbdLeft,0);
		AddDockablePane(new wxButton(this,wxID_ANY,"Command3"),wxEmptyString,NULL,0,fcbdLeft,0);
		AddDockablePane(NULL,wxEmptyString,NULL,0,fcbdRight,0);
		AddDockablePane(new wxButton(this,wxID_ANY,"Command4"),wxEmptyString,NULL,0,fcbdRight,0);
		AddDockablePane(NULL,wxEmptyString,NULL,0,fcbdBottom,0);
		AddDockablePane(new wxButton(this,wxID_ANY,"Command5"),wxEmptyString,NULL,0,fcbdBottom,0);
		/*AddDockablePane(NULL,"Command1",NULL,0,0,0);
		AddDockablePane(NULL,"Command2",NULL,0,0,0);
		AddDockablePane(NULL,"Command3",NULL,0,0,0);
		AddDockablePane(NULL,"Command4",NULL,0,fcbdLeft,0);*/
		SetClientWindow(new wxButton(this,wxID_ANY,"Command6"));

		Layout();
		UnDirtyVisible();
	}
private:
	void OnExit(wxCommandEvent& event){
		Close(true);
	}
	void OnAbout(wxCommandEvent& event){
		wxMessageBox("Hello, world!","Project1");
	}
	void mnuNew_Click(wxCommandEvent& event){
		//wxMessageBox("mnuNew_Click","Project1");
		//it works, too
		wxFakeMiniFrame *test=new wxFakeMiniFrame(this,wxID_ANY,"TEST ONLY",wxPoint(32,32),wxSize(192,128),
			fpstCloseBox|fpstMinimizeBox|fpstPinBox|fpstCustomBox|fpstDropdownBox|fpstSizable);
		wxButton *btn=new wxButton(test,wxID_ANY,"Command1");
		test->SetClientWindow(btn);
		test->Show();
	}
	void mnuOpen_Click(wxCommandEvent& event){
		wxMessageBox("mnuOpen_Click","Project1");
	}
	void mnuSave_Click(wxCommandEvent& event){
		wxMessageBox("mnuSave_Click","Project1");
	}
	void mnuSaveAs_Click(wxCommandEvent& event){
		wxMessageBox("mnuSaveAs_Click","Project1");
	}
	void OnContextMenu(wxContextMenuEvent& event){
		//PopupMenuEx(1);
		event.Skip();
	}
private:
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame,wxFakeCommandBarFrame)

EVT_MENU(IDM_EXIT,MyFrame::OnExit)
EVT_MENU(IDM_ABOUT,MyFrame::OnAbout)
EVT_MENU(IDM_NEW,MyFrame::mnuNew_Click)
EVT_MENU(IDM_OPEN,MyFrame::mnuOpen_Click)
EVT_MENU(IDM_SAVE,MyFrame::mnuSave_Click)
EVT_MENU(IDM_SAVEAS,MyFrame::mnuSaveAs_Click)

EVT_CONTEXT_MENU(MyFrame::OnContextMenu)

END_EVENT_TABLE()

wxFrame *g_frame;

class MyApp:public wxApp{
public:
	virtual bool OnInit(){
		if(!wxApp::OnInit()) return false;

		wxFakeCommandBarDefaultArtProvider=new wxFakeOffice2003CommandBarArt();

		g_frame=new MyFrame("Form1");
		g_frame->Show(true);

		return true;
	}

	virtual int OnExit(){
		delete wxFakeCommandBarDefaultArtProvider;
		wxFakeCommandBarDefaultArtProvider=NULL;

		return wxApp::OnExit();
	}
};

IMPLEMENT_APP(MyApp)

#ifdef __WXMSW__
int main(){
	return WinMain(GetModuleHandleW(NULL),NULL,NULL,SW_SHOW);
}
#endif
