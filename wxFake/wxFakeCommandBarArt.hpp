#ifndef _WXFAKE_WXFAKECOMMANDBARART_HPP_
#define _WXFAKE_WXFAKECOMMANDBARART_HPP_

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/window.h>

void MyDrawLabel(wxDC& dc, const wxString& text, wxRect rect, int dir = 0, int alignment = 0, int indexAccel = -1);

enum wxFakeButtonSizeMode{
	fbsmMenu=0,
	fbsmToolBar,
	fbsmToolBarStartNewRow,
};

enum wxFakeButtonHitTestState{
	fbhtNormal=0,
	fbhtSplit,
};

struct wxFakeButtonSize{
	//menu size mode (wxFakeButtonSizeMode)
	int sizeMode;
	//width in menu (sidebar part)
	int width0;
	//width in menu (main part) -1=don't care
	int width;
	//width in menu (shortcut part) -1=don't care
	int width2;
	//height in menu -1=don't care
	int height;
	//width in toolbar -1=don't care (unsupported)
	int btnWidth;
	//height in toolbar -1=don't care
	int btnHeight;
};

class wxFakeButtonData;
class wxFakeCommandBarIcon;
class wxFakeCommandBarArtBase;

struct wxFakeMenuPopupConnection;

class wxFakeCommandBarArtBase{
public:
	virtual ~wxFakeCommandBarArtBase();

	virtual wxSize GetColorSize()=0;
	virtual void GetButtonPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual void GetMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual void GetChevronMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual void GetBarClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual void GetMenuClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual wxSize GetEmptyMenuSize()=0;
	virtual void GetMenuGripperSize(int& height,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;

	virtual wxFakeButtonSize MeasureButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags)=0;

	virtual wxFakeButtonHitTestState HitTest(const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,const wxPoint& pt,const wxFakeButtonSize& btn_size,const wxRect& rect)=0;

	virtual void DrawColor(wxDC& dc,wxWindow* wnd,wxColour clr,int flags,int add_flags,const wxRect& rect)=0;

	virtual void DrawMenuBorderAndBackground(wxDC& dc,wxWindow* wnd,const wxFakeMenuPopupConnection& conn,const wxRect& rect)=0;
	virtual void DrawEmptyMenuItem(wxDC& dc,wxWindow* wnd,const wxRect& rect)=0;
	virtual void DrawMenuGripper(wxDC& dc,wxWindow* wnd,int add_flags,const wxRect& rect)=0;
	virtual void DrawMenuEmptyPart(wxDC& dc,wxWindow* wnd,int bar_flags,const wxRect& rect)=0;

	virtual void DrawToolbarBackground(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area)=0;

	virtual void DrawButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,int add_flags,const wxFakeButtonSize& btn_size,const wxRect& rect,const wxSize& client_size,const wxRect& dock_area)=0;

	virtual int GetToolbarGripperWidth() const=0;
	virtual int GetToolbarChevronWidth() const=0;

	virtual void DrawToolbarGripper(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area)=0;
	virtual void DrawToolbarChevron(wxDC& dc,wxWindow* wnd,bool overflow,bool chevron,int bar_flags,int dir,int add_flags,const wxRect& rect,const wxRect& dock_area)=0;

	virtual int GetMinimizedMiniFrameHeight(int style) const=0;
	virtual int MiniFrameHitTest(int style,const wxPoint& pt,const wxRect& rect)=0;
	virtual void GetMiniFrameClientSizeMargin(int style,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin)=0;
	virtual void DrawMiniFrame(wxDC& dc,wxWindow* wnd,const wxString& title,int style,int highlight_idx,int pressed_idx,int submenu_idx,const wxRect& rect)=0;
	virtual bool GetMiniFrameButtonRect(int idx,int style,const wxRect& rect,wxRect& ret)=0;
};

class wxFakeOffice2003CommandBarArt:public wxFakeCommandBarArtBase{
protected:
	//menu background color
	wxColour m_bg;
	//menu/button border color
	wxColour m_border;
	//title bar gradient color
	wxColour m_title1,m_title2;
	//command bar gradient color
	wxColour m_bar1,m_bar2;
	//highlight button gradient color
	wxColour m_hl1,m_hl2;
	//checked button gradient color
	wxColour m_checked1,m_checked2;
	//highlight checked button gradient color
	wxColour m_hlchecked1,m_hlchecked2;
	//pressed button gradient color
	wxColour m_pressed1,m_pressed2;
	//separator color
	wxColour m_sprt1,m_sprt2;
	//normal text color
	wxColour m_text;
	//highlight text color
	wxColour m_texthl;
	//disabled text color
	wxColour m_textdis;
	//chevron menu gradient color
	wxColour m_chevron1,m_chevron2;

