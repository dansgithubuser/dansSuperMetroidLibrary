
#include "main.h"
#include "gbatiles.h"

//////////////////////////////////////////////

GBATiles::GBATiles()
{
	nSize = 0;
	pBuf = 0;
}

void GBATiles::Destroy()
{
	if(pBuf)
	delete[] pBuf;
	pBuf = 0;
	nSize = 0;
}

//////////////////////////////////////////////

void GBATiles::Load(u8* src,int tiles)
{
	Destroy();
	if(tiles < 1)
	return;

	nSize = tiles;
	pBuf = new u8[tiles * 8 * 8];

	u8* dst = pBuf;
	int i;
	for(i = 0; i < (tiles*8*8/2); ++i)
	{
		dst[0] = src[0] & 0x0F;
		dst[1] = src[0] >> 4;
		++src;
		dst += 2;
	}
}

/////////////////////////////////////////////

u8* GBATiles::operator [] (int index)
{
	if((index < 0) || (index >= nSize))     return 0;

	return &pBuf[index*8*8];
}
