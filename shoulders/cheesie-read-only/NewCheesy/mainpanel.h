#include "picWindow.h"
#include "SMClass.h"
#include "gameImage.h"
#ifndef __MAINPANEL_H__       // include protection -- prevents the same file
#define __MAINPANEL_H__       //    from being included multiple times

/*
*	The class which represents the panel face (basically your dialog -- though
*    technically this isn't a dialog).
*/

class MainPanel
{
public:
	
	MainPanel();
	SMClass* TheGame;
protected:
	// a quick funciton that makes it a little easier to get a static string control


	// control IDs.  These ID numbers will be tied to controls so that you can bind
	//   and catch events from those controls
	// For the sake of keeping code easy to read, I tend to have IDs all uppercase
	//   and prefixed with ID_
	// Start IDs at wxID_HIGHEST, as lower numbers are reserved for common IDs (like the
	//   ID for the "OK" button).  enum is basically a #define, but it increments after
	//   each number so that each entry is unique
	// Controls only need a unique ID if you are catching events from them.  If you are uninterested
	//   in catching events for a control, you don't need to give it a unique ID.
	enum
	{
		
		CHKTILEEDIT,
		CHKPROPEDIT

		
	};

	// Now the actual controls.  The above IDs will be assigned to the controls when
	//   you create them.  These all must be pointers because they need to be created
	//   with 'new'.
	//
	// Keeping with naming consistency, I use a kind of hungarian notation, which
	//   prefixes the name with a short string indicating the type it is.  IE:
	//   chkBlah would be a checkbox, lstBlah would be a list box, etc
	PicWindow* TheMapWindow;
	PicWindow* TheTileset;
	
	
	// Support functions for building the dialog
	//////////////////////////////////////////////////////////////////

	
	
	// declare an event table so events get tied to those functions
	//   see also BEGIN_EVENT_TABLE in mainpanel.cpp
	//Editing Options prototypes and stuff
	
};



#endif // __MAINPANEL_H__