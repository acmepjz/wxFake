#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/cursor.h>
#include <wx/event.h>
#include <iostream>
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeCommandBarFrame.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeMiniFrame.hpp"

#include "wxFake/wxFakeGTKCursor.inl"

IMPLEMENT_CLASS(wxFakeMiniFrame,wxFrame)

BEGIN_EVENT_TABLE(wxFakeMiniFrame,wxFrame)

EVT_PAINT( wxFakeMiniFrame::OnPaint )
EVT_MOTION( wxFakeMiniFrame::OnMouseMove )
EVT_LEFT_DOWN( wxFakeMiniFrame::OnMouseDown )
EVT_LEFT_UP( wxFakeMiniFrame::OnMouseUp )
EVT_TIMER( 1, wxFakeMiniFrame::OnTimer )
EVT_CONTEXT_MENU( wxFakeMiniFrame::OnContextMenu )

#ifdef __WXGTK__
EVT_TIMER( 44, wxFakeMiniFrame::OnStupidTimer )
#endif

END_EVENT_TABLE()

void wxFakeMiniFrame::Init(){
	m_artprov=NULL;
	m_timer=NULL;

	m_fake_style=0;

	m_highlight_idx=-1;
	m_pressed_idx=-1;
	m_submenu_idx=-1;

	m_dockmgr=NULL;

	m_submenu=NULL;

	m_old_height=0;
	m_clientwnd=NULL;
#ifdef __WXGTK__
	m_stupid_timer=NULL;
#endif
}

wxFakeMiniFrame::~wxFakeMiniFrame(){
	delete m_timer;
#ifdef __WXGTK__
	delete m_stupid_timer;
#endif
}

bool wxFakeMiniFrame::Create(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name){
	m_fake_style=style;

	int x=size.x,y=size.y;
	if(x<32) x=32;
	if(y<32) y=32;

	if(style & fpstMinimized){
		m_old_height=y;

		//get art provider
		wxFakeCommandBarArtBase* artprov=m_artprov;
		if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
		if(artprov!=NULL){
			y=artprov->GetMinimizedMiniFrameHeight(style);
		}
	}

	if(!wxFrame::Create(parent,id,title,pos,wxSize(x,y),
#ifdef __WXMSW__
		wxBORDER_NONE|wxFRAME_NO_TASKBAR|wxFRAME_FLOAT_ON_PARENT|wxFULL_REPAINT_ON_RESIZE|wxFRAME_TOOL_WINDOW,
#else
		wxBORDER_NONE|wxFRAME_NO_TASKBAR|wxFRAME_FLOAT_ON_PARENT|wxFULL_REPAINT_ON_RESIZE,
#endif
		name)) return false;

	SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_timer=new wxTimer(this,1);

#ifdef __WXGTK__
	//dirty hack
	m_stupid_timer=new wxTimer(this,44);
	SetSize(x,y);
#endif

	SetAutoLayout(true);

	return true;
}

void wxFakeMiniFrame::SetArtProvider(wxFakeCommandBarArtBase* artprov){
	m_artprov=artprov;
}

void wxFakeMiniFrame::SetStyle(int style){
	m_fake_style=style;
	//TODO: refresh
}

bool wxFakeMiniFrame::Redraw(wxDC& dc){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	wxSize sz=GetSize();

	//TEST ONLY. TODO:
	artprov->DrawMiniFrame(dc,this,GetTitle(),m_fake_style,m_highlight_idx,m_pressed_idx,m_submenu_idx,wxRect(0,0,sz.x,sz.y));

	return true;
}

void wxFakeMiniFrame::OnPaint(wxPaintEvent& event){
	if(!IsShown()) return;

	wxAutoBufferedPaintDC dc(this);
	if(!Redraw(dc)) event.Skip();
}

