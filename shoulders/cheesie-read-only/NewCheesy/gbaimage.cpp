
#include "gbaimage.h"
#include <memory.h>

//////////////////////////////////////////////////

void GBAImage::Zero()
{
	pDC = 0;
	bDCValid = false;

	pImage = 0;
	nWd = 0;
	nHt = 0;
	memset(mPalette,0,sizeof(COLOR)*256);
}

void GBAImage::Destroy()
{
	if(pDC)
	delete pDC;
	if(pImage)
	delete[] pImage;
	Zero();
}

//////////////////////////////////////////////////

void GBAImage::Create(int width,int height)
{
	Destroy();
	nWd = width;
	nHt = height;
	pImage = new unsigned char[nWd*nHt];
	memset(pImage,0,sizeof(unsigned char)*nWd*nHt);

	pDC = new wxMemoryDC();
	bDCValid = false;
}

//////////////////////////////////////////////////

void GBAImage::SetPalette(const COLOR* pal,int first,int count)
{
	bDCValid = false;
	memcpy(&mPalette[first],pal,sizeof(COLOR)*count);
}

//////////////////////////////////////////////////

void GBAImage::DrawTile(unsigned char* tile,int dstx,int dsty,int palette,int flgs)
{
	if(!tile)
	return;
	if(!pImage)
	return;

	bDCValid = false;
	int xorx = (flgs & FLIPX) ? 7 : 0;
	int xory = (flgs & FLIPY) ? 7 : 0;

	unsigned char pal = (unsigned char)((palette & 0x0F) << 4);

	int ox = dstx;
	int x,y;
	int x2,y2;
	for(y = 0; (y < 8) && (dsty < nHt); ++y, ++dsty)
	{
		y2 = (y ^ xory) * 8;
		dstx = ox;

		for(x = 0; (x < 8) && (dstx < nWd); ++x, ++dstx)
		{
			x2 = (x ^ xorx) + y2;
			if(!tile[x2] && !(flgs & NOTRANS))
			continue;

			pImage[(dsty*nWd)+dstx] = pal | tile[x2];
		}
	}
}

void GBAImage::DrawTile(GBATiles& tilebuf,int dstx,int dsty,int pftt)
{
	int flgs = 0;
	/*
	vhopppcc cccccccc

c is the index of a tile in the tileset (tileset is specified per BG), SNES will get the address of a tile as (base_tileset_bits << 13) + (8 * color_depth * tile_number)
h: horizontal flip
v: vertical flip
p: palette number
o: priority bit
*/
	if(pftt & 0x4000)		flgs |= FLIPX;
	if(pftt & 0x8000)		flgs |= FLIPY;

	DrawTile(tilebuf[pftt & 0x3FF],dstx,dsty,(pftt & 0x1C00) >> 0x0A,flgs);
}

///////////////////////////////////////////////////////

wxImage GBAImage::GetImage()
{
	return GBAImage::GetImage(0,0,nWd,nHt);
}

wxImage GBAImage::GetImage(wxCoord x,wxCoord y,wxCoord wd,wxCoord ht)
{
	// nonsense bound checking
	if(x > nWd)     x = nWd;
	if(y > nHt)     y = nHt;
	if(x < 0)       x = 0;
	if(y < 0)       y = 0;
	if(wd < 0)      wd = 0;
	if(ht < 0)      ht = 0;
	if(x+wd > nWd)  wd = nWd-x;
	if(y+ht > nHt)  ht = nHt-y;

	if((wd <= 0) || (ht <= 0))
	return wxImage();           // blank image

	wxImage img(wd,ht,false);
	if(!img.HasAlpha())
	img.InitAlpha();

	// copy data over to the wxImage
	unsigned char* dst = img.GetData();
	unsigned char* dsta = img.GetAlpha();
	unsigned char* src = &pImage[y*nWd + x];

	for(y = 0; y < ht; ++y)
	{
		for(x = 0; x < wd; ++x)
		{
			COLOR& c = mPalette[src[x]];
			dsta[x]     = c.a;
			dst[x*3+0]  = c.r;
			dst[x*3+1]  = c.g;
			dst[x*3+2]  = c.b;
		}
		dsta += wd;
		dst += wd*3;
		src += nWd;
	}

	return img;
}

///////////////////////////////////////////////////////

void GBAImage::BlitToDC(wxDC* dst,wxCoord xdst,wxCoord ydst,wxCoord wd,wxCoord ht,wxCoord xsrc,wxCoord ysrc,int logicalFunc)
{
	if(!pDC)
	return;

	if(!bDCValid)
	ValidateDC();

	dst->Blit(xdst,ydst,wd,ht,pDC,0,0,wxCLEAR);
}

///////////////////////////////////////////////////////

void GBAImage::ValidateDC()
{
	pDC->SelectObject(wxBitmap(GetImage()));
	bDCValid = true;
}