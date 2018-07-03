/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

#include <list>
#include "compresch_types.h"

#include "block.h"
#include "blocklist.h"

#include "block_lzbitrev.h"

int Block_LZBitRev::BITREV_BUILT = 0;
u8 Block_LZBitRev::BITREV[0x100];


void Block_LZBitRev::BuildBitRev()
{
	if(BITREV_BUILT)		return;
	BITREV_BUILT = 1;

	int i;
	for(i = 0; i < 0x100; ++i)
	{
		BITREV[i] = (u8)(
				((i >> 7) & 0x01) | ((i >> 5) & 0x02) | ((i >> 3) & 0x04) | ((i >> 1) & 0x08) |
				((i << 1) & 0x10) | ((i << 3) & 0x20) | ((i << 5) & 0x40) | ((i << 7) & 0x80) );
	}
}