void wxFakeMiniFrame::OnMouseMove(wxMouseEvent& event){
	if(!IsShown()) return;

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return;

	wxSize sz=GetSize();
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

	//run hit test
	int i=artprov->MiniFrameHitTest(m_fake_style,p,wxRect(0,0,sz.x,sz.y));

	if(m_pressed_idx>=0 && i!=m_pressed_idx){
		i=(m_pressed_idx&0xFF)==0xFF?m_pressed_idx:(-1);
	}

	//check redraw and cursor
	if(i!=m_highlight_idx){
		if((i&0xFF)!=(m_highlight_idx&0xFF)) Refresh();

		switch(i){
#ifdef __WXGTK__
		case fphtGripper:
			SetCursor(wxFakeGTKCursor(GDK_FLEUR));
			break;
		case fphtSizeN:
			SetCursor(wxFakeGTKCursor(GDK_TOP_SIDE));
			break;
		case fphtSizeS:
			SetCursor(wxFakeGTKCursor(GDK_BOTTOM_SIDE));
			break;
		case fphtSizeW:
			SetCursor(wxFakeGTKCursor(GDK_LEFT_SIDE));
			break;
		case fphtSizeE:
			SetCursor(wxFakeGTKCursor(GDK_RIGHT_SIDE));
			break;
		case fphtSizeNW:
			SetCursor(wxFakeGTKCursor(GDK_TOP_LEFT_CORNER));
			break;
		case fphtSizeSE:
			SetCursor(wxFakeGTKCursor(GDK_BOTTOM_RIGHT_CORNER));
			break;
		case fphtSizeSW:
			SetCursor(wxFakeGTKCursor(GDK_BOTTOM_LEFT_CORNER));
			break;
		case fphtSizeNE:
			SetCursor(wxFakeGTKCursor(GDK_TOP_RIGHT_CORNER));
			break;
#else
		case fphtGripper:
			SetCursor(wxCursor(wxCURSOR_SIZING));
			break;
		case fphtSizeN:
		case fphtSizeS:
			SetCursor(wxCursor(wxCURSOR_SIZENS));
			break;
		case fphtSizeW:
		case fphtSizeE:
			SetCursor(wxCursor(wxCURSOR_SIZEWE));
			break;
		case fphtSizeNW:
		case fphtSizeSE:
			SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			break;
		case fphtSizeSW:
		case fphtSizeNE:
			SetCursor(wxCursor(wxCURSOR_SIZENESW));
			break;
#endif
		default:
			SetCursor(wxNullCursor);
			break;
		}

		m_highlight_idx=i;
	}

	//check dragging
	if(m_pressed_idx>=fphtGripper){
		wxPoint p1=ClientToScreen(p);
		p1.x-=m_drag_offset.x;
		p1.y-=m_drag_offset.y;

		wxRect r=GetScreenRect();

		int minWidth=m_minWidth,maxWidth=m_maxWidth;
		if(minWidth<32) minWidth=32;
		if(maxWidth<0) maxWidth=0x7FFFFFFF;
		else if(maxWidth<minWidth) maxWidth=minWidth;

		int minHeight=m_minHeight,maxHeight=m_maxHeight;
		if(minHeight<32) minHeight=32;
		if(maxHeight<0) maxHeight=0x7FFFFFFF;
		else if(maxHeight<minHeight) maxHeight=minHeight;

		switch(m_pressed_idx){
		case fphtSizeNW:
			if(r.x+r.width-p1.x<minWidth) p1.x=r.x+r.width-minWidth;
			else if(r.x+r.width-p1.x>maxWidth) p1.x=r.x+r.width-maxWidth;
			if(r.y+r.height-p1.y<minHeight) p1.y=r.y+r.height-minHeight;
			else if(r.y+r.height-p1.y>maxHeight) p1.y=r.y+r.height-maxHeight;
			SetSize(p1.x,p1.y,r.x+r.width-p1.x,r.y+r.height-p1.y);
			break;
		case fphtSizeN:
			if(r.y+r.height-p1.y<minHeight) p1.y=r.y+r.height-minHeight;
			else if(r.y+r.height-p1.y>maxHeight) p1.y=r.y+r.height-maxHeight;
			SetSize(r.x,p1.y,r.width,r.y+r.height-p1.y);
			break;
		case fphtSizeW:
			if(r.x+r.width-p1.x<minWidth) p1.x=r.x+r.width-minWidth;
			else if(r.x+r.width-p1.x>maxWidth) p1.x=r.x+r.width-maxWidth;
			SetSize(p1.x,r.y,r.x+r.width-p1.x,r.height);
			break;
		case fphtGripper:
			/*//check dock manager
			if(m_dockmgr!=NULL && (m_fake_style & fpstTemporaryDisableDock)==0){
				if(m_fake_style & fpstToolbarMode){
					wxFakeCommandBar *bar=wxDynamicCast(m_clientwnd,wxFakeCommandBar);
					if(bar){
						if(m_dockmgr->LayoutEx(bar,p1)) break;
					}
				}
			}*/
			Move(p1);
			break;
		case fphtSizeNE:
			p1.x-=r.x;
			if(p1.x<minWidth) p1.x=minWidth;
			else if(p1.x>maxWidth) p1.x=maxWidth;
			if(r.y+r.height-p1.y<minHeight) p1.y=r.y+r.height-minHeight;
			else if(r.y+r.height-p1.y>maxHeight) p1.y=r.y+r.height-maxHeight;
			SetSize(r.x,p1.y,p1.x,r.y+r.height-p1.y);
			break;
		case fphtSizeSW:
			if(r.x+r.width-p1.x<minWidth) p1.x=r.x+r.width-minWidth;
			else if(r.x+r.width-p1.x>maxWidth) p1.x=r.x+r.width-maxWidth;
			p1.y-=r.y;
			if(p1.y<minHeight) p1.y=minHeight;
			else if(p1.y>maxHeight) p1.y=maxHeight;
			SetSize(p1.x,r.y,r.x+r.width-p1.x,p1.y);
			break;
		case fphtSizeS:
			p1.y-=r.y;
			if(p1.y<minHeight) p1.y=minHeight;
			else if(p1.y>maxHeight) p1.y=maxHeight;
			SetSize(r.x,r.y,r.width,p1.y);
			break;
		case fphtSizeE:
			p1.x-=r.x;
			if(p1.x<minWidth) p1.x=minWidth;
			else if(p1.x>maxWidth) p1.x=maxWidth;
			SetSize(r.x,r.y,p1.x,r.height);
			break;
		case fphtSizeSE:
			p1.x-=r.x;
			if(p1.x<minWidth) p1.x=minWidth;
			else if(p1.x>maxWidth) p1.x=maxWidth;
			p1.y-=r.y;
			if(p1.y<minHeight) p1.y=minHeight;
			else if(p1.y>maxHeight) p1.y=maxHeight;
			SetSize(r.x,r.y,p1.x,p1.y);
			break;
		default:
			break;
		}
	}

	event.Skip();
}

