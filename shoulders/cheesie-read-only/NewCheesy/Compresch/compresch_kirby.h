/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#include "compresch_stdblock.h"

class Compresch_Kirby : public Compresch_StdBlock
{
public:
	static int			Compress(const u8* src,int srclen,u8* dst);
	static int			WorstCompressSize(int srclen);

	static int			Decompress(const u8* src,int srclen,u8* dst);
};