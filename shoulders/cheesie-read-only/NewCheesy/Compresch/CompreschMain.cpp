/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

#include "CompreschMain.h"

#include "compresch_kirby.h"
#include "compresch_pokemon.h"
#include "Compresch_Metroid.h"


void CompreschMain::PrintTitle()
{
	//printf("%s","Compresch w/ executable front.\nWIP (no version number)\n\n");
}

void CompreschMain::PrintUsage()
{
	//printf("%s","Usage:\nCompresch <flags> <infile> <outfile> <offset> <format> <fmt2>\nSee readme for details.\n");
}

void CompreschMain::ShouldPause(int flgs)
{
	if(!(flgs & FLG_PAUSE))
		return;

	//printf("%s","Press any key to exit.\n");
	//_getch();
}

int CompreschMain::PromptForArgs()
{
	printf("%s","Prompt interface activated.  Leave any field blank to abort and exit.\nSee readme for details.\n");

	printf("%s","Valid flags:\nd - Decompress (exclude to compress)\ni - Insert (exclude to overwrite)\np - Pause before exiting\nType '-' for no flags\n");

	printf("%s","Flags:  ");
	gets(prompt_flgs);
	if(!prompt_flgs[0])			return 1;
	
	printf("%s","Input file:  ");
	gets(prompt_infile);
	if(!prompt_infile[0])		return 1;
	
	printf("%s","Output file:  ");
	gets(prompt_outfile);
	if(!prompt_outfile[0])		return 1;
	
	printf("%s","Offset:  ");
	gets(prompt_offset);
	if(!prompt_offset[0])		return 1;
	
	printf("%s","Format:  ");
	gets(prompt_format);
	if(!prompt_format[0])		return 1;
	
	printf("%s","Fmt2:  ");
	gets(prompt_format2);
	if(!prompt_format2[0])		return 1;

	return 0;
}


int CompreschMain::main(int argc, char **argv)
{
	PrintTitle();

	char* sz_flgs;
	char* sz_infile;
	char* sz_outfile;
	char* sz_offset;
	char* sz_format;
	char* sz_format2;

	if(argc <= 1)
	{
		if(PromptForArgs())
			return 0;
		sz_flgs =		prompt_flgs;
		sz_infile =		prompt_infile;
		sz_outfile =	prompt_outfile;
		sz_offset =		prompt_offset;
		sz_format =		prompt_format;
		sz_format2 =	prompt_format2;
	}
	else if(argc != 7)
	{
		PrintUsage();
		return 0;
	}
	else
	{
		sz_flgs =		argv[1];
		sz_infile =		argv[2];
		sz_outfile =	argv[3];
		sz_offset =		argv[4];
		sz_format =		argv[5];
		sz_format2 =	argv[6];
	}

	/*
	 *	get flags
	 */
	int flgs = 0;

	int i;
	for(i = 0; sz_flgs[i]; ++i)
	{
		switch(sz_flgs[i])
		{
		case 'd':	flgs |= FLG_DECOMPRESS;		break;
		case 'i':	flgs |= FLG_INSERT;			break;
		case 'p':	flgs |= FLG_PAUSE;			break;
		}
	}

	/*
	 *	get offset
	 */
	int offset = 0;
	sscanf(sz_offset,"%X",&offset);

	/*
	 *	get format
	 */
	int format = 0;
	int format2 = 0;
	sscanf(sz_format ,"%X",&format);
	sscanf(sz_format2,"%X",&format2);


	/* Begin work */
	
	printf("%s","\n\n");

	FILE* file;
	u8* inbuf;
	int insiz;

	file = fopen(sz_infile,"rb");
	if(!file)
	{
		printf("%s","Error:  Unable to open input file.\n");
		ShouldPause(flgs);
		return 0;
	}
	fseek(file,0,SEEK_END);
	insiz = ftell(file);
	if(!insiz)
	{
		printf("%s","Error:  Input file has size of 0 bytes.\n");
		fclose(file);
		ShouldPause(flgs);
		return 0;
	}

	fseek(file,0,SEEK_SET);
	inbuf = new u8[insiz + 2000];
	fread(inbuf,1,insiz,file);
	fclose(file);

	u8* outbuf = 0;
	int outsiz;
	if(flgs & FLG_DECOMPRESS)
		outsiz = Main_Decompress(outbuf,inbuf,insiz);
	else
		outsiz = Main_Compress(&outbuf,flgs,inbuf,insiz,offset,format,format2);

	delete[] inbuf;

	if(outsiz > 0)
	{
		if((flgs & FLG_INSERT) && !(flgs & FLG_DECOMPRESS))
		{
			file = fopen(sz_outfile,"r+b");
			if(file)	fseek(file,offset,SEEK_SET);
		}
		else
			file = fopen(sz_outfile,"wb");

		if(!file)
		{
			printf("%s","Error:  Unable to open output file for writing.\n");
			if(outbuf)
				delete[] outbuf;
			ShouldPause(flgs);
			return 0;
		}

		fwrite(outbuf,1,outsiz,file);
		fclose(file);
		printf("Success!  $%X bytes output.\n",outsiz);
	}

	if(outbuf)
		delete[] outbuf;

	ShouldPause(flgs);
	return 0;
}

#define COMPRESS_NORMAL(FMT,CLS)					\
	case FMT:										\
	outsiz = CLS::WorstCompressSize(insiz);			\
	*outbuf = new u8[outsiz];						\
	outsiz = CLS::Compress(inbuf,insiz,*outbuf);	\
	break

int CompreschMain::Main_Compress(u8** outbuf,int flgs,u8* inbuf,int insiz,int offset,int format,int format2)
{
	int outsiz = 0;
	switch(format)
	{
		COMPRESS_NORMAL(FORMAT_KIRBY,		Compresch_Kirby);
		COMPRESS_NORMAL(FORMAT_POKEMON,		Compresch_Pokemon);

	default:
		printf("%s","Error:  Unknown or unsupported format ID\n");
		break;
	}

	return outsiz;
}

#define DECOMPRESS_NORMAL(FMT,CLS)									\
	case FMT:														\
	outsiz = CLS::Decompress(&inbuf[offset],insiz - offset,0);		\
	*outbuf = new u8[outsiz];										\
	outsiz = CLS::Decompress(&inbuf[offset],insiz - offset,*outbuf);\
	break

int CompreschMain::Main_Decompress(u8* outbuf,u8* inbuf,int insiz)
{

	int outsiz = 0;
	
  									
	outsiz = Compresch_Metroid::Decompress(inbuf,insiz,outbuf);
	return outsiz;
}