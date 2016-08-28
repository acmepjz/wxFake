#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/cursor.h>
#include <wx/event.h>
#include <wx/tooltip.h>
#include <iostream>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeCommandBarFrame.hpp"

#define WXCOMMANDBAR_SHOW_TOOLTIP

IMPLEMENT_CLASS(wxFakeCommandBar,wxPanel)

BEGIN_EVENT_TABLE(wxFakeCommandBar,wxPanel)

EVT_PAINT( wxFakeCommandBar::OnPaint )
EVT_MOTION( wxFakeCommandBar::OnMouseMove )
EVT_LEFT_DOWN( wxFakeCommandBar::OnMouseDown )
EVT_LEFT_UP( wxFakeCommandBar::OnMouseUp )
EVT_MOUSEWHEEL( wxFakeCommandBar::OnMouseWheel )
EVT_CONTEXT_MENU( wxFakeCommandBar::OnContextMenu )
EVT_TIMER( 1, wxFakeCommandBar::OnTimer )
EVT_MOUSE_CAPTURE_LOST( wxFakeCommandBar::OnMouseCaptureLost )
EVT_IDLE( wxFakeCommandBar::OnIdle )

END_EVENT_TABLE()

bool wxFakeCommandBar::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name){
	if(!wxPanel::Create(parent,id,pos,size,style | wxFULL_REPAINT_ON_RESIZE,name)) return false;

	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetAutoLayout(true);

	m_timer=new wxTimer(this,1);

	return true;
}

wxFakeCommandBar::~wxFakeCommandBar(){
	delete m_timer;

	delete m_chevronmenu;
	delete m_separator;
	delete m_add_or_remove_btns;
}

void wxFakeCommandBar::Init(){
	m_bar=NULL;
	m_artprov=NULL;
	m_timer=NULL;

	m_hdc=NULL;

	m_pressed_idx=-1;
	m_submenu_idx=-1;
	m_submenu=NULL;

	m_dockmgr=NULL;

	m_direction=0;
	m_gripper_add_flags=0;
	m_chevron_add_flags=0;

	m_mainmenu=false;
	m_dockable=false;
	m_undocked=false;
	m_update_on_next_idle=false;
	m_overflow=false;
	m_chevron=false;

	m_chevronmenu=NULL;
	m_separator=NULL;
	m_add_or_remove_btns=NULL;
}

void wxFakeCommandBar::SetBar(wxFakeCommandBarDataBase* bar){
	m_bar=bar;
	m_btn.clear();
	m_overflow=false;
	//TODO: dirty

	m_pressed_idx=-1;
}

void wxFakeCommandBar::SetArtProvider(wxFakeCommandBarArtBase* artprov){
	m_artprov=artprov;
	if(m_bar) m_bar->SetDirty();
}

void wxFakeCommandBar::CreateChevronMenu(){
	if(m_chevronmenu==NULL){
		m_chevronmenu=new wxFakeCommandBarRefData;
		m_chevronmenu->Flags(fcbfToolBarMode);
	}else{
		m_chevronmenu->Buttons().clear();
	}

	m_chevronmenu->SetDirty();

	if(m_bar==NULL) return;

	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();
	int m=btns.size();

	if((int)m_btn.size()<m){
		m_chevronmenu->SetOldBar(m_bar);
		m_chevronmenu->SetIndexOffset(btns[m_btn.size()].index); //???
	}else{
		m_chevronmenu->SetOldBar(NULL);
		m_chevronmenu->SetIndexOffset(0);
	}

	for(int i=m_btn.size();i<m;i++){
		m_chevronmenu->Buttons().push_back(btns[i].btn);
	}

	if(m_chevron){
		if(!m_chevronmenu->Buttons().empty()){
			if(m_separator==NULL){
				m_separator=new wxFakeButtonData;
				m_separator->Type(fbttSeparator).Flags(fbtfStartNewRow);
			}
			m_chevronmenu->Buttons().push_back(m_separator);
		}

		if(m_add_or_remove_btns==NULL){
			m_add_or_remove_btns=new wxFakeButtonData;
			//TODO:
			m_add_or_remove_btns->Flags(fbtfShowDropdown).Caption("&Add or remove buttons").Enabled(false);
		}
		m_chevronmenu->Buttons().push_back(m_add_or_remove_btns);
	}
}

