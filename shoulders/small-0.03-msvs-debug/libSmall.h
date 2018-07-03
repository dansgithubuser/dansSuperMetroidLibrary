//!begin libSmall.h
/*
   __________________________________________________________________________
  |      ___           ___           ___           ___       ___             |
  |     /\  \         /\__\         /\  \         /\__\     /\__\            |
  |    /::\  \       /::|  |       /::\  \       /:/  /    /:/  /            |
  |   /:/\ \  \     /:|:|  |      /:/\:\  \     /:/  /    /:/  /             |
  |  _\:\~\ \  \   /:/|:|__|__   /::\~\:\  \   /:/  /    /:/  /         o    |
  | /\ \:\ \ \__\ /:/ |::::\__\ /:/\:\ \:\__\ /:/__/    /:/__/               |
  | \:\ \:\ \/__/ \/__/~~/:/  / \/__\:\/:/  / \:\  \    \:\  \        o      |
  |  \:\ \:\__\         /:/  /       \::/  /   \:\  \    \:\  \              |
  |   \:\/:/  /        /:/  /        /:/  /     \:\  \    \:\  \             |
  |    \::/  /        /:/  /        /:/  /       \:\__\    \:\__\            |
  |     \/__/         \/__/         \/__/         \/__/     \/__/            |
  |                                                                          |
  |                                                                          |
  |                     The Super Metroid Allocator                          |
  |__________________________________________________________________________|


   Project SMAll
   Author: Daniel Papenburg (DFPercush)
   Development date: June 2010 - the time stamp on this file
   This document is best viewed in a fixed width font with tab spacing of 4.

   File: libSmall.h
   Description: This is the master #include file for SMAll. It will let you
       access the functionality of Small through your program. This
	   header does not provide any graphical display functions. It only deals
	   with the ROM file and the structures contained therein.
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

Thanks to a wonderful community of hackers, for all your help. Without the
rest of you, this project would not have been possible.

            _________________________________________________
		   |                                                 |
		   | "If it seems like I can see far, it is because  |
		   |  I stand on the shoulders of giants."           |
		   |_________________________________________________|
*/

// Note: The useful stuff is down a ways. Search for class SuperMetroidAllocator.


/***************
TODO:

Bank select
Undo buffer
Auto defrag (low priority)
Compare to original mdb and overwrite unused level data with FF
Dynamic banks - load from file

****************/


//#define BANK_DOOR_DATA 0x830000

#ifdef WIN32
#include <windows.h>
#endif

#include <glw/glwBtree.h>
//#include <list>
#include <glwSmartPtr.h>
#include <memory.h>
using namespace std;

#ifdef _MSC_VER
#if (_MSC_VER > 6)
#pragma warning(disable:4996)
#endif
#endif


#ifndef zero
#define zero(var) memset(&var, 0, sizeof(var))
#endif
#ifndef zeroa
#define zeroa(a, size) memset(a, 0, sizeof(a[0]) * size)
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif // DWORD

#ifndef MAX_PATH
#define MAX_PATH 254
#endif

struct smForwardLink;
struct smBgElement;
struct smLinkInfo;

struct smLongPtr
{
	unsigned long ptr;
	unsigned long size;
};

struct smShortPtr
{
	unsigned short ptr;
	unsigned short size;
};

struct smLinkInfo
{
	unsigned long begin; // Starting address of structure (room header, door out table, etc)
	unsigned long offset; // Offset of the pointer into the structure.
	unsigned long mask;  // Bitmask which specifies the size of the pointer.
	int type;
	bool ptrPresent;
};

struct smForwardLink
{
	unsigned long ptr;
	int size;
	unsigned long ptrOffset; // offset of the pointer into element
	int type;
	bool ptrPresent;
};

struct smBgElement : public smForwardLink
{
	int instruction;
	//int src; // if (src != 0 && size == 0) that means compressed data
	//int size;
};

// Room header & info   - orange
// Unallocated space, unavailable - white
// free space - green
// Door out tables - gray
typedef struct smStatePointersShort
{
	unsigned long levelData; // tan
	unsigned short unknown1;
	unsigned short fx1;      // yellow
	unsigned short enemyList;  // red
	unsigned short allowed;  // magenta
	unsigned short unknown2;
	unsigned short scroll;   // blue
	unsigned short roomVar;
	unsigned short fx2;    // yellow green
	unsigned short plm;  // purple
	unsigned short bg;  // blue green
	unsigned short layer12;  // light blue
} smStatePointers;

