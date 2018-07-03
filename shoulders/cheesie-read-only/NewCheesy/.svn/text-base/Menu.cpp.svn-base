#include "menu.h"
#include <wx/wx.h>

clsMenu::clsMenu(wxFrame* parent)

{
	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(idOpen, wxT("Open ROM"));
	file->Append(wxID_EXIT, wxT("&Quit"));
	menubar->Append(file, wxT("&File"));
	parent->SetMenuBar(menubar);

	//Does nothing for now


	//parent->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
     // (wxObjectEventFunction) &clsMenu::OnQuit );


}
void clsMenu::OnQuit(wxCommandEvent& WXUNUSED(event)){
	wxMessageBox(wxT("I DO NOTHING"));

}
clsMenu::~clsMenu()
{


}