void wxFakeCommandBar::DestroyChevronMenu(){
	delete m_chevronmenu;
	m_chevronmenu=NULL;
	//TODO: other
}

bool wxFakeCommandBar::Layout(){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	m_btn.clear();
	m_overflow=false;
	m_chevron_add_flags=0;

	m_fullsize.x=0;
	m_fullsize.y=20;

	if(m_dockable && !m_undocked) m_fullsize.x+=artprov->GetToolbarGripperWidth();

	if(m_bar){
		if(m_bar->GetDirty()){
			wxDC *hdc;

			if(m_hdc) hdc=m_hdc;
			else hdc=new wxClientDC(this);

			m_bar->LayoutMenuButtons(*hdc,this,artprov);

			if(m_hdc==NULL) delete hdc;
		}

		wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();
		int m=btns.size();

		if(m>0){
			int x=0;
			if(m_dockable && !m_undocked) x=artprov->GetToolbarGripperWidth();

			if(m_bar->GetFlags() & fcbfMultiLineToolBar){
				int btn_left=0x7FFFFFFF,btn_top=0x7FFFFFFF;
				int btn_right=0,btn_bottom=0;

				for(int i=0;i<m;i++){
					const wxRect &r=btns[i].rect;

					if(r.x<btn_left) btn_left=r.x;
					if(r.y<btn_top) btn_top=r.y;
					if(r.x+r.width>btn_right) btn_right=r.x+r.width;
					if(r.y+r.height>btn_bottom) btn_bottom=r.y+r.height;
				}

				m_btn.resize(m);

				for(int i=0;i<m;i++){
					const wxRect &r=btns[i].rect;

					m_btn[i].rect=wxRect(r.x-btn_left+x,r.y-btn_top,r.width,r.height);
					m_btn[i].add_flags=0;
				}

				if(m_fullsize.x<x+btn_right-btn_left) m_fullsize.x=x+btn_right-btn_left;
				if(m_fullsize.y<btn_bottom-btn_top) m_fullsize.y=btn_bottom-btn_top;
			}else{
				int chevron_width=artprov->GetToolbarChevronWidth();

				wxSize sz=GetSize();

				if(m_direction==fcbdLeft || m_direction==fcbdRight){
					int tmp;
					tmp=sz.x;
					sz.x=sz.y;
					sz.y=tmp;
				}

				//TODO: layout multi-line (??)
				m_btn.reserve(m);

				//check button size
				for(int i=0;i<m;i++){
					if(m_overflow){
						m_fullsize.x+=btns[i].size.btnWidth;
						int h=btns[i].size.btnHeight;
						if(h>m_fullsize.y) m_fullsize.y=h;
					}else{
						wxFakeCommandBarButtonState st={};

						st.rect.x=x;
						st.rect.width=btns[i].size.btnWidth;

						m_fullsize.x+=st.rect.width;
						int h=btns[i].size.btnHeight;
						if(h>m_fullsize.y) m_fullsize.y=h;

						//check overflow
						int x2=x+st.rect.width;
						if(x2>sz.x || (m_chevron && !m_undocked && x2>sz.x-chevron_width)){
							m_overflow=true;
							continue;
						}

						m_btn.push_back(st);

						x=x2;
					}
				}

				//check again if overflow
				if((m_chevron || m_overflow) && !m_undocked){
					while(!m_btn.empty()){
						wxRect r=m_btn.back().rect;
						if(r.x+r.width>sz.x-chevron_width){
							m_btn.pop_back();
						}else{
							break;
						}
					}
				}

				//new: set the height of button again
				for(int i=0;i<(int)m_btn.size();i++){
					int h=btns[i].size.btnHeight;
					if(h<0) h=m_fullsize.y;
					if(h>sz.y) h=sz.y;

					m_btn[i].rect.y=(sz.y-h)/2;
					m_btn[i].rect.height=h;
				}
			}
		}
	}

	if(m_chevron && !m_undocked) m_fullsize.x+=artprov->GetToolbarChevronWidth();
	if(m_fullsize.x<20) m_fullsize.x=20;

	if(m_direction==fcbdLeft || m_direction==fcbdRight){
		for(int i=0;i<(int)m_btn.size();i++){
			int tmp;

			tmp=m_btn[i].rect.x;
			m_btn[i].rect.x=m_btn[i].rect.y;
			m_btn[i].rect.y=tmp;

			tmp=m_btn[i].rect.width;
			m_btn[i].rect.width=m_btn[i].rect.height;
			m_btn[i].rect.height=tmp;
		}
	}

	CreateChevronMenu();

	return true;
}

