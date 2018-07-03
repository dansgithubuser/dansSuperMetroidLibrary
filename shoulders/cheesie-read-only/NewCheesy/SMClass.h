#include <vector>
#include "RomClass.h"
#include "Compresch/CompreschMain.h"

using namespace std;
#define LC_NOBANK  0x00
#define LC_LOROM   0x01  //LoROM
#define LC_HIROM   0x02  //HiROM
#define LC_EXHIROM 0x04  //Extended HiROM
#define LC_EXLOROM 0x08  //Extended LoROM
#define LC_LOROM_2 0x10  //LoROM, always converts to 80:8000 map
#define LC_HIROM_2 0x20  //HiROM, always converts to 40:0000 map up till 70:0000
#define LC_EXROM   0x04  //same as LC_EXHIROM (depreciated)

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#ifndef MDBSTUFF
#define MDBSTUFF
struct MDB_Header{
	unsigned char  unknown1;
	unsigned char  Area;
	unsigned char  Xpos;
	unsigned char  Ypos;
	unsigned char  Width;
	unsigned char  Height;
	unsigned char  Unknown1;
	unsigned char  Unknown2;
	unsigned char  Unknown3;
	unsigned short Pdoorout;
};
#define u8 unsigned char
struct MDB_Roomstate{
	u32 Roommap_p;
	u8 GraphicsSet;
	u8 MusicTrack;
	u8 MusicControl;
	u16 Fx1_p;
	u8 Roompop_p; //Room population
	u16 Enemyset_p;
	u16 Layer2_Scr;
	u16 Mdb_Scroll_p;
	u16 Unknown;
	u16 Fx2_p;
	u16 Plm_p;
	u16 Bgdata_p; //Background data
	u16 LayersHandling;
};
struct tTSA {
	int max;
	unsigned short ID;
	std::vector<unsigned short> nTSA;
};

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long

class SMClass{
public:
	std::vector<u16> Map; 
	int GrabRoom();
	//Vars

	int iRoomState; //Current room state
	std::vector<MDB_Roomstate> RoomStates;
	std::vector<u32> RoomStatePointers;
	long SMDecomp(u32 offset,vector<u8>* buffer);
	//long SMDecomp(u32 offset);
	RomClass System;
	CompreschMain Compression;

	//Functions
	SMClass(char* FilePath);
	int BlitToBB();
	int Setup();
	int LoadHeader(u32 Address);
	int DrawRoom();
	int  LoadMDB_StateSelect(u32 Address);
	MDB_Header RoomHeader;
	int LoadMDB_Roomstate(u32 Address, MDB_Roomstate* OutputMDB_Roomstate);
	~SMClass();
	int SNES2GBA();
	int gfxSet;
	int GrabTileset(int GraphicsSet);
	u32 ReadPointer(u32 offset);
	std::vector<u8> Tiles;
	std::vector<COLOR> Pal;
	int SNES2GBA(u8* pnt);
	int DrawBlock(short Map16n , int destX , int destY);
	void DrawTileset();
	tTSA TSA;
	u32 Pnt2Off(u32 pointer);
	int FindHeaders();


};


#endif 

