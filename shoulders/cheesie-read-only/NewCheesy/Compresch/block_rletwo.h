/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_RLETWO_H__
#define __BLOCK_RLETWO_H__

/*
 *	RLETwo -- Two byte alternating RLE.  2-byte pair to repeat 'len' times
 *
 *  dat=13 67  len=4:  13 67 13 67 13 67 13 67
 */


    class Block_RLETwo : public Block
    {
        
    protected:
        
    	u8				dat[2];
        
    public:
        
    // ==============================================
        
    	virtual int GetBodySize() { return 2; }
	    
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
	    
	    virtual int Output(u8* buf) { buf[0] = dat[0]; buf[1] = dat[1]; return 2; }
	    
    // ==============================================
        
	    virtual void DropLen(int newlen) { Shrink(start,start+(newlen*2)); }
	    
    // ==============================================
        
	    virtual int Shrink(int newstart,int newstop)
	    {
		    if((start ^ newstart) & 1)		return 1;
		    if((stop ^ newstop) & 1)		return 1;

		    start = newstart;
		    stop = newstop;
		    len = (stop - start) / 2;
		    return 0;
	    }
	    
    // ==============================================
	    
	    virtual Block* Dup()
	    {
	        return new Block_RLETwo(*this);
	    }

    // ==============================================
        
	    /*
	     *	Build all possible blocks
	     *	  RLE (2 bytes repeated:  13 65 13 65 13 65 13 65)
	     *    minimum length:  2
	     */
	    
	    static void	Build(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
	        Block_RLETwo tmp;
	        
	        tmp.BuildBlocks(type, lst, src, srcLen);
	    }
	    
    // ==============================================
        
	    void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
		    Block_RLETwo*	bk;
		    int				i, j;
		    int				len;
		    u8				run[2];
		    int				mx;

		    // first do even blocks, then go through again and to odd blocks
		    int bigloop;
		    for(bigloop = 0; bigloop < 2; ++bigloop)
		    {
			    mx = srcLen;
                
			    if(bigloop)			// odd set
			    {
				    if(!(mx & 1))
					    --mx;
				    i = 1;
			    }
			    else				// even set
			    {
				    if(mx & 1)
					    --mx;
				    i = 0;
			    }

			    while(i < mx)
			    {
				    run[0] = src[i+0];
				    run[1] = src[i+1];

				    for(j = i+2; j < mx; j += 2)
				    {
					    if(src[j+0] != run[0])		break;
					    if(src[j+1] != run[1])		break;
				    }

				    len = (j - i) / 2;
				    
				    if(len >= GetMinLength() )
				    {
					    bk = new Block_RLETwo;
					    
					    bk->type =		type;
					    bk->start =		i;
					    bk->stop =		j;
					    bk->len =		len;
					    bk->dat[0] =	run[0];
					    bk->dat[1] =	run[1];

					    lst.InsertBlock(bk);
				    }
                    
				    i = j;
			    }
		    }
	    }
    	
    // ==============================================
        
	    /*
	     *	Decompression
	     */
	    virtual int Decompress(int len,const u8* src,u8* dst,int& dstpos)
	    {
		    int i;
		    if(dst)
		    {
			    for(i = 0; i < len; ++i)
			    {
				    dst[dstpos+0] = src[0];
				    dst[dstpos+1] = src[1];
				    dstpos += 2;
			    }
		    }
		    else
			    dstpos += len * 2;

		    return 2;
	    }
	    
    // ==============================================
        
    };

#endif