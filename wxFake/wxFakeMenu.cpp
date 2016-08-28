#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/display.h>
#include <wx/dcscreen.h>
#include <wx/dcbuffer.h>
#include <wx/cursor.h>
#include <wx/event.h>
#include <wx/tooltip.h>
#include <iostream>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeMiniFrame.hpp"
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeCommandBarFrame.hpp"

//#define WXFAKEMENU_SHOW_TOOLTIP

#ifdef WIN32
#pragma comment(lib,"msimg32.lib")
#endif

IMPLEMENT_CLASS(wxFakeMenuPopupWindow,wxPopupWindowWithShadow)

BEGIN_EVENT_TABLE(wxFakeMenuPopupWindow,wxPopupWindowWithShadow)

EVT_PAINT( wxFakeMenuPopupWindow::OnPaint )
EVT_MOTION( wxFakeMenuPopupWindow::OnMouseMove )
EVT_LEFT_DOWN( wxFakeMenuPopupWindow::OnMouseDown )
EVT_MOUSEWHEEL( wxFakeMenuPopupWindow::OnMouseWheel )
EVT_LEAVE_WINDOW( wxFakeMenuPopupWindow::OnMouseMove )
EVT_KEY_DOWN( wxFakeMenuPopupWindow::OnKeyDown )
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
EVT_MOUSE_CAPTURE_LOST( wxFakeMenuPopupWindow::OnMouseCaptureLost )
EVT_TIMER( 1, wxFakeMenuPopupWindow::OnTimer )
#else
EVT_KILL_FOCUS( wxFakeMenuPopupWindow::OnLostFocus )
#endif
EVT_CONTEXT_MENU( wxFakeMenuPopupWindow::OnContextMenu )
#ifdef WXFAKEMENU_USE_MENU_ANIMATION
EVT_TIMER( 2, wxFakeMenuPopupWindow::OnAnimationTimer )
#endif

END_EVENT_TABLE()

IMPLEMENT_CLASS(wxPopupWindowWithShadow,wxPopupWindow)

wxPopupWindowWithShadow::wxPopupWindowWithShadow(){
}

wxPopupWindowWithShadow::wxPopupWindowWithShadow(wxWindow *parent,int flags){
	Create(parent,flags);
}

bool wxPopupWindowWithShadow::Create(wxWindow *parent,int flags){
#ifdef __WXMSW__
	// popup windows are created hidden by default
	Hide();

	if ( !CreateBase(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		flags | wxPOPUP_WINDOW, wxDefaultValidator, wxEmptyString) )
		return false;

	parent->AddChild(this);

	WXDWORD exstyle;
	DWORD msflags = MSWGetCreateWindowFlags(&exstyle);

#ifndef WXFAKEMENU_USE_MOUSE_CAPTURE
	// dirty hack
	msflags = (msflags & ~WS_CHILD) | WS_POPUP;
#endif

	const wxChar* wclass =
		wxApp::GetRegisteredClassName(wxT("wxPopupWindowWithShadow"), COLOR_BTNFACE, 0x00020000 /* CS_DROPSHADOW */);

	if ( !MSWCreate(wclass,
		NULL, wxDefaultPosition, wxDefaultSize, msflags, exstyle) )
		return false;

#ifndef WXFAKEMENU_USE_MOUSE_CAPTURE
	// dirty hack
	::SetWindowLongW((HWND)GetHWND(), GWL_HWNDPARENT, (LONG)(HWND)parent->GetHWND());
#endif

	InheritAttributes();

	return true;
#else

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(!wxPopupWindow::Create(parent,flags)) return false;

	return true;
#else
#error WXFAKEMENU_USE_MOUSE_CAPTURE must set on non-Windows platform
#endif

#endif
}

wxPopupWindowWithShadow::~wxPopupWindowWithShadow(){
}

void wxFakeMenuPopupWindow::Init(){
	m_conn.dir=0;
	m_bar=NULL;
	m_artprov=NULL;

	m_hdc=NULL;
	m_gripper_add_flags=0;

	m_submenu_idx=-1;
	m_submenu=NULL;
	m_parentmenu=NULL;
	m_parentbar=NULL;
	m_parentpane=NULL;

	m_dockmgr=NULL;

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	m_timer=NULL;
#endif
#ifdef WXFAKEMENU_USE_MENU_ANIMATION
	m_anim_timer=NULL;
#endif
}

bool wxFakeMenuPopupWindow::Create(wxWindow *parent,int flags){
	if(!wxPopupWindowWithShadow::Create(parent,flags | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE)) return false;

	SetBackgroundStyle(wxBG_STYLE_PAINT);

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	m_timer=new wxTimer(this,1);
#endif
#ifdef WXFAKEMENU_USE_MENU_ANIMATION
	m_anim_timer=new wxTimer(this,2);
	m_bmback=NULL;
#endif

	return true;
}

