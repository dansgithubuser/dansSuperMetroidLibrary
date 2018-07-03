
    #include <list>
    #include <memory.h>
    
    #include "compresch_lttp.h"
    
    #include "block.h"
    #include "blocklist.h"
    
    #include "block_rle.h"
    #include "block_rlealternate.h"
    #include "block_rleplus.h"
    #include "block_lz.h"
        
// ==============================================

    int Compresch_LTTP::WorstCompressSize(int srcLen)
    {
	    return Compresch_StdBlock::WorstCompressSize(srcLen);
    }

// ==============================================

    int Compresch_LTTP::Compress(const u8 *src, int srcLen, u8 *dst, bool littleEndian)
    {
        BlockList blocks;
        
        Block_RLE::Build(		  1, blocks, src, srcLen);
        Block_RLEAlternate::Build(2, blocks, src, srcLen);
        Block_RLEPlus::Build(	  3, blocks, src, srcLen);
        Block_LZ::Build(		  4, blocks, src, srcLen, littleEndian);
        
        return Compresch_StdBlock::CrunchAndOutput(blocks, src, srcLen, dst);
    }

// ==============================================

    int Compresch_LTTP::Decompress(const u8 *src, int srcLen, u8* dst, bool littleEndian)
    {   
        Block *blocks[7] = {
                                new Block_RLE(),
                                new Block_RLEAlternate(),
                                new Block_RLEPlus(),
                                new Block_LZ(littleEndian),
                                new Block_LZ(littleEndian),
                                new Block_LZ(littleEndian),
                                new Block_LZ(littleEndian)
        };
        
        return Compresch_StdBlock::Decompress(src, srcLen, dst, blocks);
    }

// ==============================================
