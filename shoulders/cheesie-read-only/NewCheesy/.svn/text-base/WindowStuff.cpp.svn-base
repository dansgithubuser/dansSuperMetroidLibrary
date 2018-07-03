#include "main.h"
using namespace std;

wxSizer* MainPanel::BuildEditingOptions(){
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Editing Options"));
	chkTileEdit=new wxCheckBox(this, CHKTILEEDIT, wxT("Edit Tiles"));
	chkPropEdit=new wxCheckBox(this, CHKPROPEDIT,wxT("Edit Properties"));

	mainsiz->Add(chkTileEdit);
	mainsiz->Add(chkPropEdit);
	return mainsiz;
	//Make a frame, checkbox for tile editing, checkbox for tile prop editing

}
wxSizer* MainPanel::BuildMap()
{
	// house the whole box in a static box (aka "group box")
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxVERTICAL,this,wxT("Map"));

	// start building "rows" of controls.
	//   these rows added to mainsiz one at a time, with 4 pixel spacers between each one
	//   note that while I reuse the 'row' variable, a new sizer is created each
	//   time with 'new'.
	wxBoxSizer* row;
	wxBoxSizer* col;
	//  top row:  name
	
	TheMapWindow= new PicWindow(this,512+20,256+20);
	row = new wxBoxSizer(wxHORIZONTAL);

	TheMapWindow->MakeVSB();
	
	TheMapWindow->VSB->SetScrollbar(0,5,16,4);
  
	TheMapWindow->MakeHSB();
	TheMapWindow->HSB->SetScrollbar(0,5,16,4);

	row = new wxBoxSizer(wxHORIZONTAL);
	TheMapWindow->gI=new gameImages(&TheMapWindow->dcPic);
	row->Add(TheMapWindow);
	
	mainsiz->Add(row);
	mainsiz->AddSpacer(4);

	// next row:  2x2 choice box
	// next row:  2x2 choice box
	row = new wxBoxSizer(wxHORIZONTAL);

	row->AddSpacer(8);
	{
		wxBoxSizer* col = new wxBoxSizer(wxHORIZONTAL);
		col->AddStretchSpacer(1);

		row->Add(col,1,wxALIGN_RIGHT | wxALIGN_TOP|wxEXPAND);

		//row->Add(col,3,wxALIGN_RIGHT | wxALIGN_BOTTOM);
	}

	mainsiz->Add(row,0,wxEXPAND);



	// that's it, this sizer is complete
	return mainsiz;

}

wxSizer* MainPanel::BuildTileset()
{
	// house the whole box in a static box (aka "group box")
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Tileset"));

	// start building "rows" of controls.
	//   these rows added to mainsiz one at a time, with 4 pixel spacers between each one
	//   note that while I reuse the 'row' variable, a new sizer is created each
	//   time with 'new'.
	wxBoxSizer* row;
	wxBoxSizer* col;
	//  top row:  name
	
	TheTileset= new PicWindow(this,528,128);
	TheTileset->MakeVSB();
	TheTileset->VSB->SetScrollbar(0,5,16,4);

	row = new wxBoxSizer(wxHORIZONTAL);
	TheTileset->gI=new gameImages(&TheTileset->dcPic);

	row->Add(TheTileset);
	
	mainsiz->Add(row);
	mainsiz->AddSpacer(4);

	// next row:  2x2 choice box
	// next row:  2x2 choice box
	row = new wxBoxSizer(wxHORIZONTAL);

	row->AddSpacer(8);
	{
		wxBoxSizer* col = new wxBoxSizer(wxHORIZONTAL);
		col->AddStretchSpacer(1);

		row->Add(col,1,wxALIGN_RIGHT | wxALIGN_TOP|wxEXPAND);

		//row->Add(col,3,wxALIGN_RIGHT | wxALIGN_BOTTOM);
	}

	mainsiz->Add(row,0,wxEXPAND);


	// that's it, this sizer is complete
	return mainsiz;
}


