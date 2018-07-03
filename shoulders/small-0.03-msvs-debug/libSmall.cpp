//!begin libSmall.cpp
/*
   __________________________________________________________________
  |      ___           ___           ___           ___       ___     |
  |     /\  \         /\__\         /\  \         /\__\     /\__\    |
  |    /::\  \       /::|  |       /::\  \       /:/  /    /:/  /    |
  |   /:/\ \  \     /:|:|  |      /:/\:\  \     /:/  /    /:/  /     |
  |  _\:\~\ \  \   /:/|:|__|__   /::\~\:\  \   /:/  /    /:/  /      |
  | /\ \:\ \ \__\ /:/ |::::\__\ /:/\:\ \:\__\ /:/__/    /:/__/       |
  | \:\ \:\ \/__/ \/__/~~/:/  / \/__\:\/:/  / \:\  \    \:\  \       |
  |  \:\ \:\__\         /:/  /       \::/  /   \:\  \    \:\  \      |
  |   \:\/:/  /        /:/  /        /:/  /     \:\  \    \:\  \     |
  |    \::/  /        /:/  /        /:/  /       \:\__\    \:\__\    |
  |     \/__/         \/__/         \/__/         \/__/     \/__/    |
  |                                                                  |
  |                                                                  |
  |                   The Super Metroid Allocator                    |
  |__________________________________________________________________|


   Project SMALL
   Author: Daniel Papenburg (DFPercush)
   Development date: June 2010 - August 2010
   This document is best viewed in a fixed width font with tab spacing of 4.

   File: libSmall.cpp
   Description: Implements the functions in libSmall.h

 _____________________________________________________________________________
|                                                                             |
| LICENSE:                                                                    |
|                                                                             |
| This license applies to the files libSmall.cpp and libSmall.h. This         |
| license does not apply to any other software or files with which this       |
| library may be packaged. If you obtained this file as part of a package,    |
| please consult that package's license for legal information governing       |
| other files.                                                                |
|                                                                             |
| You agree not to hold the author responsible for any negative results from  |
| using this library, including, but not limited to, data loss, system        |
| instability, lack of functionality, the desire to stab one's self with      |
| a sharp implement, and smashing your monitor with a baseball bat.           |
|                                                                             |
| This software is released to the public domain, as defined by United States |
| copyright law. I would appreciate some kind of credit if you link this code |
| into your project. I want resume fodder.                                    |
| Feel free to do whatever you want with it.                                  |
|                                                                             |
| If you do not agree to the terms of the license, delete the file.           |
|_____________________________________________________________________________|

Post bug reports, comments and questions on forum.metroidconstruction.com


TODO list:

. if overwriting segment, place new data at beginning of deleted segment

. in all functions which accept an address, convert to PC format if >0x800000

*/

#ifdef WIN32
#include <windows.h>
#endif
#include <list>
//#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include "libSmall.h"
#include <glw/glwRealloc.h>
#include "LunarDLL.h"
using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 1000
#endif


// This is the master rule table. It controls how data is linked together, and tells libSmall what
// pointers need to be changed when something is moved, deleted, or allocated.
const smBackLinkRule linkrules [] =
{
	// typeMoved				typeUpdate,			offset			mask		array	array	sequence
	//																			start	Stride	Reader
	{ MTYPE_DOOR_OUT,			MTYPE_ROOM_HEADER,		9,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_DOOR_DATA,			MTYPE_DOOR_OUT,			0,			0xFFFF,		0,		2,		NULL },
	{ MTYPE_DOOR_DATA,			MTYPE_FX1,				0,			0xFFFF,		0,		0,		NULL },

	// State structure
	{ MTYPE_LEVEL_DATA,			MTYPE_STATE,			0,			0xFFFFFF,	0,		0,		NULL },
	//{Unknown1,				MTYPE_STATE,			4,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_FX1,				MTYPE_STATE,			6,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_ENEMIES,			MTYPE_STATE,			8,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_ALLOWED,			MTYPE_STATE,			10,			0xFFFF,		0,		0,		NULL },
	//{Unknown2,				MTYPE_STATE,			12,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_SCROLL,				MTYPE_STATE,			14,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_ROOMVAR,			MTYPE_STATE,			16,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_FX2,				MTYPE_STATE,			18,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_PLM,				MTYPE_STATE,			20,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_BG_INSTRUCTIONS,	MTYPE_STATE,			22,			0xFFFF,		0,		0,		NULL },
	{ MTYPE_LAYER12,			MTYPE_STATE,			24,			0xFFFF,		0,		0,		NULL },

	// Room headers
	{ MTYPE_ROOM_HEADER,		MTYPE_DOOR_DATA,			0,			0xFFFF,		0,		0,		NULL },
	//{MTYPE_ROOM_HEADER,		MTYPE_SAVE_STATION,			?,			?,			?,		?,		NULL },
	{ MTYPE_STATE,				MTYPE_ROOM_HEADER,		0,			0xFFFF,		0,		0,		&smReadRoomHeader },

	// Bg reader
	{ MTYPE_BG_DATA,			MTYPE_BG_INSTRUCTIONS,	0,			0x00FFFFFF,	0,		0,		&smReadBgBase },

	// Something for the landing site, and Ceres elevator room
	//{MTYPE_ROOM_HEADER,	SPECIAL,			<engine loader asm>, ?,		?,		?,	NULL },

	// This next line is just to keep the commas straight, it has no meaning.
	{0x7FFF,0x7FFF, 0, 0, 0, NULL }
};

int smLinkRuleCount()
{
	return sizeof(linkrules) / sizeof(smBackLinkRule);
}

extern const int smMinSizeForType [] = 
{
	39, // #define MTYPE_ROOM_HEADER 1
	0, // #define MTYPE_FREE_SPACE 2
	2, // #define MTYPE_DOOR_OUT 3
	0, // #define MTYPE_LEVEL_DATA 4
	0, // #define MTYPE_UNKNOWN1 5
	0, // #define MTYPE_FX1 6
	0, // #define MTYPE_ALLOWED 7
	0, // #define MTYPE_ENEMIES 8
	0, // #define MTYPE_UNKNOWN2 9
	0, // #define MTYPE_SCROLL 10
	0, // #define MTYPE_ROOMVAR 11
	0, // #define MTYPE_FX2 12
	2, // #define MTYPE_PLM 13
	0, // #define MTYPE_BG_INSTRUCTIONS 14
	0, // #define MTYPE_LAYER12 15
	0, // #define MTYPE_STATE 16
	0, // #define MTYPE_DOOR_DATA 17
	0 // #define MTYPE_BG_DATA 18

	// #define MTYPE_MAX_VALUE 18
};

unsigned char g_lunarBuffer[0x8000];
bool g_lunar_loaded = false;

const unsigned char initRoomData1x1 [] = {0x00, 0x02, 0xE9, 0xFF, 0xFF, 0x00, 0xE4, 0xFF, 0x00, 0xF2, 0x00, 0x02, 0x00, 0xFF, 0x8D };

SuperMetroidAllocator::SuperMetroidAllocator()
{
	deleteOnUnlink = true;
	romSize = 0;
	romImage = NULL;
	originalRomSize = 0;
	originalImage = NULL;
	memset(banks, 0, sizeof(long) * MTYPE_MAX_VALUE);
	banks[MTYPE_ROOM_HEADER] = 0x8F0000;
	banks[MTYPE_FREE_SPACE] = 0;
	banks[MTYPE_DOOR_OUT] = 0x8F0000;
	banks[MTYPE_LEVEL_DATA] = 0;
	banks[MTYPE_UNKNOWN1] = 0;
	banks[MTYPE_FX1] = 0x830000;
	banks[MTYPE_ALLOWED] = 0xB40000;
	banks[MTYPE_ENEMIES] = 0xA10000;
	banks[MTYPE_UNKNOWN2] = 0;
	banks[MTYPE_SCROLL] = 0x8F0000;
	banks[MTYPE_ROOMVAR] = 0;
	banks[MTYPE_FX2] = 0x8F0000;
	banks[MTYPE_PLM] = 0x8F0000;
	banks[MTYPE_BG_INSTRUCTIONS] = 0x8F0000;
	banks[MTYPE_LAYER12] = 0x8F0000;
	banks[MTYPE_STATE] = 0x8F0000;
	banks[MTYPE_DOOR_DATA] = 0x830000;
	banks[MTYPE_BG_DATA] = 0;
}

SuperMetroidAllocator::~SuperMetroidAllocator()
{
	if (romImage) delete [] romImage; romImage = NULL; 
	if (originalImage) delete [] originalImage; originalImage = NULL;
	romSize = 0;

}

int SuperMetroidAllocator::open(char* romFile, char* mdbFile, 
	void (*progressCallback)(void* context, int level, char* desc, int percentComplete), void* context)
{
	int res;
	smMemorySegment seg;
	char tfn[MAX_PATH];

	//parsing = true;
	if (strlen(romFile) >= MAX_PATH) return 3;
	if (strlen(mdbFile) >= MAX_PATH) return 3;
	strcpy(romFilename, romFile);
	strcpy(mdbFilename, mdbFile);
	FILE* f = fopen(romFilename, "rb");
	if (!f) { /*parsing = false;*/ return 1; }
	fseek(f, 0, SEEK_END);
	romSize = ftell(f);
	originalRomSize = romSize;
	fseek(f, 0, SEEK_SET);
	romImage = new unsigned char[romSize];
	if (!romImage) { fclose(f); /*parsing = false;*/ return 2; }
	originalImage = new unsigned char[romSize];
	if (!originalImage) { fclose(f); /*parsing = false;*/ delete [] romImage; return 2; }
	fread(romImage, 1, romSize, f);
	if (f) fclose(f);
	if (romSize > 0x204AC) banks[MTYPE_PLM] = romImage[0x204AC] << 16;
	memcpy(originalImage, romImage, romSize);
	res = buildMemoryMap(romImage, mdbFilename, romSize, progressCallback, context);
	//parsing = false;
	strcpy(romFilename, romFile);
	strcpy(mdbFilename, mdbFile);

	// Load floaters
	sprintf(tfn, "%s_floaters.csv", romFilename);
	f = fopen(tfn, "r");
	unsigned int ri[6]; // read in - this is necessary so fscanf doesn't write past the boundary of short members
	if (f)
	{
		while (!feof(f))
		{
			//fscanf(f, "%d, %d, %d, %d, %d, %d\n", &seg.ofs, &seg.length, &seg.type, &seg.flags, &seg.padding, &seg.ref);
			fscanf(f, "%d, %d, %d, %d, %d, %d\n", &ri[0], &ri[1], &ri[2], &ri[3], &ri[4], &ri[5]);
			seg.ofs = ri[0];
			seg.length = ri[1];
			seg.type = ri[2];
			seg.flags = ri[3];
			seg.padding = ri[4];
			seg.ref = ri[5];
			chunks.Add(seg);
		}
		fclose(f);
	}

	BTree<smMemorySegment>::iterator itbl;
	smLinkInfo bl;

	int blc; // back link count
	int itcount = 0;
	int itctot = chunks.Count();
	for (itbl = chunks.begin(); itbl != chunks.end(); itbl++)
	{
		if (itbl->type == MTYPE_ROOM_HEADER)
		{
			blc = blc; //breakpoint
		}
		blc = listBackLinks(romAddr(itbl->ofs), &bl, 1);
		if (blc > itbl->ref)
		{
			itbl->ref = blc;
		}
		if (progressCallback)
		{
#ifdef WIN32
			if (!IsBadCodePtr((FARPROC)progressCallback))
			{
#endif
			  progressCallback(context, 0, "Loading ROM", glwNormalizei(itcount, 0, itctot, 5, 100)); //int(100.0f * (0.2f + ((float)itcount / (float)chunks.Count()))));
			  progressCallback(context, 1, "Confirming reference counters", glwNormalizei(itcount, 0, itctot, 0, 100)); //int(100.0f * (float)itcount / (float)chunks.Count()));
#ifdef WIN32
			}
#endif
		}
		itcount++;
	}

	FILE *fdmp = fopen("dump.txt", "w");
	DumpTree(&chunks, fdmp);
	fclose(fdmp);
	
	mashFreeSpace();

	return res;
}

unsigned long pcAddr(DWORD rom_addr)
{
	int ofs;
	DWORD romAddr = rom_addr & 0x7FFFFF;
	ofs = (romAddr & 0xFE0000) >> 1;
	ofs |= (romAddr & 0x7FFF);
	if (romAddr & 0x010000) ofs += 0x8000;
	return ofs;
}

unsigned long romAddr(DWORD pc_addr)
{
	DWORD pcAddr = pc_addr & 0x7FFFFF;
	DWORD ofs;
	ofs = (pcAddr & 0x7F8000) << 1;
	ofs |= pcAddr & 0x7FFF;
	ofs |= 0x808000;
	return ofs;
}

BTNode<smMemorySegment>* SuperMetroidAllocator::findSegmentByAddrPC(unsigned long addr)
{
	smMemorySegment seg;
	seg.ofs = addr;
	BTNode<smMemorySegment>* node;
	//node = chunks.Find(seg);

	for (node = chunks.Root(); node != NULL; )
	{
		if (addr >= node->data.ofs  &&  addr < node->data.ofs + node->data.length) return node;
		if (addr < node->data.ofs) node = node->Left();
		else node = node->Right();
	}
	return NULL;
}

BTNode<smMemorySegment>* SuperMetroidAllocator::findChunk(unsigned long uid)
{
	BTNode<smMemorySegment>* ptr = chunks.Root();
	while (ptr)
	{
		if (uid == ptr->data.ofs) return ptr;
		else if (uid < ptr->data.ofs) ptr = ptr->Left();
		else ptr = ptr->Right();
	}
	return NULL;
}