bool wxFakeCommandBar::Destroy(){
	if(HasCapture()) ReleaseMouse();

	if ( !wxPendingDelete.Member(this) )
		wxPendingDelete.Append(this);

	Show(false);

	return true;
}

bool wxFakeCommandBar::Redraw(wxDC& dc){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	wxSize sz=GetSize();
	int bar_flags=m_mainmenu?fcbfMainMenu:0;
	if(m_bar) bar_flags|=m_bar->GetFlags();

	wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

	//background and resize gripper
	{
		artprov->DrawToolbarBackground(dc,this,bar_flags,m_direction,wxRect(0,0,sz.x,sz.y),rcDockArea);
		if(m_dockable && !m_undocked){
			int w=artprov->GetToolbarGripperWidth();
			if(m_direction==fcbdLeft || m_direction==fcbdRight){
				artprov->DrawToolbarGripper(dc,this,bar_flags,m_direction,wxRect(0,0,sz.x,w),rcDockArea);
			}else{
				artprov->DrawToolbarGripper(dc,this,bar_flags,m_direction,wxRect(0,0,w,sz.y),rcDockArea);
			}
		}
	}


	if(m_bar){
		wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();
		int m=m_btn.size();

		for(int i=0;i<m;i++){
			wxRect r=m_btn[i].rect;

			artprov->DrawButton(dc,this,*btns[i].btn,bar_flags,true,m_direction,m_btn[i].add_flags,
				btns[i].size,r,sz,rcDockArea);
		}
	}

	//chevron
	if((m_overflow || m_chevron) && !m_undocked){
		int w=artprov->GetToolbarChevronWidth();
		if(m_direction==fcbdLeft || m_direction==fcbdRight){
			artprov->DrawToolbarChevron(dc,this,m_overflow,m_chevron,bar_flags,m_direction,m_chevron_add_flags,wxRect(0,sz.y-w,sz.x,w),rcDockArea);
		}else{
			artprov->DrawToolbarChevron(dc,this,m_overflow,m_chevron,bar_flags,m_direction,m_chevron_add_flags,wxRect(sz.x-w,0,w,sz.y),rcDockArea);
		}
	}

	//std::cout<<"Redraw\n";

	return true;
}

void wxFakeCommandBar::OnPaint(wxPaintEvent& event){
	if(!IsShown()) return;

	wxAutoBufferedPaintDC dc(this);
	if(!Redraw(dc)) event.Skip();
}

