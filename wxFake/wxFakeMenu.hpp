#ifndef _WXFAKE_WXFAKEMENU_HPP_
#define _WXFAKE_WXFAKEMENU_HPP_

#include <wx/popupwin.h>
#include <wx/defs.h>
#include <wx/vector.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/timer.h>

//if we'd like to implement fake menu
//using mouse capture (which is a bit buggy)
//#define WXFAKEMENU_USE_MOUSE_CAPTURE

//if we'd like to enable menu animation
#define WXFAKEMENU_USE_MENU_ANIMATION

#ifndef __WXMSW__

//we must use mouse capture in non-Windows platform
#ifndef WXFAKEMENU_USE_MOUSE_CAPTURE
#define WXFAKEMENU_USE_MOUSE_CAPTURE
#endif

//menu animation not avaliable for non-Windows platform
//(or is built-in feature of window manager)
#ifdef WXFAKEMENU_USE_MENU_ANIMATION
#undef WXFAKEMENU_USE_MENU_ANIMATION
#endif

#endif

struct wxFakeMenuPopupConnection{
	int dir;
	int start;
	int end;
};

class wxFakeCommandBarArtBase;

class wxFakeButtonData;
class wxFakeCommandBarDataBase;

class wxPopupWindowWithShadow:public wxPopupWindow{
public:
	wxPopupWindowWithShadow();
	wxPopupWindowWithShadow(wxWindow *parent,int flags = wxBORDER_NONE);

	~wxPopupWindowWithShadow();

	bool Create(wxWindow *parent,int flags = wxBORDER_NONE);
private:
	DECLARE_CLASS(wxPopupWindowWithShadow)
};

class wxFakeCommandBar;
class wxFakeMiniFrame;
class wxFakeCommandBarFrame;

class wxFakeMenuPopupWindow:public wxPopupWindowWithShadow{
public:
	friend class wxFakeCommandBar;
	friend class wxFakeMiniFrame;
protected:
	wxFakeMenuPopupConnection m_conn;
	wxFakeCommandBarDataBase* m_bar;
	wxFakeCommandBarArtBase* m_artprov;
	wxVector<int> m_btn_add_flags;

	wxDC *m_hdc;
	wxRect m_rcMenu;
	wxRect m_rcGripper;
	int m_gripper_add_flags;

	int m_submenu_idx;
	wxFakeMenuPopupWindow *m_submenu;
	wxFakeMenuPopupWindow *m_parentmenu;
	wxFakeCommandBar *m_parentbar;
	wxFakeMiniFrame *m_parentpane;

	wxFakeCommandBarFrame *m_dockmgr;

	void Init();
public:
	wxFakeMenuPopupWindow(){
		Init();
	}
	wxFakeMenuPopupWindow(wxWindow *parent,int flags = wxBORDER_NONE){
		Init();
		Create(parent,flags);
	}

	bool Create(wxWindow *parent,int flags = wxBORDER_NONE);

	~wxFakeMenuPopupWindow();

	wxFakeCommandBarDataBase* GetBar() const{
		return m_bar;
	}
	void SetBar(wxFakeCommandBarDataBase* bar);

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

	void PositionEx(const wxPoint& ptOrigin=wxDefaultPosition,const wxSize& size=wxDefaultSize,int dir=0,int leftMargin=0,int topMargin=0,int rightMargin=0,int bottomMargin=0);

	virtual bool Layout();
	virtual bool Show(bool show = true);

	virtual bool Destroy();
	virtual bool DestroyAll();

	bool Redraw(wxDC& dc);
protected:
	void OnPaint(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	wxTimer *m_timer;
	void OnTimer(wxTimerEvent& event);
	bool ForwardMouseEvent(wxMouseEvent& event);
#else
	void OnLostFocus(wxFocusEvent& event);
#endif

	void OnContextMenu(wxContextMenuEvent& event);

#ifdef WXFAKEMENU_USE_MENU_ANIMATION
	wxTimer *m_anim_timer;
	wxBufferedDC *m_bmback;
	int m_transparency;
	void OnAnimationTimer(wxTimerEvent& event);
#endif

	virtual void OnSubMenuDestroy();
private:
	DECLARE_CLASS(wxFakeMenuPopupWindow)
	DECLARE_EVENT_TABLE()
};

#endif
