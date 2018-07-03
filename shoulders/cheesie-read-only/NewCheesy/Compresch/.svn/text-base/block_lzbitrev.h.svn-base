/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_LZBITREV_H__
    #define __BLOCK_LZBITREV_H__
#define u8 unsigned char
    /*
     *	LZ -- Basic LZ copy, with bits reversed.  2-byte absolute address to copy from
     *
     *  if @ 0816:  16 18 EA 6F 3F 67 11 09
     *
     *  dat=0818  len=4:   57 F6 FC E6     (which is 'EA 6F 3F 67' bit reversed)
     */
    
    class Block_LZBitRev : public Block
    {
    protected:
        int				dat;
        
        static void		BuildBitRev();
        static int		BITREV_BUILT;
        static unsigned char		BITREV[0x100];
        
        
    public:
        
        virtual int GetBodySize() { return 2; }
        
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
        virtual int Output(u8* buf)
        {
            buf[0] = (u8) (dat >> 8); buf[1] = (u8)dat;
            
            return 2;
        }
        
    // ==============================================
        
        virtual int Shrink(int newstart, int newstop)
        {
            int newdat = (dat + newstart - start);
            
            if(newdat > 0xFFFF)		return 1;
            
            dat   = newdat;
            start = newstart;
            stop  = newstop;
            len   = stop - start;
            
            return 0;
        }
        
    // ==============================================
        
        virtual Block* Dup()
        {
            return new Block_LZBitRev(*this);
        }
        
    // ==============================================
        
        static void Build(u8 type, BlockList &lst, const u8 *src, int srclen)
        {
            Block_LZBitRev tmp;
            
            tmp.BuildBlocks(type, lst, src, srclen, 3, 0xFFFF);
        }
        
    // ==============================================
        
        virtual int Decompress(int len, const u8 *src, u8 *dst, int &dstpos)
        {
            int from = (src[0] << 8) | src[1];
            
            DoDecompress(from, len, dst, dstpos);
            
            return 2;
        }
        
    // ==============================================
        
    protected:
        
        virtual Block_LZBitRev* InnerDup()
        {
            return new Block_LZBitRev(*this);
        }
        
        void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srclen, int minsize, int mxstrt)
        {
            BuildBitRev();
            
            Block_LZBitRev* bk;
            
            int i = 1;
            int j;
            int len;
            
            int bestlen;
            int beststrt;
            
            int justblocked = 0;
            int adj;
            int bestadj;
            
            while(i < srclen)
            {
                bestadj = 0;
                bestlen = minsize - 1;
                
                for(j = 0; j < i; ++j)
                {
                    if(j > mxstrt)
                        break;
                    
                    for
                    (
                        len = 0;
                        (len + i < srclen) && (BITREV[src[len + j]] == src[len + i]);
                        ++len
                    )
                    {
                        continue;
                    }
                    
                    adj = 0;
                    
                    if(justblocked && len)
                    {
                        for
                        (
                            ;
                            (j - adj >= 0) && (src[i - adj] == BITREV[src[j - adj]]);
                            ++adj
                        )
                        {
                            continue;
                        }
                        
                        --adj;
                        
                        len += adj;
                        
                        if(len < minsize)
                            continue;
                    }
                    
                    if( ( (adj == bestadj) && (len > bestlen) ) || (adj > bestadj) )
                    {
                        bestlen  = len;
                        beststrt = j - adj;
                        bestadj  = adj;
                    }
                }
                
                if(bestlen >= minsize)
                {
                    bk        = InnerDup();
                    bk->type  = type;
                    bk->start = i - bestadj;
                    bk->stop  = i - bestadj + bestlen;
                    bk->len   = bestlen;
                    bk->dat   = beststrt;
                    
                    if(bk->Shrink(bk->start, bk->stop))
                    {
                        delete bk;
                        ++i;
                        justblocked = 0;
                    }
                    else
                    {
                        lst.InsertBlock(bk);
                        i = bk->stop;
                        justblocked = 1;
                    }
                }
                else
                {
                    ++i;
                    justblocked = 0;
                }
            }
        }
        
    // ==============================================
        
        static void DoDecompress(int from, int len, u8 *dst, int &dstPos)
        {
            if(from < 0)
            {
                return;
            }
            
            if(from >= dstPos)
            {
                return;
            }
            
            if(dst)
            {
                BuildBitRev();
                
                for(int i = 0; i < len; ++i)
                {
                    dst[dstPos + i] = BITREV[ dst[from + i] ];
                }
            }
            
            dstPos += len;
        }
    };

#endif