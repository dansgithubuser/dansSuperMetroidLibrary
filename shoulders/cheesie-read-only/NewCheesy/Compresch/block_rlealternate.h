/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_RLEALTERNATE_H__
#define __BLOCK_RLEALTERNATE_H__

/*
 *	RLE Alternate -- Alternates 2 values.  Similar to RLETwo, only handles length differently
 *      2 1-byte values to alternatively repeat
 *
 *  dat=61 45  len=5:  61 45 61 45 61
 */


    class Block_RLEAlternate : public Block
    {
        
    protected:
        
	    u8 dat[2];
        
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
	        buf[0] = dat[0]; buf[1] = dat[1];
	        
	        return GetBodySize();
	    }
	    
    // ==============================================
	    
	    virtual int Shrink(int newstart,int newstop)
	    {
		    if((newstart ^ start) & 1)
		    {
			    u8 tmp = dat[0];
			    dat[0] = dat[1];
			    dat[1] = tmp;
		    }
		    
		    start = newstart;
		    stop  = newstop;
		    len   = stop - start;
		    
		    return 0;
	    }
	    
    // ==============================================
        
	    virtual Block* Dup()
	    {
	        return new Block_RLEAlternate(*this);
	    }

    // ==============================================
        
	    static void	Build(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
	        Block_RLEAlternate tmp;
	        
	        tmp.BuildBlocks(type, lst, src, srcLen);
	    }
	    
	    void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
		    Block_RLEAlternate*		bk;
            
		    u8 run[2];
            
		    int i = 0;
		    int j;
		    int len;
		    
		    while(i < srcLen)
		    {
			    run[0] = src[i + 0];
			    run[1] = src[i + 1];
			    
			    len = 0;
			    
			    for(j = i + 2; j < srcLen; ++j, len ^= 1)
			    {
				    if(src[j] != run[len])		break;
			    }
                
			    len = j - i;
			    
			    if( len >= GetMinLength() )
			    {
				    bk = new Block_RLEAlternate;
				    
				    bk->type =		type;
				    bk->start =		i;
				    bk->stop =		j;
				    bk->len =		len;
				    bk->dat[0] =	run[0];
				    bk->dat[1] =	run[1];

				    lst.InsertBlock(bk);
			    }

			    i = j - 1;
		    }
	    }
    	
    // ==============================================
        
        virtual int Decompress(int len,const u8* src,u8* dst,int& dstpos)
	    {
		    int i;

            if(dst)
		    {
			    for(i = 0; i < len; ++i)
				    dst[dstpos + i] = src[i & 1];
		    }

		    dstpos += len;

		    return GetBodySize();
	    }
	    
    // ==============================================
        
    };

#endif