void wxFakeMiniFrame::OnMouseDown(wxMouseEvent& event){
	if(!HasCapture()) CaptureMouse();
	if(!IsShown()) return;

	//set pressed index
	if(m_highlight_idx>=0){
		m_pressed_idx=m_highlight_idx;
		Refresh();

		//check popup menu
		if(m_pressed_idx==fphtCustom || m_pressed_idx==fphtDropdown){
			//get art provider
			wxFakeCommandBarArtBase* artprov=m_artprov;
			if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
			if(artprov!=NULL){
				wxSize sz=GetSize();
				wxRect r;
				if(artprov->GetMiniFrameButtonRect(m_pressed_idx,m_fake_style,wxRect(0,0,sz.x,sz.y),r)){
					//ugly code
					if(HasCapture()) ReleaseMouse();

					//popup submenu
					wxFakeCommandBarDataBase *submenu=NULL;

					//TODO: get popup menu
					switch(m_pressed_idx){
					case fphtCustom:
						if(m_fake_style & fpstToolbarMode){
							wxFakeCommandBar *bar=wxDynamicCast(m_clientwnd,wxFakeCommandBar);
							if(bar){
								submenu=bar->m_chevronmenu;
							}
						}
						break;
					case fphtDropdown:
						break;
					default:
						break;
					}

					//hide old popup submenu
					if(m_submenu){
						m_submenu->Destroy();
						m_submenu=NULL;
					}

					//create new one
					m_submenu=new wxFakeMenuPopupWindow();
					m_submenu->SetBar(submenu);
					m_submenu->SetArtProvider(m_artprov);
					m_submenu->m_parentpane=this;
					m_submenu->m_dockmgr=m_dockmgr;
					m_submenu->Create(this);

					wxPoint pp=ClientToScreen(r.GetTopLeft());

					int leftMargin,topMargin,rightMargin,bottomMargin;
					artprov->GetButtonPopupMargin(leftMargin,topMargin,rightMargin,bottomMargin);

					m_submenu_idx=m_pressed_idx;

					m_submenu->Layout();
					m_submenu->PositionEx(pp,r.GetSize(),wxDOWN,leftMargin,topMargin,rightMargin,bottomMargin);
					m_submenu->Show();
				}
			}
		}

		//check start dragging
		if(m_pressed_idx>=fphtGripper){
			wxPoint p=event.GetPosition();
			wxSize sz=GetSize();
			switch(m_pressed_idx){
			case fphtSizeNW:
			case fphtSizeN:
			case fphtSizeW:
				m_drag_offset=p;
				break;
			case fphtGripper:
				//check dock manager
				if(m_dockmgr!=NULL && (m_fake_style & fpstTemporaryDisableDock)==0){
					if(m_fake_style & fpstToolbarMode){
						wxFakeCommandBar *bar=wxDynamicCast(m_clientwnd,wxFakeCommandBar);
						if(bar){
							m_highlight_idx=-1;
							m_pressed_idx=-1;
							if(HasCapture()) ReleaseMouse();
							if(m_dockmgr->BeginDragCommandBar(bar,p)) break;
						}
					}
				}
				m_drag_offset=p;
				break;
			case fphtSizeNE:
				m_drag_offset=wxPoint(p.x-sz.x,p.y);
				break;
			case fphtSizeSW:
				m_drag_offset=wxPoint(p.x,p.y-sz.y);
				break;
			case fphtSizeS:
			case fphtSizeE:
			case fphtSizeSE:
				m_drag_offset=wxPoint(p.x-sz.x,p.y-sz.y);
				break;
			default:
				break;
			}
		}
	}

	event.Skip();
}

