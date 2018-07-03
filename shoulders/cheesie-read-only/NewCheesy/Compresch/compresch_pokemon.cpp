    /*
     *	Compresch - compression library
     *  Copyright 2008 Disch
     *  See license.txt for details
     */
    
    #include <list>
    #include <memory.h>
    #include "compresch_pokemon.h"
    
    #include "block.h"
    #include "blocklist.h"
    
    #include "block_rle.h"
    #include "block_rlealternate.h"
    #include "block_zero.h"
    #include "block_hybridlz.h"
    #include "block_hybridlzbitrev.h"
    #include "block_hybridlzrev.h"

    int Compresch_Pokemon::WorstCompressSize(int srcLen)
    {
        return Compresch_StdBlock::WorstCompressSize(srcLen);
    }

    int Compresch_Pokemon::Compress(const u8 *src, int srcLen, u8 *dst)
    {
        BlockList blocks;
        
        Block_RLE::Build           (1, blocks, src, srcLen);
        Block_RLEAlternate::Build  (2, blocks, src, srcLen);
        Block_Zero::Build          (3, blocks, src, srcLen);
        Block_HybridLZ::Build      (4, blocks, src, srcLen);
        Block_HybridLZBitRev::Build(5, blocks, src, srcLen);
        Block_HybridLZRev::Build   (6, blocks, src, srcLen);
        
        return Compresch_StdBlock::CrunchAndOutput(blocks, src, srcLen, dst);
    }

    int Compresch_Pokemon::Decompress(const u8 *src, int srcLen, u8 *dst)
    {
        Block* blocks[7] = {
                                new Block_RLE(),
    						    new Block_RLEAlternate(),
    						    new Block_Zero(),
    						    new Block_HybridLZ(),
    						    new Block_HybridLZBitRev(),
    						    new Block_HybridLZRev(),
    						    new Block_HybridLZRev() // Added this ~MoN
					       };
        
        return Compresch_StdBlock::Decompress(src, srcLen, dst, blocks);
    }