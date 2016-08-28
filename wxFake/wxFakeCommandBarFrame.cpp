#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include <math.h>
#include <iostream>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeCommandBarFrame.hpp"
#include "wxFake/wxFakeMiniFrame.hpp"

IMPLEMENT_CLASS(wxFakeCommandBarFrame,wxFrame)

BEGIN_EVENT_TABLE(wxFakeCommandBarFrame,wxFrame)

EVT_PAINT( wxFakeCommandBarFrame::OnPaint )
EVT_MOTION( wxFakeCommandBarFrame::OnMouseMove )
EVT_LEFT_DOWN( wxFakeCommandBarFrame::OnMouseDown )
EVT_LEFT_UP( wxFakeCommandBarFrame::OnMouseUp )
EVT_ACTIVATE( wxFakeCommandBarFrame::OnActivate )
EVT_CONTEXT_MENU( wxFakeCommandBarFrame::OnContextMenu )
EVT_MOUSE_CAPTURE_LOST( wxFakeCommandBarFrame::OnMouseCaptureLost )
EVT_IDLE( wxFakeCommandBarFrame::OnIdle )
EVT_TIMER( 1, wxFakeCommandBarFrame::OnTimer )

END_EVENT_TABLE()

void wxFakeCommandBarRow::Destroy(){
	for(unsigned int i=0;i<m_bars.size();i++){
		m_bars[i]->Destroy();
	}

	m_bars.clear();
	m_size=0;
}

void wxFakeCommandBarFrame::Init(){
	m_drag_index=-1;
	m_dockablebars_dirty=true;
	m_temporary_disable_dock=false;
	m_update_on_next_idle=false;
	m_dockablebars_menu=NULL;
	m_artprov=NULL;
	m_clientwnd=NULL;
	m_timer=NULL;

	m_pane_drag.m_layer=-1;
	m_pane_drag.m_dir=-1;
	m_pane_drag.m_row=-1;
	m_pane_drag.m_idx=-1;
	m_pane_drag.m_subrow=NULL;
}

bool wxFakeCommandBarFrame::Create(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name){
	if(!wxFrame::Create(parent,id,title,pos,size,style | wxFULL_REPAINT_ON_RESIZE,name)) return false;

	static unsigned char data[] = { 0,0,0,192,192,192, 192,192,192,0,0,0 };

	m_stipple_brush=wxBrush(wxBitmap(wxImage(2,2,data,true)));

	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetAutoLayout(true);

	wxEvtHandler::AddFilter(this);

	m_timer=new wxTimer(this,1);

	return true;
}

void wxFakeCommandBarFrame::DrawResizeHint(wxDC& dc, const wxRect& rect){
	dc.SetBrush(m_stipple_brush);
#ifdef __WXMSW__
	::PatBlt((HDC)dc.GetHDC(), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), PATINVERT);
#else
	dc.SetPen(*wxTRANSPARENT_PEN);

	wxRasterOperationMode rop=dc.GetLogicalFunction();
	dc.SetLogicalFunction(wxXOR);
	dc.DrawRectangle(rect);
	dc.SetLogicalFunction(rop);
#endif
}

void wxFakeCommandBarFrame::SetArtProvider(wxFakeCommandBarArtBase* artprov){
	m_artprov=artprov;
}

//add button with specified id.
wxFakeButtonData* wxFakeCommandBarFrame::AddButton(int id){
	wxFakeButtonData *btn=new wxFakeButtonData(id);
	m_btns.insert(std::pair<int,wxFakeButtonData*>(id,btn));
	return btn;
}

//add button from existing button (will create a copy).
wxFakeButtonData* wxFakeCommandBarFrame::AddButton(const wxFakeButtonData& btn){
	wxFakeButtonData *btn2=new wxFakeButtonData(btn);
	m_btns.insert(std::pair<int,wxFakeButtonData*>(btn.ID(),btn2));
	return btn2;
}

//add button from pointer. The ownership of this button will be
//transferd to button manager
wxFakeButtonData* wxFakeCommandBarFrame::AddButton(wxFakeButtonData* btn){
	m_btns.insert(std::pair<int,wxFakeButtonData*>(btn->ID(),btn));
	return btn;
}

//add command bar with specified id.
wxFakeCommandBarRefData* wxFakeCommandBarFrame::AddBarData(int id){
	wxFakeCommandBarRefData *bar=new wxFakeCommandBarRefData(id);
	m_bars.insert(std::pair<int,wxFakeCommandBarRefData*>(id,bar));
	return bar;
}

//add command bar from existing one (will create a copy).
wxFakeCommandBarRefData* wxFakeCommandBarFrame::AddBarData(const wxFakeCommandBarRefData& bar){
	wxFakeCommandBarRefData *bar2=new wxFakeCommandBarRefData(bar);
	m_bars.insert(std::pair<int,wxFakeCommandBarRefData*>(bar.GetID(),bar2));
	return bar2;
}

//add command bar from pointer. The ownership will be
//transferd to button manager
wxFakeCommandBarRefData* wxFakeCommandBarFrame::AddBarData(wxFakeCommandBarRefData* bar){
	m_bars.insert(std::pair<int,wxFakeCommandBarRefData*>(bar->GetID(),bar));
	return bar;
}

wxFakeButtonData* wxFakeCommandBarFrame::FindButton(int id){
	std::multimap<int,wxFakeButtonData*>::iterator it=m_btns.find(id);
	if(it==m_btns.end()){
		return NULL;
	}else{
		return it->second;
	}
}

wxFakeButtonData* wxFakeCommandBarFrame::FindButton(const wxString& key){
	for(
		std::multimap<int,wxFakeButtonData*>::iterator it=m_btns.begin();
		it!=m_btns.end();++it)
	{
		if(it->second->Key()==key) return it->second;
	}
	return NULL;
}

wxFakeCommandBarRefData* wxFakeCommandBarFrame::FindBarData(int id){
	std::multimap<int,wxFakeCommandBarRefData*>::iterator it=m_bars.find(id);
	if(it==m_bars.end()){
		return NULL;
	}else{
		return it->second;
	}
}

wxFakeCommandBarRefData* wxFakeCommandBarFrame::FindBarData(const wxString& key){
	for(
		std::multimap<int,wxFakeCommandBarRefData*>::iterator it=m_bars.begin();
		it!=m_bars.end();++it)
	{
		if(it->second->GetKey()==key) return it->second;
	}
	return NULL;
}

wxFakeCommandBarFrame::~wxFakeCommandBarFrame(){
	for(
		std::multimap<int,wxFakeCommandBarRefData*>::iterator it=m_bars.begin();
		it!=m_bars.end();++it)
	{
		delete it->second;
	}
	m_bars.clear();

	for(
		std::multimap<int,wxFakeButtonData*>::iterator it=m_btns.begin();
		it!=m_btns.end();++it)
	{
		delete it->second;
	}
	m_btns.clear();
}

bool wxFakeCommandBarFrame::PopupMenuEx(wxFakeCommandBarDataBase* bar,const wxPoint& ptOrigin,const wxSize& size,int dir,int leftMargin,int topMargin,int rightMargin,int bottomMargin){
	wxFakeMenuPopupWindow *popup=new wxFakeMenuPopupWindow(this);
	popup->SetArtProvider(m_artprov);
	popup->SetBar(bar);
	popup->DockManager(this);
	popup->Layout();
	popup->PositionEx(ptOrigin,size,dir,leftMargin,topMargin,rightMargin,bottomMargin);
	return popup->Show();
}

void wxFakeCommandBarFrame::UpdateAcceleratorTable(bool useCommandBar){
	wxVector<wxAcceleratorEntry> v;
	std::map<int,wxFakeButtonData*> accel_map;

	m_accel_btns.clear();
	m_accel_bars.clear();
	m_accel_btnidx.clear();

	//add shortcut key
	if(useCommandBar){
		for(std::multimap<int,wxFakeCommandBarRefData*>::iterator it=m_bars.begin();
			it!=m_bars.end();++it)
		{
			wxFakeCommandBarRefData* bar=it->second;
			for(int i=0,m=bar->ButtonCount();i<m;i++){
				wxFakeButtonData* btn=bar->GetButton(i);
				const wxAcceleratorEntry& accel=btn->Accelerator();

				if(btn->Enabled() && accel.IsOk()){
					int flags=accel.GetFlags();
					int keycode=accel.GetKeyCode();

					int key=(flags<<24)|(keycode&0xFFFFFF);
					if(accel_map[key]==NULL){
						accel_map[key]=btn;

						v.push_back(wxAcceleratorEntry(flags,keycode,0x6000+v.size(),NULL));
						m_accel_btns.push_back(btn);
						m_accel_bars.push_back(bar);
						m_accel_btnidx.push_back(i);
					}
				}
			}
		}
	}else{
		for(
			std::multimap<int,wxFakeButtonData*>::iterator it=m_btns.begin();
			it!=m_btns.end();++it)
		{
			wxFakeButtonData* btn=it->second;
			const wxAcceleratorEntry& accel=btn->Accelerator();

			if(btn->Enabled() && accel.IsOk()){
				int flags=accel.GetFlags();
				int keycode=accel.GetKeyCode();

				int key=(flags<<24)|(keycode&0xFFFFFF);
				if(accel_map[key]==NULL){
					accel_map[key]=btn;

					v.push_back(wxAcceleratorEntry(flags,keycode,0x6000+v.size(),NULL));
					m_accel_btns.push_back(btn);
					m_accel_bars.push_back(NULL);
					m_accel_btnidx.push_back(-1);
				}
			}
		}
	}

	//add hot key (i.e. Alt+F, etc.)
	for(int i=0,m0=m_dockablebars.size();i<m0;i++){
		if(m_dockablebars[i].m_bar!=NULL){
			int m=m_dockablebars[i].m_bar->m_btn.size();
			wxVector<wxFakeButtonState> &btns=m_dockablebars[i].m_bardata->GetLayoutedButtons();
			if(m>(int)btns.size()) m=btns.size();

			for(int j=0;j<m;j++){
				wxFakeButtonData *btn=btns[j].btn;
				int keycode=btn->Mnemonics();
				if((btn->Flags() & (fbtfDisabled | fbtfHidden | fbtfHideCationInToolBar))==0 && keycode!=0){
					int flags=wxACCEL_ALT;
					int key=(flags<<24)|(keycode&0xFFFFFF);
					if(accel_map[key]==NULL){
						accel_map[key]=btn;

						v.push_back(wxAcceleratorEntry(flags,keycode,0x6000+v.size(),NULL));
						m_accel_btns.push_back(btn);
						m_accel_bars.push_back(m_dockablebars[i].m_bardata);
						m_accel_btnidx.push_back(btns[j].index | 0x10000);
					}
				}
			}
		}
	}

	if(v.empty()){
		SetAcceleratorTable(wxAcceleratorTable());
	}else{
		SetAcceleratorTable(wxAcceleratorTable(v.size(),&v[0]));
	}
}