typedef struct smStatePointersLong_t
{
	unsigned long levelData; // tan
	unsigned long unknown1;
	unsigned long fx1;      // yellow
	unsigned long enemyList;  // red
	unsigned long allowed;  // magenta
	unsigned long unknown2;
	unsigned long scroll;   // blue
	unsigned long roomVar;
	unsigned long fx2;    // yellow green
	unsigned long plm;  // purple
	unsigned long bg;  // blue green
	unsigned long layer12;  // light blue
} smStatePointersLong;

struct smRoomStateHeader
{
	unsigned short instruction;
	unsigned char arg;
	unsigned short ptr;
};

#define MFLAG_DIRTY 0x01
#define MFLAG_STAY_BEFORE 0x02
#define MFLAG_STAY_AFTER 0x04


// Memory types
#define MTYPE_ROOM_HEADER 1
#define MTYPE_FREE_SPACE 2
#define MTYPE_DOOR_OUT 3

#define MTYPE_LEVEL_DATA 4
#define MTYPE_UNKNOWN1 5
#define MTYPE_FX1 6
#define MTYPE_ALLOWED 7
#define MTYPE_ENEMIES 8
#define MTYPE_UNKNOWN2 9
#define MTYPE_SCROLL 10
#define MTYPE_ROOMVAR 11
#define MTYPE_FX2 12
#define MTYPE_PLM 13
#define MTYPE_BG_INSTRUCTIONS 14
#define MTYPE_LAYER12 15
#define MTYPE_STATE 16
#define MTYPE_DOOR_DATA 17
#define MTYPE_BG_DATA 18

#define MTYPE_MAX_VALUE 18


//extern const int smMinSizeForType [];
int smDefaultSizeForType(int type);

__inline char* sm_typestr(int t);
__inline char* smTypeStr(int t) { return sm_typestr(t); }

struct smBackLinkRule
{
	int typeMoved;  // Type of data being moved - this is the forward link
	int typeUpdate;  // Type of data that needs updating - this is the back link
	int offset; // offset within the structure to the pointer
	long mask;  // bitmask to modify. dest &= ~mask; dest |= (src & mask);
	int arrayStart;  // Offset within the structure to the first element of the array.
	int arrayStride;  // Size of an element. If this == 0 treat as a single pointer with known offset. Otherwise, scan entire segment as an array, starting at [arrayStart], every [arrayStride] bytes, compare the [offset]'th byte.
	int (*sequenceReader)(unsigned char* buffer, unsigned long segment_begin, unsigned long offset, smForwardLink*);
};
int smLinkRuleCount();  // For exact measurement
extern const smBackLinkRule linkrules [];
#define SMALL_MAX_RULES 100 // If you need a static array that is equal or larger in size, use this.


struct smMemorySegment
{
	unsigned long ofs; // 24 bit pointer
	unsigned long length;
	short type;
	short flags;
	// padding; // Will only reclaim (length) bytes for free space, but (length + padding) free space will be copied to dest.
	short padding; // padding: This accommodates door out pointer tables, where the only terminating condition is a pointer <0x8000.
	short ref;  // ref: The number of items which point to this data.
	  //A value of 0 means this is a floater and is not linked to anything else.
};

class smTrashEntry
{
public:
	smMemorySegment info; // same data type as used in chunks
	char* data;  // dynamically allocated
	time_t timestamp;
	smTrashEntry() { data = 0; zero(info); }
	~smTrashEntry() { if (data) delete [] data; }
};

int seg_cmp(smMemorySegment& s1, smMemorySegment& s2);

extern BTree<smMemorySegment> chunks; // TODO: Move inside class SuperMetroidAllocator


struct smRoomHeader
{
	//char unknown1[9]; // 9
	char unknown1[4];
	char width;
	char height;
	char unknown2[3];
	unsigned short doorOut;
	//char unknown2[3]; // 5?
};



class smMyRoomStruct
{
public:
	smMyRoomStruct();
	void clear();
	~smMyRoomStruct() { clear(); }
	unsigned long pcAddr;
	unsigned long romAddr;
	smRoomHeader header;
	int stateCount;
	//smStateId *states;
	smRoomStateHeader *states;
	smStatePointersShort *pointers;
	smStatePointersShort *sizes;
	//smStatePointersLong banks;
	int doorOutSize;
	//unsigned long doorOutBank;
	//unsigned long stateHeaderBank;
};



