/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_ZERO_H__
#define __BLOCK_ZERO_H__

/*
 *	Zero -- RLE of byte 0.  No data needed
 *
 *  len=5:  00 00 00 00 00
 */

    class Block_Zero : public Block
    {
        
    public:
        
	    virtual int GetBodySize()
	    {
	        return 0;
	    }
	    
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
    	virtual int Output(u8* buf)
    	{
    	    return GetBodySize();
    	}
	    
    // ==============================================
        
	    virtual int Shrink(int newstart,int newstop)
	    {
		    start = newstart;
		    stop  = newstop;
		    len   = stop - start;
		    
		    return 0;
	    }
        
    // ==============================================
        
        // duplicate
	    virtual Block* Dup()
	    {
	        return new Block_Zero(*this);
	    }
        
    // ==============================================
        
	    static void	Build(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
	        Block_Zero tmp;
	        
	        tmp.BuildBlocks(type, lst, src, srcLen);
	    }
	    
    // ==============================================
        
	    void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
		    Block_Zero* bk;
            
		    int i = 0;
		    int j;
		    int len;
            
		    while(i < srcLen)
		    {
			    for(j = i; j < srcLen; ++j)
			    {
				    if(src[j])		break;
			    }
                
			    len = j - i;
			    
			    if( len >= GetMinLength() )
			    {
				    bk = new Block_Zero;
				    
				    bk->type =		type;
				    bk->start =		i;
				    bk->stop =		j;
				    bk->len =		len;

				    lst.InsertBlock(bk);
			    }
                
			    i = j + 1;
		    }
	    }
    	
    // ==============================================
        
        virtual int Decompress(int len, const u8 *src, u8 *dst, int &dstpos)
	    {
		    if(dst)
		    {
			    while(len > 0)
			    {
				    dst[dstpos++] = 0;
				    --len;
			    }
		    }
		    else
		    {
			    dstpos += len;
			}

		    return GetBodySize();
	    }
	    
    // ==============================================
        
    };

#endif