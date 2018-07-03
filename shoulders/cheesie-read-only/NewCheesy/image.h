
#include "tilebuffer.h"


typedef	UINT		pal_t;

typedef int			DRAWFLAGS;
#define FLIPX		0x01
#define FLIPY		0x02
#define NOTRANS		0x04



class Image
{
public:
	Image();
	~Image();
	int			Height;
	int         Width;
	void		Destroy();
	void		Create(int width,int height);

	void		Clear();
	void		Clear(int x,int y,int w,int h);
	void		Fill(int x,int y,int w,int h,BYTE color);

	void		SetPalette(long* pal,int first = 0,int count = 256);
	void		SetPaletteRaw(const BYTE* pal,int first = 0,int count = 256);

	// Draw -- draws tiles to this image
	void		Draw(tile_t tile,long dstx,long dsty,int palette = 0,DRAWFLAGS flgs = 0,int forceclr=-1);
	void		Draw(TileBuffer& tilebuf,long dstx,long dsty,int pftt,int forceclr=-1);

	// Blit -- Blits this image to an HDC (actually display on screen)
	void		Blit(HDC dc,int dstx,int dsty,int width,int height,int srcx,int srcy);
	void		TransBlit(HDC dc,int dstx,int dsty,int width,int height,int srcx,int srcy);
	void        AlphaBlit(HDC dc,int dstx,int dsty,int width,int height,int srcx,int srcy,int a);
	HDC         DC(); 
	int  IsCreated();
	HDC			mDC;
	void SaveToFile(FILE* file);
	BYTE*		pixels;
protected:
	void		Zero();
	void		RebuildMask();

	BYTE*		maskpixels;
	HBITMAP		hBmp;
	HBITMAP		hBmpOld;

	int			nWidth;
	int			nHeight;
	int			nPitch;

	HDC			mMaskDC;
	HBITMAP		hMaskBmp;
	HBITMAP		hMaskBmpOld;
	
	int			nMaskPitch;

	int			bMaskValid;
};