int sizeofAsm(unsigned char *filebuf, int start);
int sizeofAllowedList(unsigned char *filebuf, int start);
int sizeofDoorOut(unsigned char *filebuf, int start);
int sizeofEnemyList(unsigned char *filebuf, int start);
int sizeofPLM(unsigned char* filebuf, int start);
int sizeofScrollData(unsigned char* filebuf, int rid);
int sizeofFX1(unsigned char* filebuf, long ofs);
int sizeofCompressedData(unsigned char* fileBuffer, int start);
int sizeofBGInstructions(unsigned char* fileBuffer, int start);
unsigned long romAddr(DWORD pc_addr);
unsigned long pcAddr(DWORD rom_addr);
void DumpTree(BTree<smMemorySegment>* tree, FILE* f);
int smReadBgElement(unsigned char* fileBuffer, int offset, smBgElement* info);
int smReadBgBase(unsigned char* fileBuffer, unsigned long segment_begin, unsigned long offset, smForwardLink* info);
int smReadRoomHeader(unsigned char* filebuffer, unsigned long segment_begin, unsigned long offset, smForwardLink* info);
int smLinkRuleCount();

//===================================================
// begin flags

// Ok to automatically add a bank to the end of the rom file if more space is needed.
#define SMALL_CAN_EXPAND_ROM 1

// when resizing to a smaller size, move the data to the smallest available free block
#define SMALL_ALWAYS_MOVE 2

// end flags
//==================================================



// ###########################################################################
// #                                                                         #
// #                       SuperMetroidAllocator                             #
// #                                                                         #
// #   This is the main class which applications will use to control SMAll.  #
// #                                                                         #
// ###########################################################################

class SuperMetroidAllocator
{
	friend class smetallocWindow;
public:
	SuperMetroidAllocator();
	~SuperMetroidAllocator();
	int moveData(unsigned long src, unsigned long dest, int* bypassList, int bypassCount);  // bypassList is a list of warnings (return codes) to ignore.
	int moveData(unsigned long src, unsigned long dest, int* bypassList, int listSize, bool copyPadding);  // recommended for internal use only
	bool isMoveSafe(unsigned long addr, int size);
	BTNode<smMemorySegment>* findSegmentByAddrPC(unsigned long addr);
	BTNode<smMemorySegment>* findChunk(unsigned long uid);
	//bool updatePointer16(long ofs_ptr, void* base, void* member, unsigned short oldValue, unsigned short newValue);
	//bool updatePointer24(long ofs_ptr, void* base, void* member, long oldValue, long newValue);
	bool updatePointers(int segmentType, void* base, void* member, long oldValue, long newValue, long mask);
	//int open(char* romFile, char* mdbFile);
	int open(char* romFile, char* mdbFile,
	  void (*progressCallback)(void* context, int level, char* desc, int percentComplete) = NULL, void* context = NULL);
	int save(int conflictMode = 0);  // How to handle conflicts (Some other program saved over the file while open in SMAll.)
	BTNode<smMemorySegment>* findBackLink(unsigned long snes_rom_addr, unsigned long mask); // Finds first structure which points to addr
	int listBackLinksLong(unsigned long rom_addr, unsigned long* yourArray, int size); // Returns number of available links. Only copies up to (size) pointers. size is # of int's, not bytes.
	//int listForwardLinksLong(long rom_addr, long mask, long* yourArray, int size); // Returns number of available links. Only copies up to (size) pointers. size is # of int's, not bytes.
	int readRoom(unsigned char *fileContents, unsigned long rid, smMyRoomStruct *rm);
	int listBackLinks(unsigned long snes_rom_addr, smLinkInfo* yourArray, int size);
	long maskForType(int type);
	int relink(unsigned long src, unsigned long dest);  // pc addr. This does not copy data, only updates pointers.
	int addRef(unsigned long addr, unsigned long size, int type, int padding,  long flags);  // returns # of references after add, uses pc addr
	int removeRef(unsigned long addr); // Returns # of references after removal. Becomes free space if ref count reaches zero.
	int getSafeSize(unsigned long src, int* size, int* destOfs);
	void mashFreeSpace();
	// TODO: The following functions:
	long changeSegment(unsigned long addr, int size, unsigned char* buffer);  // Modify buffer / length
	//long newSegment(int type, int size);  // Allocate from free space - only some types require a size parameter
	int deleteSegment(unsigned long snes_rom_addr); // Returns number of back links removed.
	int listForwardLinks(unsigned long snes_rom_addr, smForwardLink* yourArray, unsigned long arraySize);  // Build a list of items to which the current item points.
	int paddingForType(int type);  // Number of bytes following the actual data that must be preserved
	unsigned short defaultFlagsForType(int type);
	long allocNew(int type, unsigned long size, unsigned long flags);  // Creates a new segment from free space. Room headers are special, see newRoom().
	int allocInit(unsigned long addr_pc, unsigned long size, int type); // Initializes memory after allocation.
	long allocResize(unsigned long src, long new_size, long flags);
	long link(unsigned long target_snes_addr, int type, unsigned long backlink_segment_start_snesaddr, unsigned long backlink_offset, bool copyExisting);
	/****************************************************
	*****************************************************/
	int read(unsigned long pc_addr, int size, void* dest) { memcpy(dest, &(romImage[pc_addr]), size); }
	int write(unsigned long pc_addr, int size, void* src) { memcpy(&(romImage[pc_addr]), src, size); }
	void libSmall_modifyChunksForNewRoom(unsigned long rid); // helper function
	//long newRoom(int width, int height, int n4states, unsigned long *a4states, int n5states, unsigned long *a5states, int doors, int plms, int fx1, int enemyList, int enemyAllowed); // Returns the address (pc) of the new room header, or 0 on fail.
	long newRoom(int width, int height, int n4states, int n5states);

