
#ifndef __GBATILES_H__
#define __GBATILES_H__

/*
*	Class which actually loads the tiles from the ROM or whatever
*    Stores them in 4bpp with no palette information
*/

////////////////////////////////////////////
#include "gbatypes.h"

////////////////////////////////////////////

class GBATiles
{
public:
	GBATiles();
	~GBATiles()     { Destroy(); }

	void            Load(unsigned char* src,int tiles);    // number of 8x8 tiles to load.  Each
	//   tile = 32 bytes from 'buffer'
	void            Destroy();

	unsigned char*             operator [] (int index);    // get data for a tile

protected:
	unsigned char*             pBuf;
	int             nSize;      // size in tiles (each tile = 64 bytes in pBuf)

private:
	GBATiles(const GBATiles&);
	GBATiles& operator = (GBATiles&);
};

#endif // __GBATILES_H__