wxFakeCommandBar* wxFakeCommandBarFrame::AddDockableCommandBar(wxFakeCommandBarDataBase* bar,int dir,int row,int pos,int flags){
	wxFakeDockableCommandBar cb={};

	m_dockablebars_dirty=true;

	flags&=~fdcbUndocked;

	if(flags & fdcbAlwaysFloat){
		flags&=~fdcbAlwaysDocked;
		if(dir>=0 && dir<4) dir=fcbdUndocked;
	}else if(flags & fdcbAlwaysDocked){
		if(dir==fcbdUndocked) dir=0;
	}

	cb.m_dir=dir;
	cb.m_row=row;
	cb.m_idx=pos;
	cb.m_flags=flags|((dir==fcbdUndocked)?fdcbUndocked:0);
	cb.m_bardata=bar;

	if(dir>=0 && dir<4){
		wxFakeCommandBarDockArea& dockarea=m_dockedbars[dir];

		if(row<0 || row>=(int)dockarea.m_rows.size()){
			dockarea.m_rows.push_back(new wxFakeCommandBarRow);
			row=dockarea.m_rows.size()-1;
		}

		wxFakeCommandBarRow* dockrow=dockarea.m_rows[row];

		if(pos<0 || pos>=(int)dockrow->m_bars.size()){
			dockrow->m_bars.push_back(NULL);
			pos=dockrow->m_bars.size()-1;
		}else{
			dockrow->m_bars.insert(dockrow->m_bars.begin()+pos,NULL);
		}

		cb.m_frame=NULL;

		cb.m_bar=new wxFakeCommandBar(this,wxID_ANY,wxPoint(-10000,-10000),wxSize(1,1));
		cb.m_bar->SetArtProvider(m_artprov);
		cb.m_bar->IsMainMenu((flags & fdcbMainMenu)!=0);
		cb.m_bar->Dockable(true);
		cb.m_bar->DockManager(this);
		cb.m_bar->ShowChevron((flags & fdcbShowChevron)!=0);
		cb.m_bar->Direction(dir);
		cb.m_bar->SetBar(bar);

		dockrow->m_bars[pos]=cb.m_bar;
	}else if(dir==fcbdUndocked){
		cb.m_dir=fcbdTop;

		cb.m_frame=new wxFakeMiniFrame();
		cb.m_frame->SetArtProvider(m_artprov);
		cb.m_frame->DockManager(this);
		//TODO: wxDefaultPosition doesn't work
		cb.m_frame->Create(this,wxID_ANY,bar->GetCaption(),wxDefaultPosition,wxSize(64,16),
			fpstToolbarMode
			|((flags & fdcbClosable)!=0?fpstCloseBox:0)
			|((flags & fdcbShowChevron)!=0?fpstCustomBox:0)
			);

		cb.m_bar=new wxFakeCommandBar(cb.m_frame,wxID_ANY,wxPoint(-10000,-10000),wxSize(1,1));
		cb.m_bar->SetArtProvider(m_artprov);
		cb.m_bar->IsMainMenu((flags & fdcbMainMenu)!=0);
		cb.m_bar->Dockable(true);
		cb.m_bar->Undocked(true);
		cb.m_bar->DockManager(this);
		cb.m_bar->ShowChevron((flags & fdcbShowChevron)!=0);
		cb.m_bar->SetBar(bar);

		cb.m_bar->Layout();
		cb.m_frame->SetClientSize(cb.m_bar->FullSize());
		cb.m_frame->SetClientWindow(cb.m_bar);

		cb.m_frame->Show();
	}else{
		cb.m_dir=fcbdTop;
		cb.m_frame=NULL;
		cb.m_bar=NULL;
	}

	m_dockablebars.push_back(cb);

	return cb.m_bar;
}

int wxFakeCommandBarFrame::LayoutOneRow(int dir,int row,int offset,int offset2,int max_size,wxFakeCommandBar* selected_bar,const wxPoint& pos){
	//TODO: different direction

	int y=0;
	int m=m_dockedbars[dir].m_rows[row]->m_bars.size();
	if(m==0) return 0;

	bool layout_from_scratch=false;
	bool need_refresh=false;

	//calc max height
	for(int i=0;i<m;i++){
		wxFakeCommandBar *bar=m_dockedbars[dir].m_rows[row]->m_bars[i];

		bar->Layout();

		wxSize fullsize=bar->FullSize();
		if(fullsize.y>y) y=fullsize.y;

		wxPoint currentpos=bar->GetPosition();
		if(currentpos.x<0 || currentpos.y<0) layout_from_scratch=true;
	}

	m_dockedbars[dir].m_rows[row]->m_size=y;

	if(dir==fcbdBottom || dir==fcbdRight) offset-=y;

	//size it (??? TODO:)
	wxVector<wxRect> newsize(m); //x=left y=width (unused for input) w=prefered width

	if(layout_from_scratch){
		int x=0;

		for(int i=0;i<m;i++){
			wxFakeCommandBar *bar=m_dockedbars[dir].m_rows[row]->m_bars[i];
			wxSize fullsize=bar->FullSize();

			if(fullsize.x>max_size-x-(m-i-1)*32){
				fullsize.x=max_size-x-(m-i-1)*32;
			}

			newsize[i].x=x;
			newsize[i].y=fullsize.x;

			x+=fullsize.x;
		}
	}else{
		int w=0;

		int selected_bar_idx=-1;
		int selected_bar_x=0;

		for(int i=0;i<m;i++){
			wxFakeCommandBar *bar=m_dockedbars[dir].m_rows[row]->m_bars[i];

			if(bar==selected_bar){
				selected_bar_idx=i;
				wxPoint p2=ScreenToClient(pos);
				if(dir==fcbdLeft || dir==fcbdRight){
					selected_bar_x=p2.y-offset2;
				}else{
					selected_bar_x=p2.x-offset2;
				}
			}

			wxSize fullsize=bar->FullSize();
			wxPoint currentpos=bar->GetPosition();
			wxSize currentsize=bar->GetSize();

			if(dir==fcbdLeft || dir==fcbdRight){
				newsize[i].x=currentpos.y-offset2;
				newsize[i].y=currentsize.y;
			}else{
				newsize[i].x=currentpos.x-offset2;
				newsize[i].y=currentsize.x;
			}
			newsize[i].width=fullsize.x;

			w+=fullsize.x;
		}

		//check swapping commandbar position (buggy)
		if(selected_bar_idx>=0){
			if(selected_bar_idx>0 &&
				selected_bar_x<newsize[selected_bar_idx-1].x/*+newsize[selected_bar_idx-1].y/2*/)
			{
				//move to the left
				int j=selected_bar_idx-1;
				while(j>0 && selected_bar_x<newsize[j-1].x/*+newsize[j-1].y/2*/) j--;

				//std::cout<<"Swap "<<selected_bar_idx<<" "<<j<<"\n"; //debug

				int tmp_width=newsize[selected_bar_idx].width;

				for(int i=selected_bar_idx;i>j;i--){
					newsize[i].x=newsize[i-1].x+newsize[selected_bar_idx].y;
					newsize[i].width=newsize[i-1].width;
					m_dockedbars[dir].m_rows[row]->m_bars[i]=m_dockedbars[dir].m_rows[row]->m_bars[i-1];
				}

				newsize[j].width=tmp_width;
				m_dockedbars[dir].m_rows[row]->m_bars[j]=selected_bar;
				selected_bar_idx=j;

				need_refresh=true;
			}else if(selected_bar_idx<m-1 &&
				selected_bar_x>newsize[selected_bar_idx+1].x/*+newsize[selected_bar_idx+1].y/2*/)
			{
				//move to the right
				int j=selected_bar_idx+1;
				while(j<m-1 && selected_bar_x>newsize[j+1].x/*+newsize[j+1].y/2*/) j++;

				//std::cout<<"Swap "<<selected_bar_idx<<" "<<j<<"\n"; //debug

				int tmp_width=newsize[selected_bar_idx].width;

				for(int i=selected_bar_idx;i<j;i++){
					newsize[i].x=newsize[i+1].x-newsize[selected_bar_idx].y;
					newsize[i].width=newsize[i+1].width;
					m_dockedbars[dir].m_rows[row]->m_bars[i]=m_dockedbars[dir].m_rows[row]->m_bars[i+1];
				}

				newsize[j].width=tmp_width;
				m_dockedbars[dir].m_rows[row]->m_bars[j]=selected_bar;
				selected_bar_idx=j;

				need_refresh=true;
			}
		}

		int x=0;

		if(w>max_size){
			//space is not enough for display all command bars

			//check selected_bar_x. (???) buggy. TODO:
			if(selected_bar_idx>=0){
				int xx=0;
				for(int i=0;i<selected_bar_idx;i++){
					xx+=newsize[i].width;
				}

				if(selected_bar_x>xx) selected_bar_x=xx;
				else if(selected_bar_x<max_size-w+xx) selected_bar_x=max_size-w+xx;

				if(selected_bar_x>max_size-(m-selected_bar_idx)*32){
					selected_bar_x=max_size-(m-selected_bar_idx)*32;
				}
				if(selected_bar_x<selected_bar_idx*32){
					selected_bar_x=selected_bar_idx*32;
				}

				newsize[selected_bar_idx].x=selected_bar_x;

				for(int i=0;i<selected_bar_idx;i++){
					if(newsize[i].x>x) newsize[i].x=x;
					else if(newsize[i].x<selected_bar_x-xx) newsize[i].x=selected_bar_x-xx;

					if(newsize[i].x>selected_bar_x-(selected_bar_idx-i)*32){
						newsize[i].x=selected_bar_x-(selected_bar_idx-i)*32;
					}
					if(newsize[i].x<i*32){
						newsize[i].x=i*32;
					}

					x=newsize[i].x+newsize[i].width;
					xx-=newsize[i].width;
				}

				x=0;
			}

			for(int i=0;i<m;i++){
				if(newsize[i].x>x) newsize[i].x=x;
				else if(newsize[i].x<max_size-w) newsize[i].x=max_size-w;

				if(newsize[i].x>max_size-(m-i)*32){
					newsize[i].x=max_size-(m-i)*32;
				}
				if(newsize[i].x<i*32){
					newsize[i].x=i*32;
				}

				x=newsize[i].x+newsize[i].width;
				w-=newsize[i].width;
			}

			for(int i=0;i<m-1;i++){
				newsize[i].y=newsize[i+1].x-newsize[i].x;
			}
			newsize[m-1].y=max_size-newsize[m-1].x;

			for(int i=0;i<m;i++){
				if(newsize[i].y<32) newsize[i].y=32;
			}
		}else{
			//enough space
			if(selected_bar_idx>=0){
				int xx=0;
				for(int i=0;i<selected_bar_idx;i++){
					xx+=newsize[i].width;
				}

				if(selected_bar_x<xx) selected_bar_x=xx;
				newsize[selected_bar_idx].x=selected_bar_x;

				for(int i=selected_bar_idx-1;i>=0;i--){
					if(newsize[i].x+newsize[i].width>newsize[i+1].x){
						newsize[i].x=newsize[i+1].x-newsize[i].width;
					}
				}
			}

			for(int i=0;i<m;i++){
				if(newsize[i].x<x) newsize[i].x=x;
				else if(newsize[i].x>max_size-w+x) newsize[i].x=max_size-w+x;

				if(i>0 && newsize[i-1].x+newsize[i-1].width>newsize[i].x){
					newsize[i].x=newsize[i-1].x+newsize[i-1].width;
				}

				newsize[i].y=newsize[i].width;

				x+=newsize[i].width;
			}
		}
	}

	for(int i=0;i<m;i++){
		wxFakeCommandBar *bar=m_dockedbars[dir].m_rows[row]->m_bars[i];
		if(dir==fcbdLeft || dir==fcbdRight){
			bar->SetSize(offset,newsize[i].x+offset2,y,newsize[i].y);
		}else{
			bar->SetSize(newsize[i].x+offset2,offset,newsize[i].y,y);
		}

		//???
		if(need_refresh){
			bar->Refresh();
		}
	}

	return y;
}