void wxFakeCommandBar::OnMouseMove(wxMouseEvent& event){
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

	int bar_flags=m_bar->GetFlags() | (m_mainmenu?fcbfMainMenu:0);
	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

	wxDC *dc=NULL;
	wxSize sz=GetSize();
#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
	bool bSetToolTipText=false;
#endif

	wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

	wxPoint p=event.GetPosition();

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

	//check gripper
	if(m_dockable && !m_undocked){
		int w=artprov->GetToolbarGripperWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,0,sz.x,w)
			:wxRect(0,0,w,sz.y);

		int add_flags=0;

		if(m_pressed_idx>=0) add_flags=(m_pressed_idx==m_gripper_idx)?(fbtaHighlight|fbtaPressed):0;
		else if(r.Contains(p)) add_flags=fbtaHighlight;

		if(add_flags!=m_gripper_add_flags){
			m_gripper_add_flags=add_flags;

#ifdef __WXGTK__
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZENWSE):wxNullCursor);
#else
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZING):wxNullCursor);
#endif
		}

		/*if(add_flags & fbtaPressed){
			//drag it
			if(m_dockmgr!=NULL){
				wxPoint p2=ClientToScreen(p);
				m_dockmgr->LayoutEx(this,wxPoint(p2.x-m_drag_offset.x,p2.y-m_drag_offset.y));
			}
		}*/
	}

	//check buttons
	for(int i=0;i<(int)m_btn.size();i++){
		wxFakeButtonType nType=btns[i].btn->Type();
		int nFlags=btns[i].btn->Flags();
		wxRect r=m_btn[i].rect;

		int add_flags=0;

		if((nFlags & fbtfHidden)==0){
			if(/*(nFlags & fbtfDisabled)==0 && */nType!=fbttSeparator && nType!=fbttColumnSeparator){
				if(r.Contains(p)){
					add_flags=m_pressed_idx>=0?(m_pressed_idx==i?(fbtaHighlight|fbtaPressed):0):fbtaHighlight;
				}else{
					add_flags=0;
				}
			}
		}

		//check popup submenu
		if(m_submenu_idx>=0){
			if(m_submenu_idx==i){
				add_flags=m_btn[i].add_flags;
			}else if(add_flags==fbtaHighlight){
				wxFakeCommandBarDataBase *submenu=(wxFakeCommandBarDataBase*)btns[i].btn->SubMenu();
				if(submenu!=NULL){
					//popup submenu
					PopupSubMenu(i,r,submenu,artprov);
					add_flags|=fbtaPopupMenu;
				}
			}
		}

		//draw updated button only
		if(add_flags!=m_btn[i].add_flags){
			m_btn[i].add_flags=add_flags;

			if((nFlags & fbtfDisabled)==0){
				if(dc==NULL) dc=new wxClientDC(this);
				artprov->DrawButton(*dc,this,*btns[i].btn,bar_flags,true,m_direction,m_btn[i].add_flags | fbtaRedrawBackground,
					btns[i].size,r,sz,rcDockArea);
			}

#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
			if(add_flags & fbtaHighlight){
				if(m_submenu!=NULL){
					UnsetToolTip(); //??
				}else{
					const wxString& s=btns[i].btn->ToolTipText();
					//strange, it keeps flickering
					SetToolTip(s);
				}
			}
#endif
		}

#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
		if(add_flags & fbtaHighlight) bSetToolTipText=true;
#endif
	}

	//check the chevron menu
	if((m_overflow || m_chevron) && !m_undocked){
		int w=artprov->GetToolbarChevronWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,sz.y-w,sz.x,w)
			:wxRect(sz.x-w,0,w,sz.y);

		int add_flags=0;

		if(r.Contains(p)){
			add_flags=m_pressed_idx>=0?(m_pressed_idx==m_chevron_idx?(fbtaHighlight|fbtaPressed):0):fbtaHighlight;
		}else{
			add_flags=0;
		}

		//check popup submenu
		if(m_submenu_idx>=0){
			if(m_submenu_idx==m_chevron_idx){
				add_flags=m_chevron_add_flags;
			}else if(add_flags==fbtaHighlight){
				if(m_chevronmenu!=NULL){
					//popup submenu
					PopupSubMenu(m_chevron_idx,r,m_chevronmenu,artprov);
					add_flags|=fbtaPopupMenu;
				}
			}
		}

		//draw updated button only
		if(add_flags!=m_chevron_add_flags){
			m_chevron_add_flags=add_flags;

			if(dc==NULL) dc=new wxClientDC(this);
			artprov->DrawToolbarChevron(*dc,this,m_overflow,m_chevron,bar_flags,m_direction,add_flags,r,rcDockArea);

#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
			if(add_flags & fbtaHighlight){
				wxString s=m_chevron?"Toolbar Options":"More Buttons";
				SetToolTip(s);
			}
#endif
		}

#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
		if(add_flags & fbtaHighlight) bSetToolTipText=true;
#endif
	}

	//over
	if(dc) delete dc;

