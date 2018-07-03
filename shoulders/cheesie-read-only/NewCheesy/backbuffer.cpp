
#include "backbuffer.h"
/*
BackBuffer::BackBuffer()
{
Zero();
}

BackBuffer::~BackBuffer()
{
Destroy();
}

void            BackBuffer::Create(int width, int height)
{
Destroy();

memset(&bi, 0, sizeof(BITMAPINFO));
bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
bi.bmiHeader.biBitCount = 32;
bi.bmiHeader.biHeight = height;
bi.bmiHeader.biPlanes = 1;
bi.bmiHeader.biWidth = width;

void           *dummy;

mDC = CreateCompatibleDC(0);
if (!mDC)
return;

hBmp = CreateDIBSection(mDC, &bi, DIB_RGB_COLORS, &dummy, 0, 0);

if (!hBmp)
{
DeleteDC(mDC);
Zero();
return;
}
hBmpOld = (HBITMAP) SelectObject(mDC, hBmp);

nWidth = width;
nHeight = height;
}
void            BackBuffer::Destroy()
{
if (mDC)
{
SelectObject(mDC, hBmpOld);
DeleteDC(mDC);
}
if (hBmp)
DeleteObject(hBmp);

Zero();
}

void            BackBuffer::Clear(COLORREF clr)
{
if (!mDC)
return;

HBRUSH          br = CreateSolidBrush(clr);
RECT            rc;

SetRect(&rc, 0, 0, nWidth, nHeight);
FillRect(mDC, &rc, br);

DeleteObject(br);
}

void            BackBuffer::Zero()
{
mDC = 0;
hBmp = 0;
hBmpOld = 0;

nWidth = 0;
nHeight = 0;
}

void            BackBuffer::SaveToFile(FILE * file)
{
if (!mDC) // can't save if there's no image

return;

/*
* You could probably use BITMAPFILEHEADER and BITMAPINFOHEADER structs here,
*    but i hate dumping structs to a file because there's no guarantee things will align
*    properly
*/

int             tmp;
short           sht;
DIBSECTION      dib;

GetObject(
GetCurrentObject(mDC, OBJ_BITMAP)
,sizeof(DIBSECTION), &dib);

nPitch = ((nWidth + 3) / 4) * 4;
// BITMAPFILEHEADER crap
fputc('B', file);
fputc('M', file);

tmp = (nPitch * nHeight) + (256 * 4) + 0x28 + 0x0E; // file size

fwrite(&tmp, 1, 4, file);

tmp = 0; // reserved BS

fwrite(&tmp, 1, 4, file);

tmp = (256 * 4) + 0x28 + 0x0E; // file size minus bitmap bits

fwrite(&tmp, 1, 4, file);

// BITMAPINFOHEADER crap
tmp = 0x28;
fwrite(&tmp, 1, 4, file); // size of BITMAPINFOHEADER

fwrite(&nWidth, 1, 4, file);
tmp = -nHeight; // negative height because we want a top-down bitmap

fwrite(&tmp, 1, 4, file);
// fwrite(&bi.bmiHeader+12,1,28,file);

sht = 1;
fwrite(&sht, 1, 2, file); // planes

sht = 32;
fwrite(&sht, 1, 2, file); // bpp

tmp = 0;
fwrite(&tmp, 1, 4, file); // stupid meaningless stuff

fwrite(&tmp, 1, 4, file);
fwrite(&tmp, 1, 4, file);
fwrite(&tmp, 1, 4, file);
fwrite(&tmp, 1, 4, file);
fwrite(&tmp, 1, 4, file);

// palette

RGBQUAD         pal[256];

/*
* if(!GetDIBColorTable(this->DC(),0,256,pal)) {
* int blah=GetLastError();
* MessageBox(0,"WTF",NULL,MB_OK);
* }
*/

for (int i = 0; i < 256; i++)
{
	if (i % 2)
	{
		pal[i].rgbBlue = 255;
		pal[i].rgbGreen = 255;
		pal[i].rgbRed = 255;

	} else
	{
		pal[i].rgbBlue = 0;
		pal[i].rgbGreen = 0;
		pal[i].rgbRed = 0;
	}
}
fwrite(pal, 4, 256, file);
// bits
char           *mybits = new char[nHeight * nPitch * 32];

GetBitmapBits(
this->hBmp, // handle to bitmap
this->nHeight * this->nPitch * 32, // number of bytes to
// copy
mybits // buffer to receive bits
);

fwrite(mybits, 1, nPitch * nHeight * 32, file);
delete[]mybits;
}