void wxFakeCommandBarFrame::LayoutDockedBars(int dir,int offset,int offset2,int max_size,wxFakeCommandBar* selected_bar,const wxPoint& pos){
	int size=0;

	for(int i=0,m=m_dockedbars[dir].m_rows.size();i<m;i++){
		if(m_dockedbars[dir].m_rows[i]->m_bars.empty()){
			//remove this row
			delete m_dockedbars[dir].m_rows[i];
			m_dockedbars[dir].m_rows.erase(m_dockedbars[dir].m_rows.begin()+i);
			i--;
			m--;
		}else{
			//layout this row
			int y=LayoutOneRow(dir,i,offset,offset2,max_size,selected_bar,pos);

			//next row
			size+=y;
			if(dir==fcbdBottom || dir==fcbdRight){
				offset-=y;
			}else{
				offset+=y;
			}
		}
	}

	//over
	m_dockedbars[dir].m_size=size;
}

bool wxFakeCommandBarFrame::Layout(){
	bool ret=LayoutEx(NULL,wxPoint());

	UpdateAcceleratorTable();

	return ret;
}

int wxFakeCommandBarFrame::GetNewDockedRowFromPos(int dir,int old_row,int offset,int width,const wxPoint& pos,int& new_row,int& new_idx){
	int dist=0;

	if(pos.x<0) dist=-pos.x;
	else if(pos.x>width) dist=pos.x-width;
	if(pos.y<-dist) dist=-pos.y;
	else if(pos.y-m_dockedbars[dir].m_size>dist) dist=pos.y-m_dockedbars[dir].m_size;

	if(dist>16) return -1;

	int m=m_dockedbars[dir].m_rows.size();

	//calc new row
	if(pos.y<-8){
		new_row=-1;
	}else if(pos.y>=m_dockedbars[dir].m_size){
		new_row=m;
	}else if(old_row>=0 && old_row<m){
		int y0=0;

		if(old_row>0){
			for(int i=0;i<old_row-1;i++) y0+=m_dockedbars[dir].m_rows[i]->m_size;
			y0+=m_dockedbars[dir].m_rows[old_row-1]->m_size/2;
		}else{
			y0=-8;
		}

		if(pos.y<y0){
			//move up
			new_row=0;

			for(int i=old_row-1;i>0;i--){
				y0-=m_dockedbars[dir].m_rows[i-1]->m_size;
				if(pos.y>=y0){
					new_row=i;
					break;
				}
			}
		}else{
			y0=0;
			for(int i=0;i<old_row;i++) y0+=m_dockedbars[dir].m_rows[i]->m_size;
			y0+=m_dockedbars[dir].m_rows[old_row]->m_size/2;

			if(pos.y>=y0){
				//move down
				new_row=m-1;

				for(int i=old_row+1;i<m;i++){
					y0+=m_dockedbars[dir].m_rows[i]->m_size;
					if(pos.y<y0){
						new_row=i;
						break;
					}
				}
			}else{
				//not moved
				new_row=old_row;
			}
		}
	}else{
		int y0=0;
		new_row=m-1;

		for(int i=0;i<m;i++){
			y0+=m_dockedbars[dir].m_rows[i]->m_size;
			if(pos.y<y0){
				new_row=i;
				break;
			}
		}
	}

	//calc new pos
	if(new_row>=0 && new_row<m && new_row!=old_row){
		int m2=m_dockedbars[dir].m_rows[new_row]->m_bars.size();
		new_idx=m2;

		for(int i=0;i<m2;i++){
			int x;
			if(dir==fcbdLeft || dir==fcbdRight){
				x=m_dockedbars[dir].m_rows[new_row]->m_bars[i]->GetPosition().y-offset;
			}else{
				x=m_dockedbars[dir].m_rows[new_row]->m_bars[i]->GetPosition().x-offset;
			}
			if(pos.x<x){
				new_idx=i;
				break;
			}
		}
	}else{
		new_idx=0;
	}

	return dist;
}