#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
	if(!bSetToolTipText){
		wxToolTip *tooltip=GetToolTip();
		if(tooltip && !(tooltip->GetTip().empty())){
			tooltip->SetTip(wxEmptyString);
		}
	}
#endif

	event.Skip();
}

void wxFakeCommandBar::PopupSubMenu(int i,const wxRect& r,wxFakeCommandBarDataBase* submenu,wxFakeCommandBarArtBase* artprov){
#ifdef WXCOMMANDBAR_SHOW_TOOLTIP
	UnsetToolTip(); //??
#endif

	//hide old popup submenu
	if(m_submenu){
		m_submenu->Destroy();
		m_submenu=NULL;
	}

	m_submenu=new wxFakeMenuPopupWindow();
	m_submenu->SetBar(submenu);
	m_submenu->SetArtProvider(m_artprov);
	m_submenu->m_parentbar=this;
	m_submenu->m_dockmgr=m_dockmgr;
	m_submenu->Create(m_parent);

	wxPoint pp=ClientToScreen(r.GetTopLeft());

	int leftMargin,topMargin,rightMargin,bottomMargin;
	int dir;

	//TODO: direction
	if(i==m_chevron_idx){
		artprov->GetChevronMenuPopupMargin(leftMargin,topMargin,rightMargin,bottomMargin);
	}else{
		artprov->GetButtonPopupMargin(leftMargin,topMargin,rightMargin,bottomMargin);
	}
	switch(m_direction){
	case fcbdBottom:
		dir=wxUP;
		break;
	case fcbdLeft:
		dir=wxRIGHT;
		break;
	case fcbdRight:
		dir=wxLEFT;
		break;
	default:
		dir=wxDOWN;
		break;
	}

	m_submenu_idx=i;

	m_submenu->Layout();
	m_submenu->PositionEx(pp,r.GetSize(),dir,leftMargin,topMargin,rightMargin,bottomMargin);
	m_submenu->Show();
}

void wxFakeCommandBar::OnMouseDown(wxMouseEvent& event){
	if(!HasCapture()) CaptureMouse();
	if(!IsShown()) return;

	//forward mouse event to submenu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(event.GetEventType()==wxEVT_LEFT_DOWN){
		if(ForwardMouseEvent(event)) return;
		else{
			//click outside the menu, we should close the menu
			if(m_submenu){
				m_submenu->Destroy();
				m_submenu=NULL;
			}
		}
	}
#endif

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL || m_bar==NULL) return;

	int bar_flags=m_bar->GetFlags() | (m_mainmenu?fcbfMainMenu:0);
	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

	wxDC *dc=NULL;

	wxPoint p=event.GetPosition();

	wxSize sz=GetSize();
	wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

	//check if clicked outside the menu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(!(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y)){
		if(m_submenu){
			m_submenu->Destroy();
			m_submenu=NULL;
		}
	}