wxFakeMenuPopupWindow::~wxFakeMenuPopupWindow(){
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	delete m_timer;
#endif
#ifdef WXFAKEMENU_USE_MENU_ANIMATION
	delete m_anim_timer;
	delete m_bmback;
#endif
}

// x - left/top of the menu [in, out]
// w - width/height of the menu
// ll - left/top of screen
// ww - width/height of screen
// w1 - width/height of the button
// conn - connection data [out]
// bReverse - prefer left/up rather than right/down
static void pMenuPos_CalcConnected(int &x,int w,int ll,int ww,int w1,wxFakeMenuPopupConnection& conn,bool bReverse){
	//save the temp value
	conn.start=x;

	if(bReverse){
		if(x-w+w1>=ll){ //check if it can popup to the left
			x=x-w+w1;
		}else if(x>ll+ww-w){ //check if if can't popup to the right
			x=ll;
		}
	}else{
		if(x>ll+ww-w){ //check if if can't popup to the right
			x=x-w+w1; //try popup to the left, and check
			if(x<ll) x=ll+ww-w;
		}
	}

	//calc the position of connection
	conn.start+=1-x;
	conn.end=conn.start+w1-2;
	if(conn.start<1) conn.start=1;
	if(conn.end>w-1) conn.end=w-1;
}

// pos - position of menu [in, out]
// s - size of menu
// ss - position and size of screen
// s1 - size of button
// conn - connection data [out]
// dir - prefered popup direction
// margin - margin of direction XXX in pMenuPos_CheckXXX
// return value: can be popuped in this direction
static bool pMenuPos_CheckDown(wxPoint& pos,const wxSize& s,const wxRect& ss,const wxSize& s1,wxFakeMenuPopupConnection& conn,int dir,int margin){
	//if it's enough space then OK
	if(pos.y+s1.y+margin<=ss.y+ss.height-s.y){
		pos.y=pos.y+s1.y+margin;
		pMenuPos_CalcConnected(pos.x,s.x,ss.x,ss.width,s1.x,conn,dir==wxLEFT || wxTheApp->GetLayoutDirection()==wxLayout_RightToLeft);
		conn.dir=(margin==-1)?wxUP:0;
		return true;
	}
	return false;
}

// pos - position of menu [in, out]
// s - size of menu
// ss - position and size of screen
// s1 - size of button
// conn - connection data [out]
// dir - prefered popup direction
// margin - margin of direction XXX in pMenuPos_CheckXXX
// return value: can be popuped in this direction
static bool pMenuPos_CheckUp(wxPoint& pos,const wxSize& s,const wxRect& ss,const wxSize& s1,wxFakeMenuPopupConnection& conn,int dir,int margin){
	//if it's enough space then OK
	if(pos.y-s.y-margin>=ss.y){
		pos.y=pos.y-s.y-margin;
		pMenuPos_CalcConnected(pos.x,s.x,ss.x,ss.width,s1.x,conn,dir==wxLEFT || wxTheApp->GetLayoutDirection()==wxLayout_RightToLeft);
		conn.dir=(margin==-1)?wxDOWN:0;
		return true;
	}
	return false;
}

// pos - position of menu [in, out]
// s - size of menu
// ss - position and size of screen
// s1 - size of button
// conn - connection data [out]
// dir - prefered popup direction
// margin - margin of direction XXX in pMenuPos_CheckXXX
// return value: can be popuped in this direction
static bool pMenuPos_CheckRight(wxPoint& pos,const wxSize& s,const wxRect& ss,const wxSize& s1,wxFakeMenuPopupConnection& conn,int dir,int margin){
	//if it's enough space then OK
	if(pos.x+s1.x+margin<=ss.x+ss.width-s.x){
		pos.x=pos.x+s1.x+margin;
		pMenuPos_CalcConnected(pos.y,s.y,ss.y,ss.height,s1.y,conn,dir==wxTOP);
		conn.dir=(margin==-1)?wxLEFT:0;
		return true;
	}
	return false;
}

// pos - position of menu [in, out]
// s - size of menu
// ss - position and size of screen
// s1 - size of button
// conn - connection data [out]
// dir - prefered popup direction
// margin - margin of direction XXX in pMenuPos_CheckXXX
// return value: can be popuped in this direction
static bool pMenuPos_CheckLeft(wxPoint& pos,const wxSize& s,const wxRect& ss,const wxSize& s1,wxFakeMenuPopupConnection& conn,int dir,int margin){
	//if it's enough space then OK
	if(pos.x-s.x-margin>=ss.y){
		pos.x=pos.x-s.x-margin;
		pMenuPos_CalcConnected(pos.y,s.y,ss.y,ss.height,s1.y,conn,dir==wxTOP);
		conn.dir=(margin==-1)?wxRIGHT:0;
		return true;
	}
	return false;
}