//TODO: don't redraw too often
bool wxFakeCommandBarFrame::LayoutEx(wxFakeCommandBar* selected_bar,const wxPoint& pos){
	wxSize sz=GetClientSize();

	//TODO: status bar

	//check dragging command bars
	if(selected_bar!=NULL){
		int dir=0,row=0,idx=0;
		bool bar_found=false;

		for(dir=0;dir<4;dir++){
			int m1=m_dockedbars[dir].m_rows.size();
			for(row=0;row<m1;row++){
				int m2=m_dockedbars[dir].m_rows[row]->m_bars.size();
				for(idx=0;idx<m2;idx++){
					if(m_dockedbars[dir].m_rows[row]->m_bars[idx]==selected_bar){
						bar_found=true;
						break;
					}
				}
				if(bar_found) break;
			}
			if(bar_found) break;
		}

		int indexInDockableBars=-1;

		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_bar==selected_bar){
				indexInDockableBars=i;
				if(m_dockablebars[i].m_frame!=NULL && !bar_found){
					dir=-1;
					row=-1;
					idx=i;
					bar_found=true;
				}
				break;
			}
		}

		if(indexInDockableBars>=0 && bar_found){
			int new_dir=-1,new_row=-1,new_idx=-1;
			int dist=0xFF;

			wxPoint p0=ScreenToClient(pos);

			int bar_height;
			if(dir==fcbdLeft || dir==fcbdRight){
				bar_height=selected_bar->GetSize().x;
			}else{
				bar_height=selected_bar->GetSize().y;
			}

			wxFakeDockableCommandBar &cb=m_dockablebars[indexInDockableBars];

			if((cb.m_flags & fdcbAlwaysFloat)==0 && !wxGetKeyState(WXK_CONTROL)){
				int new_row1=-1,new_idx1=-1,dist1;

				//check top
				dist1=GetNewDockedRowFromPos(fcbdTop,(dir==fcbdTop)?row:-1,0,sz.x,p0,new_row1,new_idx1);
				if(dist1>=0 && dist1<dist){
					new_dir=fcbdTop;
					new_row=new_row1;
					new_idx=new_idx1;
					dist=dist1;
				}

				//check bottom
				dist1=GetNewDockedRowFromPos(fcbdBottom,(dir==fcbdBottom)?row:-1,0,sz.x,
					wxPoint(p0.x,sz.y-p0.y-bar_height),
					new_row1,new_idx1);
				if(dist1>=0 && dist1<dist){
					new_dir=fcbdBottom;
					new_row=new_row1;
					new_idx=new_idx1;
					dist=dist1;
				}

				int h1=m_dockedbars[fcbdTop].m_size,h2=m_dockedbars[fcbdBottom].m_size;

				//check left
				dist1=GetNewDockedRowFromPos(fcbdLeft,(dir==fcbdLeft)?row:-1,h1,sz.y-h1-h2,
					wxPoint(p0.y-h1,p0.x),
					new_row1,new_idx1);
				if(dist1>=0 && dist1<dist){
					new_dir=fcbdLeft;
					new_row=new_row1;
					new_idx=new_idx1;
					dist=dist1;
				}

				//check right
				dist1=GetNewDockedRowFromPos(fcbdRight,(dir==fcbdRight)?row:-1,h1,sz.y-h1-h2,
					wxPoint(p0.y-h1,sz.x-p0.x-bar_height),
					new_row1,new_idx1);
				if(dist1>=0 && dist1<dist){
					new_dir=fcbdRight;
					new_row=new_row1;
					new_idx=new_idx1;
					dist=dist1;
				}
			}

			//check if float is not allowed
			if(dir>=0 && dir<4 && new_dir==-1 && (cb.m_flags & fdcbAlwaysDocked)!=0){
				new_dir=dir;
				new_row=row;
				new_idx=idx;
			}

			//check if we'd like to insert bar to existing row (??)
			bool insert_mode=(dir>=0 && new_dir==dir && m_dockedbars[dir].m_rows[row]->m_bars.size()<=1);

			if(insert_mode){
				if(new_row<0) new_row=0;
				else if(new_row>=(int)m_dockedbars[new_dir].m_rows.size()){
					new_row=m_dockedbars[new_dir].m_rows.size()-1;
				}
			}

			//check we should change the row
			if(new_dir!=dir || new_row!=row){
				if(new_dir==-1){
					m_dockedbars[dir].m_rows[row]->m_bars.erase(m_dockedbars[dir].m_rows[row]->m_bars.begin()+idx);

					//float it
					wxFakeCommandBarDataBase *bar=selected_bar->GetBar();

					//record old position
					cb.m_dir=dir;
					cb.m_row=row;
					cb.m_idx=idx;
					cb.m_flags|=fdcbUndocked;

					if(selected_bar->HasCapture()){
						selected_bar->ReleaseMouse();
					}
					selected_bar->SetCursor(wxNullCursor);
					selected_bar->m_pressed_idx=-1;

					cb.m_frame=new wxFakeMiniFrame();
					cb.m_frame->SetArtProvider(m_artprov);
					cb.m_frame->DockManager(this);
					cb.m_frame->Create(this,wxID_ANY,bar==NULL?wxString():(bar->GetCaption()),p0,wxSize(64,16),
						fpstToolbarMode
						|((cb.m_flags & fdcbClosable)!=0?fpstCloseBox:0)
						|((cb.m_flags & fdcbShowChevron)!=0?fpstCustomBox:0)
						);

					selected_bar->Direction(fcbdTop);
					selected_bar->Undocked(true);
					selected_bar->Reparent(cb.m_frame);

					selected_bar->Layout();
#ifdef __WXGTK__
					cb.m_frame->Move(p0);
#endif
					cb.m_frame->SetClientSize(selected_bar->FullSize());
					cb.m_frame->SetClientWindow(selected_bar);

					cb.m_frame->Show();
				}else{
					//remove old one
					if(dir==-1){
						cb.m_flags&=~fdcbUndocked;
						cb.m_pos=cb.m_frame->GetPosition();

						//unfloat it
						selected_bar->Direction(new_dir);
						selected_bar->Undocked(false);
						selected_bar->Reparent(this);
						selected_bar->Move(p0);

						if(cb.m_frame->HasCapture()){
							cb.m_frame->ReleaseMouse();
						}
						cb.m_frame->Destroy();
						cb.m_frame=NULL;
					}else{
						m_dockedbars[dir].m_rows[row]->m_bars.erase(m_dockedbars[dir].m_rows[row]->m_bars.begin()+idx);
					}

					//set the direction
					selected_bar->Direction(new_dir);

					//move and resize the bar
					selected_bar->Move(p0);
					if((dir==fcbdLeft || dir==fcbdRight) ^ (new_dir==fcbdLeft || new_dir==fcbdRight)){
						wxSize size=selected_bar->GetSize();
						selected_bar->SetSize(size.y,size.x);
					}

					if(insert_mode){
						if(new_idx<0) new_idx=0;
						else if(new_idx>(int)m_dockedbars[new_dir].m_rows[new_row]->m_bars.size()){
							new_idx=m_dockedbars[new_dir].m_rows[new_row]->m_bars.size();
						}

						m_dockedbars[new_dir].m_rows[new_row]->m_bars.insert(
							m_dockedbars[new_dir].m_rows[new_row]->m_bars.begin()+new_idx,
							selected_bar);
					}else{
						wxFakeCommandBarRow *objRow=new wxFakeCommandBarRow;
						objRow->m_bars.push_back(selected_bar);

						if(new_row<0) new_row=0;
						else if(new_row>(int)m_dockedbars[new_dir].m_rows.size()){
							new_row=m_dockedbars[new_dir].m_rows.size();
						}

						m_dockedbars[new_dir].m_rows.insert(m_dockedbars[new_dir].m_rows.begin()+new_row,objRow);
					}

					Refresh();
				}
			}else if(dir<0){
				//keep it float
				return false;
			}
		}else{
			//bar not found, do nothing, exit
			return false;
		}
	}

	//process command bars
	{
		LayoutDockedBars(fcbdTop,0,0,sz.x,selected_bar,pos);
		LayoutDockedBars(fcbdBottom,sz.y,0,sz.x,selected_bar,pos);

		int h1=m_dockedbars[fcbdTop].m_size,h2=m_dockedbars[fcbdBottom].m_size;
		LayoutDockedBars(fcbdLeft,0,h1,sz.y-h1-h2,selected_bar,pos);
		LayoutDockedBars(fcbdRight,sz.x,h1,sz.y-h1-h2,selected_bar,pos);
	}

	//docked panes
	{
		int x0=m_dockedbars[fcbdLeft].m_size;
		int y0=m_dockedbars[fcbdTop].m_size;

		wxRect rcClient(x0,y0,
			sz.x-x0-m_dockedbars[fcbdRight].m_size,
			sz.y-y0-m_dockedbars[fcbdBottom].m_size);

		for(int layer=0;layer<(int)m_panelayer.size();layer++){
			wxFakeDockablePaneDockLayer* docklayer=m_panelayer[layer];

			LayoutDockedPanes(docklayer,fcbdTop,wxPoint(rcClient.x,rcClient.y),rcClient.width);
			LayoutDockedPanes(docklayer,fcbdBottom,wxPoint(rcClient.x,rcClient.y+rcClient.height),rcClient.width);

			int h1=docklayer->m_dockedpanes[fcbdTop].m_size;
			int h2=docklayer->m_dockedpanes[fcbdBottom].m_size;

			LayoutDockedPanes(docklayer,fcbdLeft,wxPoint(rcClient.x,rcClient.y+h1),rcClient.height-h1-h2);
			LayoutDockedPanes(docklayer,fcbdRight,wxPoint(rcClient.x+rcClient.width,rcClient.y+h1),rcClient.height-h1-h2);

			//check if it's empty
			if(docklayer->m_dockedpanes[0].m_rows.empty()
				&& docklayer->m_dockedpanes[1].m_rows.empty()
				&& docklayer->m_dockedpanes[2].m_rows.empty()
				&& docklayer->m_dockedpanes[3].m_rows.empty())
			{
				delete docklayer;
				m_panelayer.erase(m_panelayer.begin()+layer);
				layer--;
				continue;
			}

			docklayer->m_rect=rcClient;

			x0=docklayer->m_dockedpanes[fcbdLeft].m_size;

			rcClient.x+=x0;
			rcClient.y+=h1;
			rcClient.width-=x0+docklayer->m_dockedpanes[fcbdRight].m_size;
			rcClient.height-=h1+h2;
		}
	}

	//resize client window
	LayoutClientWindow();

	return true;
}

void wxFakeCommandBarFrame::LayoutDockedPanes(wxFakeDockablePaneDockLayer* docklayer,int dir,const wxPoint& offset,int max_size){
	wxFakeDockablePaneDockArea& dockarea=docklayer->m_dockedpanes[dir];

	dockarea.m_size=0;

	for(int row=0;row<(int)dockarea.m_rows.size();row++){
		wxFakeDockablePaneRow* dockrow=dockarea.m_rows[row];

		//check if it's empty
		if(dockrow->DeleteEmptySubRow()){
			delete dockrow;
			dockarea.m_rows.erase(dockarea.m_rows.begin()+row);
			row--;
			continue;
		}

		if(dockrow->m_size<32) dockrow->m_size=32;

		wxRect r;

		switch(dir){
		default:
			r.x=offset.x;r.y=offset.y;
			r.width=max_size;r.height=dockrow->m_size;
			break;
		case fcbdBottom:
			r.x=offset.x;r.y=offset.y-dockrow->m_size;
			r.width=max_size;r.height=dockrow->m_size;
			break;
		case fcbdLeft:
			r.x=offset.x;r.y=offset.y;
			r.width=dockrow->m_size;r.height=max_size;
			break;
		case fcbdRight:
			r.x=offset.x-dockrow->m_size;r.y=offset.y;
			r.width=dockrow->m_size;r.height=max_size;
			break;
		}

		LayoutDockedPaneRow(dockrow,r);

		dockarea.m_size+=dockrow->m_size+4;
	}
}

void wxFakeCommandBarFrame::LayoutDockedPaneRow(wxFakeDockablePaneRow* dockrow,const wxRect& rect){
	int m=dockrow->m_pane.size();
	if(m==0) return;

	dockrow->m_rect=rect;

	//calc the scale
	float f=0.0f;
	for(int i=0;i<m;i++){
		float *f1;
		if(dockrow->m_pane[i].m_subrow!=NULL){
			f1=&dockrow->m_pane[i].m_subrow->m_scale;
		}else{
			f1=&dockrow->m_pane[i].m_pane->m_scale;
		}
		if(*f1<0.001f) *f1=0.001f;
		f+=*f1;
	}

	for(int i=0;i<m;i++){
		if(dockrow->m_pane[i].m_subrow!=NULL){
			dockrow->m_pane[i].m_subrow->m_scale/=f;
		}else{
			dockrow->m_pane[i].m_pane->m_scale/=f;
		}
	}

	//resize it
	f=0.0f;
	for(int i=0;i<m;i++){
		float f1=f;
		if(dockrow->m_pane[i].m_subrow!=NULL){
			f1+=dockrow->m_pane[i].m_subrow->m_scale;
		}else{
			f1+=dockrow->m_pane[i].m_pane->m_scale;
		}

		wxRect r;

		if(dockrow->m_dir==fcbdLeft || dockrow->m_dir==fcbdRight){
			r.x=rect.x;
			r.y=int(float(rect.height)*f);
			r.width=rect.width;
			r.height=int(float(rect.height)*f1)-r.y;
			r.y+=rect.y;

			if(i>0){
				r.y+=2;
				r.height-=2;
			}
			if(i<m-1){
				r.height-=2;
			}
		}else{
			r.x=int(float(rect.width)*f);
			r.y=rect.y;
			r.width=int(float(rect.width)*f1)-r.x;
			r.height=rect.height;
			r.x+=rect.x;

			if(i>0){
				r.x+=2;
				r.width-=2;
			}
			if(i<m-1){
				r.width-=2;
			}
		}

		f=f1;

		if(dockrow->m_pane[i].m_subrow!=NULL){
			LayoutDockedPaneRow(dockrow->m_pane[i].m_subrow,r);
		}else{
			LayoutSingleDockedPane(dockrow->m_pane[i].m_pane,r);
		}
	}
}