// Here's where the useful stuff begins
int SuperMetroidAllocator::readRoom(unsigned char *fileContents, unsigned long rid, smMyRoomStruct *rm)
{
	// 12 E6 and 29 E6 are using extra byte as argument.
	int i;
	int sc;
	int s;
	//int ptrofs;
	list<unsigned short> ids;
	//unsigned short curId;

	rm->clear();

	/*************************************
	rm->banks.levelData = 0x000000;
	rm->banks.fx1 = 0x830000;
	rm->banks.enemyList = 0xA10000;
	rm->banks.allowed = 0xB40000;
	rm->banks.scroll = 0x8F0000;
	rm->banks.roomVar = 0x00000;
	rm->banks.fx2 = 0x8F0000;
	rm->banks.plm = 0x8F0000;
	rm->banks.bg = 0x8F0000;
	rm->banks.layer12 = 0x8F0000;
	rm->doorOutBank = 0x8F0000;
	*************************************/

	
	memcpy(&rm->header.unknown1, &fileContents[rid], 4);
	rm->header.width = fileContents[rid + 4];
	rm->header.height = fileContents[rid + 5];
	memcpy(&(rm->header.unknown2), &(fileContents[rid + 6]), 3);
	rm->header.doorOut = *(unsigned short*)(&fileContents[rid + 9]);
	rm->doorOutSize = sizeofDoorOut(fileContents, pcAddr(banks[MTYPE_DOOR_OUT] | rm->header.doorOut));
	sc = 0;
	ids.clear();
	/*for (i = rid + 11; // sizeof(smRoomHeader);  See note below about sizeof()
		*(unsigned short*)(&fileContents[i]) != 0xE5E6; 
		i++) 
	{
		if (fileContents[i] == 0xE6)
		{
			sc++;
			curId = (fileContents[i] << 8) | (fileContents[i - 1]);
			ids.push_back(curId);
		}
	}
	*/
	//list<unsigned short> lsp;
	//unsigned short is;
	//unsigned char ia;
	//unsigned short sp; // state pointer
	smRoomStateHeader sh;
	list<smRoomStateHeader> lsh;
	i = rid + 11; //sizeof(smRoomHeader); ! Can't use sizeof() macro in C++ because it pads structures to the nearest word.
	lsh.clear();
	sc = 0;
	while (*(unsigned short*)(&(fileContents[i])) != 0xE5E6)
	{
		sh.instruction = *(unsigned short*)(&(fileContents[i]));

		// All alternate state instructions contain E6 as the MSB
		if (sh.instruction >> 8 != 0xE6 && sh.instruction >> 8 != 0xE5)
		{
			fprintf(stderr, "Parsing ROM failed at line %d of %s. Terminating program.", __LINE__, __FILE__);
			return 0; // Assumption is wrong
		}
		//is = *(unsigned short*)(&(fileContents[i]));
		if (sh.instruction == 0xE612 || sh.instruction == 0xE629)
		{
			sh.arg = fileContents[i + 2];
			i += 3;
		}
		else
		{
			sh.arg = 0;
			i += 2;
		}
		sh.ptr = *(unsigned short*)(&(fileContents[i]));
		i += 2;
		lsh.push_back(sh);
		sc++;
	}
	// Add standard state
	sh.arg = 0;
	sh.instruction = 0xE5E6;
	int std = i + 2;  // standard state
	sh.ptr = std & 0x7FFF;
	//rm->stateHeaderBank = 0x8F0000;
	lsh.push_back(sh);
	sc++;
	rm->stateCount = sc;
	rm->states = new smRoomStateHeader[sc];
	i = 0;
	for (list<smRoomStateHeader>::iterator itlsh = lsh.begin(); itlsh != lsh.end(); itlsh++)
	{
		rm->states[i++] = *itlsh;
	}
	lsh.clear();

	/*

	sc++;
	ptrofs = i + 2;
	rm->stateCount = sc;
	rm->states = new unsigned short[sc];
	i = 0;
	for (list<unsigned short>::iterator it = ids.begin(); it != ids.end(); it++)
	{
		rm->states[i++] = *it;
	}
	rm->states[i] = 0xE5E6;
	ids.clear();
	rm->pointers = new smStatePointers[sc];
	memcpy(rm->pointers, &(fileContents[ptrofs]), 0x1A * sc);
	*/

	//for (i = 0; i < sc; i++)
	//{
	//}

	// Load state pointer tables
	rm->pointers = new smStatePointers[sc];
	for (s = 0; s < sc; s++)
	{
		i = pcAddr(banks[MTYPE_STATE] | rm->states[s].ptr);
		rm->pointers[s].levelData = (*(unsigned long*)(&(fileContents[i])) & 0x00FFFFFF); i += 4;
		rm->pointers[s].unknown1 = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].fx1 = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].enemyList = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].allowed = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].unknown2 = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].scroll = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].roomVar = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].fx2 = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].plm = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].bg = *(unsigned short*)(&(fileContents[i])); i += 2;
		rm->pointers[s].layer12 = *(unsigned short*)(&(fileContents[i])); i += 2;
	}

	rm->sizes = new smStatePointers[sc];
	for (s = 0; s < sc; s++)
	{
		rm->sizes[s].levelData = sizeofCompressedData(fileContents, pcAddr(rm->pointers[s].levelData));
		rm->sizes[s].fx1 = 16;
		rm->sizes[s].enemyList = sizeofEnemyList(fileContents, pcAddr(banks[MTYPE_ENEMIES] | rm->pointers[s].enemyList));
		rm->sizes[s].allowed = sizeofAllowedList(fileContents, pcAddr(banks[MTYPE_ALLOWED] | rm->pointers[s].allowed));
		rm->sizes[s].scroll = sizeofScrollData(fileContents, rid);
		rm->sizes[s].roomVar = 0;
		rm->sizes[s].fx2 = sizeofAsm(fileContents, pcAddr(banks[MTYPE_FX2] | rm->pointers[s].fx2));
		rm->sizes[s].plm = sizeofPLM(fileContents, pcAddr(banks[MTYPE_PLM] | rm->pointers[s].plm));
		rm->sizes[s].bg = sizeofCompressedData(fileContents, pcAddr(banks[MTYPE_BG_INSTRUCTIONS] | rm->pointers[s].bg));
		rm->sizes[s].layer12 = sizeofAsm(fileContents, pcAddr(banks[MTYPE_LAYER12] | rm->pointers[s].layer12));
		rm->doorOutSize = sizeofDoorOut(fileContents, pcAddr(banks[MTYPE_DOOR_OUT] | rm->header.doorOut));
	}

	//return (0x1A * sc) + ptrofs;

	// Size of room header + state headers + first (standard) state
	// This is all the stuff that must stay in one block and can not be repointed separately.
	return std /*+ 0x1A*/ - rid;
}

int SuperMetroidAllocator::buildMemoryMap(unsigned char *filebuf, char *roomIdFilePath, int romSize, 
	void (*progressCallback)(void* context, int level, char* desc, int percentComplete), void* context)
{
	ifstream mdb; //, *le;
	string line;
	long rid;
	chunks.clear();	
	smMyRoomStruct rm;
	smMemorySegment seg;
	//int hr;
	//Recordset rs;
	//char qstr[1000];
	//char outstr[1000];
	//glwStatic *progress;
	char sbuf[3];
	int ofs, startOfs; //, i;
	bool runFree = false;
	//bool match;
	int consecutiveFF = 0;
	int i;
	int tofs;
	int mdbLineCount;
	int mdbProcessingLine;


	mdb.open(roomIdFilePath);
	if (!mdb.is_open()) return 1;
	//le = fopen(levelEntriesFilePath, "r");
	//if (!le) { fclose(mdb); return 2; }

	mdbLineCount = 0;
	while(!mdb.eof())
	{
		getline(mdb, line);
		mdbLineCount++;
	}
	mdb.clear();
	mdb.seekg(0);

	//if (!db) return 1;
	int s;
	mdbProcessingLine = 0;
	while (!mdb.eof())
	{
		getline(mdb, line);

		if (progressCallback)
		{
#ifdef WIN32
			if (!IsBadCodePtr((FARPROC)progressCallback)) 
			{
#endif
			  progressCallback(context, 0, "Loading ROM", glwNormalizei(mdbProcessingLine, 0, mdbLineCount, 0, 5)); // int(0.2f * 100.0f * (float)mdbProcessingLine / (float)mdbLineCount));
			  progressCallback(context, 1, "Parsing room data", glwNormalizei(mdbProcessingLine, 0, mdbLineCount, 0, 100)); //int(100.0f * (float)mdbProcessingLine / (float)mdbLineCount));
#ifdef WIN32
			}
#endif
		}
		mdbProcessingLine++;
		if (line.length() <= 6)
		{
			// Security: no string vars involved so i think we're ok
			sscanf(line.c_str(), "%X", &rid);
			rm.pcAddr = rid;
			rm.romAddr = romAddr(rid);
			seg.flags = 0;
			seg.ofs = rid;
			seg.length = readRoom(filebuf, rid, &rm);
			if (seg.length)
			{
				//seg.flags = 0;
				//seg.ofs = rid;
				//seg.padding = 0;
				//seg.ref = 1;
				//seg.type = MTYPE_ROOM_HEADER;
				//chunks.AddIfUnique(seg);
				addRef(rid, seg.length, MTYPE_ROOM_HEADER, 0, MFLAG_STAY_BEFORE);

				seg.type = MTYPE_DOOR_OUT;
				seg.ofs = pcAddr(banks[MTYPE_DOOR_OUT] | rm.header.doorOut);
				seg.length = sizeofDoorOut(filebuf, seg.ofs);
				//chunks.AddIfUnique(seg);
				addRef(seg.ofs, seg.length, MTYPE_DOOR_OUT, 2, 0);

				// Door data table
				for (i = 0; i < rm.doorOutSize; i++)
				{
					seg.type = MTYPE_DOOR_DATA;
					tofs = pcAddr(banks[MTYPE_DOOR_OUT] | rm.header.doorOut) + (i * 2);
					// If pointer < 0x8000 it's not a rom address. This marks the end.
					if (!(romImage[tofs + 1] & 0x80)) break;
					seg.ofs = romImage[tofs] | (romImage[tofs + 1] << 8) | banks[MTYPE_DOOR_DATA];
					if (seg.ofs & 0x8000)
					{
						//seg.ofs = pcAddr(seg.ofs);
						//seg.length = 12;
						//seg.flags = 0;
						//chunks.AddIfUnique(seg);
						tofs = pcAddr(seg.ofs);
						if (*(unsigned short*)&(romImage[tofs]) & 0x8000)
						{
							addRef(pcAddr(seg.ofs), 12, MTYPE_DOOR_DATA, 0, 0);
						}
					}
				}

				if (rm.states)
				{
				for (s = 0; s < rm.stateCount; s++)
				{
					//sprintf(qstr, "INSERT INTO state (uid, size, instruction, arg) VALUES (%d, %d, %d, %d);", rm.states[s].ptr, rm.states[s].instruction, (int)rm.states[s].arg);
					//dosql(db, &rs, qstr);

					seg.type = MTYPE_STATE;
					seg.ofs = pcAddr(banks[MTYPE_STATE] | rm.states[s].ptr);
					seg.length = 26; // 0x1A;
					if (rm.states[s].instruction == 0xE5E6)
					{
						seg.flags = MFLAG_STAY_AFTER;
					}
					else
					{
						seg.flags = 0;
					}
					//chunks.AddIfUnique(seg);
					addRef(seg.ofs, seg.length, MTYPE_STATE, 0, seg.flags);

					seg.type = MTYPE_LEVEL_DATA;
					seg.ofs = pcAddr(rm.pointers[s].levelData);
					seg.length = (short)rm.sizes[s].levelData;
					//chunks.AddIfUnique(seg);
					addRef(seg.ofs, seg.length, MTYPE_LEVEL_DATA, 0, 0);

					seg.type = MTYPE_FX1;
					seg.ofs = pcAddr(banks[MTYPE_FX1] | rm.pointers[s].fx1);
					seg.length = sizeofFX1(romImage, seg.ofs);
					//chunks.AddIfUnique(seg);
					addRef(seg.ofs, seg.length, MTYPE_FX1, 0, 0);

					seg.type = MTYPE_ENEMIES;
					seg.ofs = pcAddr(banks[MTYPE_ENEMIES] | rm.pointers[s].enemyList);
					seg.length = (short)rm.sizes[s].enemyList;
					//chunks.AddIfUnique(seg);
					if ((romAddr(seg.ofs) & 0xFFFF) != 0x8000)	addRef(seg.ofs, seg.length, MTYPE_ENEMIES, 0, 0);

					seg.type = MTYPE_ALLOWED;
					seg.ofs = pcAddr(banks[MTYPE_ALLOWED] | rm.pointers[s].allowed);
					seg.length = rm.sizes[s].allowed;
					//chunks.AddIfUnique(seg);
					if ((romAddr(seg.ofs) & 0xFFFF) != 0x8000) addRef(seg.ofs, seg.length, MTYPE_ALLOWED, 0, 0);

					seg.type = MTYPE_SCROLL;
					seg.ofs = pcAddr(banks[MTYPE_SCROLL] | rm.pointers[s].scroll);
					if (rm.pointers[s].scroll > 1)
					{
						seg.length = rm.sizes[s].scroll;
						//chunks.AddIfUnique(seg);
						addRef(seg.ofs, seg.length, MTYPE_SCROLL, 0, 0);
					}

					seg.type = MTYPE_PLM;
					seg.ofs = pcAddr(banks[MTYPE_PLM] | rm.pointers[s].plm);
					seg.length = rm.sizes[s].plm;
					//chunks.AddIfUnique(seg);
					addRef(seg.ofs, seg.length, MTYPE_PLM, 0, 0);


					smBgElement bge;
					int bgiSize = 0;
					int tsize, a;
					// Read BG Data
					seg.type = MTYPE_BG_INSTRUCTIONS;
					seg.ofs = pcAddr(banks[MTYPE_BG_INSTRUCTIONS] | rm.pointers[s].bg);
					if (rm.pointers[s].bg)
					{
						a = seg.ofs;
						bgiSize = 0;
						do
						{
							bge.instruction = 0;
							bge.ptr = 0;
							bge.size = 0;
							tsize = smReadBgElement(romImage, a, &bge);
							if ((bge.ptr != 0) &&
								(((bge.ptr >> 16) & 0xFF) >= 0x80) &&
								(((bge.ptr >> 16) & 0xFF) <= 0xFF) 
								)
							{
								seg.type = MTYPE_BG_DATA;
								seg.ofs = pcAddr(bge.ptr);
								seg.length = bge.size;
								//chunks.AddIfUnique(seg);
								addRef(seg.ofs, seg.length, MTYPE_BG_DATA, 0, 0);
							}
							a += tsize;
							bgiSize += tsize;
						} while (tsize);
					}
					seg.type = MTYPE_BG_INSTRUCTIONS;
					seg.ofs = pcAddr(banks[MTYPE_BG_INSTRUCTIONS] | rm.pointers[s].bg);
					seg.length = bgiSize;
					addRef(seg.ofs, seg.length, MTYPE_BG_INSTRUCTIONS, 0, 0);

					seg.type = MTYPE_LAYER12;
					seg.ofs = pcAddr(banks[MTYPE_LAYER12] | rm.pointers[s].layer12);
					seg.length = rm.sizes[s].layer12;
					//chunks.AddIfUnique(seg);
					addRef(seg.ofs, seg.length, MTYPE_LAYER12, 0, 0);

				}}
			}
			else
			{
				// TODO: Error condition - room failed to load
			}
		}
	}

	mdb.close();

	// Scan for free space
	runFree = false;
	BTNode<smMemorySegment>* tmpseg;
	memset(sbuf, 0, sizeof(sbuf));
	long consecutiveFFStart = 0;
	for (ofs = 0; ofs < romSize; ofs++)
	{
		if (runFree && filebuf[ofs] != 0xFF)
		{
			tmpseg = findSegmentByAddrPC(startOfs);
			if (!tmpseg) 
			{
				seg.ofs = startOfs;
				seg.length = ofs - startOfs;
				seg.flags = 0;
				seg.type = MTYPE_FREE_SPACE;
				//chunks.AddIfUnique(seg);
				addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0);
			}
			runFree = false;
		}

		if (filebuf[ofs] == 0xFF)
		{
			consecutiveFF++;
			if (!consecutiveFFStart) consecutiveFFStart = ofs;
		}
		else 
		{
			consecutiveFF = 0;
			consecutiveFFStart = 0;
		}

		if (consecutiveFF >= 5 && !runFree)
		{
			runFree = true;
			startOfs = consecutiveFFStart; //ofs;
		}
		
		/*
		for (i = 1; i < sizeof(sbuf); i++)
		{
			sbuf[i] = sbuf[i - 1];
		}
		sbuf[0] = filebuf[ofs];
		match = true;
		for (i = 0; i < sizeof(sbuf); i++)
		{
			if (sbuf[i] != 0xFF) 
			{
				match = false;
				break;
			}
			else
			{
				match = true;
			}
		}
		if (match)
		{
			runFree = true;
			startOfs = ofs;
		}
		*/
	}

	if (runFree)
	{
		// There is a chunk of free space at the end of the rom.
		tmpseg = findSegmentByAddrPC(startOfs);
		if (!tmpseg) 
		{
			seg.ofs = startOfs;
			seg.length = ofs - startOfs;
			seg.flags = 0;
			seg.type = MTYPE_FREE_SPACE;
			addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0);
		}
	}

	//dosql(db, &rs, "END TRANSACTION;");

	/*
	ofstream dbg;
	dbg.open("E:\\SnesDev\\smrommap_dbg.txt");
	if (!dbg.is_open())
	{
		int asfgda = 0;
	}
	BTree<smMemorySegment>::iterator itseg;
	for (itseg = chunks.begin(); itseg != chunks.end(); itseg++)
	{
		dbg << hex << "$" << itseg->ofs << "  " << "$" << itseg->length << dec << "  " << itseg->type << endl;
	}
	dbg.close();
	*/

	//dosql(db, &rs, "SELECT * FROM segment;");
	//for (i = 0; i < rs.rowCount(); i++)
	//{
	//	sprintf(outstr, "%d \t %d \t %d\n", atoi(rs.cell(i, "addr")), atoi(rs.cell(i, "size")), atoi(rs.cell(i, "type")));
	//}

	//dosql(db, &rs, "END TRANSACTION;");

	// Double check reference count. Favor the higher number, even if there is a discrepancy. 
	// This will hopefully preventy the accidental deletion of data, even if there is extra stuff
	// left in.
	//listBackLinks(0x8F8001, NULL, 0);
	return 0;
}

