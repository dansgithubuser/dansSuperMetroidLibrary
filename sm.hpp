#ifndef SM_HPP_INCLUDED
#define SM_HPP_INCLUDED

#include <string>
#include <vector>

namespace sm{

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned U32;
typedef std::vector<U8> Buffer;

const unsigned TILE_SIZE=16;
const unsigned CHUNK_SIZE=16;

std::string codeDescription(U16 code);

template <class T> class Array2D{
	public:
		void resize(unsigned maxI, unsigned maxJ){
			data.resize(maxI*maxJ);
			w=maxI;
			h=maxJ;
		}
		T& at(unsigned i, unsigned j){ return data[i*h+j]; }
		const T& at(unsigned i, unsigned j) const{ return data[i*h+j]; }
		void clear(){ data.clear(); }
		unsigned readISize() const{ return w; }
		unsigned readJSize() const{ return h; }
	private:
		std::vector<T> data;
		unsigned w, h;
};

struct Color{
	Color(): r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
	Color(float r, float g, float b, float a): r(r), g(g), b(b), a(a) {}
	float r, g, b, a;
};

struct Vertex{
	Vertex(){}
	Vertex(unsigned x, unsigned y, unsigned tx, unsigned ty): x(x), y(y), tx(tx), ty(ty) {}
	unsigned x, y;//position
	unsigned tx, ty;//tile coordinates
};

class Rom{
	public:
		std::string open(std::string fileName);
		const Buffer& readBuffer() const{ return buffer; }
		U16 readHeaderLength() const{ return headerLength; }
	private:
		Buffer buffer;
		U16 headerLength;
};

struct RoomHeader{
	static const unsigned SIZE=11;//size of room header on rom
	RoomHeader(){}
	RoomHeader(const Buffer& buffer, U32 offset);
	U8
		index,//index value for room
    region,//crateria, brinstar, norfair, wrecked ship, maridia, tourian, ceres, debug
    x,//x on map
    y,//y on map
    width,//in screens and/or map tiles
    height,//in screens and/or map tiles
    upScroller,
    downScroller,
    graphicsFlags;
			//0=normal,
			//1=boss and miniboss rooms that overwrite some common room elements?,
			//2=force refresh of common room elements. Use in rooms that change/clear the common room elements (01/05)
			//5=special rooms that wipe out common room elements. Used in Ceres shaft, Ceres Ridley, Kraid's Room
   U16 doors;//[$8F] pointer
};

struct RoomState{
	RoomState(){}
	RoomState(const Buffer& buffer, U32 offset);
	U32 data;
	U8
		tileSet,
		musicTrack,
		musicControl;
	U16
		fx1,
		enemies,
		enemySet,
		layer2,
		scroll,
		unknown,//used in escape version of Bomb Torizo's room -- xray casing code?
		fx2,
		plm,//plm stands for "post load modifications"
		background,
		layerHandling;
};

struct TileAssembler{
	TileAssembler(){}
	TileAssembler(U16 ul, U16 ur, U16 dl, U16 dr): ul(ul), ur(ur), dl(dl), dr(dr) {}
	U16 ul, ur, dl, dr;
};

struct TileLayer{
	TileLayer(): index(0), flipH(false), flipV(false), property(0) {}
	TileLayer(const Buffer& buffer, U32 offset);
	U16 index;
	bool flipH, flipV;
	U8 property;
};

struct Tile{
	TileLayer layer1, layer2;
	bool hasLayer2;
	U8 bts;//behind-the-scenes data
};

struct Door{
	Door(){}
	Door(const Buffer& buffer, U32 offset);
	U32 room;//pointer to room this door leads to
	U8
		doorBitFlag,//7th=elevator, 6th=switches regions
		direction,//direction and whether or not door closes behind... others?
		illusionX,//x of door "illusion" on exit (16x16)
		illusionY,//y of door "illusion" on exit (16x16)
		x,//x of door on exit (16x16)
		y;//y of door on exit (16x16)
	U16
		distance,//distance Samus is placed from door (not 16x16)
		scrollData;//pointer to code for updating scroll data (ie if you start in a room with no scroll) [$8F]
};

class Room{
	public:
		bool open(const Rom& rom, U32 offset, int stateIndex=-1);//nonexistant state indices default to standard state
		void drawTileSet(Array2D<Color>&, unsigned tilesWide) const;
		void getQuadsVertexArray(std::vector<Vertex>&, unsigned tilesWide) const;//tilesWide should be same as used in drawTileSet
		const Door* readDoor(unsigned x, unsigned y) const;
		unsigned readW() const{ return header.width *CHUNK_SIZE*TILE_SIZE; }
		unsigned readH() const{ return header.height*CHUNK_SIZE*TILE_SIZE; }
	private:
		RoomHeader header;
		std::vector<U32> stateCodes;
		U8 stateCodeValue;
		RoomState state;
		Array2D<U8> scroll;
		Array2D<Tile> tiles;
		std::vector<Array2D<Color> > tileSet;
		std::vector<Door> doors;
};

extern const unsigned ROOMS;
extern const U32 ROOM_OFFSETS[];

};//namespace sm

#endif