void wxFakeCommandBarFrame::LayoutSingleDockedPane(wxFakeDockablePane* pane,const wxRect& rect){
	pane->m_rect=rect;

	if(pane->m_window!=NULL){
		int leftMargin=0,topMargin=0,rightMargin=0,bottomMargin=0;

		/*//get art provider (experimental)
		wxFakeCommandBarArtBase* artprov=m_artprov;
		if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
		if(artprov){
			artprov->GetMiniFrameClientSizeMargin(pane->m_flags,leftMargin,topMargin,rightMargin,bottomMargin);
		}*/

		pane->m_window->SetSize(wxRect(rect.x+leftMargin,rect.y+topMargin,
			rect.width-leftMargin-rightMargin,
			rect.height-topMargin-bottomMargin));
	}
}

wxFakeDockablePane* wxFakeCommandBarFrame::AddDockablePane(wxWindow *window,const wxString& caption,wxFakeDockablePane* parent,int layer,int dir,int row,int pos,float scale,int height,int flags){
	wxFakeDockablePane* newpane=new wxFakeDockablePane;

	flags&=~(fpstToolbarMode|fpstDocked|fpstHidden);

	newpane->m_window=window;
	newpane->m_frame=NULL;
	newpane->m_caption=caption;

	if(dir>=0 && dir<4){
		flags|=fpstDocked;

		if(parent==NULL){
			//get layer
			if(layer<0 || layer>=(int)m_panelayer.size()){
				layer=m_panelayer.size();
				m_panelayer.push_back(new wxFakeDockablePaneDockLayer);
			}

			//get row
			wxFakeDockablePaneDockArea& dockarea=m_panelayer[layer]->m_dockedpanes[dir];
			if(row<0 || row>=(int)dockarea.m_rows.size()){
				row=dockarea.m_rows.size();
				dockarea.m_rows.push_back(new wxFakeDockablePaneRow);
			}

			//get pos
			wxFakeDockablePaneRow* dockrow=dockarea.m_rows[row];
			dockrow->m_dir=dir;
			if(height>dockrow->m_size) dockrow->m_size=height;
			if(pos<0 || pos>=(int)dockrow->m_pane.size()){
				pos=dockrow->m_pane.size();
			}

			//insert new item
			wxFakeDockablePaneItem item={NULL,newpane};
			dockrow->m_pane.insert(dockrow->m_pane.begin()+pos,item);
			newpane->m_idxs.push_back(pos);
		}else{
			//TODO:
		}

		if(window){
			window->Reparent(this);
			if(!window->IsShown()) window->Show();
		}
	}else if(dir==fcbdUndocked){
		dir=fcbdTop;

		wxFakeMiniFrame *frame=new wxFakeMiniFrame();
		newpane->m_frame=frame;

		frame->SetArtProvider(m_artprov);
		frame->DockManager(this);
		//TODO: wxDefaultPosition doesn't work
		frame->Create(this,wxID_ANY,caption,wxDefaultPosition,wxSize(128,96),
			fpstSizable|flags
			);

		if(window){
			window->Reparent(frame);
			frame->SetClientSize(window->GetSize());
			frame->SetClientWindow(window);
			if(!window->IsShown()) window->Show();
		}

		frame->Show();
	}else{
		flags|=fpstHidden;
		dir=fcbdTop;

		if(window){
			window->Reparent(this);
			if(window->IsShown()) window->Show(false);
		}
	}

	newpane->m_scale=scale;
	newpane->m_layer=layer;
	newpane->m_dir=dir;
	newpane->m_row=row;
	newpane->m_flags=flags;

	m_dockablepanes.push_back(newpane);

	return newpane;
}

bool wxFakeCommandBarFrame::Destroy(){
	if(HasCapture()) ReleaseMouse();

	for(int i=0,m=m_panelayer.size();i<m;i++){
		m_panelayer[i]->Destroy();
		delete m_panelayer[i];
	}
	m_panelayer.clear();

	for(int i=0,m=m_dockablepanes.size();i<m;i++){
		if(m_dockablepanes[i]->m_frame!=NULL){
			m_dockablepanes[i]->m_frame->Destroy();
		}
		delete m_dockablepanes[i];
	}
	m_dockablepanes.clear();

	for(int i=0;i<4;i++){
		m_dockedbars[i].Destroy();
	}

	for(int i=0;i<(int)m_dockablebars.size();i++){
		if(m_dockablebars[i].m_frame!=NULL){
			m_dockablebars[i].m_frame->Destroy();
		}
	}
	m_dockablebars.clear();

	delete m_dockablebars_menu;
	m_dockablebars_menu=NULL;

	delete m_timer;
	m_timer=NULL;

	wxEvtHandler::RemoveFilter(this);

	Show(false);

	return wxFrame::Destroy();
}

#ifdef __WXMSW__
// dirty hack
WXLRESULT wxFakeCommandBarFrame::MSWWindowProc(WXUINT message,WXWPARAM wParam,WXLPARAM lParam){
	switch(message){
	case WM_NCACTIVATE:
	case WM_ACTIVATE:
		if(wParam==0){
			int style=GetWindowLongW((HWND)lParam,GWL_EXSTYLE);
			if((style & WS_EX_TOOLWINDOW)==0) break;
			wParam=1;
		}
		break;
	case WM_ACTIVATEAPP:
		if(wParam==0){
			wxFrame::MSWWindowProc(WM_NCACTIVATE,wParam,lParam);
			wxFrame::MSWWindowProc(WM_ACTIVATE,wParam,lParam);
		}else{
			HWND hWnd=GetActiveWindow();
			if(hWnd==(HWND)GetHWND() || (GetWindowLongW(hWnd,GWL_EXSTYLE) & WS_EX_TOOLWINDOW)!=0){
				wxFrame::MSWWindowProc(WM_NCACTIVATE,wParam,lParam);
				wxFrame::MSWWindowProc(WM_ACTIVATE,wParam,lParam);
			}
		}
		break;
	default:
		break;
	}
	return wxFrame::MSWWindowProc(message,wParam,lParam);
}
#endif

bool wxFakeCommandBarFrame::Redraw(wxDC& dc){
	//get art provider
	wxFakeCommandBarArtBase* artprov=m_artprov;
	if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;
	if(artprov==NULL) return false;

	wxRect rect(GetClientSize());

	//draw background
	artprov->DrawToolbarBackground(dc,this,fcbfMainMenu,0,rect,wxRect());

	/*//draw docked pane label (experimental)
	for(int i=0,m=m_dockablepanes.size();i<m;i++){
		if((m_dockablepanes[i]->m_flags & (fpstDocked|fpstHidden))==fpstDocked){
			artprov->DrawMiniFrame(dc,this,m_dockablepanes[i]->m_caption,
				m_dockablepanes[i]->m_flags,-1,-1,-1,
				m_dockablepanes[i]->m_rect);
		}
	}*/

	//over
	return true;
}

void wxFakeCommandBarFrame::OnPaint(wxPaintEvent& event){
	if(!IsShown()) return;

	wxAutoBufferedPaintDC dc(this);

	Redraw(dc);
}

void wxFakeCommandBarFrame::SetClientWindow(wxWindow* wnd){
	if(m_clientwnd==wnd) return;

	if(m_clientwnd!=NULL && m_clientwnd->IsShown()){
		m_clientwnd->Show(false);
	}

	m_clientwnd=wnd;

	LayoutClientWindow();

	if(wnd!=NULL && !wnd->IsShown()){
		wnd->Show();
	}
}

void wxFakeCommandBarFrame::LayoutClientWindow(){
	if(m_clientwnd==NULL) return;

	wxRect r=this->GetClientRect();

	//TODO: status bar size

	//docked commandbar
	int x0=m_dockedbars[fcbdLeft].m_size;
	int y0=m_dockedbars[fcbdTop].m_size;
	r.x+=x0;
	r.y+=y0;
	r.width-=x0+m_dockedbars[fcbdRight].m_size;
	r.height-=y0+m_dockedbars[fcbdBottom].m_size;

	//docked panes
	for(int i=0,m=m_panelayer.size();i<m;i++){
		x0=m_panelayer[i]->m_dockedpanes[fcbdLeft].m_size;
		y0=m_panelayer[i]->m_dockedpanes[fcbdTop].m_size;
		r.x+=x0;
		r.y+=y0;
		r.width-=x0+m_panelayer[i]->m_dockedpanes[fcbdRight].m_size;
		r.height-=y0+m_panelayer[i]->m_dockedpanes[fcbdBottom].m_size;
	}

	m_clientwnd->SetSize(r);
}

void wxFakeCommandBarFrame::OnActivate(wxActivateEvent& event){
	//... doesn't work properly
/*#ifdef __WXMSW__
	if(event.GetActive() && IsShownOnScreen() && !IsIconized()){
		for(int i=0;i<(int)m_undockedbars.size();i++){
			if(!m_undockedbars[i].m_frame->IsShown()){
				m_undockedbars[i].m_frame->Show();
			}
		}
	}else{
		for(int i=0;i<(int)m_undockedbars.size();i++){
			if(m_undockedbars[i].m_frame->IsShown()){
				m_undockedbars[i].m_frame->Show(false);
			}
		}
	}
#endif*/

	event.Skip();
}