bool myLunarLoader()
{
	int response;
	bool ret;
	//char curdir[MAX_PATH];
	/******
#ifdef WIN32
	char cdir[MAX_PATH];
#endif
	*****/
	static bool notify = true;
	if (!g_lunar_loaded)
	{
		/*********
#ifdef WIN32
		glwGetStartDirectory(cdir, MAX_PATH);
		SetCurrentDirectory(cdir);
#endif
		********/
		ret = LunarLoadDLL();
		if (!ret)
		{
			if (notify) 
			{
#ifdef WIN32
				//glwGetStartDirectory(curdir, MAX_PATH);
				//SetCurrentDirectory(curdir);
				response = MessageBox(NULL, "'Lunar Compress.dll' could not be loaded. Make sure file is present. Clicking cancel will disable this notice, but all compressed data will be reported as having 0 size.", "Error", MB_ICONEXCLAMATION | MB_OKCANCEL);
				if (response == 2) notify = false;
#endif
			}
		}
		else
		{
			g_lunar_loaded = true;
		}
	}
	return g_lunar_loaded;
}

int sizeofCompressedData(unsigned char* fileBuffer, int start)
{
	int sizeofDecompressed;
	unsigned int lastPos;

	if (!myLunarLoader()) return 0;
	if (!LunarOpenRAMFile(fileBuffer, LC_LOCKARRAYSIZE | LC_READONLY, 0x800000)) return 0;
	LunarSetFreeBytes(0xFFFFFF);
	sizeofDecompressed = LunarDecompress(g_lunarBuffer, start, 0x8000, LC_LZ5, 0, &lastPos);
	LunarCloseFile();
	if (sizeofDecompressed == 0) return 0;
	return (int)lastPos - start;
}

// The following function is used for level data and BG_Data
int x_sizeofCompressedData(unsigned char* fileBuffer, int start)
{
	// Thanks to the contributors at romhacking.net for deciphering the
	// compression routines in SM.
	// http://www.romhacking.net/docs/smwcompress.txt

	// *AHEM* ... Except that is for super Mario World, not Metroid.
	// This won't work :(
	return 0;

	if (start == pcAddr(0xB9C972)) 
	{
		start = start; //breakpoint
	}
	int pos = start;
	char cmd, ext;
	short ilen;  // instruction length
	int instruction;
	while (fileBuffer[pos] != 0xFF)
	{
		cmd = fileBuffer[pos];
		if ((cmd >> 5) == 7)
		{
			instruction = (cmd >> 2) & 0x7;
			ext = fileBuffer[++pos];
			ilen = (short(cmd & 3) << 8) | ext;
		}
		else
		{
			instruction = cmd >> 5;
			ilen = cmd & 0x1F;
		}
		pos++;
		ilen++;
		switch (instruction)
		{
		case 0:
			// Direct Copy
			// Copies the next (# + 1) bytes from
			// Compressed Data (CD) to RAM.
			pos += ilen;
			break;
		case 1:
			// Byte Fill
			// Writes the next byte (# + 1) bytes
			// deep to RAM.
			pos += 1;
			break;
		case 2:
			// Word Fill
			// Writes the next word (# + 1) bytes
			// deep to RAM.
			pos += 2;
			break;
		case 3:
			// Sigma Fill
			// Writes the next byte to RAM. Then
			// adds 1 to that byte and writes it
			// again...etc.  Writes (# + 1) times.
			pos += 1;
			break;
		case 4:
			// Library Copy
			// Copies (# + 1) bytes from the RAM
			// address provided in the next two
			// bytes.
			pos += 2;
			break;
		case 5:
			// EORed Copy
			// Much like the Library Copy (4). The only
			// difference is that all data copied
			// is EORed with %11111111.
			pos += 2;
			break;
		case 6:
			// Minus Copy
			// Subtracts next byte from Y, the RAM
			// offset, and copies (# + 1) bytes to
			// current Y. Can copy last tile.
			pos += 1;
			break;
		default:
			break;
		}
	}
	return pos - start + 1;  // Size includes terminating FF
}


int sizeofFX1(unsigned char* filebuf, long ofs) 
{
	if ((filebuf[ofs] == 0xFF) && (filebuf[ofs + 1] == 0xFF)) return 2;
	else return 16;
}

int sizeofScrollData(unsigned char* filebuf, int rid)
{
	return filebuf[rid + 4] * filebuf[rid + 5];
}

int sizeofPLM(unsigned char* filebuf, int start)
{
	int pos;
	//for (pos = start; (*(short*))(&(filebuf[pos])) != 0; pos += 2) {}
	for (pos = start; filebuf[pos] != 0 || filebuf[pos + 1] != 0; pos += 6) {}
	return pos - start + 2;
}

int sizeofEnemyList(unsigned char *filebuf, int start)
{
	int pos;
	for (pos = start; filebuf[pos] != 0xFF || filebuf[pos + 1] != 0xFF; pos += 16) {}
	return pos - start + 2;  //was  + 3 ...?
}

int sizeofDoorOut(unsigned char *filebuf, int start)
{
	int pos;
	for (pos = start; filebuf[pos + 1] >= 0x80; pos += 2) {};
	return pos - start;
}

int sizeofAllowedList(unsigned char *filebuf, int start)
{
	int pos;
	for (pos = start; filebuf[pos] != 0xFF || filebuf[pos + 1] != 0xFF; pos+= 4) {}
	return pos - start + 2;
}

int sizeofAsm(unsigned char *filebuf, int start)
{
	// TODO: This function.
	// Note: FX2 and Layer1_2 are pointers to asm code.
	// This needs to follow all conditional branches and determine the location
	// of the function's final rts/rtl instruction.
	// list<int> branches; // <--- Whenever a conditional branch is encountered,
	// allocate another int to represent the PC register if that code is executed.
	// Iterate through all possible control paths, and when two PC's converge
	// delete one of the values from the array. When all paths converge and
	// a RTS/RTL instruction is encountered, that is the end of the routine.
	// This function should prioritize which "thread" is running by the min
	// of all values. This way it will be easy to detect convergence, and 
	// we don't run the risk of advancing one thread too far.
	// Note: Also  need stacks for all threads
	return 0;
}

bool isAlphaNumeric(char *s)
{
	while (*s++)
	{
		if (!
			(*s >= 'A' && *s <= 'Z') ||
			(*s >= 'a' && *s <= 'z') ||
			(*s >= '0' && *s <= '9'))
		{
			return false;
		}
	}
	return true;
}

bool SuperMetroidAllocator::isMoveSafe(unsigned long addr, int size)
{
	//BTree<smMemorySegment>::iterator it;

	BTNode<smMemorySegment> *node;
	node = findSegmentByAddrPC(addr);
	if (!node) return false;
	if (node->data.type != MTYPE_FREE_SPACE) return false;
	if (node->data.ofs + node->data.length < addr + size) return false;
	return true;


	BTNode<smMemorySegment>* cmp = chunks.Root();

	while(cmp)
	{
		if ((((addr >= cmp->data.ofs) && (addr < cmp->data.ofs + cmp->data.length)) ||
			((addr + size >= cmp->data.ofs) && (addr + size < cmp->data.ofs + cmp->data.length)))
			&& (cmp->data.type != MTYPE_FREE_SPACE)) 
		{
			return false;
		}

		//if (addr == cmp->data.ofs)
		//{
		//}
		if (addr < cmp->data.ofs)
		{
			cmp = cmp->Left();
		}
		else // if addr > cmp->data.ofs
		{
			cmp = cmp->Right();
		}
	}
	return true;
}


bool SuperMetroidAllocator::updatePointers(int segmentType, void* base, void* member, long oldValue, long newValue, long mask)
{
	int memberOfs = (int)member - (int)base;
	BTree<smMemorySegment>::iterator it;
	smMemorySegment seg;
	bool ret = false;
	//int iOfs;

	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		seg = (smMemorySegment)(**it); //iOfs = it->ofs;
		if ((it->type == segmentType) && (((*(long*)&(romImage[seg.ofs + memberOfs])) & mask) == (oldValue & mask)))
		{
			(*(long*)&(romImage[it->ofs + memberOfs])) &= ~mask;
			(*(long*)&(romImage[it->ofs + memberOfs])) |= newValue & mask;
			ret = true;
		}
	}
	return ret;
}

int SuperMetroidAllocator::getSafeSize(unsigned long src, int* size, int* destOfs)
{
	BTNode<smMemorySegment> *node;
	smMemorySegment seg;
	int sizeBefore, sizeAfter;
	int lastPadding;
	int moveSize;

	node = findSegmentByAddrPC(src);
	if (!node) return 0;
	seg = node->data;
	moveSize = seg.length;
	lastPadding = node->data.padding;
	if (seg.flags & MFLAG_STAY_BEFORE)
	{
		// This segment must be followed by another consecutive segment
		sizeAfter = 0;
		while (node = findSegmentByAddrPC(src + moveSize + sizeAfter))
		{
			sizeAfter += node->data.length;
			lastPadding = node->data.padding;
			if (!(node->data.flags & MFLAG_STAY_BEFORE)) break;
		}
	}
	if (seg.flags & MFLAG_STAY_AFTER)
	{
		// This segment must be preceded by the previous segment.
		sizeBefore = 0;
		while (node = findSegmentByAddrPC(src - sizeBefore - 1))
		{
			sizeBefore += node->data.length;
			if (!(node->data.flags & MFLAG_STAY_AFTER)) break;
		}
	}
	if (destOfs) *destOfs = -sizeBefore;
	if (size) *size = moveSize + sizeAfter + lastPadding;
	return 0;
}

void SuperMetroidAllocator::mashFreeSpace()
{
	BTree<smMemorySegment>::iterator it;
	smMemorySegment seg1, seg2;
	bool found = false;
	unsigned long start, end;
	int foundcount;
	//BTNode<smMemorySegment>* node;
	BTNode<smMemorySegment>* foundStatic[100];
	BTNode<smMemorySegment> **foundDynamic = NULL;
	long foundistatic[100];
	long *foundidynamic = NULL;
	int i;

	do
	{
		found = false;
		seg2.ofs = 0;
		seg2.length = 0;
		seg2.type = 0;
		for (it = chunks.begin(); it != chunks.end(); it++)
		{
			seg1 = **it;
			if ((seg1.type == MTYPE_FREE_SPACE) && (seg2.type == MTYPE_FREE_SPACE) && (seg2.ofs + seg2.length >= seg1.ofs))
			{
				found = true;
				if (seg1.ofs + seg1.length > end) end = seg1.ofs + seg1.length;
			}
			else
			{
				if (found)
				{
					// Commit changes
					seg1.ofs = start;
					seg2.ofs = end - 1;
					//while (node = chunks.find_in_range(seg1, seg2))
					//{
						//chunks.Remove(node);
					//}
					foundcount = chunks.find_in_range(seg1, seg2, foundStatic, 100);
					if (foundcount > 100)
					{
						foundDynamic = new BTNode<smMemorySegment>*[foundcount];
						foundidynamic = new long[foundcount];
						chunks.find_in_range(seg1, seg2, foundDynamic, foundcount);
					}
					else
					{
						foundDynamic = foundStatic;
						foundidynamic = foundistatic;
					}
					for (i = 0; i < foundcount; i++)
					{
						foundidynamic[i] = foundDynamic[i]->data.ofs;
					}
					for (i = 0; i < foundcount; i++)
					{
						seg1.ofs = foundidynamic[i];
						chunks.Remove(seg1);
					}
					seg1.ofs = start;
					seg1.length = (short)(end - start);
					seg1.flags = 0;
					seg1.type = MTYPE_FREE_SPACE;
					seg1.padding = 0;
					seg1.ref = 0;
					chunks.Add(seg1);
					break; // exit for loop (iterator)
				}
				else
				{
					start = seg1.ofs;
					end = seg1.ofs + seg1.length;
				}
			}
			// ...
			seg2 = seg1;
		}
	} while (found);

	if (foundDynamic != NULL && foundDynamic != foundStatic) delete [] foundDynamic;
	if (foundidynamic != NULL && foundidynamic != foundistatic) delete [] foundidynamic;
	foundDynamic = NULL;
	foundidynamic = NULL;
}

