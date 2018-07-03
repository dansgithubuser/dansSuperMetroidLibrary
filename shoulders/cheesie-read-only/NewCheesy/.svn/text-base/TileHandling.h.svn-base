//OPERATOR OVERLOADING CLASSS >:D
#include <vector>
#include "TileData.h"

struct MyRect{
int X;
int Y;
int Width;
int Height;


};
class TileDataStuff:public MyRect{
	MyRect Me;
	public:
	TileDataStuff();
	int x();
	int y();
	int width();
	int height();
	int x(int val);
	int y(int val);
	int width(int val);
	int height(int val);
	int MyWidth;
	int MyHeight;
	int Create(std::vector<unsigned short>* pointer, int Width, int Height);
	int Create(int Width, int Height);
	std::vector<unsigned short> Data;
	std::vector<unsigned short>* pData;//Don't ask someone else may want to fix this?
	int usePnt;
	unsigned short* operator [] (int index);
	void operator = (MyRect* rect);
	void FromTileSetCoords();//Converts coords in the tileset to an array to Data for easy copying.
	std::vector<unsigned short>* DataArray();
	std::vector<unsigned short> NullVect;
};