#endif

	//check gripper
	if(m_dockable && !m_undocked){
		int w=artprov->GetToolbarGripperWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,0,sz.x,w)
			:wxRect(0,0,w,sz.y);

		if(r.Contains(p)){
			//hide old popup submenu
			if(m_submenu){
				m_submenu->Destroy();
				m_submenu=NULL;
			}

			m_gripper_add_flags=fbtaHighlight|fbtaPressed;

			//new: using dock manager to perfrom dragging
			if(m_dockmgr!=NULL){
				m_pressed_idx=-1;
				if(HasCapture()) ReleaseMouse();
				m_dockmgr->BeginDragCommandBar(this,p);
				return;
			}

			//update pressed index
			m_pressed_idx=m_gripper_idx;
		}
	}

	//check buttons
	for(int i=0;i<(int)m_btn.size();i++){
		wxFakeButtonType nType=btns[i].btn->Type();
		int nFlags=btns[i].btn->Flags();
		wxRect r=m_btn[i].rect;

		if((nFlags & fbtfHidden)==0){

			if((nFlags & fbtfDisabled)==0 && nType!=fbttSeparator && nType!=fbttColumnSeparator){
				if(r.Contains(p)){
					int add_flags=fbtaHighlight|fbtaPressed;

					//check popup submenu
					wxFakeCommandBarDataBase *submenu=(wxFakeCommandBarDataBase*)btns[i].btn->SubMenu();
					if(submenu!=NULL){
						wxFakeButtonHitTestState state=artprov->HitTest(*btns[i].btn,
							bar_flags,true,m_direction,p,btns[i].size,r);
						if(nType!=fbttSplit || state==fbhtSplit){
							wxLongLong new_time=wxGetLocalTimeMillis();

							if(m_submenu_idx==i || new_time-m_submenu_time<50){
								//hide old popup submenu
								if(m_submenu){
									m_submenu->Destroy();
									m_submenu=NULL;
								}

								m_submenu_time=0;
							}else{
								//popup submenu
								PopupSubMenu(i,r,submenu,artprov);
								add_flags|=fbtaPopupMenu;
							}
						}
					}

					//draw updated button only
					if(add_flags!=m_btn[i].add_flags){
						m_btn[i].add_flags=add_flags;

						if(dc==NULL) dc=new wxClientDC(this);
						artprov->DrawButton(*dc,this,*btns[i].btn,bar_flags,true,m_direction,m_btn[i].add_flags | fbtaRedrawBackground,
							btns[i].size,r,sz,rcDockArea);
					}

					//update pressed index
					m_pressed_idx=i;

					break;
				}
			}
		}
	}

	//check the chevron menu
	if((m_overflow || m_chevron) && !m_undocked){
		int w=artprov->GetToolbarChevronWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,sz.y-w,sz.x,w)
			:wxRect(sz.x-w,0,w,sz.y);

		if(r.Contains(p)){
			int add_flags=fbtaHighlight|fbtaPressed;

			//check popup submenu
			if(m_chevronmenu!=NULL){
				wxLongLong new_time=wxGetLocalTimeMillis();

				if(m_submenu_idx==m_chevron_idx || new_time-m_submenu_time<50){
					//hide old popup submenu
					if(m_submenu){
						m_submenu->Destroy();
						m_submenu=NULL;
					}

					m_submenu_time=0;
				}else{
					//popup submenu
					PopupSubMenu(m_chevron_idx,r,m_chevronmenu,artprov);
					add_flags|=fbtaPopupMenu;
				}
			}

			//draw updated button only
			if(add_flags!=m_chevron_add_flags){
				m_chevron_add_flags=add_flags;

				if(dc==NULL) dc=new wxClientDC(this);
				artprov->DrawToolbarChevron(*dc,this,m_overflow,m_chevron,bar_flags,m_direction,add_flags,r,rcDockArea);
			}

			//update pressed index
			m_pressed_idx=m_chevron_idx;
		}
	}

	//over
	if(dc) delete dc;
}

void wxFakeCommandBar::OnMouseUp(wxMouseEvent& event){
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(m_submenu==NULL && HasCapture()) ReleaseMouse();
#else
	if(HasCapture()) ReleaseMouse();
#endif

	if(!IsShown()) return;

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL || m_bar==NULL) return;

	int bar_flags=m_bar->GetFlags() | (m_mainmenu?fcbfMainMenu:0);
	wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

	wxDC *dc=NULL;

	wxPoint p=event.GetPosition();

	wxSize sz=GetSize();
	wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

	//check gripper
	if(m_dockable && !m_undocked){
		int w=artprov->GetToolbarGripperWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,0,sz.x,w)
			:wxRect(0,0,w,sz.y);

		int add_flags=0;

		if(r.Contains(p)) add_flags=fbtaHighlight;

		if(add_flags!=m_gripper_add_flags){
			/*//check if we need to redraw all command bars
			if(m_gripper_add_flags & fbtaPressed){
				if(m_dockmgr){
					m_dockmgr->Refresh();
				}
			}*/

			m_gripper_add_flags=add_flags;

#ifdef __WXGTK__
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZENWSE):wxNullCursor);
#else
			SetCursor((add_flags & fbtaHighlight)!=0?wxCursor(wxCURSOR_SIZING):wxNullCursor);