int SuperMetroidAllocator::moveData(unsigned long src, unsigned long dest, int* bypassList, int listSize) //bool silent)
{
	return moveData(src, dest, bypassList, listSize, true);
}

int SuperMetroidAllocator::moveData(unsigned long src, unsigned long dest, int* bypassList, int listSize, bool copyPadding) //bool silent)
{
#ifdef WIN32
	if (chunks.checkIntegrity()) 
	{
		return 901;
	}
#endif
	// TODO: Handle the case where src and dest overlap
	//int hr;
	//bool move;
	//char qstr[1000];
	int moveSize;
	smMemorySegment seg;
	BTNode<smMemorySegment>* ptSeg;
	BTree<smMemorySegment>::iterator it;
	//smRoomHeader rm;
	long size;
	//FILE *dump;
	smMemorySegment original_segment;
	//int result;
	int freeStart = 0;
	int freeEnd = 0;
	int bypassScan;
	int sizeBefore, sizeAfter;
	smMemorySegment rangeStart, rangeEnd;
	bool bypass;
	
	BTNode<smMemorySegment> *freenode, *node;
	
	//dump = fopen("dump_before.txt", "w"); DumpTree(&chunks, dump); fclose(dump);
	
	// security checks
	//if (strlen(type) > 100) return 1;  
	//if (!isAlphaNumeric(type)) return 4;
	seg.ofs = src;
	ptSeg = chunks.Find(seg);
	if (!ptSeg) return 1;
	seg = ptSeg->data;
	original_segment = seg;
	size = seg.length;
	if ((dest & 0xFFFF) < 4)
	{
		// I recommend against moving anything to the very beginning of the bank because
		// certain values in this range are sentinels, like $8001 for BG data. Also some things
		// depend on NULL pointers ($0000) which would conflict with this.
		bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 8) { bypass = true; }} if (!bypass) return 8;
	}
	if (((romAddr(dest) & 0xFF0000) != ((romAddr(dest) + size) & 0xFF0000)) && (banks[seg.type] != 0))
	{
		// Destination crosses bank boundary, but data is bound to one bank.
		bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 7) { bypass = true; }} if (!bypass) return 7;
	}

	long imask; // inverted mask. This will be used to compare banks
	imask = maskForType(seg.type);
	imask = ~imask;
	if ((romAddr(src) & imask) != (romAddr(dest) & imask))
	{
		// Moving across banks but only a 16 bit pointer
		bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 6) bypass = true; } if (!bypass) return 6;
		//if (!silent) return 6;
	}

	/*
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if ((it->type == MTYPE_FREE_SPACE) && (
			((it->ofs >= dest) && (it->ofs <= dest + size)) ||
			((it->ofs + it->length >= dest) && (it->ofs + it->length <= dest + size)) ||
			((it->ofs <= dest && it->ofs + it->length >= dest + size))
			))
		{
			freeStart = it->ofs;
			freeEnd = it->ofs + it->length - 1;
			freenode = findSegmentByAddrPC(it->ofs);
		}
	}
	*/
	chunks.checkIntegrity();
	freenode = findSegmentByAddrPC(dest);
	chunks.checkIntegrity();
	if ((!freenode))
	{
		//if (!silent) return 5; // would overwrite important data.
		bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 5) bypass = true; } if (!bypass) return 5;
	}
	if (freenode)
	{
		//if ((freenode->data.type != MTYPE_FREE_SPACE) && (!silent)) return 5;
		if (freenode->data.type != MTYPE_FREE_SPACE)
		{
			bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 5) bypass = true; } if (!bypass) return 5;
		}
		freeStart = freenode->data.ofs;
		freeEnd = freeStart + freenode->data.length;
		chunks.checkIntegrity();
	}

	chunks.checkIntegrity();

	//BTNode<smMemorySegment> *nodeBefore, nodeAfter;
	int lastPadding = seg.padding;
	sizeAfter = 0;
	if (seg.flags & MFLAG_STAY_BEFORE)
	{
		// This segment must be followed by another consecutive segment
		while (node = findSegmentByAddrPC(src + size + sizeAfter))
		{
			sizeAfter += node->data.length;
			lastPadding = node->data.padding;
			if (!(node->data.flags & MFLAG_STAY_BEFORE)) break;
		}
	}
	sizeBefore = 0;
	chunks.checkIntegrity();
	if (seg.flags & MFLAG_STAY_AFTER)
	{
		chunks.checkIntegrity();
		// This segment must be preceded by the previous segment.
		while (node = findSegmentByAddrPC(src - sizeBefore - 1))
		{
			chunks.checkIntegrity();
			sizeBefore += node->data.length;
			if (!(node->data.flags & MFLAG_STAY_AFTER)) break;
		}
	}
	chunks.checkIntegrity();
	
	//if (ptSeg->type == MTYPE_ROOM_HEADER) //(true) //!strcmp(type, "room"))
	//sprintf(qstr, "SELECT FROM room WHERE uid==%d;", src);
	//dosql(db, &rs, qstr);
	//if (rs.rowCount() == 0) return 3;
	//moveSize = atoi(rs.cell(0, "size"));
	ptSeg = findChunk(src);
	chunks.checkIntegrity();
	if (!ptSeg) return 3;
	seg = ptSeg->data;
	moveSize = seg.length;
	
	// Check if the move will overwrite any data.
	if (moveSize == 0) return 3;
	
	rangeStart.ofs = src - sizeBefore;
	rangeEnd.ofs = src + moveSize + sizeAfter - 1;
	/*
	if (silent)
	{
		move = true;
	}
	else if (isMoveSafe(dest - sizeBefore, moveSize + sizeAfter + lastPadding))
	{
		move = true;
	}
	else
	{
	*/
	chunks.checkIntegrity();
	if (!isMoveSafe(dest - sizeBefore, moveSize + sizeAfter + lastPadding))
	{
		// Move would overwrite data
		bypass = false; for (bypassScan = 0; bypassScan < listSize; bypassScan++) { if (bypassList[bypassScan] == 5) bypass = true; } if (!bypass) return 5;
		//return 5;
		/***************
		if (glwPrompt("Warning: This will overwrite other data. Move it anyway?", "Yes\0No\0\0") == 0)
		{
			move = true;
		}
		else
		{
			move = false;
		}
		***************/
	}

	chunks.checkIntegrity();
	if (copyPadding)
	{
		memmove(&(romImage[dest]), &(romImage[src]), moveSize + seg.padding);
	}
	else
	{
		memmove(&(romImage[dest]), &(romImage[src]), moveSize);
	}
	
	chunks.checkIntegrity();
	// Check for overlap
	int setStart, setEnd;
	if (dest + moveSize > src && dest <= src) setStart = dest + moveSize;
	else setStart = src;
	if (dest < src + moveSize && dest >= src) setEnd = dest;
	else setEnd = src + moveSize;

	chunks.checkIntegrity();
	// Wipe free space
	memset(&(romImage[setStart]), 0xFF, setEnd - setStart);

	chunks.checkIntegrity();
	// update sql tables
	//sprintf(qstr, "UPDATE room SET uid=%d WHERE uid==%d;", dest, src);
	//dosql(db, &rs, qstr);
	//sprintf(qstr, "UPDATE segment SET addr=%d WHERE addr==%d;", dest, src);
	//dosql(db, &rs, qstr);
	//dump = fopen("dump.txt", "w"); DumpTree(&chunks, dump); fclose(dump);

	relink(src, dest); // Let the magic happen!

	chunks.checkIntegrity();
	node = findSegmentByAddrPC(src);
	chunks.checkIntegrity();
	if (!node) return 4;
	seg.ofs = node->data.ofs;
	seg.type = node->data.type;
	seg.length = node->data.length;
	seg.padding = node->data.padding;
	seg.ref = node->data.ref;
	seg.flags = node->data.flags;
	chunks.checkIntegrity();

	chunks.Remove(seg);
	chunks.checkIntegrity();
	ptSeg = NULL;
	freenode = findSegmentByAddrPC(dest);
	node = NULL;

	//removeRef(seg.ofs);
	seg.ofs = dest;
	seg.length = moveSize;
	////chunks.Remove(seg);
	chunks.Add(seg);
	chunks.checkIntegrity();
	//addRef(seg.ofs, seg.length, seg.type, seg.padding, seg.flags);
	//dump = fopen("dump.txt", "w"); DumpTree(&chunks, dump); fclose(dump);


	/*****
	switch (original_segment.type)
	{
		case MTYPE_ROOM_HEADER:
			// TODO: Update mdb file
			// TODO: If landing site, update engine pointer
			break;
		case MTYPE_FREE_SPACE:
			// Hopefully nothing points to free space
			break;
		case MTYPE_DOOR_OUT:
			//result = updatePointers(MTYPE_ROOM_HEADER, &rm, &rm.doorOut, romAddr(original_segment.ofs), romAddr(dest), 0xFFFF);
			result = updatePointers(MTYPE_ROOM_HEADER, (void*)0, (void*)9, romAddr(original_segment.ofs), romAddr(dest), 0xFFFF);
			break;
		case MTYPE_LEVEL_DATA:
			break;
		case MTYPE_UNKNOWN1:
			break;
		case MTYPE_FX1:
			break;
		case MTYPE_ALLOWED:
			break;
		case MTYPE_ENEMIES:
			break;
		case MTYPE_UNKNOWN2:
			break;
		case MTYPE_SCROLL:
			break;
		case MTYPE_ROOMVAR:
			break;
		case MTYPE_FX2:
			break;
		case MTYPE_PLM:
			break;
		case MTYPE_BG_INSTRUCTIONS:
			break;
		case MTYPE_LAYER12:
			break;
		default:
			break;
	}
	*******/
	/*****************************************
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		switch (it->type)
		{
		case MTYPE_ROOM_HEADER:
			//updatePointer16(seg.ofs, &rm, &rm.doorOut, src, dest);
			break;
		case MTYPE_FREE_SPACE:
			break;
		case MTYPE_DOOR_OUT:
			break;
		case MTYPE_LEVEL_DATA:
			break;
		case MTYPE_UNKNOWN1:
			break;
		case MTYPE_FX1:
			break;
		case MTYPE_ALLOWED:
			break;
		case MTYPE_ENEMIES:
			break;
		case MTYPE_UNKNOWN2:
			break;
		case MTYPE_SCROLL:
			break;
		case MTYPE_ROOMVAR:
			break;
		case MTYPE_FX2:
			break;
		case MTYPE_PLM:
			break;
		case MTYPE_BG:
			break;
		case MTYPE_LAYER12:
			break;
		default:
			break;
		}
	}
	***************************************/


	if (1) //(moveSize > 4)
	{
		// Add free space node
		seg.type = MTYPE_FREE_SPACE;
		seg.ofs = setStart; //src;
		seg.length = setEnd - setStart; //moveSize;
		seg.flags = 0;
		//chunks.AddIfUnique(seg);
		chunks.checkIntegrity();
		addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0);
		chunks.checkIntegrity();
	}
	//dump = fopen("dump_after.txt", "w"); DumpTree(&chunks, dump); fclose(dump);
	//return 0;

	
	// Resize free space
	
	/********************
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if ((it->type == MTYPE_FREE_SPACE) && (
			((it->ofs >= dest) && (it->ofs <= dest + size)) ||
			((it->ofs + it->length >= dest) && (it->ofs + it->length <= dest + size)) ||
			((it->ofs <= dest && it->ofs + it->length >= dest + size))
			))
	************************/
	chunks.checkIntegrity();
	if (!freenode)
	{
		freeStart = dest;
		freeEnd = dest + moveSize;
	}
	if ((freeStart > 0) && freenode != NULL)
	{
		sizeBefore = dest - freeStart; //it->ofs;
		if (sizeBefore < 0) sizeBefore = 0;
		sizeAfter = (freeEnd) - (dest + size);
		if (sizeAfter < 0) sizeAfter = 0;
		seg.flags = 0;
		seg.padding = 0;
		seg.ref = 0;
		seg.ofs = freeStart; // it->ofs;
		if (freenode) 
		{
			chunks.Remove(freenode);
		}
		seg.ofs = freeStart; //dest - sizeBefore;
		seg.length = sizeBefore;
		seg.type = MTYPE_FREE_SPACE;
		seg.flags = 0;
		if (sizeBefore > 0) addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0); //chunks.Add(seg);
		seg.ofs = dest + size;
		seg.length = sizeAfter;
		if (sizeAfter > 0) addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0); //chunks.Add(seg);
		//break;
	}
	//}
	chunks.checkIntegrity();

	// Recursively move blocks which must stay together, e.g. standard state and room headers
	int bypassAll[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int bypassAllSize = sizeof(bypassAll) / sizeof(int);
	BTNode<smMemorySegment>** rangeSet;
	int rangeSetCount;
	int i;
	int *rangeSeti;
	rangeSetCount = chunks.find_in_range(rangeStart, rangeEnd, NULL, 0);
	rangeSet = new BTNode<smMemorySegment>*[rangeSetCount];
	rangeSeti = new int[rangeSetCount];
	chunks.find_in_range(rangeStart, rangeEnd, rangeSet, rangeSetCount);
	for (i = 0; i < rangeSetCount; i++)
	{
		rangeSeti[i] = rangeSet[i]->data.ofs;
	}
	if (rangeSet) delete [] rangeSet; rangeSet = NULL;
	for (i = 0; i < rangeSetCount; i++)
	{
		node = findChunk(rangeSeti[i]);
		if (node)
		{
			if (node->data.type != MTYPE_FREE_SPACE)
			{
				moveData(rangeSeti[i], dest + rangeSeti[i] - src, bypassAll, bypassAllSize, false);
			}
		}
		//if (rangeSet[i]->data.type != MTYPE_FREE_SPACE)
		//{
		//	moveData(rangeSet[i]->data.ofs, dest + (rangeSet[i]->data.ofs - src), bypassAll, bypassAllSize);
		//}
	}
	if (rangeSeti) delete [] rangeSeti; rangeSeti = NULL;
	//while (node = chunks.find_in_range(rangeStart, rangeEnd))
	//{
	//	
	//}

	chunks.checkIntegrity();

	mashFreeSpace();
	return 0;
}

int verifyFileContents(FILE* f, long seekTo, char* array, long ofs, long size)
{
	char readBuf[0x1000];
	int ft = ftell(f);  // Back up file pointer
	fseek(f, seekTo, SEEK_SET);
	int p = 0;
	int rds; // read size
	int result;
	while (p < size)
	{
		if (p + 0x1000 > size) rds = size - p;
		else rds = 0x1000;
		fread(readBuf, 1, rds, f);
		result = memcmp(&(array[ofs + p]), readBuf, rds);
		if (result) return result;
		p += rds;
	}
	fseek(f, ft, SEEK_SET);  // Restore file pointer
	return 0;
}
__inline int verifyFileContents(FILE* f, long seekTo, unsigned char* array, long ofs, long size)
{
	return verifyFileContents(f, seekTo, (char*)array, ofs, size);
}

