#ifndef _WXFAKE_WXFAKECOMMANDBAR_HPP_
#define _WXFAKE_WXFAKECOMMANDBAR_HPP_

#include <wx/panel.h>
#include <wx/defs.h>
#include <wx/vector.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/timer.h>
#include "wxFake/wxFakeMenu.hpp"

class wxFakeCommandBarArtBase;

class wxFakeButtonData;
class wxFakeCommandBarDataBase;
class wxFakeCommandBarRefData;

struct wxFakeCommandBarButtonState{
	wxRect rect;
	int add_flags;
};

enum wxFakeCommandBarDockDirection{
	fcbdTop=0,
	fcbdBottom=1,
	fcbdLeft=2,
	fcbdRight=3,

	fcbdUndocked=4,
	fcbdHidden=5,
};

class wxFakeMenuPopupWindow;
class wxFakeCommandBarFrame;
class wxFakeMiniFrame;

class wxFakeCommandBar:public wxPanel{
public:
	static const int m_gripper_idx=0x7FFFFFFE;
	static const int m_chevron_idx=0x7FFFFFFF;
public:
	friend class wxFakeMenuPopupWindow;
	friend class wxFakeCommandBarFrame;
	friend class wxFakeMiniFrame;
protected:
	wxFakeCommandBarDataBase* m_bar;
	wxFakeCommandBarArtBase* m_artprov;
	wxVector<wxFakeCommandBarButtonState> m_btn;
	wxTimer *m_timer;

	wxDC *m_hdc;

	int m_pressed_idx;
	int m_submenu_idx;
	wxLongLong m_submenu_time;
	wxFakeMenuPopupWindow *m_submenu;

	wxFakeCommandBarFrame *m_dockmgr;

	int m_direction;
	int m_gripper_add_flags;
	int m_chevron_add_flags;

	bool m_mainmenu;
	bool m_dockable:1;
	bool m_undocked:1;
	bool m_update_on_next_idle:1;
	bool m_overflow;
	bool m_chevron;

	wxSize m_fullsize;

	wxFakeCommandBarRefData* m_chevronmenu;
	wxFakeButtonData *m_separator,*m_add_or_remove_btns;

	void Init();
public:
	wxFakeCommandBar(){
		Init();
	}
	wxFakeCommandBar(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxBORDER_NONE, const wxString &name=wxEmptyString){
		Init();
		Create(parent,id,pos,size,style,name);
	}

	bool Create(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxBORDER_NONE, const wxString &name=wxEmptyString);

	~wxFakeCommandBar();

	wxFakeCommandBarDataBase* GetBar() const{
		return m_bar;
	}
	void SetBar(wxFakeCommandBarDataBase* bar);

	wxFakeCommandBarArtBase* GetArtProvider() const{
		return m_artprov;
	}
	void SetArtProvider(wxFakeCommandBarArtBase* artprov);

	bool IsMainMenu() const{
		return m_mainmenu;
	}
	void IsMainMenu(bool b);

	bool Dockable() const{
		return m_dockable;
	}
	void Dockable(bool b);

	bool Undocked() const{
		return m_undocked;
	}
	void Undocked(bool b);

	wxFakeCommandBarFrame* DockManager() const{
		return m_dockmgr;
	}
	void DockManager(wxFakeCommandBarFrame* mgr){
		m_dockmgr=mgr;
	}

	bool ShowChevron() const{
		return m_chevron;
	}
	void ShowChevron(bool b);

	bool ShowOverflow() const{
		return m_overflow;
	}

	wxSize FullSize() const{
		return m_fullsize;
	}

	int Direction() const{
		return m_direction;
	}
	void Direction(int dir);

	virtual bool Layout();

	virtual bool Destroy();

	void UpdateOnNextIdle(bool update=true){
		m_update_on_next_idle=update;
	}
	void UpdateDirty(bool unDirty=true);
	void UnDirty();

	bool Redraw(wxDC& dc);
protected:
	void OnPaint(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnIdle(wxIdleEvent& event);
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	bool ForwardMouseEvent(wxMouseEvent& event);
#endif

	virtual void OnSubMenuDestroy();

	//internal function
	void PopupSubMenu(int i,const wxRect& r,wxFakeCommandBarDataBase* submenu,wxFakeCommandBarArtBase* artprov);

	void CreateChevronMenu();
	void DestroyChevronMenu();
private:
	DECLARE_CLASS(wxFakeCommandBar)
	DECLARE_EVENT_TABLE()
};

#endif