void wxFakeMiniFrame::OnMouseUp(wxMouseEvent& event){
	if(HasCapture()) ReleaseMouse();
	if(!IsShown()) return;

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return;

	//reset flags
	m_fake_style&=~fpstTemporaryDisableDock;

	//TODO: check button pressed, etc.
	if(m_highlight_idx==m_pressed_idx && m_pressed_idx>=0){
		switch(m_pressed_idx){
		case fphtClose:
			{
				if(m_dockmgr!=NULL){
					if(m_fake_style & fpstToolbarMode){
						if(m_dockmgr->ShowCommandBar(this,false)) return;
					}else{
						//TODO:
					}
				}
				Destroy();
				return;
			}
			break;
		case fphtMinimize:
			{
				if(m_fake_style & fpstMinimized){
					wxRect r=GetScreenRect();

					r.height=m_old_height;
					if(r.height<32) r.height=32;

					m_fake_style&=~fpstMinimized;

					SetSize(r);
				}else{
					wxRect r=GetScreenRect();

					m_old_height=r.height;
					r.height=artprov->GetMinimizedMiniFrameHeight(m_fake_style);

					m_fake_style|=fpstMinimized;

					SetSize(r);
				}
			}
			break;
		case fphtPin:
			std::cout<<"cmdPin_Click\n";
			break;
		default:
			break;
		}
	}

	//reset pressed index
	if(m_pressed_idx>=0){
		m_pressed_idx=-1;
		Refresh();
	}

	event.Skip();
}

void wxFakeMiniFrame::OnContextMenu(wxContextMenuEvent& event){
	if(!IsShown()) return;
	if(wxWindow::GetCapture()!=NULL) return;

	if(m_dockmgr!=NULL){
		m_dockmgr->PopupCustomCommandBarMenu();
		return;
	}
}