#endif
		}
	}

	//check buttons
	for(int i=0;i<(int)m_btn.size();i++){
		wxFakeButtonType nType=btns[i].btn->Type();
		int nFlags=btns[i].btn->Flags();
		wxRect r=m_btn[i].rect;

		int add_flags=0;

		if((nFlags & fbtfHidden)==0){
			if((nFlags & fbtfDisabled)==0 && nType!=fbttSeparator && nType!=fbttColumnSeparator){
				if(r.Contains(p)){
					add_flags=fbtaHighlight; //???

					//check if button is clicked
					if(m_pressed_idx==i){
						wxFakeButtonHitTestState state=artprov->HitTest(*btns[i].btn,
							bar_flags,true,m_direction,p,btns[i].size,r);
						if(state!=fbhtSplit && m_submenu_idx!=i
							&& (btns[i].btn->Type()==fbttSplit || btns[i].btn->SubMenu()==NULL))
						{
							//raise event.
							btns[i].btn->OnClick(this,GetEventHandler(),btns[i].bar,btns[i].index);

							//check the parent object and tell them to update button state
							if(m_dockmgr!=NULL){
								m_dockmgr->UpdateOnNextIdle();
							}else{
								UpdateOnNextIdle();
							}
						}
					}
				}
			}
		}

		//check popup submenu
		if(m_submenu_idx==i) add_flags=m_btn[i].add_flags;

		//draw updated button only
		if(add_flags!=m_btn[i].add_flags){
			m_btn[i].add_flags=add_flags;

			if(dc==NULL) dc=new wxClientDC(this);
			artprov->DrawButton(*dc,this,*btns[i].btn,bar_flags,true,m_direction,m_btn[i].add_flags | fbtaRedrawBackground,
				btns[i].size,r,sz,rcDockArea);
		}
	}

	//check the chevron menu
	if((m_overflow || m_chevron) && !m_undocked){
		wxSize sz=GetSize();
		int w=artprov->GetToolbarChevronWidth();
		wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
			?wxRect(0,sz.y-w,sz.x,w)
			:wxRect(sz.x-w,0,w,sz.y);

		int add_flags=0;

		if(r.Contains(p)) add_flags=fbtaHighlight;

		//check popup submenu
		if(m_submenu_idx==m_chevron_idx) add_flags=m_chevron_add_flags;

		//draw updated button only
		if(add_flags!=m_chevron_add_flags){
			m_chevron_add_flags=add_flags;

			if(dc==NULL) dc=new wxClientDC(this);
			artprov->DrawToolbarChevron(*dc,this,m_overflow,m_chevron,bar_flags,m_direction,add_flags,r,rcDockArea);
		}
	}

	//reset pressed index
	m_pressed_idx=-1;

	//over
	if(dc) delete dc;

	//FIXME: should redraw all because after RaiseEvent button state updates
}

void wxFakeCommandBar::OnMouseWheel(wxMouseEvent& event){
	if(!IsShown()) return;

	//forward mouse event to submenu
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	if(event.GetEventType()==wxEVT_MOUSEWHEEL){
		if(ForwardMouseEvent(event)) return;
	}
#endif

	//TODO:
}

void wxFakeCommandBar::OnContextMenu(wxContextMenuEvent& event){
	if(!IsShown()) return;
	if(wxWindow::GetCapture()!=NULL) return;

	if(m_dockmgr!=NULL){
		m_dockmgr->PopupCustomCommandBarMenu();
		return;
	}
}