	//resize gripper bitmap
	wxBitmap m_gripper;
	//check sign and option sign
	wxBitmap m_checksign,m_optionsign;
	wxBitmap m_checksign_disabled,m_optionsign_disabled;
	//dropdown sign in tool bar
	wxBitmap m_toolbar_up,m_toolbar_down,m_toolbar_left,m_toolbar_right;
	wxBitmap m_toolbar_up_disabled,m_toolbar_down_disabled,m_toolbar_left_disabled,m_toolbar_right_disabled;
	//dropdown sign in menu bar
	wxBitmap m_menu_left,m_menu_right;
	wxBitmap m_menu_left_disabled,m_menu_right_disabled;
	//overflow sign in tool bar
	wxBitmap m_overflow_right,m_overflow_down;
	//chevron menu sign in tool bar
	wxBitmap m_chevron_right,m_chevron_down;

	//mini frame color
	wxColour m_pane_outerborder,m_pane_innerborder;
	wxColour m_pane_title1,m_pane_title2;
	wxColour m_pane_bg1,m_pane_bg2;

	//mini frame bitmap
	wxBitmap m_pane_close,m_pane_dropdown,m_pane_minimize,m_pane_restore;
	wxBitmap m_pane_moveleft,m_pane_moveright;
	wxBitmap m_pane_pin,m_pane_unpin;
	wxBitmap m_pane_topleftcorner,m_pane_toprightcorner;
public:
	wxFakeOffice2003CommandBarArt();
public:
	virtual wxSize GetColorSize();
	virtual void GetButtonPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual void GetMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual void GetChevronMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual void GetBarClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual void GetMenuClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual wxSize GetEmptyMenuSize();
	virtual void GetMenuGripperSize(int& height,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);

	virtual wxFakeButtonSize MeasureButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags);

	virtual wxFakeButtonHitTestState HitTest(const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,const wxPoint& pt,const wxFakeButtonSize& btn_size,const wxRect& rect);

	virtual void DrawColor(wxDC& dc,wxWindow* wnd,wxColour clr,int flags,int add_flags,const wxRect& rect);

	virtual void DrawMenuBorderAndBackground(wxDC& dc,wxWindow* wnd,const wxFakeMenuPopupConnection& conn,const wxRect& rect);
	virtual void DrawEmptyMenuItem(wxDC& dc,wxWindow* wnd,const wxRect& rect);
	virtual void DrawMenuGripper(wxDC& dc,wxWindow* wnd,int add_flags,const wxRect& rect);
	virtual void DrawMenuEmptyPart(wxDC& dc,wxWindow* wnd,int bar_flags,const wxRect& rect);

	virtual void DrawToolbarBackground(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area);

	virtual void DrawButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,int add_flags,const wxFakeButtonSize& btn_size,const wxRect& rect,const wxSize& client_size,const wxRect& dock_area);

	virtual int GetToolbarGripperWidth() const;
	virtual int GetToolbarChevronWidth() const;

	virtual void DrawToolbarGripper(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area);
	virtual void DrawToolbarChevron(wxDC& dc,wxWindow* wnd,bool overflow,bool chevron,int bar_flags,int dir,int add_flags,const wxRect& rect,const wxRect& dock_area);

	virtual int GetMinimizedMiniFrameHeight(int style) const;
	virtual int MiniFrameHitTest(int style,const wxPoint& pt,const wxRect& rect);
	virtual void GetMiniFrameClientSizeMargin(int style,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin);
	virtual void DrawMiniFrame(wxDC& dc,wxWindow* wnd,const wxString& title,int style,int highlight_idx,int pressed_idx,int submenu_idx,const wxRect& rect);
	virtual bool GetMiniFrameButtonRect(int idx,int style,const wxRect& rect,wxRect& ret);
public:
	virtual void UpdateDisabledTextColor(wxColour clr);
protected:
	void DrawOneGripper(wxDC& dc,wxWindow* wnd,int x,int y);
};

extern wxFakeCommandBarArtBase* wxFakeCommandBarDefaultArtProvider;

#endif
