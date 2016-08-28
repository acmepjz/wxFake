#include <wx/event.h>
#include <iostream>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"

IMPLEMENT_CLASS(wxFakeCommandBarButtonEvent,wxCommandEvent)

wxFakeButtonData& wxFakeButtonData::Caption(const wxString& caption,bool setShortcut,bool setAccelerator){
	if(setShortcut){
		size_t pos=caption.find_first_of('\t');
		if(pos==wxString::npos){
			m_caption=caption;
		}else{
			m_caption=caption.substr(0,pos);
			m_shortcut=caption.substr(pos+1);
			if(setAccelerator){
				wxAcceleratorEntry accel;
				if(!m_shortcut.empty() && accel.FromString(m_shortcut)){
					m_accel=wxAcceleratorEntry(accel.GetFlags(),accel.GetKeyCode(),m_id);
				}else{
					m_accel=wxAcceleratorEntry();
				}
			}
		}
	}else{
		m_caption=caption;
	}

	m_captiontext=MyStripMenuCodesEx(m_caption,m_mnemonics,m_mnemonics_pos);

	m_dirty=true;
	m_sizedirty=true;

	return *this;
}

wxFakeButtonData& wxFakeButtonData::Shortcut(const wxString& shortcut,bool setAccelerator){
	m_shortcut=shortcut;

	if(setAccelerator){
		wxAcceleratorEntry accel;
		if(!m_shortcut.empty() && accel.FromString(m_shortcut)){
			m_accel=wxAcceleratorEntry(accel.GetFlags(),accel.GetKeyCode(),m_id);
		}else{
			m_accel=wxAcceleratorEntry();
		}
	}

	m_dirty=true;
	m_sizedirty=true;

	return *this;
}

void wxFakeButtonData::OnClick(wxWindow* wnd,wxEvtHandler* handler,wxFakeCommandBarDataBase* bar,int btnindex){
	//automatic change the value
	switch(m_type){
	case fbttCheck:
		m_value=(m_value==fbtvUnchecked)?fbtvChecked:fbtvUnchecked;
		m_dirty=true;
		break;
	case fbttCheck3State:
		m_value=(m_value==fbtvUnchecked)?fbtvChecked:((m_value==fbtvChecked)?fbtvUndetermined:fbtvUnchecked);
		m_dirty=true;
		break;
	case fbttOptionNullable:
		if(m_value==fbtvChecked){
			m_value=fbtvUnchecked;
			m_dirty=true;
			break;
		}
		//fall through
	case fbttOption:
		if(bar!=NULL){
			//TODO: something goes wrong with chevron menu
			for(int j=0,m=bar->ButtonCount();j<m;j++){
				wxFakeButtonData *btn=bar->GetButton(j);
				if(btn->m_type==m_type && btn->m_groupindex==m_groupindex){
					wxFakeButtonValue newvalue=(btn==this?fbtvChecked:fbtvUnchecked);
					if(newvalue!=btn->m_value){
						btn->m_value=newvalue;
						btn->m_dirty=true;
					}
				}
			}
		}
		break;
	default:
		break;
	}

	//raise event. TODO: set correct event receiver
	if(handler!=NULL){
		wxFakeCommandBarButtonEvent evt2(wxEVT_COMMAND_MENU_SELECTED,m_id);
		//evt2.SetEventObject(this); // XXX error
		evt2.SetButton(this);
		evt2.SetBar(bar);
		evt2.SetInt(m_value);
		evt2.SetButtonIndex(btnindex);

		//debug
		std::cout<<"wxFakeButtonData::OnClick btn="<<this<<" bar="<<bar<<" index="<<btnindex<<"\n";

		handler->AddPendingEvent(evt2);
	}
}

wxSize wxFakeCommandBarIcon::GetSize(wxFakeCommandBarArtBase* artprov) const{
	switch(m_type){
	case fcbiBitmap:
		return m_bitmap.GetSize();
		break;
	case fcbiColor:
		return artprov->GetColorSize();
		break;
	case fcbiOwnerDraw:
		return m_ownerdraw->GetSize(*this,artprov);
		break;
	default:
		return wxSize(0,0);
	}
}

void wxFakeCommandBarIcon::Draw(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,wxFakeCommandBarArtBase* artprov,int add_flags,const wxRect& rect){
	//TODO: draw highlight, etc.
	switch(m_type){
	case fcbiBitmap:
		if(btn.Flags()&fbtfDisabled){
			//??? performance issues
			dc.DrawBitmap(m_bitmap.ConvertToDisabled(),rect.x,rect.y,true);
		}else{
			dc.DrawBitmap(m_bitmap,rect.x,rect.y,true);
		}
		break;
	case fcbiColor:
		artprov->DrawColor(dc,wnd,m_color,btn.Flags(),add_flags,rect);
		break;
	case fcbiOwnerDraw:
		m_ownerdraw->Draw(dc,wnd,btn,*this,artprov,add_flags,rect);
		break;
	default:
		break;
	}
}

