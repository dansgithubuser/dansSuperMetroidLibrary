#include <list>
#include <memory.h>

#include "compresch_castlevania4.h"

#include "block.h"
#include "blocklist.h"

#include "block_lz.h"
#include "block_copy.h"
#include "block_copypadded.h"
#include "block_rle.h"
#include "block_zero.h"

int Compresch_CV4::WorstCompressSize(int srclen)
{
    return Compresch_KonamiBlock::WorstCompressSize(srclen);
}

int Compresch_CV4::Compress(const u8* src, int srclen, u8* dst)
{
	BlockList		blocks;

    Block_LZ_CV4::Build    (0, blocks, src, srclen);
	Block_Copy::Build      (1 ,blocks, src, srclen);
    Block_CopyPadded::Build(2, blocks, src, srclen);
	Block_RLE::Build       (3 ,blocks, src, srclen);
	Block_Zero::Build      (4 ,blocks, src, srclen);

	return Compresch_KonamiBlock::CrunchAndOutput(blocks,src,srclen,dst);
}

int Compresch_CV4::Decompress(const u8* src,int srclen,u8* dst)
{
	Block* blocks[6] = {
                            new Block_LZ_CV4,
							new Block_Copy,
							new Block_CopyPadded,
							new Block_RLE,
                            new Block_Zero,
                            NULL
						};


	return Compresch_KonamiBlock::Decompress(src,srclen,dst,blocks);
}