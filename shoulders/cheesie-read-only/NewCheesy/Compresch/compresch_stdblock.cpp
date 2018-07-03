/*
 *    Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

    #include <list>
    #include <memory.h>
    
    #include "compresch_stdblock.h"
    
    #include "block.h"
    #include "blocklist.h"

// =================================================

    int Compresch_StdBlock::WorstCompressSize(int srclen)
    {
        // worse case compression is that nothing can be compressed
        
        int ret = (srclen / 1024) * (1024 + 2);
        
        // ------------------------------------
        
        srclen %= 1024;
        
        if(srclen > 32)
        {
            ret += 2 + srclen;
        }
        else if(srclen > 0)
        {
            ret += 1 + srclen;
        }
        
        return ret + 1;
    }

// =================================================

    int Compresch_StdBlock::OutputBlock(Block *blk, u8 *dst)
    {
        if(blk->len > 32)
        {
            dst[0] = 0xE0 | (blk->type << 2) | (((blk->len - 1) >> 8) & 3);
            dst[1] = (blk->len - 1) & 0xFF;
            
            return 2 + blk->Output(&dst[2]);
        }
        else if(blk->type == 7)
        {
            dst[0] = 0xE0 | (blk->type << 2) | (((blk->len - 1) >> 8) & 3);
            dst[1] = (blk->len - 1) & 0xFF;
            
            return 2 + blk->Output(&dst[2]);
        }
        
        dst[0] = (blk->type << 5) | ((blk->len - 1) & 0x1F);
        
        return 1 + blk->Output(&dst[1]);
    }

// =================================================

    int Compresch_StdBlock::OutputRawBlock(const u8 *src, u8 *dst, int len)
    {
        int ret = 0;
        
        // ------------------------------
        
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
        
        return ret + len + 1;
    }

// =================================================

    int Compresch_StdBlock::CrunchAndOutput(BlockList &blocks, const u8 *src, int srclen, u8 *dst)
    {
        int i      = 0;
        int dstpos = 0;
        int srcpos = 0;
        
        Block* blk;
        
        // break the blocks down
        
        SIZELIMITS sizeLimits[8];
        
        // ---------------------------------------
        
        // output the blocks
        for(i = 0; i < 8; ++i)
        {
            sizeLimits[i].blocksize      = 1;    // 1 byte header
            sizeLimits[i].extrabyteafter = 32;   // use a 2 byte header if the block length exceeds 32
            sizeLimits[i].lenmax         = 1024; // max length of 1024 (even when using a 2 byte header)
        }
        
        blocks.CrunchList(sizeLimits, sizeLimits, srclen); // 1 byte block header -- 2 bytes after len of 32 -- max len of 1024
        
        while(srcpos < srclen)
        {
            blk = blocks.PopFront();
            
            if(!blk)                    // if there are no more blocks in the list
            {                            //  output the rest of the data as straight copy (not compressed; "raw")
                dstpos += OutputRawBlock(&src[srcpos], &dst[dstpos], srclen - srcpos);
                srcpos  = srclen;
            }
            else
            {
                // otherwise, see if the next block starts further ahead than we are currently at
                // if the block starts up a ways, we need to output raw data up until the start of the next block
                if(blk->start > srcpos)
                {
                    dstpos += OutputRawBlock(&src[srcpos], &dst[dstpos], blk->start - srcpos);
                }
                
                // then output the compressed block
                dstpos += OutputBlock(blk, &dst[dstpos]);
                
                // set source position to the end of the block
                srcpos = blk->stop;
                
                // and delete the block (must be done because BlockList no longer owns it after being popped)
                delete blk;
            }
        }
        
        // this is just a termination byte used by the nintendo format
        dst[dstpos] = 0xFF;
        
        return dstpos + 1;
    }

// =================================================

    int Compresch_StdBlock::Decompress(const u8 *src, int srclen, u8 *dst, Block **blocks)
    {
        u8 op;
        int len;
        int dstpos = 0;
        int srcpos = 0;
        while(srcpos < srclen){
            op = src[srcpos];
            ++srcpos;
            if(op == 0xFF) break;
            if((op & 0xE0) == 0xE0){
                len = ((op & 3) << 8) | src[srcpos];
                op = (op >> 2) & 7;
                ++srcpos;
            }
            else{
                len = (op & 0x1F);
                op >>= 5;
            }
            ++len;
            if(op){
                if(blocks[op - 1]) srcpos += blocks[op - 1]->Decompress(len, &src[srcpos], dst, dstpos);
            }
            else{
                if(dst) memcpy(&dst[dstpos], &src[srcpos], len);
                dstpos += len;
                srcpos += len;
            }
        }
        for(len = 0; len < 7; ++len)
            if(blocks[len])    
                delete blocks[len];
        return dstpos;
    }

// =================================================
