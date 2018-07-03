/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_LZREV_H__
#define __BLOCK_LZREV_H__

/*
 *	LZ -- Basic LZ copy, in reverse order.  2-byte absolute address to copy from
 *    bytes are copied backwards.
 *
 *  if @ 0816:  16 18 EA 6F 3F 67 11 09
 *
 *  dat=081B  len=4:   67 3F 6F EA
 */

    class Block_LZRev : public Block
    {
        
    // ==============================================
        
    protected:
        
        int dat;
        
    // ==============================================
        
    public:
        
        virtual int GetBodySize() { return 2; }
        
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
        virtual int Output(u8* buf) { buf[0] = (u8)(dat >> 8); buf[1] = (u8)dat; return 2; }
        virtual int Shrink(int newstart,int newstop)
        {
            int newdat = (dat - newstart + start);
            if(newdat > 0xFFFF)			return 1;
            if(newdat < 0)				return 1;
            
            dat   = newdat;
            start = newstart;
            stop  = newstop;
            len   = stop - start;
            
            return 0;
        }
        
    // ==============================================
        
        virtual Block* Dup()
        {
            return new Block_LZRev(*this);
        }
        
    // ==============================================
        
        static void	Build(u8 type,BlockList& lst,const u8* src,int srclen)
        {
            Block_LZRev tmp;
            tmp.BuildBlocks(type,lst,src,srclen,3,0xFFFF);
        }
        
    // ==============================================
        
        virtual int Decompress(int len,const u8* src,u8* dst,int& dstpos)
        {
            int from = (src[0] << 8) | src[1];
            DoDecompress(from,len,dst,dstpos);
            return 2;
        }
        
    // ==============================================
        
    protected:
        
        virtual Block_LZRev* InnerDup()	{	return (new Block_LZRev(*this));		}
        
        void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srclen, int minsize, int mxstrt)
        {
            Block_LZRev *bk;
            
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
                
                for(j = i - 1; j >= 0; --j)
                {
                    if(j > mxstrt)
                        j = mxstrt;
                    
                    for
                    (
                        len = 0;
                        (len + i < srclen) && (j - len >= 0) && (src[j - len] == src[len + i]);
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
                            (i - adj >= 0) && (j + adj < i - adj) && (j + adj < 0x10000) && (src[i - adj] == src[j + adj]);
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
                        beststrt = j + adj;
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
            if( (from - len) < -1)
            {
                return;
            }
            
            if(from >= dstPos)
            {
                return;
            }
            
            if(dst)
            {
                for(int i = 0; i < len; ++i)
                {
                    dst[dstPos + i] = dst[from - i];
                }
            }
            
            dstPos += len;
        }
        
    // ==============================================
        
    };

#endif