wxFakeCommandBarDataBase::~wxFakeCommandBarDataBase(){
}

bool wxFakeCommandBarDataBase::UpdateDirty(){
	int m=ButtonCount();

	for(int i=0;i<m;i++){
		if(GetButton(i)->SizeDirty()){
			m_dirty=true;
			break;
		}
	}

	return m_dirty;
}

void wxFakeCommandBarDataBase::LayoutMenuButtons(wxDC& dc,wxWindow* wnd,wxFakeCommandBarArtBase* artprov){
	if(!m_dirty) return;

	wxFakeCommandBarDataBase *bar=m_old_bar;
	if(bar==NULL) bar=this;

	int leftMargin,topMargin,rightMargin,bottomMargin;
	wxSize sz(0,0);

	//measure and layout buttons
	m_btn.clear();

	int m=ButtonCount();

	int columnStart=0,rowStart=0;
	int x2=0,y=0;
	int max_w0=0,max_w=0,max_w2=0,max_wTotal=0;
	int max_h2=0;
	int oldMode=0;

	for(int i=0;i<m;i++){
		//get button
		wxFakeButtonData *btn=GetButton(i);
		if(btn==NULL || (btn->Flags() & fbtfHidden)!=0) continue;

		wxFakeButtonState state={};
		state.btn=btn;
		state.bar=bar;
		state.index=i+m_index_offset;

		//measure button
		state.size=artprov->MeasureButton(dc,wnd,*btn,m_flags);

		//flush toolbar mode row
		if((state.size.sizeMode!=fbsmToolBar || state.size.width<0) && oldMode==fbsmToolBar){
			//set button size
			for(int j=rowStart;j<(int)m_btn.size();j++){
				if(m_btn[j].size.sizeMode!=fbsmMenu){
					if(m_btn[j].size.height<0){
						m_btn[j].size.height=max_h2;
						m_btn[j].rect.height=max_h2;
					}
				}
			}

			rowStart=m_btn.size();

			if(x2-sz.x>max_wTotal) max_wTotal=x2-sz.x;
			y+=max_h2;

			max_h2=0;
			x2=sz.x;
		}

		switch(state.size.sizeMode){
		case fbsmMenu:
			if(state.size.height<0){ //start new column
				//check max size
				if(max_w0+max_w+max_w2<max_wTotal) max_w=max_wTotal-max_w0-max_w2;
				max_wTotal=max_w0+max_w+max_w2;

				//flush new column
				for(int j=columnStart;j<(int)m_btn.size();j++){
					if(m_btn[j].size.sizeMode==fbsmMenu || m_btn[j].size.width<0){
						if(m_btn[j].size.width<0){
							m_btn[j].size.width0=max_w0;
							m_btn[j].size.width=max_wTotal-max_w0;
							m_btn[j].size.width2=0;
						}else{
							m_btn[j].size.width0=max_w0;
							m_btn[j].size.width=max_w;
							m_btn[j].size.width2=max_w2;
						}
						m_btn[j].rect.width=max_wTotal;
					}
				}

				rowStart=columnStart=m_btn.size()+1;

				sz.x+=max_wTotal;
				if(y>sz.y) sz.y=y;

				//set rect
				state.rect=wxRect(sz.x,0,state.size.width,0);
				sz.x+=state.size.width;

				y=0;
				max_w0=0;
				max_w=0;
				max_w2=0;
				max_wTotal=0;
				max_h2=0;
				x2=sz.x;
			}else{
				//check max size
				if(state.size.width0>max_w0) max_w0=state.size.width0;
				if(state.size.width>max_w) max_w=state.size.width;
				if(state.size.width2>max_w2) max_w2=state.size.width2;

				rowStart=m_btn.size()+1;

				//set rect
				state.rect=wxRect(sz.x,y,0,state.size.height);
				y+=state.size.height;
			}

			oldMode=fbsmMenu;
			break;
		case fbsmToolBar:
		case fbsmToolBarStartNewRow:
			if(state.size.width<0){
				state.rect=wxRect(sz.x,y,0,state.size.height);
				y+=state.size.height;
				rowStart=m_btn.size()+1;
			}else{
				state.rect=wxRect(x2,y,state.size.width,state.size.height);
				x2+=state.size.width;
				if(state.size.height>max_h2) max_h2=state.size.height;
			}

			oldMode=fbsmToolBar;
			break;
		}

		m_btn.push_back(state);
	}

	//flush last row
	for(int j=rowStart;j<(int)m_btn.size();j++){
		if(m_btn[j].size.sizeMode!=fbsmMenu){
			if(m_btn[j].size.height<0){
				m_btn[j].size.height=max_h2;
				m_btn[j].rect.height=max_h2;
			}
		}
	}

	if(x2-sz.x>max_wTotal) max_wTotal=x2-sz.x;
	y+=max_h2;

	//flush last column
	if(max_w0+max_w+max_w2<max_wTotal) max_w=max_wTotal-max_w0-max_w2;
	max_wTotal=max_w0+max_w+max_w2;

	for(int j=columnStart;j<(int)m_btn.size();j++){
		if(m_btn[j].size.sizeMode==fbsmMenu || m_btn[j].size.width<0){
			if(m_btn[j].size.width<0){
				m_btn[j].size.width0=max_w0;
				m_btn[j].size.width=max_wTotal-max_w0;
				m_btn[j].size.width2=0;
			}else{
				m_btn[j].size.width0=max_w0;
				m_btn[j].size.width=max_w;
				m_btn[j].size.width2=max_w2;
			}
			m_btn[j].rect.width=max_wTotal;
		}
	}

	sz.x+=max_wTotal;
	if(y>sz.y) sz.y=y;

	//resize column separator
	for(int j=0;j<(int)m_btn.size();j++){
		if(m_btn[j].size.sizeMode==fbsmMenu && m_btn[j].size.height<0){
			m_btn[j].size.height=sz.y;
			m_btn[j].rect.height=sz.y;
		}
	}

	//check if there is no buttons
	if(m_btn.empty()){
		sz=artprov->GetEmptyMenuSize();
	}

	m_rcMenu=wxRect(sz);

	//check if there are grippers
	if(m_flags & fcbfDragToMakeThisMenuFloat){
		int h;
		artprov->GetMenuGripperSize(h,leftMargin,topMargin,rightMargin,bottomMargin);
		m_rcGripper.x=leftMargin;
		m_rcGripper.y=topMargin;
		m_rcGripper.width=sz.x-leftMargin-rightMargin;
		m_rcGripper.height=h;
		m_rcMenu.y+=topMargin+h+bottomMargin;
	}else{
		m_rcGripper=wxRect();
	}

	//check menu client size margin
	artprov->GetMenuClientSizeMargin(leftMargin,topMargin,rightMargin,bottomMargin);
	m_rcMenu.x+=leftMargin;
	m_rcMenu.y+=topMargin;
	m_rcGripper.x=leftMargin;
	m_rcGripper.y=topMargin;

	//resize the window
	m_szMenu=wxSize(m_rcMenu.x+sz.x+rightMargin,m_rcMenu.y+sz.y+bottomMargin);

	m_dirty=false;
}

