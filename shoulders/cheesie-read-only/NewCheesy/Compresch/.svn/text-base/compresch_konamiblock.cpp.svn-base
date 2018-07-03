/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

    #include <list>
    #include <memory.h>

    #include "compresch_konamiblock.h"

    #include "block.h"
    #include "blocklist.h"

// ***************************************

    int Compresch_KonamiBlock::WorstCompressSize(int srclen)
    {
	    // worse case compression is that nothing can be compressed

	    int ret = (srclen / 1024) * (1024 + 2);
	    srclen %= 1024;

		if(srclen > 32)	        ret += 2 + srclen;
	    else if(srclen > 0)		ret += 1 + srclen;

	    return ret + 1;
    }

// ***************************************

    int Compresch_KonamiBlock::OutputBlock(Block* blk,u8* dst)
    {
	    if(blk->len > 32)
	    {
		    dst[0] = 0xE0 | (blk->type << 2) | (((blk->len - 1) >> 8) & 3);
		    dst[1] = (blk->len - 1) & 0xFF;
		    return 2 + blk->Output(&dst[2]);
	    }
	    dst[0] = (blk->type << 5) | ((blk->len - 1) & 0x1F);
	    return 1 + blk->Output(&dst[1]);
    }

// ***************************************

    int Compresch_KonamiBlock::OutputRawBlock(const u8* src,u8* dst,int len)
    {
	    int ret = 0;
	    while(len > 1024)
	    {
		    dst[0] = 0xE3;
		    dst[1] = 0xFF;

		    memcpy(&dst[2],src,1024);

		    dst += 1024 + 2;
		    src += 1024;
		    ret += 1024 + 2;
		    len -= 1024;
	    }

	    if(len > 32)
	    {
		    dst[0] = 0xE0 | (((len - 1) >> 8) & 3);
		    dst[1] = (len - 1) & 0xFF;
		    memcpy(&dst[2],src,len);
		    return ret + len + 2;
	    }
	    
	    dst[0] = ((len - 1) & 0x1F);
	    memcpy(&dst[1],src,len);

	    return (ret + len + 1);
    }

// ***************************************

    int Compresch_KonamiBlock::CrunchAndOutput(BlockList& blocks, const u8 *src, int srcLen, u8 *dst)
    {
	    int i = 0;
        int dstPos = 2;
	    int srcPos = 0;

        Block* blk;
        
        // break the blocks down
        SIZELIMITS* sizLimits[8];

        // --------------------------------

        sizLimits[0] = new SIZELIMITS(1, srcLen + 1, 0x101);
        sizLimits[1] = new SIZELIMITS(1, srcLen + 1, 0x1F);
        sizLimits[2] = new SIZELIMITS(1, srcLen + 1, 0x21);
        sizLimits[3] = new SIZELIMITS(1, srcLen + 1, 0x21);
        sizLimits[4] = new SIZELIMITS(1, srcLen + 1, 0x21);

        // initialized to default values but won't end up being used
        sizLimits[5] = new SIZELIMITS(1, srcLen + 1, 0x1F);
        sizLimits[6] = new SIZELIMITS(1, srcLen + 1, 0x1F);
        sizLimits[7] = new SIZELIMITS(1, srcLen + 1, 0x1F);

        // the konami block system varies a bit from the Nintendo block format. Just keep that in mind.
        blocks.CrunchList((const SIZELIMITS*) sizLimits,
                          (const SIZELIMITS*) sizLimits,
                          srcLen);

        while(srcPos < srcLen)
	    {
		    blk = blocks.PopFront();

		    if(!blk)
		    {
			    dstPos += OutputRawBlock(&src[srcPos], &dst[dstPos], srcLen - srcPos);
			    srcPos = srcLen;
		    }
		    else
		    {
			    if(blk->start > srcPos)
				    dstPos += OutputRawBlock(&src[srcPos], &dst[dstPos], blk->start - srcPos);

                dstPos += OutputBlock(blk,&dst[dstPos]);
			    srcPos = blk->stop;

			    delete blk;
		    }
	    }

        dst[0] = (dstPos & 0x00FF);
        dst[1] = (dstPos & 0xFF00) >> 8;

        for(i = 0; i < 8; ++i)
            delete sizLimits[i];

	    return dstPos + 1;
    }

// ***************************************

    int Compresch_KonamiBlock::Decompress(const u8* src, int srcLen, u8* dst, Block** blocks)
    {
	    u8 op;

	    int srcPos = 0;
	    int dstPos = 0;
	    int len;

        // -------------------------

        srcLen = (short) (src[0] | (src[1] << 8));
        srcPos += 2;

	    while(srcPos < srcLen)
	    {
		    op = src[srcPos++];
            
            if( op < 0x80 )
            {
                // handle LZ
                len = ((op >> 2) & 0xFF) + 0x02;
                op  = 0;
            }
            else if(op < 0xA0)
            {
                // block copy
                len = (op & 0x1F);
                op  = 1;
            }
            else if(op < 0xC0)
            {
                // alternating block copy
                len = (op & 0x1F) + 2;
                op  = 2;
            }
            else if(op < 0xE0) 
            {
                // RLE (normal)
                len = (op & 0x1F) + 2;
                op  = 3;
            }
            else
            {
                // RLE (zeroes)
                len = (op & 0x1F) + 2;
                op  = 4;
            }

            if(blocks[op])
                srcPos += blocks[op]->Decompress(len, &src[srcPos], dst, dstPos);
	    }

	    for(len = 0; len < 6; ++len)
		    if(blocks[len])
                delete blocks[len];

        return dstPos;
    }

// ***********************************************************
