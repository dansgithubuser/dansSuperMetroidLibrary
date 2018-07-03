#include "gameImage.h"

#ifndef __TRAINERPIC_H__       // include protection -- prevents the same file
#define __TRAINERPIC_H__       //    from being included multiple times

class  PicWindow
{
	enum{
		HORIZSB,
		VERTSB
	};
public:
	// constructor
	//wxBitmap bmpTrainer;
	gameImages*  gI;
	int Height;
	int Width;
	int rButton;
	int lButton;
	PicWindow(int iWidth, int iHeight)   // just have it call the wxWindow ctor
	{

		gI=new gameImages;
		Height=iHeight;
		Width=iWidth;
	}

	void MakeHSB();
	void MakeVSB();
private:
protected:


};

#endif // __MAINPANEL_H__