#include "main.h"

int isRDown;
/*
int             DrawTileRect(HDC hdc, TileDataStuff* rect, int mul)
{
   RECT            myrect;

   HBRUSH          curbrush = CreateSolidBrush(RGB(255, 0, 0));

   myrect.left = rect->X ;
   myrect.top = rect->Y;
   myrect.right = rect->Width + myrect.left;
   myrect.bottom = (myrect.top + rect->Height) ;
   myrect.left  *=mul;
   myrect.right *=mul;
   myrect.bottom *=mul;
   myrect.top *= mul;
   FrameRect(hdc, &myrect, curbrush);
   DeleteObject(curbrush);

   return 0;
}
BOOL CALLBACK TilesetProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT ps;
	HDC hdc;
	RECT coord;
	RECT Move;
	RECT srcRect;
	RECT dstRect;
	RECT mouse;
	int i=0;
	int x=0;
	int y=0;

	switch (message) 
	{
		
	case WM_INITDIALOG:
		hwndTileset = hWnd;
		scrTilesetVal=0;
		scrTilesetMax=12;
		ChangeScrollbars(hWnd, 1, 0, &scrTilesetVal, scrTilesetMax);
		GetWindowRect(Tools[0],&Move);
		GetWindowRect(hwndTileset,&coord);
		coord.bottom -= coord.top;
		coord.right -= coord.left;
		coord.top = 0;
		coord.left = Move.right-Move.left+32+16+16;
		MoveWindow(hWnd,coord.left, coord.top, coord.right, coord.bottom,1);
		break;
	case WM_LBUTTONDOWN:
		
		
		if(wParam == MK_LBUTTON)	{
			x=GetX(lParam);
			x	/=16;
			y=GetY(lParam);
			y	/=16;
            y+=scrTilesetVal;
			TileData.x(x);
			TileData.y(y);
			LastTile=(x)*16+(y);
			TileData.FromTileSetCoords();
		}
		
		break;
	case WM_VSCROLL:
		UpdateScroll(hWnd,wParam,1,0,0, &scrTilesetVal, scrTilesetMax);

		//UpdateScroll(hWnd,wParam,1,vsbScroll,sTileset);
		//InvalidateRect(hwndTileset,0,1);
		
		InvalidateRect(hWnd,0,1 );
		break;
	case WM_RBUTTONDOWN:
		if(wParam == MK_RBUTTON)	{
			isRDown=1;
			
			
		}
		

		break;
	case WM_RBUTTONUP:
		if(wParam == MK_RBUTTON)	{
			isRDown=0;
			
			
		}
		
		break;
	case WM_MOUSEMOVE:
		if(!isRDown||wParam!=MK_RBUTTON){
			TileData.x(GetX(lParam)/TILEWIDTH);
			TileData.y(GetY(lParam)/TILEWIDTH);
		}else if(wParam==MK_RBUTTON){
		//R is down. 
			if(GetX(lParam)/16 > TileData.x()){
				TileData.width(GetX(lParam)/16-TileData.x());


			}
		}else if(wParam==MK_LBUTTON){

		SendMessage(hWnd, WM_LBUTTONDOWN, wParam, lParam);

		}
		InvalidateRect(hWnd,0,0);
		return 0;
		break;
	case WM_PAINT:
		
		
		
		hdc = BeginPaint(hWnd, &ps);
		
		//img[iTileset].Blit(hdc, 0,0,coord.right - coord.left ,coord.bottom-coord.top,0,nVScroll[sTileset]*16);
		//DrawTileRect(hdc,mpTileset);
		dstRect.left = 0;
		dstRect.top = 0;
		dstRect.right = 512;
		dstRect.bottom = 128;
		srcRect.left = 0;
		srcRect.top = scrTilesetVal*32;
		srcRect.right = 512;
		srcRect.bottom = 128;
		SNES.Blit(hdc,&dstRect,bbTileset.DC(),&srcRect, 1 );
		//BitBlt(hdc,0, 0,512,128,bbTileset.DC(),0,scrTilesetVal*32,SRCCOPY);
		DrawTileRect(hdc, &TileData,TILEWIDTH);
		
		DeleteDC(hdc);
		
		EndPaint(hWnd, &ps);
		return 0;
		break;
	case WM_SIZE:
		//Bleh this is an evil thing resizing tilesets
		//DrawTileset();
		break;
	case WM_DESTROY:
		
		break;
		
		
		
	}		
	return 0;//DefWindowProc(hWnd, message, wParam, lParam);
	
	
}

*/