int wxFakeCommandBarFrame::FilterEvent(wxEvent &event){
	if(event.GetEventType()==wxEVT_COMMAND_MENU_SELECTED){
		wxFakeCommandBarButtonEvent *evt0=wxDynamicCast(&event,wxFakeCommandBarButtonEvent);
		if(evt0==NULL){
			wxCommandEvent *evt=wxDynamicCast(&event,wxCommandEvent);
			if(evt!=NULL){
				wxFakeCommandBarFrame *wnd=wxDynamicCast(evt->GetEventObject(),wxFakeCommandBarFrame);
				if(wnd==this){
					int id=evt->GetId();
					if(id>=0x6000 && id<=0x7FFF && id<0x6000+(int)m_accel_btns.size()){
						if(wxWindow::GetCapture()!=NULL) return Event_Ignore;

						//get button and bar
						id-=0x6000;
						wxFakeButtonData *btn=m_accel_btns[id];
						wxFakeCommandBarDataBase *bar=m_accel_bars[id];
						int idx=m_accel_btnidx[id];

						//check if it's a hot key and we should popup menu
						if(idx & 0x10000){
							idx&=~0x10000;
							if(btn->SubMenu()!=NULL && btn->Type()!=fbttSplit){
								//find command bar
								int idx=0,m=m_dockablebars.size();
								for(;idx<m;idx++){
									if(m_dockablebars[idx].m_bardata==bar && m_dockablebars[idx].m_bar!=NULL) break;
								}

								if(idx<m){
									int m=m_dockablebars[idx].m_bar->m_btn.size();
									wxVector<wxFakeButtonState> &btns=m_dockablebars[idx].m_bardata->GetLayoutedButtons();
									if(m>(int)btns.size()) m=btns.size();

									for(int i=0;i<m;i++){
										if(btns[i].btn==btn){
											//get art provider
											wxFakeCommandBarArtBase* artprov=m_dockablebars[idx].m_bar->m_artprov;
											if(artprov==NULL) artprov=wxFakeCommandBarDefaultArtProvider;

											m_dockablebars[idx].m_bar->m_btn[i].add_flags=fbtaPopupMenu;
											m_dockablebars[idx].m_bar->Refresh();

#ifdef WXFAKEMENU_USE_MOUSE_CAPTURE
											m_dockablebars[idx].m_bar->CaptureMouse();
#endif

											m_dockablebars[idx].m_bar->PopupSubMenu(i,
												m_dockablebars[idx].m_bar->m_btn[i].rect,
												(wxFakeCommandBarDataBase*)btn->SubMenu(),
												artprov);

											return Event_Ignore;
										}
									}
								}
							}
						}

						//click the button
						btn->OnClick(this,GetEventHandler(),bar,idx);

						//check the parent object and tell them to update button state
						UpdateOnNextIdle();

						return Event_Ignore;
					}
				}
			}
		}else{
			wxFakeCommandBarDataBase *bar=evt0->GetBar();
			if(bar==(wxFakeCommandBarDataBase*)m_dockablebars_menu){
				int idx=evt0->GetId();
				if(idx>=0 && idx<(int)m_dockablebars.size()){
					ShowCommandBarByIndex(idx,evt0->GetInt()!=0);
				}
				return Event_Processed;
			}
		}
	}

	return Event_Skip;
}

bool wxFakeCommandBarFrame::ShowCommandBarByIndex(int index,bool show){
	wxFakeDockableCommandBar &cb=m_dockablebars[index];

	if(cb.m_bar==NULL && show){ //show it
		if(cb.m_flags & fdcbAlwaysFloat){
			cb.m_flags|=fdcbUndocked;
		}else if(cb.m_flags & fdcbAlwaysDocked){
			cb.m_flags&=~fdcbUndocked;
		}

		if(cb.m_flags & fdcbUndocked){
			wxFakeCommandBarDataBase *bar=cb.m_bardata;

			cb.m_frame=new wxFakeMiniFrame();
			cb.m_frame->SetArtProvider(m_artprov);
			cb.m_frame->DockManager(this);
			cb.m_frame->Create(this,wxID_ANY,bar->GetCaption(),cb.m_pos,wxSize(64,16),
				fpstToolbarMode
				|((cb.m_flags & fdcbClosable)!=0?fpstCloseBox:0)
				|((cb.m_flags & fdcbShowChevron)!=0?fpstCustomBox:0)
				);
#ifdef __WXGTK__
			cb.m_frame->Move(cb.m_pos);
#endif

			cb.m_bar=new wxFakeCommandBar(cb.m_frame,wxID_ANY,wxPoint(-10000,-10000),wxSize(1,1));
			cb.m_bar->SetArtProvider(m_artprov);
			cb.m_bar->IsMainMenu((cb.m_flags & fdcbMainMenu)!=0);
			cb.m_bar->Dockable(true);
			cb.m_bar->Undocked(true);
			cb.m_bar->DockManager(this);
			cb.m_bar->ShowChevron((cb.m_flags & fdcbShowChevron)!=0);
			cb.m_bar->SetBar(bar);

			cb.m_bar->Layout();
			cb.m_frame->SetClientSize(cb.m_bar->FullSize());
			cb.m_frame->SetClientWindow(cb.m_bar);

			cb.m_frame->Show();
		}else{
			int dir=cb.m_dir;
			int row=cb.m_row;
			int pos=cb.m_idx;

			if(!(dir>=0 && dir<4)) dir=fcbdTop;

			wxFakeCommandBarDockArea& dockarea=m_dockedbars[dir];

			if(row<0 || row>=(int)dockarea.m_rows.size()){
				dockarea.m_rows.push_back(new wxFakeCommandBarRow);
				row=dockarea.m_rows.size()-1;
			}

			wxFakeCommandBarRow* dockrow=dockarea.m_rows[row];

			if(pos<0 || pos>=(int)dockrow->m_bars.size()){
				dockrow->m_bars.push_back(NULL);
				pos=dockrow->m_bars.size()-1;
			}else{
				dockrow->m_bars.insert(dockrow->m_bars.begin()+pos,NULL);
			}

			cb.m_frame=NULL;

			cb.m_bar=new wxFakeCommandBar(this,wxID_ANY,cb.m_pos,cb.m_size);
			cb.m_bar->SetArtProvider(m_artprov);
			cb.m_bar->IsMainMenu((cb.m_flags & fdcbMainMenu)!=0);
			cb.m_bar->Dockable(true);
			cb.m_bar->DockManager(this);
			cb.m_bar->ShowChevron((cb.m_flags & fdcbShowChevron)!=0);
			cb.m_bar->Direction(dir);
			cb.m_bar->SetBar(cb.m_bardata);

			dockrow->m_bars[pos]=cb.m_bar;

			LayoutEx(NULL,wxPoint());
		}

		UpdateAcceleratorTable();
	}else if(cb.m_bar!=NULL && !show){ //hide it
		//find it first
		int dir=0,row=0,idx=0;
		bool bar_found=false;

		for(dir=0;dir<4;dir++){
			int m1=m_dockedbars[dir].m_rows.size();
			for(row=0;row<m1;row++){
				int m2=m_dockedbars[dir].m_rows[row]->m_bars.size();
				for(idx=0;idx<m2;idx++){
					if(m_dockedbars[dir].m_rows[row]->m_bars[idx]==cb.m_bar){
						bar_found=true;
						break;
					}
				}
				if(bar_found) break;
			}
			if(bar_found) break;
		}

		if(bar_found){
			//it's docked
			cb.m_dir=dir;
			cb.m_row=row;
			cb.m_idx=idx;

			m_dockedbars[dir].m_rows[row]->m_bars.erase(m_dockedbars[dir].m_rows[row]->m_bars.begin()+idx);

			if(cb.m_frame==NULL){
				cb.m_pos=cb.m_bar->GetPosition();
				cb.m_size=cb.m_bar->GetSize();
			}
		}

		cb.m_bar->Destroy();
		cb.m_bar=NULL;

		if(cb.m_frame!=NULL){
			//it's undocked
			cb.m_pos=cb.m_frame->GetPosition();
			cb.m_frame->Destroy();
			cb.m_frame=NULL;
		}

		if(bar_found) LayoutEx(NULL,wxPoint());
		
		UpdateAcceleratorTable();
	}

	return true;
}

bool wxFakeCommandBarFrame::MakeMenuFloatByIndex(int index,const wxPoint& pos,const wxPoint& drag_offset){
	wxFakeDockableCommandBar &cb=m_dockablebars[index];

	//use new dragging mechanism
	if(cb.m_frame!=NULL){
		cb.m_frame->Move(pos);
	}else{
		cb.m_flags|=fdcbUndocked;

		wxFakeCommandBarDataBase *bar=cb.m_bardata;

		cb.m_frame=new wxFakeMiniFrame();
		cb.m_frame->SetArtProvider(m_artprov);
		cb.m_frame->DockManager(this);
		cb.m_frame->Create(this,wxID_ANY,bar->GetCaption(),pos,wxSize(64,16),
			fpstToolbarMode
			|((cb.m_flags & fdcbClosable)!=0?fpstCloseBox:0)
			|((cb.m_flags & fdcbShowChevron)!=0?fpstCustomBox:0)
			);
		cb.m_frame->Move(pos);

		if(cb.m_bar==NULL){
			cb.m_bar=new wxFakeCommandBar(cb.m_frame,wxID_ANY,wxPoint(-10000,-10000),wxSize(1,1));
			cb.m_bar->SetArtProvider(m_artprov);
			cb.m_bar->IsMainMenu((cb.m_flags & fdcbMainMenu)!=0);
			cb.m_bar->Dockable(true);
			cb.m_bar->Undocked(true);
			cb.m_bar->DockManager(this);
			cb.m_bar->ShowChevron((cb.m_flags & fdcbShowChevron)!=0);
			cb.m_bar->SetBar(bar);
		}else{
			//find it first
			int dir=0,row=0,idx=0;
			bool bar_found=false;

			for(dir=0;dir<4;dir++){
				int m1=m_dockedbars[dir].m_rows.size();
				for(row=0;row<m1;row++){
					int m2=m_dockedbars[dir].m_rows[row]->m_bars.size();
					for(idx=0;idx<m2;idx++){
						if(m_dockedbars[dir].m_rows[row]->m_bars[idx]==cb.m_bar){
							bar_found=true;
							break;
						}
					}
					if(bar_found) break;
				}
				if(bar_found) break;
			}

			if(bar_found){
				//it's docked
				cb.m_dir=dir;
				cb.m_row=row;
				cb.m_idx=idx;

				m_dockedbars[dir].m_rows[row]->m_bars.erase(m_dockedbars[dir].m_rows[row]->m_bars.begin()+idx);

				cb.m_pos=cb.m_bar->GetPosition();
				cb.m_size=cb.m_bar->GetSize();
			}

			//float it
			cb.m_bar->Direction(fcbdTop);
			cb.m_bar->Undocked(true);
			cb.m_bar->Reparent(cb.m_frame);

			if(bar_found) LayoutEx(NULL,wxPoint());
		}

		cb.m_bar->Layout();
		cb.m_frame->SetClientSize(cb.m_bar->FullSize());
		cb.m_frame->SetClientWindow(cb.m_bar);

		cb.m_frame->Show();
	}

	m_drag_index=index;
	m_drag_offset=drag_offset;
	m_temporary_disable_dock=true;

#ifdef __WXGTK__
	SetCursor(wxCursor(wxCURSOR_SIZENWSE));
#else
	SetCursor(wxCursor(wxCURSOR_SIZING));
#endif

	if(!HasCapture()) CaptureMouse();

	return true;
}

