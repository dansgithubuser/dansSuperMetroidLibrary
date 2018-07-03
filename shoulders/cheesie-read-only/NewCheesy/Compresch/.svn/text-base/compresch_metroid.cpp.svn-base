
    #include <list>
    #include <memory.h>
    
    #include "Compresch_Metroid.h"
    
    #include "block.h"
    #include "blocklist.h"
    
    #include "block_rle.h"
    #include "block_rlealternate.h"
    #include "block_rleplus.h"
    #include "block_lz2.h"
        
// ==============================================

    int Compresch_Metroid::WorstCompressSize(int srclen)
    {
	    return Compresch_StdBlock::WorstCompressSize(srclen);
    }

// ==============================================

    int Compresch_Metroid::Compress(const u8 *src, int srcLen, u8 *dst)
    {
        BlockList blocks;
        
        Block_RLE::Build         (1, blocks, src, srcLen);
        Block_RLEAlternate::Build(2, blocks, src, srcLen);
        Block_RLEPlus::Build     (3, blocks, src, srcLen);
        LZ_LE_Abs::Build         (4, blocks, src, srcLen);
        LZ_LE_Xor_Abs::Build     (5, blocks, src, srcLen);
        LZ_Byte_Rel::Build       (6, blocks, src, srcLen);
        LZ_Byte_Xor_Rel::Build   (7, blocks, src, srcLen);
        
        return Compresch_StdBlock::CrunchAndOutput(blocks, src, srcLen, dst);
    }

// ==============================================

    int Compresch_Metroid::Decompress(const u8 *src, int srcLen, u8 *dst)
    {   
        Block *blocks[7] = {
                                new Block_RLE(),
                                new Block_RLEAlternate(),
                                new Block_RLEPlus(),
                                new LZ_LE_Abs(),
                                new LZ_LE_Xor_Abs(),
                                new LZ_Byte_Rel(),
                                new LZ_Byte_Xor_Rel()
                            };
        
        return Compresch_StdBlock::Decompress(src, srcLen, dst, blocks);
    }

// ==============================================
