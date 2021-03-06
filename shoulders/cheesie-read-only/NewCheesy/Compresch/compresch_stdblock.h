/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __COMPRESCH_STDBLOCK_H__
#define __COMPRESCH_STDBLOCK_H__

#include "compresch_types.h"
#define u8 unsigned char
class Compresch_StdBlock
{
protected:
	static int			WorstCompressSize(int srclen);

	static int			CrunchAndOutput(BlockList& blocks,const u8* src,int srclen,u8* dst);

	static int			OutputRawBlock(const u8* src,u8* dst,int len);
	static int			OutputBlock(Block* blk,u8* dst);

	
	static int			Decompress(const u8* src,int srclen,u8* dst,Block** blocks);
};

#endif