#include "TileHandling.h"

#include <windows.h>
unsigned short LastTile;
void TileDataStuff::operator = (MyRect*rect){
	X=rect->X;
	Y = rect ->Y;
	Height = rect->Height;
	Width = rect->Width;
}

unsigned short* TileDataStuff::operator [] (int index){
	return &(*DataArray())[index];
}

int TileDataStuff::Create(int Width, int Height){

	return Create(NULL, Width, Height);

}
int TileDataStuff::Create(std::vector<unsigned short>* pointer, int Width, int Height){

	usePnt=0;
	if(pointer){

		usePnt = 1;
		pData=pointer;
	}
	MyWidth=Width;
	MyHeight=Height;
	return Width*Height;
}
TileDataStuff::TileDataStuff(){
	//set everything to default
	X=0;
	Y=0;
	Width=1;
	Height=1;
	usePnt = 0;
	NullVect.resize(1);

}
int TileDataStuff::x(){ return X; };
int TileDataStuff::y(){ return Y; };
int TileDataStuff::width(){ return Width;};
int TileDataStuff::height(){return Height;};

int TileDataStuff::x(int val){return X=val; };
int TileDataStuff::y(int val){return Y=val; };
int TileDataStuff::width(int val){return Width=val;};
int TileDataStuff::height(int val){return Height=val;};

void TileDataStuff::FromTileSetCoords(){
	int baseX=0;
	int baseY=0;
	baseX=LastTile/TILEWIDTH;
	baseY=LastTile%(TILEWIDTH);
	Data.resize(Width*Height);//I think?
	int tmpx=0, tmpy=0;
	for(tmpx=0;tmpx<Width;tmpx++){
		for(tmpy=0;tmpy<Height;tmpy++){
			Data[tmpx+tmpy*Width]=(baseX+tmpx)+(baseY+tmpy)*32;//CONFUSING YES YES?
		}
	}

}

std::vector<unsigned short>* TileDataStuff::DataArray(){
	std::vector<unsigned char> tmp;

	if(usePnt){
		if(this->pData!=0){
			return this->pData;
		}else{
			return (std::vector<unsigned short> *)&NullVect;
		}
	}else{
		if(&this->Data!=0){
			
			return &this->Data;
		}else{
			return (std::vector<unsigned short> *)&NullVect;
		}
	}


}
void TileCopy(TileDataStuff* src, TileDataStuff* dst){
	int srcForm=0;
	int dstForm=0;
	int width;
	int height;

	unsigned short* srcPnt=NULL;
	unsigned short* dstPnt=NULL;
	//People are so gonna hate me for this.
	if(LastTile==-1) return;
	std::vector<unsigned short>* srcvPnt=src->DataArray();
	std::vector<unsigned short>* dstvPnt=dst->DataArray();
	if(srcvPnt==0 ||

			dstvPnt==0) return; 
	if(srcvPnt==(std::vector<unsigned short> *)&src->NullVect ||  
			dstvPnt==(std::vector<unsigned short> *)&dst->NullVect 
			) return;
	srcPnt=&srcvPnt->front();
	dstPnt=&dstvPnt->front();
	
	width  = dst->Width; //max(   mpTileset.Width-mpMap.Width,  0);// + 1;
	height = dst->Height; //max(  mpTileset.Height-mpMap.Height, 0);// + 1;
	int ny,nx,y,x,baseX,baseY;
	baseX=dst->X;
	baseY=dst->Y;
	for(ny=0;ny<height;ny++){
		for(nx=0;nx<width;nx++){
			for (y = 0; y < src->Height; y++){
				if(ny* src->Height+y>=dst->Height) break;
				for (x = 0; x <  src->Width; x++){
					if(nx* src->Width + x >= dst->Width) break;
					dstPnt[1+(baseX + x ) + ( baseY + y) * ((dst->MyWidth*TILEWIDTH)) + nx* src->Width + ny* src->Height *(dst->MyWidth*TILEWIDTH)]= srcPnt[0]+x+y*32;
				}
			}
		}
	}
}