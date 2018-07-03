#ifndef __ROMCLASS_H__
#define __ROMCLASS_H__
#include "gbatiles.h"
#include "gbaimage.h"
#include "gbatypes.h"

#include <stdio.h>
#include <vector>
using namespace std;
#define u32 unsigned long
class RomClass{
public:
	RomClass();
	~RomClass();
	int Loaded;
	
	void DecodePal(short *palGBA, COLOR* palPC, int numpals, char palpos);
	long FindFreeSpace(long bNo, unsigned char bConst);
	//void* DecodeSNESPal(long offset, long* dstPal, int numPals, char palpos, int size, vector<unsigned char>* buffer);
	void* DecodeSNESPal(long offset, COLOR* dstPal, int numPals, char palpos, int size, vector<unsigned char>* buffer);
	
	FILE* ROM;
	//int SMDecomp(u32 offset);//Auto writes to CompBuffer
	//int SMDecomp(u32 offset, unsigned char* buffer);//Writes to buffer
	char RomFilePath[1024];


};
#endif