void wxFakeMiniFrame::OnTimer(wxTimerEvent& event){
	wxSize sz=GetSize();
	wxPoint p=ScreenToClient(wxGetMouseState().GetPosition());

	bool b=false;

	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov!=NULL && (m_fake_style & fpstMinimized)==0){
		int leftMargin,topMargin,rightMargin,bottomMargin;
		artprov->GetMiniFrameClientSizeMargin(m_fake_style,leftMargin,topMargin,rightMargin,bottomMargin);
		if(p.x>=leftMargin && p.y>=topMargin && p.x<sz.x-rightMargin && p.y<sz.y-bottomMargin){
			b=true;
		}
	}

	//use the old-fashioned timer method to detect mouse leave event
	if(b || !(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y)){
		if(m_timer->IsRunning()){
			m_timer->Stop();
		}

		wxMouseEvent evt2(wxEVT_LEAVE_WINDOW);
		evt2.m_x=p.x;
		evt2.m_y=p.y;
		OnMouseMove(evt2);
	}
}

bool wxFakeMiniFrame::Destroy(){
	if(HasCapture()) ReleaseMouse();

	if ( !wxPendingDelete.Member(this) )
		wxPendingDelete.Append(this);

	Show(false);

	return true;
}

void wxFakeMiniFrame::OnSubMenuDestroy(){
	m_pressed_idx=-1;
	m_submenu_idx=-1;
	m_submenu=NULL;

	{
		wxPoint p=ScreenToClient(wxGetMouseState().GetPosition());
		wxMouseEvent evt2(wxEVT_MOTION);
		evt2.m_x=p.x;
		evt2.m_y=p.y;
		OnMouseMove(evt2);
	}

	Refresh();
}

void wxFakeMiniFrame::SetClientWindow(wxWindow* wnd){
	m_clientwnd=wnd;
	Layout();
}

bool wxFakeMiniFrame::Layout(){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov!=NULL && m_clientwnd!=NULL){
		if(m_fake_style & fpstMinimized){
			if(m_clientwnd->IsShown()){
				m_clientwnd->Show(false);
			}
		}else{
			int leftMargin,topMargin,rightMargin,bottomMargin;
			artprov->GetMiniFrameClientSizeMargin(m_fake_style,leftMargin,topMargin,rightMargin,bottomMargin);

			wxSize sz=GetSize();
			m_clientwnd->SetSize(leftMargin,topMargin,sz.x-leftMargin-rightMargin,sz.y-topMargin-bottomMargin);

			if(!m_clientwnd->IsShown()){
				m_clientwnd->Show(true);
			}
		}
	}

	return true;
}

void wxFakeMiniFrame::DoSetClientSize(int width,int height){
	int leftMargin=0,topMargin=0,rightMargin=0,bottomMargin=0;

	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov!=NULL){
		artprov->GetMiniFrameClientSizeMargin(m_fake_style,leftMargin,topMargin,rightMargin,bottomMargin);
	}

	wxSize sz=GetSize();
	sz.x=width+leftMargin+rightMargin;

	if(m_fake_style & fpstMinimized){
		m_old_height=height+topMargin+bottomMargin;
	}else{
		sz.y=height+topMargin+bottomMargin;
	}
	SetSize(sz);
}

#ifdef __WXGTK__

void wxFakeMiniFrame::DoSetSize(int x, int y, int width, int height, int sizeFlags){
	if(width>0 && height>0){
		m_stupid_size.x=width;
		m_stupid_size.y=height;

		if(!m_stupid_timer->IsRunning()){
			m_stupid_timer->Start(200);
		}
	}

	wxFrame::DoSetSize(x,y,width,height,sizeFlags);
}

void wxFakeMiniFrame::OnStupidTimer(wxTimerEvent& event){
	wxSize sz=GetSize();

	if(sz.x==m_stupid_size.x && sz.y==m_stupid_size.y){
		if(m_stupid_timer->IsRunning()){
			m_stupid_timer->Stop();
		}
	}else{
		//do some stupid thing
		std::cout<<"Something goes wrong "<<sz.x<<","<<sz.y<<" should be "<<m_stupid_size.x<<","<<m_stupid_size.y<<"\n";
		wxPoint pos=GetPosition();
		wxFrame::DoSetSize( pos.x, pos.y, m_stupid_size.x, m_stupid_size.y, 3 );
	}
}

#endif