void wxFakeCommandBarFrame::PopupCustomCommandBarMenu(){
	if(m_dockablebars_dirty){
		if(m_dockablebars_menu==NULL){
			m_dockablebars_menu=new wxFakeCommandBarData();
		}else{
			m_dockablebars_menu->Buttons().clear();
		}

		int m=m_dockablebars.size();

		for(int i=0;i<m;i++){
			if((m_dockablebars[i].m_flags & fdcbHideFromCustomMenu)==0){
				m_dockablebars_menu->Buttons().push_back(wxFakeButtonData(i)
					.Type(fbttCheck)
					.CaptionText(m_dockablebars[i].m_bardata->GetCaption())
					.Enabled((m_dockablebars[i].m_flags & fdcbClosable)!=0 || m_dockablebars[i].m_bar==NULL)
					.Value(m_dockablebars[i].m_bar!=NULL?fbtvChecked:fbtvUnchecked)
					);
			}
		}

		if(!m_dockablebars_menu->Buttons().empty()){
			m_dockablebars_menu->Buttons().push_back(wxFakeButtonData().Type(fbttSeparator));
		}

		//TODO:
		m_dockablebars_menu->Buttons().push_back(wxFakeButtonData(m+1).Caption("&Customize...").Enabled(false));

		m_dockablebars_menu->SetDirty();

		m_dockablebars_dirty=false;
	}else{
		int m=m_dockablebars.size();
		wxVector<wxFakeButtonData> &btns=m_dockablebars_menu->Buttons();

		for(int i=0;i<(int)btns.size();i++){
			int idx=btns[i].ID();
			if(idx>=0 && idx<m){
				btns[i].Enabled((m_dockablebars[idx].m_flags & fdcbClosable)!=0 || m_dockablebars[idx].m_bar==NULL)
					.Value(m_dockablebars[idx].m_bar!=NULL?fbtvChecked:fbtvUnchecked);
			}
		}
	}

	PopupMenuEx(m_dockablebars_menu);
}

void wxFakeCommandBarFrame::OnContextMenu(wxContextMenuEvent& event){
	if(!IsShown()) return;
	if(wxWindow::GetCapture()!=NULL) return;

	wxPoint p=ScreenToClient(event.GetPosition());
	wxSize sz=GetClientSize();

	if(p.x>=0 && p.y>=0 && p.x<sz.x && p.y<sz.y &&
		(p.x<m_dockedbars[fcbdLeft].m_size || p.x>=sz.x-m_dockedbars[fcbdRight].m_size
		|| p.y<m_dockedbars[fcbdTop].m_size || p.y>=sz.y-m_dockedbars[fcbdBottom].m_size))
	{
		PopupCustomCommandBarMenu();
		return;
	}

	event.Skip();
}

bool wxFakeCommandBarFrame::BeginDragCommandBarByIndex(int index,const wxPoint& drag_offset){
	m_drag_index=index;
	m_drag_offset=drag_offset;
	m_temporary_disable_dock=false;

#ifdef __WXGTK__
	SetCursor(wxCursor(wxCURSOR_SIZENWSE));
#else
	SetCursor(wxCursor(wxCURSOR_SIZING));
#endif

	if(!HasCapture()) CaptureMouse();

	return true;
}

//internal function
wxFakeDockablePaneSplitterHitTestResult wxFakeDockablePaneRow::SplitterHitTest(int layer,int dir,int row,const wxPoint& pos){
	wxFakeDockablePaneSplitterHitTestResult ret={-1,-1,-1,-1,NULL};
	wxFakeDockablePaneRow *subrow=this;

	while(subrow!=NULL){
		if(!subrow->m_rect.Contains(pos)) break;

		int m=subrow->m_pane.size();
		if(m<=0) break;

		for(int i=0;i<m;i++){
			wxRect r;

			if(subrow->m_pane[i].m_subrow!=NULL){
				r=subrow->m_pane[i].m_subrow->m_rect;
			}else{
				r=subrow->m_pane[i].m_pane->m_rect;
			}

			if(r.Contains(pos)){
				subrow=subrow->m_pane[i].m_subrow;
				break;
			}

			if(i<m-1){
				if((dir==fcbdLeft || dir==fcbdRight)
					?(pos.y<r.y+r.height+4)
					:(pos.x<r.x+r.width+4))
				{
					ret.m_layer=layer;
					ret.m_dir=dir;
					ret.m_row=row;
					ret.m_idx=i;
					ret.m_subrow=subrow;
					return ret;
				}
			}else{
				return ret;
			}
		}
	}

	return ret;
}

//internal function
wxFakeDockablePaneSplitterHitTestResult wxFakeDockablePaneDockArea::SplitterHitTest(int layer,int dir,const wxRect& rect,const wxPoint& pos){
	wxFakeDockablePaneSplitterHitTestResult ret={-1,-1,-1,-1,NULL};

	for(int row=0;row<(int)m_rows.size();row++){
		wxRect r=m_rows[row]->m_rect;

		if(r.Contains(pos)){
			return m_rows[row]->SplitterHitTest(layer,dir,row,pos);
		}

		bool b=false;
		switch(dir){
		default:
			b=(pos.y<r.y+r.height+4);
			break;
		case fcbdBottom:
			b=(pos.y>=r.y-4);
			break;
		case fcbdLeft:
			b=(pos.x<r.x+r.width+4);
			break;
		case fcbdRight:
			b=(pos.x>=r.x-4);
			break;
		}

		if(b){
			ret.m_layer=layer;
			ret.m_dir=dir;
			ret.m_row=row;
			break;
		}
	}

	//over
	return ret;
}

wxFakeDockablePaneSplitterHitTestResult wxFakeCommandBarFrame::SplitterHitTest(const wxPoint& pos){
	wxFakeDockablePaneSplitterHitTestResult ret={-1,-1,-1,-1,NULL};

	for(int layer=0;layer<(int)m_panelayer.size();layer++){
		wxFakeDockablePaneDockLayer* docklayer=m_panelayer[layer];

		wxRect r=docklayer->m_rect;
		if(!r.Contains(pos)) break;

		int y1=docklayer->m_dockedpanes[fcbdTop].m_size;
		int y2=docklayer->m_dockedpanes[fcbdBottom].m_size;

		if(pos.y<r.y+y1){
			return docklayer->m_dockedpanes[fcbdTop].SplitterHitTest(layer,fcbdTop,r,pos);
		}
		if(pos.y>=r.y+r.height-y2){
			return docklayer->m_dockedpanes[fcbdBottom].SplitterHitTest(layer,fcbdBottom,r,pos);
		}

		r.y+=y1;
		r.height-=y1+y2;

		if(pos.x<r.x+docklayer->m_dockedpanes[fcbdLeft].m_size){
			return docklayer->m_dockedpanes[fcbdLeft].SplitterHitTest(layer,fcbdLeft,r,pos);
		}
		if(pos.x>=r.x+r.width-docklayer->m_dockedpanes[fcbdRight].m_size){
			return docklayer->m_dockedpanes[fcbdRight].SplitterHitTest(layer,fcbdRight,r,pos);
		}
	}

	//over
	return ret;
}