wxFakeCommandBarData::~wxFakeCommandBarData(){
}

int wxFakeCommandBarData::ButtonCount() const{
	return m_buttons.size();
}

wxFakeButtonData* wxFakeCommandBarData::GetButton(int idx){
	return &m_buttons[idx];
}

wxFakeCommandBarRefData::~wxFakeCommandBarRefData(){
}

int wxFakeCommandBarRefData::ButtonCount() const{
	return m_buttons.size();
}

wxFakeButtonData* wxFakeCommandBarRefData::GetButton(int idx){
	return m_buttons[idx];
}

wxString MyStripMenuCodesEx(const wxString& in,int& out_mnemonics,int& out_mnemonics_pos)
{
	wxString out;

	size_t len = in.length();
	out.reserve(len);

	out_mnemonics = 0;
	out_mnemonics_pos = -1;

	for ( size_t n = 0; n < len; n++ )
	{
		wxChar ch = in[n];
		if ( ch == wxT('&') )
		{
			// skip it, it is used to introduce the accel char (or to quote
			// itself in which case it should still be skipped): note that it
			// can't be the last character of the string
			if ( ++n == len )
			{
				break;
			}
			else
			{
				// use the next char instead
				ch = in[n];
				if ( ch != wxT('&') && out_mnemonics_pos < 0)
				{
					// we got the mnemonics
					wxAcceleratorEntry accel;
					if(accel.FromString(wxString(ch))){
						out_mnemonics = accel.GetKeyCode();
					}
					out_mnemonics_pos = out.length();
				}
			}
		}

		out += ch;
	}

	return out;
}

wxEvent* wxFakeCommandBarButtonEvent::Clone() const{
	return new wxFakeCommandBarButtonEvent(*this);
}