void wxFakeCommandBar::OnSubMenuDestroy(){
	//redraw
	if(IsShown() && m_bar!=NULL){
		wxFakeCommandBarArtBase* artprov=m_artprov;
		if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
		if(artprov!=NULL){
			int bar_flags=m_bar->GetFlags() | (m_mainmenu?fcbfMainMenu:0);
			wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

			wxSize sz=GetSize();
			wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

			if(m_submenu_idx>=0 && m_submenu_idx<(int)m_btn.size()){
				m_btn[m_submenu_idx].add_flags=0;

				wxClientDC dc(this);
				wxRect r=m_btn[m_submenu_idx].rect;
				artprov->DrawButton(dc,this,*btns[m_submenu_idx].btn,bar_flags,true,m_direction,
					m_btn[m_submenu_idx].add_flags | fbtaRedrawBackground,btns[m_submenu_idx].size,r,sz,rcDockArea);
			}else if(m_submenu_idx==m_chevron_idx){
				m_chevron_add_flags=0;

				wxClientDC dc(this);

				int w=artprov->GetToolbarChevronWidth();
				wxRect r=(m_direction==fcbdLeft || m_direction==fcbdRight)
					?wxRect(0,sz.y-w,sz.x,w)
					:wxRect(sz.x-w,0,w,sz.y);

				artprov->DrawToolbarChevron(dc,this,m_overflow,m_chevron,bar_flags,m_direction,m_chevron_add_flags,r,rcDockArea);
			}
		}
	}

	m_submenu=NULL;
	m_submenu_idx=-1;

	//something weird
	m_submenu_time=wxGetLocalTimeMillis();
}

void wxFakeCommandBar::OnTimer(wxTimerEvent& event){
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

void wxFakeCommandBar::IsMainMenu(bool b){
	m_mainmenu=b;
}

void wxFakeCommandBar::Dockable(bool b){
	m_dockable=b;
}

void wxFakeCommandBar::Undocked(bool b){
	m_undocked=b;
}

void wxFakeCommandBar::ShowChevron(bool b){
	m_chevron=b;
}

void wxFakeCommandBar::Direction(int dir){
	m_direction=dir;
	//TODO: refresh
}

void wxFakeCommandBar::OnMouseCaptureLost(wxMouseCaptureLostEvent& event){
#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
	//hide old popup submenu
	if(m_submenu){
		m_submenu->Destroy();
		m_submenu=NULL;
	}
#endif
}

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE

bool wxFakeCommandBar::ForwardMouseEvent(wxMouseEvent& event){
	wxFakeMenuPopupWindow *submenu=m_submenu;
	if(submenu!=NULL){
		while(submenu->m_submenu!=NULL){
			submenu=submenu->m_submenu;
		}

		wxPoint pGlobal=ClientToScreen(event.GetPosition());

		while(submenu!=NULL){
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

#endif

void wxFakeCommandBar::UpdateDirty(bool unDirty){
	if(IsShown() && m_bar!=NULL){
		//get art provider
		wxFakeCommandBarArtBase* artprov=m_artprov;
		if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
		if(artprov==NULL) return;

		//FIXME: if there is two identical command bar then serious bug occurs (?)
		if(m_bar->UpdateDirty()){
			std::cout<<"Bar size updated\n"; //debug
			Layout();
			Refresh();
		}else{
			int bar_flags=m_bar->GetFlags() | (m_mainmenu?fcbfMainMenu:0);
			wxVector<wxFakeButtonState> &btns=m_bar->GetLayoutedButtons();

			wxRect rcDockArea=(m_dockable && !m_undocked)?wxRect(-GetPosition(),m_parent->GetClientSize()):wxRect();

			wxSize sz=GetSize();
			wxClientDC *dc=NULL;

			for(int i=0,m=btns.size();i<m;i++){
				if(btns[i].btn->Dirty()){
					std::cout<<"Button state updated\n"; //debug

					//redraw it
					if(dc==NULL) dc=new wxClientDC(this);
					artprov->DrawButton(*dc,this,*btns[i].btn,bar_flags,true,m_direction,m_btn[i].add_flags | fbtaRedrawBackground,
						btns[i].size,m_btn[i].rect,sz,rcDockArea);
				}
			}

			if(dc) delete dc;
		}

		if(unDirty) UnDirty();
	}
}

void wxFakeCommandBar::UnDirty(){
	for(int i=0,m=m_bar->ButtonCount();i<m;i++){
		m_bar->GetButton(i)->UnDirty();
	}
	m_bar->SetDirty(false);
}

void wxFakeCommandBar::OnIdle(wxIdleEvent& event){
	if(m_update_on_next_idle){
		m_update_on_next_idle=false;
		UpdateDirty();
	}

	event.Skip();
}