// pos - position of menu [in, out]
// s - size of menu
// ss - position and size of screen
// dir - prefered popup direction
static void pMenuPos_Calc(wxPoint& pos,const wxSize& s,const wxRect& ss,int dir){
	//check x coord
	if(dir==wxLEFT || wxTheApp->GetLayoutDirection()==wxLayout_RightToLeft){
		if(pos.x-s.x+1>=ss.x){ //check if it can popup to the left
			pos.x=pos.x-s.x+1;
		}else if(pos.x>ss.x+ss.width-s.x){ //check if it can't popup to the right
			pos.x=ss.x;
		}
	}else{
		if(pos.x>ss.x+ss.width-s.x){ //check if it can't popup to the right
			pos.x=pos.x-s.x+1; //try popup to the left, and check
			if(pos.x<ss.x) pos.x=ss.x+ss.width-s.x;
		}
	}

	//check y coord
	if(dir==wxUP){
		if(pos.y-s.y+1>=ss.y){ //check if it can popup to the up
			pos.y=pos.y-s.y+1;
		}else if(pos.y>ss.y+ss.height-s.y){ //check if it can't popup to the down
			pos.y=ss.y;
		}
	}else{
		if(pos.y>ss.y+ss.height-s.y){ //check if it can't popup to the down
			pos.y=pos.y-s.y+1; //try popup to the up, and check
			if(pos.y<ss.y) pos.y=ss.y+ss.height-s.y;
		}
	}
}

void wxFakeMenuPopupWindow::PositionEx(const wxPoint& ptOrigin,const wxSize& size,int dir,int leftMargin,int topMargin,int rightMargin,int bottomMargin){
	//init default popup position and connection data
	wxPoint p = ptOrigin;
	if(p == wxDefaultPosition) p = wxGetMouseState().GetPosition();
	m_conn.dir = 0;

	// determine the position and size of the screen we clamp the popup to
	wxRect rectScreen;

	const int displayNum = wxDisplay::GetFromPoint(p);
	if ( displayNum != wxNOT_FOUND )
	{
		rectScreen = wxDisplay(displayNum).GetGeometry();
	}
	else // outside of any display?
	{
		// just use the primary one then
		rectScreen = wxRect(wxGetDisplaySize());
	}

	const wxSize sizeSelf = GetSize();

	switch(dir){
	case wxDOWN:
	case wxUP:
		//check up and down
		if(dir==wxUP){
			if(
				pMenuPos_CheckUp(p,sizeSelf,rectScreen,size,m_conn,dir,topMargin) ||
				pMenuPos_CheckDown(p,sizeSelf,rectScreen,size,m_conn,dir,bottomMargin)
				)
			{
				break;
			}
		}else{
			if(
				pMenuPos_CheckDown(p,sizeSelf,rectScreen,size,m_conn,dir,bottomMargin) ||
				pMenuPos_CheckUp(p,sizeSelf,rectScreen,size,m_conn,dir,topMargin)
				)
			{
				break;
			}
		}

		//check left and right
		if(wxTheApp->GetLayoutDirection()==wxLayout_RightToLeft){
			if(
				pMenuPos_CheckLeft(p,sizeSelf,rectScreen,size,m_conn,dir,leftMargin) ||
				pMenuPos_CheckRight(p,sizeSelf,rectScreen,size,m_conn,dir,rightMargin)
				)
			{
				break;
			}
		}else{
			if(
				pMenuPos_CheckRight(p,sizeSelf,rectScreen,size,m_conn,dir,rightMargin) ||
				pMenuPos_CheckLeft(p,sizeSelf,rectScreen,size,m_conn,dir,leftMargin)
				)
			{
				break;
			}
		}

		//finally try standard popup
		pMenuPos_Calc(p,sizeSelf,rectScreen,dir);
		break;
	case wxRIGHT:
	case wxLEFT:
		//check left and right
		if(dir==wxLEFT){
			if(
				pMenuPos_CheckLeft(p,sizeSelf,rectScreen,size,m_conn,dir,leftMargin) ||
				pMenuPos_CheckRight(p,sizeSelf,rectScreen,size,m_conn,dir,rightMargin)
				)
			{
				break;
			}
		}else{
			if(
				pMenuPos_CheckRight(p,sizeSelf,rectScreen,size,m_conn,dir,rightMargin) ||
				pMenuPos_CheckLeft(p,sizeSelf,rectScreen,size,m_conn,dir,leftMargin)
				)
			{
				break;
			}
		}

		//check up and down
		if(
			pMenuPos_CheckDown(p,sizeSelf,rectScreen,size,m_conn,dir,bottomMargin) ||
			pMenuPos_CheckUp(p,sizeSelf,rectScreen,size,m_conn,dir,topMargin)
			)
		{
			break;
		}

		//finally try standard popup
		pMenuPos_Calc(p,sizeSelf,rectScreen,dir);
		break;
	default:
		//use standard popup
		pMenuPos_Calc(p,sizeSelf,rectScreen,dir);
		break;
	}

	Move(p, wxSIZE_NO_ADJUSTMENTS);
}

