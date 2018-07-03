#include "gameImage.h"
#include "gbatypes.h"
gameImages::gameImages(){
}
void gameImages::Set(vector<u8>* buffer){
	tiles=*buffer;	
};

void gameImages::Set(vector<COLOR>* buffer){
	palette=*buffer;
}
void gameImages::Set(vector<u16>* buffer){
	TSA=*buffer;
}

// <--Compile the above vars into a bitmap, can be used to draw a single block

void gameImages::MakeImage(GBAImage* theimage,vector<u16>* TileIDs,int x,int y,int width, int height){
	int i=0;
	GBATiles TileImage;	
	if(theimage==NULL) theimage=&img;
	vector<u16> tmp;

	TileImage.Load(&tiles[0],1024);
	img.Create(1024,1024);
	img.SetPalette(&palette[0]);
	u16 curTile;
	int nx=0,ny=0;
	i=0;

	//for( i= 0; i<TSA.size()/8;i++)
	//{
	//this is gonna be so fucking dumb
	int tst=0;
	if(TileIDs==NULL){
		for( i=tst; i<TSA.size()/8;i++)
		{
			curTile=i*4;
			
			
			theimage->DrawTile(TileImage,((i-tst) % 32) * 16,(((i-tst) /32)) * 16, TSA[curTile]);
			theimage->DrawTile(TileImage,((i-tst) % 32) * 16 + 8, (((i-tst) /32)) * 16 , TSA[(curTile)+1]);
			theimage->DrawTile(TileImage,((i-tst) % 32) * 16, (((i-tst) /32)) * 16+ 8, TSA[curTile+2]);
			theimage->DrawTile(TileImage,((i-tst) % 32) * 16 + 8, (((i-tst) /32)) * 16 + 8, TSA[curTile+3]);

		}
	}else{
		for(ny=0;ny<3;ny++){
			for(nx=0;nx<width;nx++){
				curTile=(*TileIDs)[(nx+ny*32)*4];
				
				theimage->DrawTile(TileImage,(nx * 16), (ny* 16), TSA[curTile]);
				theimage->DrawTile(TileImage,((nx * 16)) + 8, ( (ny* 16) * 16), TSA[(curTile)+1]);
				theimage->DrawTile(TileImage,((nx * 16)), ( (ny* 16)) + 8, TSA[curTile+2]);
				theimage->DrawTile(TileImage,((nx * 16)) + 8, ( (ny* 16)) + 8, TSA[curTile+3]);
			}
		}
	}
	// }
	//bbTileset.Create(2048,1024);

	
	//wxDCTileset->DrawBitmap(wxBitmap(imgTileset.GetImage()),0,0);
	theimage->BlitToDC(mDC,0,0,width*16,height*32,0,0);
}


