
#ifndef __TILEBUFFER_H__
#define __TILEBUFFER_H__

typedef const BYTE*		tile_t;


class TileBuffer
{
public:
					TileBuffer();
					~TileBuffer();
BYTE*			pBuf;
	void			Destroy();
	void			Load(const BYTE* dat,int tiles);
    void			Save(BYTE* dest,int first,int count);
	tile_t			operator [] (int index);

protected:
	void			Zero();
	
	int				nBufSize;		// size in tiles
	int				nFullSize;		// in tiles (size of allocated buf)
};

#endif