void wxFakeMenuPopupWindow::SetBar(wxFakeCommandBarDataBase* bar){
	m_bar=bar;
	m_btn_add_flags.clear();
	//TODO: dirty
}

void wxFakeMenuPopupWindow::SetArtProvider(wxFakeCommandBarArtBase* artprov){
	m_artprov=artprov;
	if(m_bar) m_bar->SetDirty();
}

bool wxFakeMenuPopupWindow::Layout(){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	wxSize sz;

	//measure and layout buttons
	if(m_bar){
		if(m_bar->GetDirty()){
			wxDC *hdc;

			if(m_hdc) hdc=m_hdc;
			else hdc=new wxClientDC(this);

			m_bar->LayoutMenuButtons(*hdc,this,artprov);

			if(m_hdc==NULL) delete hdc;
		}

		m_rcMenu=m_bar->MenuRect();
		m_rcGripper=m_bar->GripperRect();
		sz=m_bar->MenuSize();
	}else{
		int leftMargin,topMargin,rightMargin,bottomMargin;

		sz=artprov->GetEmptyMenuSize();
		m_rcMenu=wxRect(sz);
		m_rcGripper=wxRect();

		//check menu client size margin
		artprov->GetMenuClientSizeMargin(leftMargin,topMargin,rightMargin,bottomMargin);
		m_rcMenu.x+=leftMargin;
		m_rcMenu.y+=topMargin;

		sz.x+=m_rcMenu.x+rightMargin;
		sz.y+=m_rcMenu.y+bottomMargin;
	}

	//resize the window
	SetSize(sz);

	//over
	return true;
}

bool wxFakeMenuPopupWindow::Show(bool show){
	if(show==IsShown()) return true;

	bool ret;

	if(show){
		//reset flags
		m_gripper_add_flags=0;
		int m=0;
		if(m_bar!=NULL && (m=m_bar->GetLayoutedButtons().size())>0){
			m_btn_add_flags.resize(m);
			for(int i=0;i<m;i++){
				m_btn_add_flags[i]=0;
			}
		}else{
			m_btn_add_flags.clear();
		}
		SetCursor(wxNullCursor);
		m_submenu_idx=-1;
		m_submenu=NULL;

#ifdef WXFAKEMENU_USE_MENU_ANIMATION
		m_transparency=51;
		m_anim_timer->Start(30);
		{
			if(m_bmback){
				delete m_bmback;
				m_bmback=NULL;
			}

			wxScreenDC hdcScreen;
			wxRect r=GetScreenRect();

			m_bmback=new wxBufferedDC(NULL,r.GetSize());

			m_bmback->Blit(0,0,r.width,r.height,&hdcScreen,r.x,r.y);
		}
#endif

		//TODO: etc.
		ret=wxPopupWindowWithShadow::Show(show);
		SetFocus();

		//check mouse capture
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
		bool bCapture=true;

		wxFakeMenuPopupWindow *mnu=this;
		while(mnu!=NULL){
			if(mnu->HasCapture()){
				bCapture=false;
				break;
			}
			wxFakeCommandBar *bar=mnu->m_parentbar;
			if(bar!=NULL && bar->HasCapture()){
				bCapture=false;
				break;
			}
			mnu=mnu->m_parentmenu;
		}

		if(bCapture && !HasCapture()){
			CaptureMouse();
		}
#endif
	}else{
		//TODO: etc.
		if(m_submenu){
			m_submenu->m_parentmenu=NULL;
			m_submenu->Destroy();
			m_submenu=NULL;
		}
		ret=wxPopupWindowWithShadow::Show(show);
	}

	return ret;
}