void wxFakeCommandBarFrame::OnMouseMove(wxMouseEvent& event){
	wxPoint p=event.GetPosition();

	if(m_drag_index>=0 && m_drag_index<(int)m_dockablebars.size() && event.LeftIsDown()){
		wxPoint p2=ClientToScreen(p);
		p2.x-=m_drag_offset.x;
		p2.y-=m_drag_offset.y;
		if(m_temporary_disable_dock || !LayoutEx(m_dockablebars[m_drag_index].m_bar,p2)){
			if(m_dockablebars[m_drag_index].m_frame!=NULL){
				m_dockablebars[m_drag_index].m_frame->Move(p2);
			}
		}

		if(m_timer->IsRunning()){
			m_timer->Stop();
		}

		return;
	}

	//check mouse cursor
	if(!event.LeftIsDown()){

		wxFakeDockablePaneSplitterHitTestResult hittest=SplitterHitTest(p);
		if(hittest.m_layer>=0){
			int dir;

			if(hittest.m_subrow!=NULL){
				dir=hittest.m_subrow->m_dir;
				dir=(dir==fcbdLeft || dir==fcbdRight)?fcbdTop:fcbdLeft;
			}else{
				dir=hittest.m_dir;
			}

			SetCursor(wxCursor((dir==fcbdLeft || dir==fcbdRight)?wxCURSOR_SIZEWE:wxCURSOR_SIZENS));

			if(!m_timer->IsRunning()){
				m_timer->Start(100);
			}
		}else{
			SetCursor(wxNullCursor);

			if(m_timer->IsRunning()){
				m_timer->Stop();
			}
		}
	}else{
		//check and draw dragging splitter
		if(m_pane_drag.m_layer>=0){
			wxRect r;
			int new_height;

			if(m_pane_drag.m_subrow!=NULL){
				int dir=m_pane_drag.m_subrow->m_dir;
				wxFakeDockablePaneItem &item=m_pane_drag.m_subrow->m_pane[m_pane_drag.m_idx];

				if(item.m_subrow!=NULL){
					r=item.m_subrow->m_rect;
				}else{
					r=item.m_pane->m_rect;
				}

				if(dir==fcbdLeft || dir==fcbdRight){
					new_height=m_drag_offset.x+p.y-(m_pane_drag.m_idx>0?4:2);

					r.y+=new_height;
					r.height=4;
				}else{
					new_height=m_drag_offset.x+p.x-(m_pane_drag.m_idx>0?4:2);

					r.x+=new_height;
					r.width=4;
				}				
			}else{
				r=m_panelayer[m_pane_drag.m_layer]->m_dockedpanes[m_pane_drag.m_dir].m_rows[m_pane_drag.m_row]->m_rect;

				switch(m_pane_drag.m_dir){
				default:
					new_height=m_drag_offset.x+p.y;
					if(new_height<32) new_height=32;
					r.y+=new_height;
					r.height=4;
					break;
				case fcbdBottom:
					new_height=m_drag_offset.x-p.y;
					if(new_height<32) new_height=32;
					r.y+=r.height-new_height-4;
					r.height=4;
					break;
				case fcbdLeft:
					new_height=m_drag_offset.x+p.x;
					if(new_height<32) new_height=32;
					r.x+=new_height;
					r.width=4;
					break;
				case fcbdRight:
					new_height=m_drag_offset.x-p.x;
					if(new_height<32) new_height=32;
					r.x+=r.width-new_height-4;
					r.width=4;
					break;
				}
			}

			wxPoint p1=ClientToScreen(wxPoint());
			wxPoint p2=GetScreenRect().GetTopLeft();
			r.x+=p1.x-p2.x;
			r.y+=p1.y-p2.y;

			wxWindowDC dc(this);
			DrawResizeHint(dc,m_pane_drag_old_rect);
			DrawResizeHint(dc,r);

			m_pane_drag_old_rect=r;
		}
	}
}

void wxFakeCommandBarFrame::OnMouseDown(wxMouseEvent& event){
	if(!HasCapture()) CaptureMouse();

	//check dragging splitter
	wxPoint p=event.GetPosition();

	m_pane_drag=SplitterHitTest(p);
	if(m_pane_drag.m_layer>=0){
		wxRect r;

		if(m_pane_drag.m_subrow!=NULL){
			int dir=m_pane_drag.m_subrow->m_dir;
			wxFakeDockablePaneItem &item=m_pane_drag.m_subrow->m_pane[m_pane_drag.m_idx];

			if(item.m_subrow!=NULL){
				r=item.m_subrow->m_rect;
			}else{
				r=item.m_pane->m_rect;
			}

			if(dir==fcbdLeft || dir==fcbdRight){
				m_drag_offset.x=r.height+(m_pane_drag.m_idx>0?4:2)-p.y;

				r.y+=r.height;
				r.height=4;
			}else{
				m_drag_offset.x=r.width+(m_pane_drag.m_idx>0?4:2)-p.x;

				r.x+=r.width;
				r.width=4;
			}
		}else{
			r=m_panelayer[m_pane_drag.m_layer]->m_dockedpanes[m_pane_drag.m_dir].m_rows[m_pane_drag.m_row]->m_rect;

			switch(m_pane_drag.m_dir){
			default:
				m_drag_offset.x=r.height-p.y;
				r.y+=r.height;
				r.height=4;
				break;
			case fcbdBottom:
				m_drag_offset.x=r.height+p.y;
				r.y-=4;
				r.height=4;
				break;
			case fcbdLeft:
				m_drag_offset.x=r.width-p.x;
				r.x+=r.width;
				r.width=4;
				break;
			case fcbdRight:
				m_drag_offset.x=r.width+p.x;
				r.x-=4;
				r.width=4;
				break;
			}
		}

		wxPoint p1=ClientToScreen(wxPoint());
		wxPoint p2=GetScreenRect().GetTopLeft();
		r.x+=p1.x-p2.x;
		r.y+=p1.y-p2.y;

		wxWindowDC dc(this);
		DrawResizeHint(dc,r);

		m_pane_drag_old_rect=r;
	}
}

void wxFakeCommandBarFrame::OnMouseUp(wxMouseEvent& event){
	if(HasCapture()) ReleaseMouse();

	SetCursor(wxNullCursor);

	if(m_drag_index>=0){
		if(m_drag_index<(int)m_dockablebars.size() && m_dockablebars[m_drag_index].m_frame==NULL){
			Refresh();
		}
		m_drag_index=-1;
		m_temporary_disable_dock=false;
		UpdateAcceleratorTable();
	}

	if(m_pane_drag.m_layer>=0){
		//erase old resize hint
		{
			wxWindowDC dc(this);
			DrawResizeHint(dc,m_pane_drag_old_rect);
		}

		//check dragging splitter
		wxPoint p=event.GetPosition();

		if(m_pane_drag.m_subrow!=NULL){
			int dir=m_pane_drag.m_subrow->m_dir;

			wxFakeDockablePaneItem &item1=m_pane_drag.m_subrow->m_pane[m_pane_drag.m_idx];
			wxFakeDockablePaneItem &item2=m_pane_drag.m_subrow->m_pane[m_pane_drag.m_idx+1];

			float *f1,*f2;

			if(item1.m_subrow!=NULL){
				f1=&item1.m_subrow->m_scale;
			}else{
				f1=&item1.m_pane->m_scale;
			}
			if(item2.m_subrow!=NULL){
				f2=&item2.m_subrow->m_scale;
			}else{
				f2=&item2.m_pane->m_scale;
			}

			wxRect r0=m_pane_drag.m_subrow->m_rect;

			float f1n=0.0f,f2n,threshold;
			if(dir==fcbdLeft || dir==fcbdRight){
				if(r0.height>0){
					f1n=float(m_drag_offset.x+p.y)/float(r0.height);
					threshold=32.0f/float(r0.height);
				}else{
					threshold=0.01f;
				}
			}else{
				if(r0.width>0){
					f1n=float(m_drag_offset.x+p.x)/float(r0.width);
					threshold=32.0f/float(r0.width);
				}else{
					threshold=0.01f;
				}
			}

			if(f1n<threshold) f1n=threshold;
			f2n=*f1+*f2-f1n;
			if(f2n<threshold){
				f2n=threshold;
				f1n=*f1+*f2-f2n;
				if(f1n<threshold) f1n=threshold;
			}

			if(abs(*f1-f1n)>0.0001f || abs(*f2-f2n)>0.0001f){
				*f1=f1n;
				*f2=f2n;

				LayoutEx(NULL,wxPoint());
			}
		}else{
			int new_height;

			switch(m_pane_drag.m_dir){
				default:
					new_height=m_drag_offset.x+p.y;
					break;
				case fcbdBottom:
					new_height=m_drag_offset.x-p.y;
					break;
				case fcbdLeft:
					new_height=m_drag_offset.x+p.x;
					break;
				case fcbdRight:
					new_height=m_drag_offset.x-p.x;
					break;
			}

			if(new_height<32) new_height=32;
			if(new_height!=m_panelayer[m_pane_drag.m_layer]->m_dockedpanes[m_pane_drag.m_dir].m_rows[m_pane_drag.m_row]->m_size){
				m_panelayer[m_pane_drag.m_layer]->m_dockedpanes[m_pane_drag.m_dir].m_rows[m_pane_drag.m_row]->m_size=new_height;
				LayoutEx(NULL,wxPoint());
			}
		}

		Refresh();
		m_pane_drag.m_layer=-1;
	}
}

void wxFakeCommandBarFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent& event){
	wxMouseEvent evt2(wxEVT_LEAVE_WINDOW);
	evt2.m_x=-1;
	evt2.m_y=-1;
	OnMouseUp(evt2);
}

void wxFakeCommandBarFrame::UpdateDirty(bool unDirty){
	for(int i=0;i<(int)m_dockablebars.size();i++){
		wxFakeCommandBar *cb=m_dockablebars[i].m_bar;
		if(cb!=NULL){
			cb->UpdateDirty(false);
		}
	}

	if(unDirty) UnDirtyVisible();
}

void wxFakeCommandBarFrame::UnDirty(){
	for(int i=0;i<(int)m_dockablebars.size();i++){
		wxFakeCommandBarDataBase *bar=m_dockablebars[i].m_bardata;
		for(int j=0,m=bar->ButtonCount();j<m;j++){
			bar->GetButton(j)->UnDirty();
		}
		bar->SetDirty(false);
	}
}

void wxFakeCommandBarFrame::UnDirtyVisible(){
	for(int i=0;i<(int)m_dockablebars.size();i++){
		wxFakeCommandBar *cb=m_dockablebars[i].m_bar;
		if(m_dockablebars[i].m_bar!=NULL && m_dockablebars[i].m_bardata!=NULL){
			wxFakeCommandBarDataBase *bar=m_dockablebars[i].m_bardata;
			for(int j=0,m=bar->ButtonCount();j<m;j++){
				bar->GetButton(j)->UnDirty();
			}
			bar->SetDirty(false);
		}
	}
}

void wxFakeCommandBarFrame::OnIdle(wxIdleEvent& event){
	if(m_update_on_next_idle){
		m_update_on_next_idle=false;
		UpdateDirty();
	}

	event.Skip();
}

void wxFakeCommandBarFrame::OnTimer(wxTimerEvent& event){
	if(HasCapture()) return;

	//check mouse cursor
	wxFakeDockablePaneSplitterHitTestResult hittest=SplitterHitTest(ScreenToClient(wxGetMousePosition()));
	if(hittest.m_layer<0){
		SetCursor(wxNullCursor);
		if(m_timer->IsRunning()){
			m_timer->Stop();
		}
	}
}