	// Public direct access vars
	bool deleteOnUnlink; // Controls whether data is erased and put in the trash (this adds free space), or marked as floating. Default is true (erase).
	int convertUnknownToFree(unsigned long start_pc, unsigned long end_pc);

private:
	int buildMemoryMap(unsigned char *filebuf, char *roomIdFilePath, int romSize,
		void (*progressCallback)(void* context, int level, char* desc, int percentComplete) = NULL, void* context = NULL);
	bool dirtyRoomHeaders;  // for SMILE compatibility
	bool dirtyLevelData;	// for SMILE compatibility
	unsigned char* romImage;
	unsigned char* originalImage;
	unsigned long romSize;
	unsigned long originalRomSize;
	char romFilename[MAX_PATH];
	char mdbFilename[MAX_PATH];
	unsigned long banks[MTYPE_MAX_VALUE];
	//list<smTrashEntry> trash;  // When M$ gets their act together maybe I can use STL again... sigh
	glwSmartPtr<smTrashEntry> trash;
};

DWORD pcAddr(DWORD rom_addr);
DWORD romAddr(DWORD pc_addr);


int sizeofCompressedData(unsigned char* fileBuffer, int start);
int sizeofFX1();
int sizeofScrollData(unsigned char* filebuf, int rid);
int sizeofPLM(unsigned char* filebuf, int start);
int sizeofEnemyList(unsigned char *filebuf, int start);
int sizeofDoorOut(unsigned char *filebuf, int start);
int sizeofAllowedList(unsigned char *filebuf, int start);
int sizeofAsm(unsigned char *filebuf, int start);
bool isAlphaNumeric(char *s);


/*
   __________________________________________________________________________
  |      ___           ___           ___           ___       ___             |
  |     /\  \         /\__\         /\  \         /\__\     /\__\            |
  |    /::\  \       /::|  |       /::\  \       /:/  /    /:/  /            |
  |   /:/\ \  \     /:|:|  |      /:/\:\  \     /:/  /    /:/  /             |
  |  _\:\~\ \  \   /:/|:|__|__   /::\~\:\  \   /:/  /    /:/  /         o    |
  | /\ \:\ \ \__\ /:/ |::::\__\ /:/\:\ \:\__\ /:/__/    /:/__/               |
  | \:\ \:\ \/__/ \/__/~~/:/  / \/__\:\/:/  / \:\  \    \:\  \        o      |
  |  \:\ \:\__\         /:/  /       \::/  /   \:\  \    \:\  \              |
  |   \:\/:/  /        /:/  /        /:/  /     \:\  \    \:\  \             |
  |    \::/  /        /:/  /        /:/  /       \:\__\    \:\__\            |
  |     \/__/         \/__/         \/__/         \/__/     \/__/            |
  |                                                                          |
  |                                                                          |
  |                     The Super Metroid Allocator                          |
  |__________________________________________________________________________|

*/
//!end libSmall.h