bool wxFakeMenuPopupWindow::Redraw(wxDC& dc){
	if(!IsShown()) return false;

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	wxRect rect(GetSize());

	//draw background
	artprov->DrawMenuBorderAndBackground(dc,this,m_conn,rect);

	if(m_bar==NULL){
		artprov->DrawEmptyMenuItem(dc,this,m_rcMenu);
	}else{
		int bar_flags=m_bar->GetFlags();
		wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

		//draw gripper
		if(bar_flags & fcbfDragToMakeThisMenuFloat){
			artprov->DrawMenuGripper(dc,this,m_gripper_add_flags,m_rcGripper);
		}

		//draw menu item
		if(btns.empty()){
			artprov->DrawEmptyMenuItem(dc,this,m_rcMenu);
		}else{
			//draw buttons
			wxRect oldRect;

			for(int i=0;i<(int)btns.size();i++){
				wxRect r=btns[i].rect;

				//draw menu empty part
				if(btns[i].btn->Type()==fbttColumnSeparator){
					if(oldRect.width>0 && oldRect.y<m_rcMenu.height){
						oldRect.height=m_rcMenu.height-oldRect.y;

						oldRect.x+=m_rcMenu.x;
						oldRect.y+=m_rcMenu.y;
						artprov->DrawMenuEmptyPart(dc,this,bar_flags,oldRect);
					}
					oldRect.x=r.x+r.width;
					oldRect.y=0;
					oldRect.width=0;
				}else{
					if(r.x+r.width-oldRect.x>oldRect.width){
						oldRect.width=r.x+r.width-oldRect.x;
					}
					if(r.y+r.height>oldRect.y){
						oldRect.y=r.y+r.height;
					}
				}

				//draw button
				r.x+=m_rcMenu.x;
				r.y+=m_rcMenu.y;
				artprov->DrawButton(dc,this,*btns[i].btn,bar_flags,false,0,m_btn_add_flags[i],btns[i].size,r,wxSize(),wxRect());
			}

			//draw menu empty part
			if(oldRect.width>0 && oldRect.y<m_rcMenu.height){
				oldRect.height=m_rcMenu.height-oldRect.y;

				oldRect.x+=m_rcMenu.x;
				oldRect.y+=m_rcMenu.y;
				artprov->DrawMenuEmptyPart(dc,this,bar_flags,oldRect);
			}
		}
	}

#ifdef WXFAKEMENU_USE_MENU_ANIMATION
	if(m_bmback!=NULL && m_transparency<255){
		wxSize sz=m_bmback->GetSize();
		//dirty hack
		::AlphaBlend((HDC)dc.GetHDC(),0,0,sz.x,sz.y,(HDC)m_bmback->GetHDC(),0,0,sz.x,sz.y,(const BLENDFUNCTION&)(const int&)((255-m_transparency)<<16));
	}
#endif

	return true;
}

void wxFakeMenuPopupWindow::OnPaint(wxPaintEvent& event){
	if(!IsShown()) return;

	wxAutoBufferedPaintDC dc(this);
	if(!Redraw(dc)) event.Skip();
}

void wxFakeMenuPopupWindow::OnMouseMove(wxMouseEvent& event){
	if(!IsShown()) return;

	//forward mouse event to submenu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(event.GetEventType()==wxEVT_MOTION){
		if(ForwardMouseEvent(event)) return;
	}
#endif

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL || m_bar==NULL) return;

	int bar_flags=m_bar->GetFlags();
	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

	wxDC *dc=NULL;
#ifdef WXFAKEMENU_SHOW_TOOLTIP
	bool bSetToolTipText=false;
#endif

	wxPoint p=event.GetPosition();

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	{
		wxSize sz=GetSize();
		//use the old-fashioned timer method to detect mouse leave event
		if(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y){
			if(!m_timer->IsRunning()){
				m_timer->Start(30);
			}
		}else{
			if(m_timer->IsRunning()){
				m_timer->Stop();
			}
		}
	}
#endif

	//check drag to make this menu float
	if((bar_flags & fcbfDragToMakeThisMenuFloat)!=0
		&& (m_gripper_add_flags & fbtaHighlight)!=0
		&& event.LeftIsDown())
	{
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
		wxFakeMenuPopupWindow *mnu=this;
		while(mnu!=NULL){
			if(mnu->HasCapture()){
				mnu->ReleaseMouse();
				break;
			}
			wxFakeCommandBar *bar=mnu->m_parentbar;
			if(bar!=NULL && bar->HasCapture()){
				bar->ReleaseMouse();
				break;
			}
			mnu=mnu->m_parentmenu;
		}
#else
		if(HasCapture()) ReleaseMouse();
#endif
		wxPoint p0=GetPosition();
		DestroyAll();
		if(m_dockmgr!=NULL){
			m_dockmgr->MakeMenuFloat(m_bar,p0,p);
		}
		return;
	}

	//check gripper
	if(bar_flags & fcbfDragToMakeThisMenuFloat){
		int add_flags=0;

		if(m_rcGripper.Contains(p) && !event.LeftIsDown()){
			add_flags=fbtaHighlight;
		}

		if(add_flags!=m_gripper_add_flags){
			m_gripper_add_flags=add_flags;
			if(dc==NULL) dc=new wxClientDC(this);
			artprov->DrawMenuGripper(*dc,this,m_gripper_add_flags | fbtaRedrawBackground,m_rcGripper);

#ifdef __WXGTK__
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZENWSE):wxNullCursor);
#else
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZING):wxNullCursor);
#endif

			//hide the popup menu
			if(m_submenu){
				m_submenu->m_parentmenu=NULL;
				m_submenu->Destroy();
				m_submenu=NULL;

				SetFocus();
			}
			if(m_submenu_idx>=0 && m_submenu_idx<(int)btns.size()){
				m_btn_add_flags[m_submenu_idx]=0;

				if(dc==NULL) dc=new wxClientDC(this);
				wxRect r=btns[m_submenu_idx].rect;
				r.x+=m_rcMenu.x;
				r.y+=m_rcMenu.y;
				artprov->DrawButton(*dc,this,*btns[m_submenu_idx].btn,bar_flags,false,0,
					m_btn_add_flags[m_submenu_idx] | fbtaRedrawBackground,btns[m_submenu_idx].size,r,wxSize(),wxRect());
			}
			m_submenu_idx=-1;