char* sm_typestr(int t)
{
	switch (t)
	{
	case MTYPE_ROOM_HEADER:
		return "room_header";
	case MTYPE_FREE_SPACE:
		return "free_space";
	case MTYPE_DOOR_OUT:
		return "door_out";
	case MTYPE_LEVEL_DATA:
		return "level_data";
	case MTYPE_UNKNOWN1:
		return "unknown1_valid";
	case MTYPE_FX1:
		return "fx1";
	case MTYPE_ALLOWED:
		return "enemy_allowed_list";
	case MTYPE_ENEMIES:
		return "enemy_instances";
	case MTYPE_UNKNOWN2:
		return "unknown2_valid";
	case MTYPE_SCROLL:
		return "scroll";
	case MTYPE_ROOMVAR:
		return "roomvar";
	case MTYPE_FX2:
		return "fx2";
	case MTYPE_PLM:
		return "plm";
	case MTYPE_BG_INSTRUCTIONS:
		return "bg_instructions";
	case MTYPE_LAYER12:
		return "layer1_2";
	case MTYPE_STATE:
		return "state";
	case MTYPE_DOOR_DATA:
		return "door_data";
	case MTYPE_BG_DATA:
		return "bg_data";
	default:
		return "unknown_type";
	}
}


char* mkPath(const char* filename, char* out_path, int max_length)
{
	for (int p = strlen(filename) - 1; p >= 0; p--)
	{
		if (filename[p] == '\\' || filename[p] == '/')
		{
			if (p < max_length - 1)	
			{
				memcpy(out_path, filename, p);
				out_path[p] = 0;
				return out_path;
			}
			else 
			{
				return NULL;
			}
		}
	}
	if (max_length > 0)
	{
		out_path[0] = 0;
		return out_path;
	}
	else
	{
		return NULL;
	}
}

bool smaCopyFile(const char* source, const char* dest)
{
	// Returns true on success, false on fail.
	int nread;
	char buffer[0x1000];
	FILE *fsrc, *fdest;
	fsrc = fopen(source, "rb");
	if (!fsrc) return false;
	fdest = fopen(dest, "w");
	if (!fdest)
	{
		fclose(fsrc);
		return false;
	}
	nread = 0x1000;
	while (nread == 0x1000)
	{
		nread = fread(buffer, 1, 0x1000, fsrc);
		fwrite(buffer, 1, nread, fdest);
	}
	fclose(fsrc);
	fclose(fdest);
	return true;
}

