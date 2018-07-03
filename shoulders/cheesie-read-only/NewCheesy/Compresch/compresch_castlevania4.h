/*
 *	Compresch - compression library
 *  Copyright 2008 Disch 
 *  Copyright 2009 MathOnNapkins (this file)
 *  See license.txt for details
 */

#include "compresch_konamiblock.h"

class Compresch_CV4 : public Compresch_KonamiBlock
{
public:
	static int			Compress(const u8* src,int srclen,u8* dst);
	static int			WorstCompressSize(int srclen);

	static int			Decompress(const u8* src,int srclen,u8* dst);
};