#ifdef WXFAKEMENU_SHOW_TOOLTIP
			if(add_flags & fbtaHighlight){
				SetToolTip("Drag to make this menu float");
			}
#endif
		}

#ifdef WXFAKEMENU_SHOW_TOOLTIP
		if(add_flags & fbtaHighlight) bSetToolTipText=true;
#endif
	}

	p.x-=m_rcMenu.x;
	p.y-=m_rcMenu.y;

	//check buttons
	for(int i=0;i<(int)btns.size();i++){
		wxFakeButtonType nType=btns[i].btn->Type();
		int nFlags=btns[i].btn->Flags();

		int add_flags=0;

		if((nFlags & fbtfHidden)==0 && nType!=fbttSeparator && nType!=fbttColumnSeparator){
			if(btns[i].rect.Contains(p)){
				add_flags=fbtaHighlight;
			}
		}

		//check popup submenu
		if((add_flags & fbtaHighlight)!=0 && (nFlags & fbtfDisabled)==0){
			wxFakeCommandBarDataBase *submenu=(wxFakeCommandBarDataBase*)btns[i].btn->SubMenu();

			if(submenu!=NULL){
				//show new popup menu
				if(m_submenu_idx!=i){
					//hide old one
					if(m_submenu){
						m_submenu->m_parentmenu=NULL;
						m_submenu->Destroy();
						m_submenu=NULL;

						SetFocus();
					}
					if(m_submenu_idx>=0 && m_submenu_idx<(int)btns.size()){
						m_btn_add_flags[m_submenu_idx]=0;

						if(dc==NULL) dc=new wxClientDC(this);
						wxRect r=btns[m_submenu_idx].rect;
						r.x+=m_rcMenu.x;
						r.y+=m_rcMenu.y;
						artprov->DrawButton(*dc,this,*btns[m_submenu_idx].btn,bar_flags,false,0,
							m_btn_add_flags[m_submenu_idx] | fbtaRedrawBackground,btns[m_submenu_idx].size,r,wxSize(),wxRect());
					}

					//show new one
					add_flags|=fbtaPopupMenu;

					m_submenu=new wxFakeMenuPopupWindow();
					m_submenu->SetBar(submenu);
					m_submenu->SetArtProvider(m_artprov);
					m_submenu->m_parentmenu=this;
					m_submenu->m_dockmgr=m_dockmgr;
					m_submenu->Create(m_parent);

					wxRect r=btns[i].rect;
					r.x+=m_rcMenu.x;
					r.y+=m_rcMenu.y;
					ClientToScreen(&r.x,&r.y);

					int leftMargin,topMargin,rightMargin,bottomMargin;
					int dir;
					if(btns[i].size.sizeMode==fbsmMenu){
						artprov->GetMenuPopupMargin(leftMargin,topMargin,rightMargin,bottomMargin);
						dir=wxRIGHT;
					}else{
						artprov->GetButtonPopupMargin(leftMargin,topMargin,rightMargin,bottomMargin);
						dir=wxDOWN;
					}

					m_submenu_idx=i;

					m_submenu->Layout();
					m_submenu->PositionEx(r.GetTopLeft(),r.GetSize(),dir,leftMargin,topMargin,rightMargin,bottomMargin);
					m_submenu->Show();
				}
			}else{
				//hide the popup menu
				if(m_submenu){
					m_submenu->m_parentmenu=NULL;
					m_submenu->Destroy();
					m_submenu=NULL;

					SetFocus();
				}
				if(m_submenu_idx>=0 && m_submenu_idx<(int)btns.size()){
					m_btn_add_flags[m_submenu_idx]=0;

					if(dc==NULL) dc=new wxClientDC(this);
					wxRect r=btns[m_submenu_idx].rect;
					r.x+=m_rcMenu.x;
					r.y+=m_rcMenu.y;
					artprov->DrawButton(*dc,this,*btns[m_submenu_idx].btn,bar_flags,false,0,
						m_btn_add_flags[m_submenu_idx] | fbtaRedrawBackground,btns[m_submenu_idx].size,r,wxSize(),wxRect());
				}
				m_submenu_idx=-1;
			}
		}

		//draw updated button only
		if(add_flags!=m_btn_add_flags[i] && (m_btn_add_flags[i] & fbtaPopupMenu)==0){
			m_btn_add_flags[i]=add_flags;

			//if((nFlags & fbtfDisabled)==0)
			{
				if(dc==NULL) dc=new wxClientDC(this);
				wxRect r=btns[i].rect;
				r.x+=m_rcMenu.x;
				r.y+=m_rcMenu.y;
				artprov->DrawButton(*dc,this,*btns[i].btn,bar_flags,false,0,m_btn_add_flags[i] | fbtaRedrawBackground,btns[i].size,r,wxSize(),wxRect());
			}

#ifdef WXFAKEMENU_SHOW_TOOLTIP
			if(add_flags & fbtaHighlight){
				const wxString& s=btns[i].btn->ToolTipText();
				SetToolTip(s);
			}
#endif
		}

#ifdef WXFAKEMENU_SHOW_TOOLTIP
		if(add_flags & fbtaHighlight) bSetToolTipText=true;
#endif
	}

	if(dc) delete dc;

