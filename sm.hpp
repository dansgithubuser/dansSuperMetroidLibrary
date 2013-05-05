#ifndef SM_HPP_INCLUDED
#define SM_HPP_INCLUDED

#include <sstream>
#include <string>
#include <vector>
#include <map>

namespace sm{

//=====utility=====//
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned U32;
typedef std::vector<U8> Buffer;

template<class T> class Array2D{
	public:
		Array2D(): w(0), h(0) {}

		void resize(unsigned maxI, unsigned maxJ){
			data.resize(maxI*maxJ);
			w=maxI;
			h=maxJ;
		}

		T& at(unsigned i, unsigned j){ return data[i*h+j]; }
		const T& at(unsigned i, unsigned j) const{ return data[i*h+j]; }
		unsigned readISize() const{ return w; }
		unsigned readJSize() const{ return h; }
		unsigned size() const{ return data.size(); }

		void clear(){
			data.clear();
			w=0;
			h=0;
		}

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
	Vertex(): x(0), y(0), tx(0), ty(0) {}
	Vertex(unsigned x, unsigned y, unsigned tx, unsigned ty): x(x), y(y), tx(tx), ty(ty) {}
	unsigned x, y;//position
	unsigned tx, ty;//tile coordinates
};

template<class T> class SparseRangeArray{
	public:
		void set(unsigned start, unsigned size, T value){
			data.resize(max(data.size(), (start+size)/BLOCK_SIZE+1), std::vector<T>(BLOCK_SIZE, T(0)));
			for(unsigned i=start; i<start+size; ++i){
				unsigned a=i/BLOCK_SIZE;
				unsigned b=i%BLOCK_SIZE;
				data[a][b]=value;
			}
		}

		T operator[](unsigned i) const{
			unsigned a=i/BLOCK_SIZE;
			unsigned b=i%BLOCK_SIZE;
			if(a>=data.size()) return T(0);
			if(b>=data[a].size()) return T(0);
			return data[a][b];
		}

		unsigned size() const{ return BLOCK_SIZE*(data.size()); }

		std::string print() const{
			std::stringstream s;
			T value=T(0);
			for(unsigned a=0; a<data.size(); ++a)
				for(unsigned b=0; b<data[a].size(); ++b)
					if(data[a][b]!=value){
						value=data[a][b];
						s<<std::hex<<a*BLOCK_SIZE+b<<" "<<value<<"\n";
					}
			return s.str();
		}

	private:
		static const unsigned BLOCK_SIZE=0x8000u;
		std::vector<std::vector<T> > data;
};

template<class K, class V> void rekey(std::map<K, V>& map, const std::map<K, K>& keys){
	std::vector<K> newKeys;
	std::vector<V> values;
	for(typename std::map<K, V>::iterator i=map.begin(); i!=map.end(); ++i){
		newKeys.push_back(i->first);
		values.push_back(i->second);
	}
	map.clear();
	for(unsigned i=0; i<newKeys.size(); ++i) map[newKeys[i]]=values[i];
}

//=====Super Metroid stuff=====//
const unsigned TILE_SIZE=16;//tile size in pixels
const unsigned SCREEN_SIZE=16;//screen size in tiles
const unsigned VANILLA_ROOMS=263;
extern const U32 VANILLA_ROOM_OFFSETS[];

enum Region{
	CRATERIA,
	BRINSTAR,
	NORFAIR,
	WRECKED_SHIP,
	MARIDIA,
	TOURIAN,
	CERES,
	DEBUG,
	REGIONS
};

class Rom{
	public:
		enum Usage{ UNKNOWN, HACKABLE, HACKED };
		typedef SparseRangeArray<Usage> Index;
		std::string open(std::string fileName);
		bool indexVanilla();
		bool takeSpace(U8 bank, U16 size, U32& offset);
		bool takeSpace(U8 minBank, U8 maxBank, U16 size, U32& offset);
		bool save(std::string fileName);
		void dummify();//write dummy data over unused data
		Buffer header, buffer;
	private:
		Index index;
};

class Transition{
	public:
		static const unsigned SIZE=12;
		static const U8 BANK=0x83u;
		Transition(Rom& rom): rom(&rom) {}
		void index(U32 offset, Rom::Index& index);
		void open(U32 offset);
		bool save(U32& offset);
		U32 room;//pointer to destination room
		U8
			flags,
				//0x80 elevator
				//0x40 switch region
				//0x20 used with elevators, doesn't seem to do much
				//0x10 used with elevators, doesn't seem to do much
				//0x08 used with elevators, doesn't seem to do much
				//0x04 Maridia and Tourian elevators, doesn't seem to do much
				//0x02 Maridia and Tourian elevators, doesn't seem to do much
				//0x01 always 0
			direction,
				//direction&3=0 => rightward
				//direction&3=1 => leftward
				//direction&3=2 => downward
				//direction&3=3 => upward
				//direction&4 => draw door on other side
				//direction=145 or 147 => hide health and such (used with elevators)
			animationX, animationY,//position of other side of door closing animation in tiles from top left corner of new room
			x, y;//Samus' location in new room in tiles from top left corner
		U16
			distance,//distance Samus is placed on exit in pixels, 0x8000 means default
			scroll;//pointer in bank 0x8F to code for updating scroll data
	private:
		Rom* rom;
};

class Save{
	public:
		static const U32 REGION_TABLES=0x44B5u;
		static const unsigned SIZE=14;//size in bytes on rom
		static const U8 BANK=0x80u;
		static const U16 START=0x44C5u;//offset of first save
		static const U16 END=0x4D07u;//offset just past last save -- 0x4C19u would be a more conservative value, excluding (probably) debug region saves
		static void index(Rom::Index& index);
		Save(Rom& rom): rom(&rom) {}
		U32 readRegionTable(Region region);
		void setRegionTable(Region region, U32 offset);
		void open(U32 offset);
		bool save(U32& offset);
		U32 room;//room that this save point is inside
		U32 transition;//transition to room this save point is inside
		U16 unknown;//always zero?
		U16 scrollX;
		U16 scrollY;
		U16 samusY;//offset from top
		U16 samusX;//offset from center
	private:
		Rom* rom;
};

class Mode7{
	public:
		static const U8 FIRST_BANK=0xC0u;
		static const U8 LAST_BANK=0xCEu;
		static const U8 FIRST_TILE_SET=17;
		static const U8 LAST_TILE_SET=20;
		Mode7(Rom& rom): rom(&rom) {}
		void index(U8 tileSet, Rom::Index& index);
		void open(U8 tileSet);
		bool save(U8 tileSet);
		void clear();
		Array2D<U8> tiles;
	private:
		U32 dataOffset(U8 tileSet);
		Rom* rom;
		Buffer data;
};

class Header{
	public:
		static const U8 BANK=0x8Fu;
		static const U8 DOOR_BANK=0x8Fu;
		enum Code{
			STANDARD          =0xE5E6u,
			DOORS             =0xE5EBu,
			TOURIAN_BOSS      =0xE5FFu,
			EVENTS            =0xE612u,
			BOSSES            =0xE629u,
			MORPH             =0xE640u,
			MORPH_AND_MISSILES=0xE652u,
			POWER_BOMBS       =0xE669u,
			SPEED_BOOSTER     =0xE678u
		};
		static std::string codeDescription(Code code);
		Header(){}
		Header(const Buffer& buffer, U32 offset);
		unsigned size();//size in bytes on rom
		void write(Buffer& buffer, U32 offset) const;
		U8
			index;//index value for room, doesn't seem to do anything
		Region region;//crateria=0, brinstar=1, norfair=2, wrecked ship=3, maridia=4, tourian=5, ceres=6, debug=7
		U8
			x, y,//map location
			width, height,//in screens
			upScroll,//how far the room scrolls up -- usually 0x70
			downScroll,//how far the room scrolls down -- usually 0xA0
			graphicsFlags;
				//0=normal
				//1=doesn't seem to do anything. Used in Crocomire and Draygon rooms
				//2=force refresh of common room elements. Used in neighbors of 1s and 5s except Draygon room.
				//5=rooms that wipe out common room elements. Used in Ceres shaft, Ceres Ridley, Kraid's Room
		U32 doors;
		struct StateInfo{
			Code code;
			Buffer fields;
			U32 state;
		};
		std::vector<StateInfo> stateInfo;
	private:
		static const unsigned MAIN_SIZE=11;
		static unsigned fieldSize(Code code);//size in bytes on rom
		static unsigned stateInfoSize(Code code);//size in bytes on rom
};

struct State{
	static const unsigned SIZE=26;//size in bytes on rom
	static const U8 BANK=0x8Fu;
	static const U8 SCROLL_BANK=0x8Fu;
	State(){}
	State(const Buffer& buffer, U32 offset);
	void write(Buffer& buffer, U32 offset) const;
	U32 tiles;//pointer to somewhere in banks 0xC2 to 0xCE
	U8
		tileSet,
		musicTrack,
		musicControl;
	U16 fx1;//pointer in bank 0x83
	U32 enemies;//pointer
	U16 enemySet;//pointer in bank 0xB4
	U16 layer2;//how layer 2 is drawn
	U32 scroll;
		//if >= 0x8000, pointer to 2d scroll array in bank 0x8F -- 0 means no scroll, 1 means scroll but don't show top or bottom 2 tile rows, 2 means full scroll
		//else it should be 0, 1, or 2, meaning the entire room has that scroll value
	U16 unknown;//used in escape version of Bomb Torizo's room -- xray casing code (bank 0x8F)? -- doesn't seem to do anything
	U16 fx2;//pointer in bank 0x8F
	U32 plm;//pointer
	U16 background;//pointer in bank 0x8F
	U16 layerHandling;//pointer in bank 0x8F
};

struct TileAssembler{
	TileAssembler(U16 ul, U16 ur, U16 dl, U16 dr): ul(ul), ur(ur), dl(dl), dr(dr) {}
	U16 ul, ur, dl, dr;
};

struct TileLayer{
	TileLayer(): index(0), flipH(0), flipV(0), property(0) {}
	TileLayer(const Buffer& buffer, U32 offset);
	void write(Buffer& buffer) const;
	U16 index;
	bool flipH, flipV;
	U8 property;
};

struct Tile{
	static const U8 FIRST_BANK=0xC2u;
	static const U8 LAST_BANK=0xCEu;
	TileLayer layer1, layer2;
	bool hasLayer2;
	U8 bts;//behind-the-scenes data
};

struct Enemy{
	static const unsigned SIZE=16;//size in bytes on rom
	static const U8 BANK=0xA1u;
	static const U16 SENTINEL=0xFFFFu;
	Enemy(): species(0), x(0), y(0), field1(0), field2(0), field3(0), field4(0), field5(0) {}
	Enemy(const Buffer& buffer, U32 offset);
	void write(Buffer& buffer, U32 offset) const;
	U16
		species,//pointer to enemy data in bank 0xA0
		x, y,//in pixels, from top left corner
		//these 5 fields' meanings depend on the species
		field1,//geemer orientation, ripper speed
		field2,
		field3,
		field4,
		field5;
};

struct Plm{//post load modification
	static const unsigned SIZE=6;//size in bytes on rom
	static const U8 BANK=0x8Fu;
	static const U16 SENTINEL=0;
	Plm(): x(0), y(0), field1(0), field2(0) {}
	Plm(const Buffer& buffer, U32 offset);
	void write(Buffer& buffer, U32 offset) const;
	U16 type;//pointer in bank 0x84
	U8
		x, y,
		//tese 2 fields' meanings depend on the type
		field1,
		field2;
};

class Room{
	public:
		static const U8 BANK=0x8Fu;
		Room(Rom& rom): rom(&rom), mode7(rom) {}
		bool index(U32 offset, Rom::Index& index);
		bool open(U32 offset);
		bool save(U32& offset);
		void setState(unsigned i){ stateIndex=i; }
		void loadGraphics();
		void drawTileSet(Array2D<Color>&, unsigned tilesWide) const;
		void getQuadsVertexArray(
			std::vector<Vertex>&, unsigned tilesWide,//tilesWide should be same as used in drawTileSet
			bool showLayer1=true, bool showLayer2=true, bool showMode7=true
		) const;
		bool readDoor(unsigned x, unsigned y, Transition& transition);
		unsigned readW() const{ return header.width *SCREEN_SIZE*TILE_SIZE; }
		unsigned readH() const{ return header.height*SCREEN_SIZE*TILE_SIZE; }
		unsigned readStates() const{ return states.size(); }
		Header::Code readStateCode(unsigned i) const;
	private:
		bool convertScreenToTile(unsigned& x, unsigned& y) const;
		const Array2D<Tile>& readStateTiles() const{ return tiles.find(states[stateIndex].tiles)->second; }
		Rom* rom;
		//per room
		Header header;
		std::vector<U32> doors;
		//per state
		std::vector<State> states;
		//shareable between states
		std::map<U32, Array2D<U8> > scroll;//map from identifier to scroll data
		std::map<U32, Array2D<Tile> > tiles;//map from identifier to tiles
		std::map<U32, std::vector<Enemy> > enemies;//map from identifier to enemies
		std::map<U32, std::vector<Plm> > plm;//map from identifier to plm
		//for interacting with a state
		unsigned stateIndex;
		Mode7 mode7;
		std::vector<Array2D<Color> > tileSet;
		std::vector<Array2D<Color> > mode7TileSet;
};

std::string musicControlDescription(U8 musicControl);
std::string musicTrackDescription(U8 musicTrack);

};//namespace sm

#endif