int SuperMetroidAllocator::save(int conflictMode)
{
	// TODO: When undo buffer is done, we can scan for motion on conflicts.
	// I.E. If there's a conflict, that segment from the original might have
	// been moved to another location. Then we can transport the changes to the
	// new location where there may not be any conflict. Need to check type.

	// Save stuff
	FILE *f = fopen(romFilename, "r+b");
	if (!f) return 2;
	unsigned long diffStart, diffLen, pos;
	bool diff = false;
	int verifyMode = conflictMode; // 99;
	//char warnmsg[1000];
	//BTNode<smMemorySegment>* node;
	BTree<smMemorySegment>::iterator it;

	// Trash stuff
	FILE* bkData = NULL;
	FILE* bkIndex = NULL;
	char fnData[MAX_PATH];
	char fnIndex[MAX_PATH];
	char tmpFilename[MAX_PATH];
	//time_t vtm;
	tm* lt;
	unsigned long ftStart;
	//list<smTrashEntry>::iterator itTrash;
	long itTrash;

	// ********************************************
	// Begin trash code
	sprintf(fnData, "%s_trash.bin", romFilename);
	sprintf(fnIndex, "%s_trash.csv", romFilename);
	bkIndex = fopen(fnIndex, "r");
	if (!bkIndex)
	{
		// If index file has not been created yet, start with a description of fields, so the user knows what this is.
		// We're trying to adhere to a CSV format so it can be imported and automated.
		bkIndex = fopen(fnIndex, "w");
		fprintf(bkIndex, "date_time, rom_snes_address, rom_pc_address, trash_file_address, size_hex, (size_decimal), type\n");
	}
	else
	{
		fclose(bkIndex);
		bkIndex = fopen(fnIndex, "a");
	}
	bkData = fopen(fnData, "ab");
	if (bkData != NULL && bkIndex != NULL)  // Silent fail
	{
		//for (itTrash = trash.begin(); itTrash != trash.end(); itTrash++)
		for (itTrash = 0; itTrash < trash.size(); itTrash++)
		{
			ftStart = ftell(bkData);
			if (ftStart > 0x7F000000)
			{
#ifdef WIN32
				MessageBox(NULL, "Warning: Your trash file is very large. I recommend deleting the trash file after confirming your ROM is in a good state. Trash is stored in yourRomFile_trash.bin and yourRomFile_trash.txt", "libSmall", MB_ICONEXCLAMATION);
#endif
			}
			//lt = localtime(&itTrash->timestamp);
			lt = localtime(&(trash[itTrash].timestamp));
			//fwrite(itTrash->data, 1, itTrash->info.length, bkData);
			fwrite(trash[itTrash].data, 1, trash[itTrash].info.length, bkData);
			//fprintf(bkIndex, "\"%.4d%.2d%.2d-%.2d%.2d%.2d\", $%.6X, $%.6X, $%X, $%X, %u, %s\n", 
			//	lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec,
			//	romAddr(itTrash->info.ofs), itTrash->info.ofs, ftStart, 
			//	itTrash->info.length, itTrash->info.length, sm_typestr(itTrash->info.type));
			fprintf(bkIndex, "\"%.4d%.2d%.2d-%.2d%.2d%.2d\", $%.6X, $%.6X, $%X, $%X, %u, %s\n", 
				lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec,
				romAddr(trash[itTrash].info.ofs), trash[itTrash].info.ofs, ftStart, 
				trash[itTrash].info.length, trash[itTrash].info.length, sm_typestr(trash[itTrash].info.type));
		}
		trash.clear();  // clear: Same interface whether using glwSmartPtr and STL list
	}
	else
	{
		//glwRaiseEvent() // TODO: Raise event 
	}
	if (bkData) { fclose(bkData); bkData = NULL; }
	if (bkIndex) { fclose(bkIndex); bkIndex = NULL; }
	// ***********************************************************
	// End trash code



	for (pos = 0; pos < originalRomSize; pos++)
	{
		if (romImage[pos] == originalImage[pos])
		{
			if (diff)
			{
doDiff:
				diffLen = pos - diffStart;
				// Check if conflict with file.
				if (verifyFileContents(f, diffStart, originalImage, diffStart, diffLen))
				{
					// Something else has modified the file where we're trying to write.
					if (verifyMode == 99 || verifyMode == 1 || verifyMode == 3)  // Not specified
					{
						//node = findSegmentByAddrPC(diffStart);
						//sprintf(warnmsg, "Warning: Rom has been changed externally. Conflicts with segment %x-%x (%s)", romAddr(diffStart), romAddr(diffStart + diffLen - 1), sm_typestr(node->data.type));
						//verifyMode = glwPrompt(warnmsg, "Cancel save\0Overwrite this segment\0Overwrite all conflicts.\0Skip this segment (leave external changes intact)\0Skip all\0\0");
						//verifyMode = 0;
					}
					else if (verifyMode == 0)
					{
						goto cancelSave;
					}
				}
				// Then write.
				if (verifyMode != 3 && verifyMode != 4)
				{
					fseek(f, diffStart, SEEK_SET);
					fwrite(&(romImage[diffStart]), 1, diffLen, f);
				}
				diff = false;
			}
		}
		else
		{
			if (!diff)
			{
				diffStart = pos;
				diff = true;
			}
		}
	}
	if (diff)
	{
		goto doDiff;
	}

	if (romSize > originalRomSize)
	{
		// If the rom has been expanded...
		fseek(f, originalRomSize, SEEK_SET);
		fwrite(&(romImage[originalRomSize]), 1, romSize - originalRomSize, f);
	}

	fclose(f);

	// Back up mdb file
	char bakFilename[MAX_PATH];
	time_t timestamp;
	tm* ti; // time info

	// Back up mdb file with time stamp
	timestamp = time(NULL);
	ti = localtime(&timestamp);
	char path[MAX_PATH];
	if (strlen(mkPath(mdbFilename, path, MAX_PATH - 1)) == 0)
	{
		sprintf(bakFilename, "%s\\mdb_%.4d%.2d%.2d_%.2d%.2d%.2d.txt", path, ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
	}
	else
	{
		sprintf(bakFilename, "%s\\mdb_%.4d%.2d%.2d_%.2d%.2d%.2d.txt", path, ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
	}
	smaCopyFile(mdbFilename, bakFilename);

	// Output mdb file
	f = fopen(mdbFilename, "w");
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if (it->type == MTYPE_ROOM_HEADER) 
		{
			fprintf(f, "%X\n", it->ofs);
		}
	}
	fclose(f);

	// TODO: Back up levels.txt
	char srcFilename[MAX_PATH];
	if (strlen(path) > 0)
	{
		sprintf(bakFilename, "%s\\level_entries_%.4d%.2d%.2d_%.2d%.2d%.2d.txt", path, ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
		sprintf(srcFilename, "%s\\level_entries.txt", path);
	}
	else
	{
		sprintf(bakFilename, "level_entries_%.4d%.2d%.2d_%.2d%.2d%.2d.txt", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
		strcpy(srcFilename, "level_entries.txt");
	}
	smaCopyFile(srcFilename, bakFilename);

	// Output levels.txt
	f = fopen(srcFilename, "w");
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if (it->type == MTYPE_LEVEL_DATA)
		{
			fprintf(f, "%X\n", it->ofs);
		}
	}
	fclose(f);

	// Log floaters
	sprintf(tmpFilename, "%s_floaters.csv", romFilename);
	f = fopen(tmpFilename, "w");
	if (f)
	{
		for (it = chunks.begin(); it != chunks.end(); it++)
		{
			if ((it->ref <= 0) && (it->type != MTYPE_FREE_SPACE || it->length < 5))
			{
				fprintf(f, "%d, %d, %d, %d, %d, %d\n", it->ofs, it->length, it->type, it->flags, it->padding, it->ref);
			}
		}
	}

	return 0;
cancelSave:
	fclose(f);
	return 1;
}

/*****************************
BTNode<smMemorySegment>* SuperMetroidAllocator::findBackLink(long snes_rom_addr, long mask)
{
	// Finds first structure which points to addr
	BTNode<smMemorySegment>* result = NULL;
	smMemorySegment segMoveSrc; //, segUpdate;
	//smBackLinkRule rule;
	smForwardLink fl;

	int i, a, s;
	BTree<smMemorySegment>::iterator it;

	result = findSegmentByAddrPC(pcAddr(snes_rom_addr));
	if (!result) return NULL;
	segMoveSrc = result->data;
	result = NULL;
	// Iterate through rules.
	for (i = 0; i < sizeof(linkrules) / sizeof(smBackLinkRule); i++)
	{
		// Compare source type.
		if (segMoveSrc.type == linkrules[i].typeMoved)
		{
			// Scan mem tree
			for (it = chunks.begin(); it != chunks.end(); it++)
			{
				// Compare destination type.
				if (it->type == linkrules[i].typeUpdate)
				{
					// Make a choice based on the format of the rule.
					if (linkrules[i].sequenceReader)
					{ // This is an "array" whose elements are variable size. We have a registered handler function.
						// The function returns the size of the element, and fills a struct with info about pointers.
						a = 0;
						do
						{
							fl.ptr = 0;
							fl.size = 0;
							s = linkrules[i].sequenceReader(romImage, it->ofs, a, &fl);
							if ((fl.ptr & mask) == (snes_rom_addr & mask))
							{
								return it.node();
							}
							a += s;
						} while ((s > 0) && (a < it->length));  // Depends on sequence reader returning 0 at end of sequence.
					}
					else if (linkrules[i].arrayStride == 0)
					{  // This is not an array. Simply test the given offset.
						if ( ((*(long*)&(romImage[it->ofs + linkrules[i].offset])) & mask) == (snes_rom_addr & mask))
						{
							return it.node();
						}
					}
					else
					{ // This is an array of fixed-size elements. Scan each one.
						for (a = linkrules[i].arrayStart; a < it->length; a += linkrules[i].arrayStride)
						{
							#define asdf (*(long*)&(romImage[it->ofs + a + linkrules[i].offset]))
							if ((asdf & mask) == (snes_rom_addr & mask))
							{
								return it.node();
							}
							#undef asdf
						}
					}
				}
			}
		}
	}
	return NULL;
}
*****************************************/

void DumpTree(BTree<smMemorySegment>* tree, FILE* f)
{
	BTree<smMemorySegment>::iterator it;
	unsigned long pstart, pend;
	pstart = 0;
	pend = 0;
	for (it = tree->begin(); it != tree->end(); it++)
	{
		fprintf(f, "%s$%.6X ($%.6X) $%.2X (%s)\n", (pend > it->ofs ? "* " : ""), romAddr(it->ofs), it->ofs, it->length, smTypeStr(it->type));
		pstart = it->ofs;
		pend = pstart + it->length;
	}
}

//int sizeofBGInstructions(unsigned char* fileBuffer, int start)

int smReadBgElement(unsigned char* fileBuffer, int start, smBgElement* info)
{
	//int i = 0;
	char pmsg[100];
	unsigned short inst; // instruction
	smBgElement info_local;
	if (!info) info = &info_local;

	// Thank you Kejardon, for BackgroundFormat.txt
	//do
	//{
		inst = *(short*)(&(fileBuffer[start]));
		info->instruction = inst;
		switch (inst)
		{
		case 0x0000:
			// terminate. no arguments
			info->instruction = 0;
			info->size = 0;
			info->ptr = 0;
			info->ptrOffset = 0;
			return 0;
			break;
		case 0x000E:
			info->ptrOffset = 4;
			info->ptr = (*(unsigned long*)&(fileBuffer[start + 4])) & 0x00FFFFFF;
			info->size = (*(unsigned long*)&(fileBuffer[start + 9])) & 0xFFFF;
			return 11;
			break;
		case 0x0002:
			info->ptrOffset = 2;
			info->ptr = (*(unsigned long*)&(fileBuffer[start + 2])) & 0x00FFFFFF;
			info->size = (*(unsigned long*)&(fileBuffer[start + 7])) & 0xFFFF;
			return 9;
			break;
		case 0x0004:
			info->ptrOffset = 2;
			info->ptr = (*(unsigned long*)&(fileBuffer[start + 2])) & 0x00FFFFFF;
			info->size = sizeofCompressedData(fileBuffer, pcAddr((*(unsigned long*)&(fileBuffer[start + 2]))) & 0xFFFF);
			return 7;
			break;
		case 0x0006:
			// no arguments
			info->ptr = 0;
			info->size = 0;
			info->ptrOffset = 0;
			return 2;
			break;
		case 0x0008:
			info->ptrOffset = 2;
			info->ptr = (*(unsigned long*)&(fileBuffer[start + 2])) & 0x00FFFFFF;
			info->size = (*(unsigned long*)&(fileBuffer[start + 7])) & 0xFFFF;
			return 9;
			break;
		case 0x000A:
			info->ptrOffset = 0;
			info->ptr = 0;
			info->size = 0;
			return 2;
			// no arguments
			break;
		case 0x000C:
			// unknown
			#ifdef WIN32
			sprintf(pmsg, "Warning: BG Instruction 0x000C encountered at ROM:$%.6X. Unknown handler.", start);
			//glwPrompt(pmsg, "Ok\0\0");
			//MessageBox(NULL, pmsg, "Warning", 0);
			#endif
			return 2;
			break;
		default:
			//#ifdef WIN32
			//sprintf(pmsg, "Warning: Unknown BG Instruction ($%.4X) encountered at ROM:$%.6X. This might indicate a corrupt ROM. Treating as end of sequence.", inst, start);
			//MessageBox(NULL, pmsg, "Warning", 0);
			//#endif
			info->size = 0;
			info->ptr = 0;
			info->ptrOffset = 0;
			return 0;
			break;
		}
	//} while (inst);
	return 0;
}


int smReadBgBase(unsigned char* fileBuffer, unsigned long segmentBegin, unsigned long offset, smForwardLink* info)
{
	smBgElement bge;
	int ret;
	ret = smReadBgElement(fileBuffer, segmentBegin + offset, &bge);
	if (info)
	{
		info->type = MTYPE_BG_DATA;
		if (info->ptr > 0) info->ptrPresent = true;
		else info->ptrPresent = false;
		info->ptr = bge.ptr;
		info->size = bge.size;
		info->ptrOffset = bge.ptrOffset;
	}
	return ret;
}

int smReadRoomHeader(unsigned char* fileBuffer, unsigned long segmentBegin, unsigned long offset, smForwardLink* info)
{
	int inst;

	if (offset > 2)
	{
		if (*(unsigned short*)&(fileBuffer[segmentBegin + offset - 2]) == 0xE5E6) return 0;
	}
	if (offset < 11)
	{
		info->ptrOffset = 9;
		info->ptr = *(unsigned short*)&(fileBuffer[segmentBegin + 9]);
		info->size = sizeofDoorOut(fileBuffer, info->ptr);
		info->type = MTYPE_DOOR_OUT;
		info->ptrPresent = true;
		return 11;
	}
	else
	{
		inst = *(unsigned short*)&(fileBuffer[segmentBegin + offset]);
		//*(unsigned short*)&(filebuffer[segmentBegin + offset])
		if (inst == 0xE612 || inst == 0xE629)
		{
			//extra parameter before pointer
			info->ptr = *(unsigned short*)(&(fileBuffer[segmentBegin + offset + 3]));
			info->ptrOffset = offset + 3;
			info->size = 26;
			info->type = MTYPE_STATE;
			info->ptrPresent = true;
			return 5;
		}
		else if (inst == 0xE5E6)
		{
			info->ptr = segmentBegin + offset + 2;
			info->ptrOffset = 0;
			info->size = 26;
			info->type = MTYPE_STATE;
			info->ptrPresent = false;
			return 2;
		}
		else
		{
			info->ptr = *(unsigned short*)(&(fileBuffer[segmentBegin + offset + 2]));
			info->ptrOffset = offset + 2;
			info->size = 26;
			info->type = MTYPE_STATE;
			info->ptrPresent = true;
			return 4;
		}
	}
}


template<class T> int smiCopyElement(T* ptr, int current, int size, T& value)
{
	if ((current < size) && (ptr != NULL))
	{
		ptr[current] = value;
	}
	return current + 1;
}

int SuperMetroidAllocator::listBackLinksLong(unsigned long snes_rom_addr, unsigned long* yourArray, int size)
{
	static smLinkInfo* ia = NULL;
	static int iaSize = 0;
	int result;

	if (!ia) { ia = new smLinkInfo[10]; iaSize = 10; }
	result = listBackLinks(snes_rom_addr, ia, iaSize);
	if (result > iaSize)
	{
		iaSize = glwRealloc(&ia, result + 10, iaSize, iaSize);
		result = listBackLinks(snes_rom_addr, ia, iaSize);
	}
	for (int i = 0; i < result && i < size; i++)
	{
		yourArray[i] = ia[i].begin + ia[i].offset;
	}
	return result;
}

long SuperMetroidAllocator::maskForType(int type)
{
	if (type >= MTYPE_MAX_VALUE) return 0x0000FFFF;
	else 
	{
		if (banks[type] & 0x00FF0000) return 0x0000FFFF;
		else return 0x00FFFFFF;
	}
}

int SuperMetroidAllocator::listBackLinks(unsigned long snes_rom_addr, smLinkInfo* yourArray, int size)
{
	// Finds all structures which points to addr
	BTNode<smMemorySegment>* result = NULL;
	smMemorySegment segMoveSrc; //, segUpdate;
	//smBackLinkRule rule;
	smForwardLink fl;
	smLinkInfo li;
	unsigned long mask;

	unsigned long i, a, s;
	unsigned long rhaddr = 0;
	int yaCurrent = 0;
	BTree<smMemorySegment>::iterator it;

	result = findChunk(pcAddr(snes_rom_addr)); //findSegmentByAddrPC(pcAddr(snes_rom_addr));
	if (!result) return 0;
	mask = maskForType(result->data.type);
	segMoveSrc = result->data;
	result = NULL;
	// Iterate through rules.
	for (i = 0; i < sizeof(linkrules) / sizeof(smBackLinkRule); i++)
	{
		// Compare source type.
		if (segMoveSrc.type == linkrules[i].typeMoved)
		{
			// Scan mem tree
			for (it = chunks.begin(); it != chunks.end(); it++)
			{
				// If type is state, find previous room header
				if (it->ofs > rhaddr  && it->ofs < segMoveSrc.ofs) rhaddr = it->ofs;

				// Compare destination type.
				if (it->type == linkrules[i].typeUpdate)
				{
					// Make a choice based on the format of the rule.
					if (linkrules[i].sequenceReader)
					{ // This is a stream, more or less. We have a registered handler function.
						// The function sequenceReader returns the size of the element, and fills a struct with info about pointers.
						a = 0;
						do
						{
							//fl.ptr = 0;
							//fl.size = 0;
							s = linkrules[i].sequenceReader(romImage, it->ofs, a, &fl);
							if (fl.ptrPresent && ((fl.ptr & mask) == (snes_rom_addr & mask)) && (fl.type == segMoveSrc.type))
							{
								//return it.node();
								li.begin = it->ofs;
								li.offset = fl.ptrOffset;
								li.mask = maskForType(segMoveSrc.type); //(it->type);
								li.type = it->type;
								if ((yaCurrent < size) && (yourArray != NULL)) yourArray[yaCurrent] = li; //it->ofs;
								yaCurrent++;
							}
							a += s;
						} while (s);  // Depends on sequence reader returning 0 at end of sequence.
					}
					else if (linkrules[i].arrayStride == 0)
					{  // This is not an array. Simply test the given offset.
						if ( ((*(long*)&(romImage[it->ofs + linkrules[i].offset])) & mask) == (snes_rom_addr & mask))
						{
							//return it.node();
							li.begin = it->ofs;
							li.offset = linkrules[i].offset;
							li.mask = maskForType(segMoveSrc.type); //(it->type);
							li.type = it->type;
							li.ptrPresent = true;
							if ((yaCurrent < size) && (yourArray != NULL)) yourArray[yaCurrent] = li; //it->ofs;
							yaCurrent++;
						}
					}
					else
					{ // This is an array of fixed-size elements. Scan each one.
						for (a = linkrules[i].arrayStart; a < it->length; a += linkrules[i].arrayStride)
						{
							//#define asdf (*(long*)&(romImage[it->ofs + a + linkrules[i].offset]))
							#define asdf (*(long*)&(romImage[it->ofs + a]))
							if ((asdf & mask) == (snes_rom_addr & mask))
							{
								//return it.node();
								li.begin = it->ofs;
								li.offset = a;
								li.mask = maskForType(segMoveSrc.type); //(it->type);
								li.type = it->type;
								li.ptrPresent = true;
								if ((yaCurrent < size) && (yourArray != NULL)) yourArray[yaCurrent] = li; //it->ofs;
								yaCurrent++;
							}
							#undef asdf
						}
					}
				}
			}
		}
	}
	if (segMoveSrc.type == MTYPE_STATE && yaCurrent == 0)
	{
		result = findChunk(rhaddr);
		if (result)
		{
			if (result->data.type == MTYPE_ROOM_HEADER)
			{
				li.begin = rhaddr;
				li.offset = 0;
				li.mask = 0;
				li.type = MTYPE_ROOM_HEADER;
				if ((yaCurrent < size) && (yourArray != NULL)) yourArray[yaCurrent] = li;
				yaCurrent++;
			}
		}
	}
	return yaCurrent;
}

/*********************************************
int SuperMetroidAllocator::listForwardLinksLong(long rom_addr, long mask, long* yourArray, int size)
{
	BTNode<smMemorySegment>* node;
	int yaCurrent = 0;
	int result;
	smMemorySegment seg;
	smMyRoomStruct rm;
	int s;
	int ofs;
	long L;

//	yaCurrent = smiCopyElementLong(yourArray, yaCurrent, size, );

	node = findSegmentByAddrPC(pcAddr(rom_addr));
	if (!node) return 0;
	seg = node->data;
	switch (seg.type)
	{
	case MTYPE_ROOM_HEADER:
		result = readRoom(romImage, pcAddr(rom_addr), &rm);
		L = long(banks[MTYPE_DOOR_OUT] | rm.header.doorOut);
		yaCurrent = smiCopyElement(yourArray, yaCurrent, size, L);
		for (s = 0; s < rm.stateCount; s++)
		{
			L = banks[MTYPE_STATE] | rm.states[s].ptr;
			yaCurrent = smiCopyElement(yourArray, yaCurrent, size, L);
		}
		break;
	case MTYPE_FREE_SPACE:
		break;
	case MTYPE_DOOR_OUT:
		for (ofs = 0; ofs < seg.length; ofs += 2)
		{
			L = *(short*)(romImage[pcAddr(rom_addr) + ofs]) | banks[MTYPE_DOOR_DATA];
			yaCurrent = smiCopyElement(yourArray, yaCurrent, size, L);
		}
		break;
	case MTYPE_LEVEL_DATA:
		break;
	case MTYPE_UNKNOWN1:
		break;
	case MTYPE_FX1:
		break;
	case MTYPE_ALLOWED:
		break;
	case MTYPE_ENEMIES:
		break;
	case MTYPE_UNKNOWN2:
		break;
	case MTYPE_SCROLL:
		break;
	case MTYPE_ROOMVAR:
		break;
	case MTYPE_FX2:
		break;
	case MTYPE_PLM:
		break;
	case MTYPE_BG_INSTRUCTIONS:
		break;
	case MTYPE_LAYER12:
		break;
	case MTYPE_STATE:
		break;
	case MTYPE_DOOR_DATA:
		break;
	case MTYPE_BG_DATA:
		break;
	default:
		break;
	}
	return yaCurrent;
}
***********************************************/

int SuperMetroidAllocator::relink(unsigned long src, unsigned long dest)
{
	static smLinkInfo* ia;
	static int iaSize;
	int result;

	// This function is often called from moveData, which invalidates iterators and changes
	// the 'chunks' tree. Is that a problem?

	if (!ia) { ia = new smLinkInfo[10];	iaSize = 10; }
	result = listBackLinks(romAddr(src), ia, iaSize);
	if (result > iaSize)
	{
		iaSize = glwRealloc(&ia, result + 10, iaSize, iaSize);
		result = listBackLinks(src, ia, iaSize);
	}
	for (int i = 0; i < result; i++)
	{
		if (ia[i].offset != -1)
		{
			*(long*)&(romImage[ia[i].begin + ia[i].offset]) &= ~ia[i].mask;
			*(long*)&(romImage[ia[i].begin + ia[i].offset]) |= romAddr(dest) & ia[i].mask;
		}
	}
	return result;
}

int SuperMetroidAllocator::addRef(unsigned long addr, unsigned long size, int type, int padding,  long flags)  // pc addr
{
	smMemorySegment seg;
	BTNode<smMemorySegment> *node = findChunk(addr);
	if (node)
	{
		if (node->data.type != type) return 0;
		if (size > node->data.length) node->data.length = size;
		if (padding > node->data.padding) node->data.padding = padding;
		node->data.flags = (short)flags;
		node->data.ref++;
		return node->data.ref;
	}
	else
	{
		seg.ofs = addr;
		seg.length = size;
		seg.flags = (short)flags;
		seg.type = type;
		seg.padding = padding;
		seg.ref = 1;
		chunks.Add(seg);
		return 1;
	}
}

int SuperMetroidAllocator::removeRef(unsigned long addr) // Becomes free space if ref count reaches zero.
{
	BTNode<smMemorySegment> *node;
	//smMemorySegment seg;
	int size;
	int blc;
	smLinkInfo li;
		
	node = findChunk(addr);
	if (node)
	{
		if (node->data.type == MTYPE_FREE_SPACE) return 0;  // Don't delete free node
		size = node->data.length;
		if (node->data.ref <= 1)
		{
			blc = listBackLinks(romAddr(addr), &li, 1);
			if (blc == 0)
			{
				deleteSegment(romAddr(addr));
			}
			else
			{
				node->data.ref = blc;
			}
			return 0;
		}
		/***************
		{
			memset(&(romImage[addr]), 0xFF, size);
			chunks.Remove(node);
			seg.flags = 0;
			seg.length = size;
			seg.ofs = addr;
			seg.padding = 0;
			seg.ref = 0;
			seg.type = MTYPE_FREE_SPACE;
			chunks.Add(seg);
			mashFreeSpace();
			return 0;
		}
		***************/
		else
		{
			node->data.ref--;
			return node->data.ref;
		}
	}
	else
	{
		return 0;
	}
}

int SuperMetroidAllocator::deleteSegment(unsigned long addr) // Returns number of back links removed.
{
	int ret = 0;
	smLinkInfo* ar = NULL;
	unsigned long arc, i;
	BTNode<smMemorySegment>* node;
	smMemorySegment seg;

	node = findChunk(pcAddr(addr));
	//BTNode<smMemorySegment>* nodeStd;
	if (!node) return 0;
	seg = node->data;
	long mask = maskForType(seg.type);
	arc = listBackLinks(addr, NULL, 0);
	ar = new smLinkInfo[arc];
	listBackLinks(addr, ar, arc);
	// Clear back link pointers. In other words, anything that points here 
	// needs to be reset to 0 so it won't point to invalid data.
	BTNode<smMemorySegment>* resizeNode;
	smMemorySegment resizeSegment;
	smBgElement bgi;
	//unsigned long newRoomAddr;
	//int shs;  // state header size
	unsigned long j, k; // offsets into room header
	unsigned long cs; // copy size
	unsigned long ds; // deleted size
	unsigned long cpExtra;

	static list<long> addr_stack;
	list<long>::iterator it;
	for (it = addr_stack.begin(); it != addr_stack.end(); it++)
	{
		if (*it == addr) return 0;
	}
	//it._Orphan_me(); // Not available in retail build?
	//it._Clrcont();
	addr_stack.push_front(addr);
	BTree<smMemorySegment>::iterator itStd;
	for (i = 0; i < arc; i++)
	{
		// Process backlink list
		// Zero the bits indicated by ones in the mask.
		*(long*)&(romImage[ar[i].begin + ar[i].offset]) &= ~(long)maskForType(node->data.type);

		// Some types require resizing
		if (ar[i].type == MTYPE_ROOM_HEADER && seg.type == MTYPE_STATE)
		{
			// Room header must be resized
			resizeNode = findChunk(ar[i].begin);
			itStd = chunks.make_iterator(resizeNode);
			itStd++;
			if (resizeNode)
			{
				resizeSegment = resizeNode->data;
				j = 11;
				k = 11;
				while (j < resizeSegment.length)
				{
					if (*(unsigned short*)&(romImage[resizeSegment.ofs + j]) == 0xE612 ||
						*(unsigned short*)&(romImage[resizeSegment.ofs + j]) == 0xE629)
					{
						cs = 3;
						cpExtra = 2;
					}
					else if (*(unsigned short*)&(romImage[resizeSegment.ofs + j]) == 0xE5E6)
					{
						cs = 2;
						cpExtra = 0;
					}
					else
					{
						cs = 2;
						cpExtra = 2;
					}
					//if ((*(unsigned short*)&(romImage[resizeSegment.ofs + j + cs]) & mask) != (seg.ofs & mask))
					if ((*(unsigned short*)&(romImage[resizeSegment.ofs + j + cs]) & mask) != 0)
					{
						if (j != k) memcpy(&romImage[resizeSegment.ofs + k], &romImage[resizeSegment.ofs + j], cs + cpExtra);
						k += cs + cpExtra;
					}
					j += cs + cpExtra;
				}
				// Fill difference with free space
				memset(&romImage[resizeSegment.ofs + k], 0xFF, j - k);
				//nodeStd = findSegmentByAddrPC(resizeSegment.ofs + resizeSegment.length);
				//if (nodeStd)
				//{
					// Relocate std state.
					for (i = 0; i < 0x1A; i++)
					{
						//romImage[resizeSegment.ofs + k + i] = romImage[nodeStd->data.ofs + i];
						romImage[resizeSegment.ofs + k + i] = romImage[itStd->ofs + i];
					}
					//nodeStd->data.ofs = resizeSegment.ofs + k;
					itStd->ofs = resizeSegment.ofs + k;
				//}
				resizeNode->data.length = k;
				//addRef(resizeSegment.ofs + k, j - k, MTYPE_FREE_SPACE, 0, 0);
				//addRef(nodeStd->data.ofs + nodeStd->data.length, j - k, MTYPE_FREE_SPACE, 0, 0);
				addRef(itStd->ofs + itStd->length, j - k, MTYPE_FREE_SPACE, 0, 0);


				/***********************************
				// This is the move and relink approach, seems needlessly complex
				switch(*(unsigned short*)&(romImage[ar[i].begin + ar[i].offset]))
				{
				case 0xE612:
					shs = 5;
					break;
				case 0xE629:
					shs = 5;
					break;
				default:
					shs = 4;
				}
				// alloc new room header and copy all but the deleted state header
				// 0xE612 or 0xE629 use an extra param
				newRoomAddr = newSegment(MTYPE_ROOM_HEADER, resizeSegment.length - shs);
				memcpy(&(romImage[newRoomAddr]), &(romImage[resizeSegment.ofs]), 11);
				k = 11;
				for (j = 11; j < resizeSegment.length; j++)
				{
					if (*(unsigned short*)&(romImage[resizeSegment.ofs + j]) == 0xE612 ||
						*(unsigned short*)&(romImage[resizeSegment.ofs + j]) == 0xE629)
					{
						if (*(unsigned short*)&() != 
					}
					else
					{
					}
				}
				*********************************/
			}
		}
		else if (ar[i].type == MTYPE_BG_INSTRUCTIONS && seg.type == MTYPE_BG_DATA)
		{
			// BG Instructions are variable length and must be shortened
			j = ar[i].offset;
			smReadBgElement(romImage, ar[i].begin + j, &bgi);
			ds = bgi.size;
			j += ds;
			cs = 0;
			while (smReadBgElement(romImage, ar[i].begin + j, &bgi))
			{
				cs += bgi.size;
			}
			for (k = 0; k < cs; k++)
			{
				romImage[ar[i].begin + ar[i].offset + k] = romImage[ar[i].begin + ar[i].offset + ds + k];
			}
		}
	}
	delete [] ar;

	// Now remove the references for everything to which this data points.
	// This could potentially delete the data downstream if nothing else is
	// using it. Small keeps a reference counter. When it reaches 0, that
	// data will be deleted as well.
	unsigned int fc; // forward count
	fc = listForwardLinks(addr, NULL, 0);
	smForwardLink *fa = new smForwardLink[fc];
	listForwardLinks(addr, fa, fc);
	for (i = 0; i < fc; i++)
	{
		removeRef(pcAddr(fa[i].ptr | banks[fa[i].type]));
	}
	delete [] fa;

	// And now, remove this segment from the tree.

	smTrashEntry te;
	BTNode<smMemorySegment> *pnSeg;

	if (deleteOnUnlink) // user preference, public member
	{
		chunks.Remove(seg);
	
		// Move to trash
		te.info = seg;
		te.info.ref = 0;
		te.data = new char[te.info.length];
		te.timestamp = time(&te.timestamp);
		if (!te.data)
		{
#ifdef WIN32
			MessageBox(NULL, "Ran out of memory while trying to write data to the in-memory trash bin.", "libSmall", MB_ICONEXCLAMATION);
#endif
		}
		else
		{
			memcpy(te.data, &(romImage[te.info.ofs]), te.info.length);
			//trash.push_back(te);
			//MessageBox(NULL, "trash collection disabled to circumvent a bug with MSVC. Your deleted data is not being backed up.", "Warning", MB_ICONEXCLAMATION);
			trash[trash.size()] = te;  // glwSmartPtr resizes automatically. size() will now report old value + 1
		}
		te.data = NULL;
		zero(te.info);

		// Overwrite with free space
		memset(&(romImage[seg.ofs]), 0xFF, seg.length);
		addRef(seg.ofs, seg.length, MTYPE_FREE_SPACE, 0, 0);
	}
	else
	{
		pnSeg = findChunk(seg.ofs);
		if (pnSeg) 
		{
			pnSeg->data.ref = 0;
		}
	}
	mashFreeSpace();
	addr_stack.pop_front();
	return arc;
}

int SuperMetroidAllocator::listForwardLinks(unsigned long addr, smForwardLink* yourArray, unsigned long arraySize)
{
	// TODO: Add reference to standard state if type is room header.
	BTNode<smMemorySegment> *node, *sizenode;
	smForwardLink fl;
	unsigned long i, a, s, yaCurrent;
	unsigned long mask;

	yaCurrent = 0;
	node = findChunk(pcAddr(addr));
	if (!node) return 0;
	mask = maskForType(node->data.type);
	for (i = 0; i < sizeof(linkrules) / sizeof(smBackLinkRule); i++)
	{
		if (linkrules[i].typeUpdate == node->data.type)
		{
			// Make a choice based on the format of the rule.
			if (linkrules[i].sequenceReader)
			{ // This is an "array" whose elements are variable size. We have a registered handler function.
				// The function returns the size of the element, and fills a struct with info about pointers.
				a = 0;
				do
				{
					zero(fl);
					s = linkrules[i].sequenceReader(romImage, pcAddr(addr), a, &fl);
					if (fl.ptrPresent && (fl.ptr != NULL))
					{
						if (yourArray != NULL && yaCurrent < arraySize) yourArray[yaCurrent] = fl;
						yaCurrent++;
					}
					a += s;
				} while ((s > 0) && (a < node->data.length));  // Depends on sequence reader returning 0 at end of sequence.
			}
			else if (linkrules[i].arrayStride == 0)
			{  // This is not an array. Simply test the given offset.
				fl.ptr = ((*(long*)&(romImage[pcAddr(addr) + linkrules[i].offset])) & linkrules[i].mask);
				if (fl.ptr != NULL)
				{
					fl.ptrOffset = linkrules[i].offset;
					sizenode = findChunk(pcAddr(fl.ptr));
					if (sizenode) fl.size = node->data.length;
					else fl.size = -1;
					fl.type = linkrules[i].typeMoved;
					if (yourArray != NULL && yaCurrent < arraySize) yourArray[yaCurrent] = fl;
					yaCurrent++;
				}
			}
			else
			{ // This is an array of fixed-size elements. Scan each one.
				for (a = linkrules[i].arrayStart; a < node->data.length; a += linkrules[i].arrayStride)
				{
					fl.ptr = (*(long*)&(romImage[pcAddr(addr) + a + linkrules[i].offset])) & linkrules[i].mask;
					if (fl.ptr != NULL)
					{
						fl.ptrOffset = a + linkrules[i].offset;
						sizenode = findChunk(pcAddr(fl.ptr));
						if (sizenode) fl.size = sizenode->data.length;
						else fl.size = 0;
						fl.type = linkrules[i].typeMoved;
						if (yourArray != NULL && yaCurrent < arraySize) yourArray[yaCurrent] = fl;
						yaCurrent++;
					}
				}
			}
		}
	}
	if (node->data.type == MTYPE_ROOM_HEADER)
	{
		// Add std state
		fl.ptrPresent = false;
		fl.ptr = romAddr(node->data.ofs+ node->data.length) & 0xFFFF;
		fl.size = 26;
		fl.ptrOffset = 0;
		fl.type = MTYPE_STATE;
		if (yourArray != NULL && yaCurrent < arraySize) yourArray[yaCurrent] = fl;
		yaCurrent++;
	}
	return yaCurrent;
}

//long SuperMetroidAllocator::newRoom(int width, int height, int n4states, unsigned long *a4states, int n5states, unsigned long *a5states, int doors, int plms, int fx1, int enemyList, int enemyAllowed)
long SuperMetroidAllocator::newRoom(int width, int height, int n4states, int n5states)
{
	long pHeader;
	int i;
	long ofs;
	//long pos;
	const unsigned char init4[] = {0x69, 0xE6, 0, 0};
	const unsigned char init5[] = {0x12, 0xE6, 0x0E, 0, 0};

	pHeader = allocNew(MTYPE_ROOM_HEADER, 11 + (4 * n4states) + (5 * n5states) + 2, 0);
	if (!pHeader) return 0;
	romImage[pHeader + 4] = width & 0xFF;
	romImage[pHeader + 5] = height & 0xFF;
	i = 0;
	ofs = 11;
	/*************
	while (i < n4states)
	{
		pos = pHeader + ofs;
		romImage[pos++] = a4states[i] & 0xFF;
		romImage[pos++] = (a4states[i] >> 8) & 0xFF;
		*(short*)&(romImage[pos]) = (allocNew(MTYPE_STATE, 26, 0)) & 0xFFFF;
		i++;
		ofs += 4;
	}
	i = 0;
	while (i < n5states)
	{
		pos = pHeader + ofs;
		romImage[pos++] = a5states[i] & 0xFF;
		romImage[pos++] = (a5states[i] >> 8) & 0xFF;
		romImage[pos++] = (a5states[i] >> 16) & 0xFF;
		i++;
		ofs += 5;
	}
	pos = pHeader + ofs;
	romImage[ofs++] = 0xE6;
	romImage[ofs++] = 0xE5;
	********************/
	
	// TODO: Resume here
	int s;
	for (s = 0; s < n4states; s++)
	{
		memcpy(&(romImage[pHeader + 11 + (s * 4)]), init4, 4);
	}
	for (s = 0; s < n5states; s++)
	{
		memcpy(&(romImage[pHeader + 11 + (n4states * 4) + (s * 5)]), init5, 5);
	}
	ofs = pHeader + 11 + (n4states * 4) + (n5states * 5);
	romImage[ofs] = 0xE6;
	romImage[ofs + 1] = 0xE5;
	return pHeader;
}

void SuperMetroidAllocator::libSmall_modifyChunksForNewRoom(unsigned long rid)
{
	smMemorySegment seg;
	BTNode<smMemorySegment>* pseg1;
	pseg1 = findChunk(rid);
	if (!pseg1) return;
	if (pseg1->data.length >= 11+2+26)
	{
		seg.ofs = pseg1->data.ofs + pseg1->data.length - 26;
		//seg.ofs = pseg1->data.ofs + 13;
		seg.length = 26;
		seg.flags = MFLAG_STAY_AFTER;
		seg.type = MTYPE_STATE;
		seg.padding = 0;
		seg.ref = 1;
		chunks.Add(seg);
		pseg1->data.flags |= MFLAG_STAY_BEFORE;

		//seg.length = pseg1.length - (13 + 26);
		//if (seg.length > 0)
		//{
		//	seg.type = MTYPE_FREE_SPACE;
		//	seg.ofs = pseg1->ofs + 13 + 26;
		//	seg.flags = 0;
		//	seg.ref = 0;
		//	seg.padding = 0;
		//	chunks.Add(seg);
		//}
		pseg1->data.length -= 26;
		romImage[pseg1->data.ofs + pseg1->data.length - 1] = 0xE5;
		romImage[pseg1->data.ofs + pseg1->data.length - 2] = 0xE6;
	}
	mashFreeSpace();
}

// This next function is probably the single most important function: the Allocator
long SuperMetroidAllocator::allocNew(int type, unsigned long size, unsigned long flags)
{
	smMemorySegment seg, segFree, newFree, newseg;
	unsigned long minFoundFree = 0xFFFF;
	unsigned long needfree = size + paddingForType(type);
	unsigned long minAddr, maxAddr;
	if (type < 0 || type >= MTYPE_MAX_VALUE) return 0;
	// TODO: Add conditions to span banks for segment types that are able to do so.
	if (size > 0x8000) return 0;  // You need more than a bank

	if (type == MTYPE_ROOM_HEADER)
	{
		// For redundancy: 13 is the size of all room header data, including a 2 byte door out ptr at ofs 9, plus 2 bytes for std state marker literal E5E6
		// 26 is the size of a state. At least one (the std state) must follow header, immediately after the E5E6
		if (size >= 13)
		{
			needfree = size + 26;
		}
		else
		{
			needfree = 11 + 2 + 26; // Header + std state mark (E6E5) + std state
		}
	}

	if (banks[type])
	{
		minAddr = pcAddr(banks[type]);
		maxAddr = pcAddr(banks[type] | 0xFFFF);
	}
	else
	{
		minAddr = 0;
		maxAddr = romSize;
	}
	BTree<smMemorySegment>::iterator it;
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if (it->ofs >= minAddr && it->ofs < maxAddr)
		{
			if (it->ofs >= 0x300000)
			{
				minAddr = minAddr; //breakpoint
			}
			//if (maxAddr - it->ofs >= needfree)
			if (it->length >= needfree && 
				it->length < minFoundFree && 
				it->ofs + needfree <= maxAddr &&
				it->ofs >= minAddr &&
				it->type == MTYPE_FREE_SPACE)
			{
				segFree = **it;
				minFoundFree = it->length;
			}
		}
	}
	//segFree = *it;
	//chunks.Remove(segFree);
	//seg.ofs = segFree.ofs
	if (minFoundFree != 0xFFFF) 
	{
		chunks.Remove(segFree);
		newFree.ofs = segFree.ofs + needfree;
		newFree.length = (short)(segFree.length - needfree);
		newFree.flags = segFree.flags;
		newFree.ref = 0;
		newFree.type = MTYPE_FREE_SPACE;
		newFree.padding = 0;
		chunks.Add(newFree);

		newseg.ofs = segFree.ofs;
		if (type == MTYPE_ROOM_HEADER)
			newseg.length = (short)needfree;
		else
			newseg.length = size;
		newseg.type = type;
		newseg.padding = paddingForType(type);
		newseg.flags = defaultFlagsForType(type);
		newseg.ref = 0;
		chunks.Add(newseg);

		memset(&(romImage[segFree.ofs]), 0, needfree);
		
		//addRef(segfree.ofs, size, type, paddingForType(type), flags);
		if (type == MTYPE_ROOM_HEADER) 
		{
			libSmall_modifyChunksForNewRoom(segFree.ofs);
		}
		else
		{
			allocInit(segFree.ofs, needfree, type);
		}
		return segFree.ofs;
	}
	else
	{
		if ((banks[type] == 0) && 
			((flags & SMALL_CAN_EXPAND_ROM) != 0))
		{
			// expand rom
			unsigned char* tptr = new unsigned char[romSize + 0x8000];
			if (!tptr) return 0;
			memcpy(tptr, romImage, romSize);
			memset(&tptr[romSize], 0xFF, 0x8000);

			// Add segments 
			seg.ofs = romSize + 4;
			seg.length = needfree;
			seg.flags = defaultFlagsForType(type);
			seg.type = type;
			seg.padding = paddingForType(type);
			seg.ref = 0;
			chunks.Add(seg);
			newFree.ofs = romSize + 4 + needfree;
			newFree.length = 0x10000 - (romAddr(newFree.ofs) & 0xFFFF);
			newFree.type = MTYPE_FREE_SPACE;
			newFree.flags = 0;
			newFree.ref = 0;
			newFree.padding = 0;
			chunks.Add(newFree);

			// Finalize
			delete [] romImage;
			romImage = tptr;
			romSize += 0x8000;
			memset(&romImage[seg.ofs], 0, needfree);
			memset(&romImage[newFree.ofs], 0xFF, newFree.length);
			if (type == MTYPE_ROOM_HEADER) libSmall_modifyChunksForNewRoom(seg.ofs);
			return seg.ofs;
		}
		else return 0;  // Not found
	}
	return 0;
}

int SuperMetroidAllocator::allocInit(unsigned long addr_pc, unsigned long size, int type) // , unsigned long _4states, unsigned long _5states)
{
	// Initializes memory so that the data will be properly parsed when opened in the future. However, some of the pointers,
	// or forward links, contained in the newly allocated block may be invalid. This mainly ensures that SMAll will correctly
	// discover the size of the new block when it opens the rom again, even if not listed in *_floaters.txt
	// Note: Assumes that (padding) bytes are available after requested size
	bool domemset = false;
	int msval;
	unsigned long i;
	switch (type)
	{
	case MTYPE_FREE_SPACE:
		domemset = true;
		msval = 0xFF;
		break;
	case MTYPE_PLM:
	case MTYPE_DOOR_OUT:
		size &= 0xFFFFFFFE;
		for (i = 0; i < size; i += 2)
		{
			*(unsigned short*)(&romImage[addr_pc + i]) = 0x8000;
		}
		romImage[i] = 0x01;
		romImage[i + 1] = 0x01;
		break;
	case MTYPE_FX1:
		domemset = true;
		if (size <= 2) msval = 0xFF;
		else msval = 0x00;
		break;
	case MTYPE_ENEMIES:
	case MTYPE_ALLOWED:
		size &= 0xFFFFFFFE;
		for (i = 0; i < size; i += 2)
		{
			*(unsigned short*)(&romImage[addr_pc + i]) = 0x0000;
		}
		romImage[i] = 0xFF;
		romImage[i + 1] = 0xFF;
		break;
	//case MTYPE_ROOM_HEADER:
	//	// TODO: This
	//	break;
	default:
		domemset = true;
		msval = 0;
	}
	if (domemset) memset(&romImage[addr_pc], msval, size);
	return 0;
}


/******************************************************************
xxxxx long SuperMetroidAllocator::newSegment(int type, int size)
{
	// Bounds check
	if (type >= MTYPE_MAX_VALUE) return 0;

	// Some types must be a fixed size.
	switch(type)
	{
	case MTYPE_FX1:
		if (size != 2) size = 16;
		break;
	case MTYPE_ROOM_HEADER:
		//size = 11 + 2 + 26;  // Header + standard state header + standard state
		size += 26;
		break;
	case MTYPE_STATE:
		size = 26;
		break;
	case MTYPE_DOOR_DATA:
		if (size != 2) size = 12;
		break;
	default:
		break;
	}

	BTNode<smMemorySegment> *smallestFreeNode;
	BTree<smMemorySegment>::iterator it;
	int ss; // smallest size (of free space node which is > size param)
	ss = 0x7FFF;
	smallestFreeNode = NULL;
	smMemory
	Segment freeseg;
	long imask = ~maskForType(type);

	// Search for the smallest free space node which will contain the data.
	for (it = chunks.begin(); it != chunks.end(); it++)
	{
		if ((it->type == MTYPE_FREE_SPACE) && ((it->ofs & imask) == banks[type]) )
		{
			if (it->length >= size && it->length < ss) 
			{
				smallestFreeNode = it.node();
				ss = it->length;
			}
		}
	}
	if (!smallestFreeNode)
	{
		// There is not a large enough contiguous free space.
		return 0;
	}
	smMemorySegment newseg;
	if (type == MTYPE_ROOM_HEADER) size -= 26;
	freeseg = smallestFreeNode->data;
	chunks.Remove(smallestFreeNode);

	// Don't use addRef because nothing is referencing this data. 
	// ref count needs to be 0.
	newseg.flags = defaultFlagsForType(type);
	newseg.length = size;
	newseg.ofs = freeseg.ofs;
	newseg.padding = paddingForType(type);
	newseg.ref = 0;
	newseg.type = type;
	chunks.Add(newseg);

	memset(&(romImage[freeseg.ofs]), 0x00, size + paddingForType(type));
	if (type == MTYPE_ROOM_HEADER)
	{
		// The room header does reference this state so addRef is ok
		addRef(freeseg.ofs + size, 26, MTYPE_STATE, 0, MFLAG_STAY_AFTER);
		if (freeseg.length - size - 26 > 0) 
		{
			// Reference counter doesn't matter for free space
			addRef(freeseg.ofs + size + 26, freeseg.length - size - 26, MTYPE_FREE_SPACE, 0, 0);
		}
	}
	else
	{
		// Reference counter doesn't matter for free space
		addRef(freeseg.ofs + size + paddingForType(type), freeseg.length - size, MTYPE_FREE_SPACE, 0, 0);
	}
	return freeseg.ofs;
}
*************************************************************************/

int SuperMetroidAllocator::paddingForType(int type)
{
	switch (type)
	{
	case MTYPE_ENEMIES:
	case MTYPE_PLM:
	case MTYPE_ALLOWED:
	case MTYPE_DOOR_OUT:
		return 2;
	default:
		return 0;
	}
}

unsigned short SuperMetroidAllocator::defaultFlagsForType(int type)
{
	switch (type)
	{
	case MTYPE_ROOM_HEADER:
		return MFLAG_STAY_BEFORE;
	case MTYPE_STATE:
		return 0; // MTYPE_STAY_AFTER; // Only for standard state
	default:
		return 0;
	}
}

int SuperMetroidAllocator::convertUnknownToFree(unsigned long start, unsigned long end)
{
	// TODO: Returns the number of bytes reclaimed for free space.
	BTree<smMemorySegment>::iterator it;
	unsigned long betweenStart, betweenEnd;
	long ret = 0;
	bool found;
	//smMemorySegment freeseg;

	//freeseg.type = MTYPE_FREE_SPACE;
	//freeseg.flags = 0;
	//freeseg.ref = 0;
	//freeseg.padding = 0;

	it = chunks.begin();
	found = false;
	while (it != chunks.end())
	{
		if (it->ofs >= start && it->ofs <= end)
		{
			found = true;
			break;
		}
		it++;
	}
	if (found)
	{
		//freeseg.ofs = start;
		//freeseg.length = it->data.ofs - start;
		memset(&romImage[start], 0, it->ofs - start);
		addRef(start, it->ofs - start, MTYPE_FREE_SPACE, 0, 0);
	}

	while (it != chunks.end())
	{
		if (it->ofs >= end) break;
		betweenStart = it->ofs + it->length;
		it++;
		if (it == chunks.end())
		{
			betweenEnd = romSize;
		}
		else
		{
			betweenEnd = it->ofs;
		}
		memset(&romImage[betweenStart], 0, betweenEnd - betweenStart);
		addRef(betweenStart, betweenEnd - betweenStart, MTYPE_FREE_SPACE, 0, 0);
	}
	if (betweenStart < end)
	{
		memset(&romImage[betweenStart], 0, end - betweenStart);
		addRef(betweenStart, end - betweenStart, MTYPE_FREE_SPACE, 0, 0);
	}
	mashFreeSpace();
	return 0;
}

/****************************************
int SuperMetroidAllocator::insertPtr(unsigned long base, unsigned long prefixBytes, unsigned long ptrSizeBytes, unsigned long suffixBytes, unsigned long value)
{
	BTNode<smMemorySegment> node;
	// Validation checks
	if (prefixBytes > 0xFFFF) return 1;
	if (suffixBytes > 0xFFFF) return 1;
	if (ptrSizeBytes > 3) return 1;

	node = findChunk( base);
	int mask, shift;
	if (!node) return 1;
	mask = 0xFF;
	shift = 0;
	for (int n = base+prefixBytes; n < base + prefixBytes + ptrSizeBytes
	romImage[base + prefixBytes] = (char)(value & )
}
****************************************/

long SuperMetroidAllocator::link(unsigned long target_snes_addr, int type, unsigned long backlink_segment_start_snesaddr, unsigned long backlink_offset_snesaddr, bool copyExisting)
{
	// target_snes_addr is the target, or the segment being pointed to, the pointee
	// backlink_start is the address
	// Returns new ref count
	BTNode<smMemorySegment> *node, *oldnode;
	unsigned long oldptr;

	long blOfs = pcAddr(backlink_segment_start_snesaddr + backlink_offset_snesaddr); // backlink offset
	if (blOfs < 0 || blOfs >= (long)romSize - 3)
	{
		return 0;
	}
	oldptr = (*(unsigned long*)&(romImage[blOfs])) & maskForType(type);
	*(unsigned long*)&(romImage[blOfs]) &= ~maskForType(type);
	*(unsigned long*)&(romImage[blOfs]) |= target_snes_addr & maskForType(type);
	node = findChunk(pcAddr(target_snes_addr));
	if ((oldptr != NULL) && (oldptr >= 0x8000))
	{
		oldptr |= banks[type];
		oldnode = findChunk(pcAddr(oldptr));
		if ((pcAddr(target_snes_addr) < romSize) && (node != NULL) && (oldnode != NULL) && (copyExisting))
		{
			memcpy(&romImage[node->data.ofs], &romImage[oldnode->data.ofs], min(node->data.length, oldnode->data.length));
		}
		removeRef(pcAddr(oldptr));
	}
	if (node)
	{
		node->data.ref++;
		return node->data.ref;
	}
	else
	{
		return 0;
	}
}

int smDefaultSizeForType(int type)
{
	switch (type)
	{
	case MTYPE_FX1:
		return 16;
	case MTYPE_ROOM_HEADER:
		return 13+26;
	case MTYPE_PLM:
		return 8;
	case MTYPE_ENEMIES:
		return 19;
	case MTYPE_DOOR_OUT:
		return 4;
	case MTYPE_ALLOWED:
		return 6;
	case MTYPE_STATE:
		return 26;
	case MTYPE_DOOR_DATA:
		return 12;

	default:
		return 0;
	}
}


/*
   __________________________________________________________________
  |      ___           ___           ___           ___       ___     |
  |     /\  \         /\__\         /\  \         /\__\     /\__\    |
  |    /::\  \       /::|  |       /::\  \       /:/  /    /:/  /    |
  |   /:/\ \  \     /:|:|  |      /:/\:\  \     /:/  /    /:/  /     |
  |  _\:\~\ \  \   /:/|:|__|__   /::\~\:\  \   /:/  /    /:/  /      |
  | /\ \:\ \ \__\ /:/ |::::\__\ /:/\:\ \:\__\ /:/__/    /:/__/       |
  | \:\ \:\ \/__/ \/__/~~/:/  / \/__\:\/:/  / \:\  \    \:\  \       |
  |  \:\ \:\__\         /:/  /       \::/  /   \:\  \    \:\  \      |
  |   \:\/:/  /        /:/  /        /:/  /     \:\  \    \:\  \     |
  |    \::/  /        /:/  /        /:/  /       \:\__\    \:\__\    |
  |     \/__/         \/__/         \/__/         \/__/     \/__/    |
  |                                                                  |
  |                                                                  |
  |                   The Super Metroid Allocator                    |
  |__________________________________________________________________|

*/
//!end libSmall.cpp