#ifdef WXFAKEMENU_SHOW_TOOLTIP
	if(!bSetToolTipText){
		wxToolTip *tooltip=GetToolTip();
		if(tooltip && !(tooltip->GetTip().empty())){
			tooltip->SetTip(wxEmptyString);
		}
	}
#endif

	event.Skip();
}

void wxFakeMenuPopupWindow::OnMouseDown(wxMouseEvent& event){
	if(!IsShown()) return;

	//forward mouse event to submenu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(event.GetEventType()==wxEVT_LEFT_DOWN){
		if(ForwardMouseEvent(event)) return;
	}
#endif

	wxPoint p=event.GetPosition();

	//check if clicked outside the menu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	{
		wxSize sz=GetSize();
		if(!(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y)){
			DestroyAll();
			return;
		}
	}
#endif

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL || m_bar==NULL) return;

	int bar_flags=m_bar->GetFlags();
	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

	bool bHide=false;

	wxEvtHandler *handler=NULL;
	if(m_parent!=NULL) handler=m_parent->GetEventHandler();

	p.x-=m_rcMenu.x;
	p.y-=m_rcMenu.y;

	for(int i=0;i<(int)btns.size();i++){
		wxFakeButtonType nType=btns[i].btn->Type();
		int nFlags=btns[i].btn->Flags();

		if((nFlags & (fbtfHidden | fbtfDisabled))==0 && nType!=fbttSeparator && nType!=fbttColumnSeparator
			&& (nType==fbttSplit || btns[i].btn->SubMenu()==NULL))
		{
			if(btns[i].rect.Contains(p)){
				//raise event.
				btns[i].btn->OnClick(this,handler,btns[i].bar,btns[i].index);

				//check the parent object and tell them to update button state
				wxFakeMenuPopupWindow *mnu=this;
				while(mnu!=NULL){
					if(mnu->m_parentbar!=NULL){
						if(mnu->m_parentbar->DockManager()!=NULL){
							mnu->m_parentbar->DockManager()->UpdateOnNextIdle();
						}else{
							mnu->m_parentbar->UpdateOnNextIdle();
						}
						break;
					}else if(mnu->m_parentpane!=NULL){
						if(mnu->m_parentpane->DockManager()!=NULL){
							mnu->m_parentpane->DockManager()->UpdateOnNextIdle();
						}else{
							wxFakeCommandBar *bar=wxDynamicCast(mnu->m_parentpane->GetClientWindow(),wxFakeCommandBar);
							if(bar!=NULL){
								bar->UpdateOnNextIdle();
							}else{
								//TODO:
							}
						}
						break;
					}
					mnu=mnu->m_parentmenu;
				}

				//hide the menu
				bHide=true;
				break;
			}
		}
	}

	if(bHide){
		DestroyAll();
	}
}

void wxFakeMenuPopupWindow::OnSubMenuDestroy(){
	//redraw
	if(IsShown() && m_bar!=NULL){
		wxFakeCommandBarArtBase* artprov=m_artprov;
		if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
		if(artprov!=NULL){
			int bar_flags=m_bar->GetFlags();
			wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

			if(m_submenu_idx>=0 && m_submenu_idx<(int)btns.size()){
				m_btn_add_flags[m_submenu_idx]=0;

				wxClientDC dc(this);
				wxRect r=btns[m_submenu_idx].rect;
				r.x+=m_rcMenu.x;
				r.y+=m_rcMenu.y;
				artprov->DrawButton(dc,this,*btns[m_submenu_idx].btn,bar_flags,false,0,
					m_btn_add_flags[m_submenu_idx] | fbtaRedrawBackground,btns[m_submenu_idx].size,r,wxSize(),wxRect());
			}
		}
	}

	m_submenu=NULL;
	m_submenu_idx=-1;
}

void wxFakeMenuPopupWindow::OnMouseWheel(wxMouseEvent& event){
	if(!IsShown()) return;

	//forward mouse event to submenu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(event.GetEventType()==wxEVT_MOUSEWHEEL){
		if(ForwardMouseEvent(event)) return;
	}
#endif

	//it works by using dirty hack
	std::cout<<"OnMouseWheel\n";
}

void wxFakeMenuPopupWindow::OnKeyDown(wxKeyEvent& event){
	if(!IsShown()) return;

	//it works by using dirty hack
	std::cout<<"OnKeyDown\n";
}

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE

void wxFakeMenuPopupWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent& event){
	DestroyAll();
}

bool wxFakeMenuPopupWindow::ForwardMouseEvent(wxMouseEvent& event){
	wxFakeMenuPopupWindow *submenu=m_submenu;
	if(submenu!=NULL){
		while(submenu->m_submenu!=NULL){
			submenu=submenu->m_submenu;
		}

		wxPoint pGlobal=ClientToScreen(event.GetPosition());

		while(submenu!=this && submenu!=NULL){
			wxRect r=submenu->GetScreenRect();
			if(r.Contains(pGlobal)){
				wxMouseEvent evt2(event);
				evt2.SetEventType(wxEVT_NULL);
				evt2.SetPosition(wxPoint(pGlobal.x-r.x,pGlobal.y-r.y));
				if(event.GetEventType()==wxEVT_MOTION){
					submenu->OnMouseMove(evt2);
				}else if(event.GetEventType()==wxEVT_LEFT_DOWN){
					submenu->OnMouseDown(evt2);
				}else if(event.GetEventType()==wxEVT_MOUSEWHEEL){
					submenu->OnMouseDown(evt2);
				}
				return true;
			}
			submenu=submenu->m_parentmenu;
		}
	}

	return false;
}

void wxFakeMenuPopupWindow::OnTimer(wxTimerEvent& event){
	wxSize sz=GetSize();
	wxPoint p=ScreenToClient(wxGetMouseState().GetPosition());

	//use the old-fashioned timer method to detect mouse leave event
	if(!(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y)){
		if(m_timer->IsRunning()){
			m_timer->Stop();
		}

		wxMouseEvent evt2(wxEVT_LEAVE_WINDOW);
		evt2.m_x=-1;
		evt2.m_y=-1;
		OnMouseMove(evt2);
	}
}

#else

void wxFakeMenuPopupWindow::OnLostFocus(wxFocusEvent& event){
	if(!IsShown()) return;

	//get root menu
	wxFakeMenuPopupWindow *w=this;
	while(w->m_parentmenu!=NULL){
		w=w->m_parentmenu;
	}

	wxWindow *wnd=wxWindow::FindFocus();

	//std::cout<<"FindFocus: "<<((void*)wnd)<<"\n"; //debug

	//check if some menu has focus
	if(wxTheApp->IsActive() && wnd!=NULL){
		wxFakeMenuPopupWindow *w2=w;

		while(w2!=NULL){
			//std::cout<<"FindSubMenu: "<<((void*)w2)<<"\n"; //debug
			if(((intptr_t)wnd)==((intptr_t)(wxWindow*)w2)){
				w=w2->m_submenu;
				break;
			}
			w2=w2->m_submenu;
		}
	}

	if(w!=NULL){
		w->Destroy();
	}
}

#endif

bool wxFakeMenuPopupWindow::Destroy(){
	if(m_parentmenu){
		m_parentmenu->OnSubMenuDestroy();
		m_parentmenu=NULL;
	}
	if(m_parentbar){
		m_parentbar->OnSubMenuDestroy();
		m_parentbar=NULL;
	}
	if(m_parentpane){
		m_parentpane->OnSubMenuDestroy();
		m_parentpane=NULL;
	}

	wxFakeMenuPopupWindow *w,*w2;

	//destroy all sub menu
	w=m_submenu;
	m_submenu=NULL;
	while(w!=NULL){
		w2=w->m_submenu;
		w->m_submenu=NULL;
		w->m_parentmenu=NULL;
		w->Destroy();
		w=w2;
	}

	if(HasCapture()) ReleaseMouse();

	if ( !wxPendingDelete.Member(this) )
		wxPendingDelete.Append(this);

	Show(false);

	return true;
}

bool wxFakeMenuPopupWindow::DestroyAll(){
	wxFakeMenuPopupWindow *w,*w2;

	//destroy all parent menu
	w=m_parentmenu;
	m_parentmenu=NULL;
	while(w!=NULL){
		w2=w->m_parentmenu;
		w->m_submenu=NULL;
		w->m_parentmenu=NULL;
		w->Destroy();
		w=w2;
	}

	return Destroy();
}

void wxFakeMenuPopupWindow::OnContextMenu(wxContextMenuEvent& event){
	std::cout<<"OnContextMenu\n";
}

#ifdef WXFAKEMENU_USE_MENU_ANIMATION

void wxFakeMenuPopupWindow::OnAnimationTimer(wxTimerEvent& event){
	if(m_bmback==NULL || !IsShown()){
		m_anim_timer->Stop();
		if(m_bmback){
			delete m_bmback;
			m_bmback=NULL;
		}
		return;
	}

	m_transparency+=51;
	if(m_transparency>=255) m_transparency=255;

	Refresh();

	if(m_transparency>=255){
		m_anim_timer->Stop();
		if(m_bmback){
			delete m_bmback;
			m_bmback=NULL;
		}
	}
}

#endif
