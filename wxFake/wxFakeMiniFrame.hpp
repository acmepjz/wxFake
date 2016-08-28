#ifndef _WXFAKE_WXFAKEMINIFRAME_HPP_
#define _WXFAKE_WXFAKEMINIFRAME_HPP_

//wxMiniFrame in GTK has annoying border
#include <wx/frame.h>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/timer.h>

class wxFakeCommandBarArtBase;

enum wxFakeMiniFrameStyle{
	fpstCloseBox=0x1,
	fpstMinimizeBox=0x2,
	fpstPinBox=0x4,
	fpstCustomBox=0x8,
	fpstDropdownBox=0x10,

	fpstSizable=0x100,
	fpstToolbarMode=0x200,
	fpstGradientBackground=0x400,
	fpstDocked=0x800,
	fpstMinimized=0x1000,
	fpstAutoHide=0x2000,
	fpstHidden=0x4000,

	fpstTemporaryDisableDock=0x10000,
};

enum wxFakeMiniFrameHitTestItem{
	fphtClose=0,
	fphtMinimize,
	fphtPin,
	fphtCustom,
	fphtDropdown,
	fphtMoveLeft,
	fphtMoveRight,

	fphtGripper=0xFF,
	fphtSizeNW=0x1FF,
	fphtSizeN=0x2FF,
	fphtSizeNE=0x3FF,
	fphtSizeW=0x4FF,
	fphtSizeE=0x5FF,
	fphtSizeSW=0x6FF,
	fphtSizeS=0x7FF,
	fphtSizeSE=0x8FF,

	fphtNone=-1,
};

class wxFakeCommandBarDataBase;
class wxFakeMenuPopupWindow;
class wxFakeCommandBarFrame;

class wxFakeMiniFrame:public wxFrame{
public:
	friend class wxFakeMenuPopupWindow;
	friend class wxFakeCommandBarFrame;
protected:
	wxFakeCommandBarArtBase* m_artprov;
	wxTimer *m_timer;

	int m_fake_style;

	wxFakeCommandBarFrame *m_dockmgr;
	wxPoint m_drag_offset;

	int m_highlight_idx;
	int m_pressed_idx;
	int m_submenu_idx;

	wxFakeMenuPopupWindow *m_submenu;

	int m_old_height;
	wxWindow* m_clientwnd;

	void Init();
public:
	wxFakeMiniFrame(){
		Init();
	}
	wxFakeMiniFrame(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &title=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxFrameNameStr){
		Init();
		Create(parent,id,title,pos,size,style,name);
	}
	~wxFakeMiniFrame();

	bool Create(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &title=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxFrameNameStr);

	wxFakeCommandBarArtBase* GetArtProvider() const{
		return m_artprov;
	}
	void SetArtProvider(wxFakeCommandBarArtBase* artprov);

	wxFakeCommandBarFrame* DockManager() const{
		return m_dockmgr;
	}
	void DockManager(wxFakeCommandBarFrame* mgr){
		m_dockmgr=mgr;
	}

	wxWindow* GetClientWindow() const{
		return m_clientwnd;
	}
	void SetClientWindow(wxWindow* wnd);

	int GetStyle() const{
		return m_fake_style;
	}
	void SetStyle(int style);

	virtual bool Layout();

	virtual bool Destroy();

	bool Redraw(wxDC& dc);
protected:
	void OnPaint(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnTimer(wxTimerEvent& event);

	virtual void OnSubMenuDestroy();

	virtual void DoSetClientSize(int width,int height);

	//hack for wxGTK
#ifdef __WXGTK__
	wxTimer *m_stupid_timer;
	wxSize m_stupid_size;

	void OnStupidTimer(wxTimerEvent& event);
	virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
#endif
private:
	DECLARE_CLASS(wxFakeMiniFrame)
	DECLARE_EVENT_TABLE()
};

#endif
