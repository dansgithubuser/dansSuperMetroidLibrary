#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "compresch_types.h"
#ifndef COMPRESCHMAIN_H
#define COMPRESCHMAIN_H

enum {
	FLG_DECOMPRESS =	0x01,
	FLG_INSERT =		0x02,
	FLG_PAUSE =			0x04
};

enum {
	FORMAT_KIRBY =		0,
	FORMAT_POKEMON,
	FORMAT_METROID
};
class CompreschMain{
public:
char prompt_flgs[512];
char prompt_infile[512];
char prompt_outfile[512];
char prompt_offset[512];
char prompt_format[512];
char prompt_format2[512];
//CompreschMain();
//~CompreschMain();

void PrintTitle();


void PrintUsage();

void ShouldPause(int flgs);

int  PromptForArgs();

int main(int argc, char **argv);

int Main_Compress(unsigned char** outbuf,int flgs,unsigned char* inbuf,int insiz,int offset,int format,int format2);

int Main_Decompress(unsigned char* outbuf,unsigned char* inbuf,int insiz);



};
#endif