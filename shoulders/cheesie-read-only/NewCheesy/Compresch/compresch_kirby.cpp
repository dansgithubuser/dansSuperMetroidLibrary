/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

    #include <list>
    #include <memory.h>
    #include "compresch_kirby.h"
    
    #include "block.h"
    #include "blocklist.h"
    
    #include "block_rle.h"
    #include "block_rletwo.h"
    #include "block_rleplus.h"
    #include "block_lz.h"
    #include "block_lzbitrev.h"
    #include "block_lzrev.h"

    int Compresch_Kirby::WorstCompressSize(int srcLen)
    {
        return Compresch_StdBlock::WorstCompressSize(srcLen);
    }

    int Compresch_Kirby::Compress(const u8 *src, int srcLen, u8 *dst)
    {
        BlockList blocks;
        
        Block_RLE::Build     (1, blocks, src, srcLen);
        Block_RLETwo::Build  (2, blocks, src, srcLen);
        Block_RLEPlus::Build (3, blocks, src, srcLen);
        Block_LZ::Build      (4, blocks, src, srcLen);
        Block_LZBitRev::Build(5, blocks, src, srcLen);
        Block_LZRev::Build   (6, blocks, src, srcLen);
        
        return Compresch_StdBlock::CrunchAndOutput(blocks, src, srcLen, dst);
    }

    int Compresch_Kirby::Decompress(const u8 *src, int srcLen, u8 *dst)
    {
	    Block *blocks[7] = {
                                new Block_RLE(),
							    new Block_RLETwo(),
							    new Block_RLEPlus(),
							    new Block_LZ(),
                                new Block_LZBitRev(),
                                new Block_LZRev(),
                                new Block_LZRev()
						    };

        return Compresch_StdBlock::Decompress(src, srcLen, dst, blocks);
    }
