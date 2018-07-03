
#include "main.h"
using namespace std;
///////////////////////////////////////////////////////
//  event table to tie events to our handling functions


///////////////////////////////////////////////////////
//  the constructor -- called when the window is created (with 'new')
//    you must call the base class's (in this case, wxPanel) construtor to tell
//    it who the parent window is.  After that, just fill it with the controls and stuff
//    that you want

int main(){
	int i;

	// {   // right column
	//wxBoxSizer* col = new wxBoxSizer(wxHORIZONTAL);
	///    
	//     row ->Add(BuildTileset());

	//row->Add(col,1,wxEXPAND | wxALIGN_CENTER_VERTICAL);
	//  }

	

	
	//mainbox->Add(BuildTileset());
	//rightbox->AddSpacer(8);             // put 8 pixels of padding between trainer/poke boxes


	//mainbox->Add(new wxListBox(this,1));
	int Address_Renamed=0x79938;//0x791F8;
	SMClass* TheGame=new SMClass("C:\\Cheesy\\Cheesy\\metroid.smc");
	TheGame->LoadHeader(Address_Renamed);
	
	TheGame->RoomStates.resize(TheGame->LoadMDB_StateSelect(Address_Renamed + 11));
	TheGame->iRoomState = 0;
	TheGame->LoadMDB_Roomstate(TheGame->RoomStatePointers[TheGame->iRoomState], &TheGame->RoomStates[TheGame->iRoomState]);
	TheGame->GrabRoom();
	
	TheGame->GrabTileset(TheGame->RoomStates[TheGame->iRoomState].GraphicsSet);
	gameImages* gI=new gameImages;
	gI->Set(&TheGame->Tiles);
	gI->Set(&TheGame->Pal);
	gI->Set(&TheGame->TSA.nTSA);
	gI->MakeImage(NULL,NULL,0,0,32,32);
	
	TheGame->DrawRoom(&TheMapWindow->dcPic,&TheTileset->dcPic);
	TheMapWindow->Refresh();
	return 0;
}

// more building (box building -- wee)

/////////////////////////////////////////////








