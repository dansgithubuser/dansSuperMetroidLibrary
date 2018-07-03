/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_RLEPLUS_H__

    #define __BLOCK_RLEPLUS_H__
    
    /*
     *	RLEPlus -- Basic RLE, incrementing after each byte.  1-byte value to repeat
     *
     *  dat=61  len=5:  61 62 63 64 65
     */


    class Block_RLEPlus : public Block
    {
        
    // ==============================================
        
    protected:
        
    	u8 dat;
        
    // ==============================================
        
    public:
        
    	virtual int GetBodySize()
    	{
    	    return 1;
    	}
	    
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
	    virtual int Output(u8* buf)
	    {
	        buf[0] = dat;
	        
	        return GetBodySize();
	    }
	    
    // ==============================================
        
	    virtual int Shrink(int newstart, int newstop)
	    {
		    dat   = (u8)(dat + newstart - start);
		    start = newstart;
		    stop  = newstop;
		    len   = stop - start;
		    
		    return 0;
	    }
	    
    // ==============================================
	    
	    virtual Block* Dup()
	    {
	        return new Block_RLEPlus(*this);
	    }
        
    // ==============================================
        
	    /*
	     *	Build all possible blocks
	     *	  RLE (1 byte incrementing:  13 14 15 16 17 18)
	     *    minimum length:  2
	     */
	    
	    static void	Build(u8 type,BlockList& lst,const u8* src,int srcLen)
	    {
	        Block_RLEPlus tmp;
	        
	        tmp.BuildBlocks(type, lst, src, srcLen);
	    }
	    
    // ==============================================
        
	    void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srcLen)
	    {
		    Block_RLEPlus* bk;

		    u8 run;
		    u8 currun;
		    
		    int	i = 0;
		    int j;
		    int	len;

		    while(i < srcLen)
		    {
			    run    = src[i];
			    currun = run + 1;
    			
			    for(j = i + 1; j < srcLen; ++j)
			    {
				    if(src[j] != currun)	break;
				    ++currun;
			    }
                
			    len = j - i;
    			
			    if( len >= GetMinLength() )
			    {
				    bk = new Block_RLEPlus;
    				
                    bk->type  = type;
				    bk->start = i;
				    bk->stop  = j;
				    bk->len   = len;
				    bk->dat   = run;
                    
                    lst.InsertBlock(bk);
                }
                
                i = j;
            }
        }
        
    // ==============================================
        
	    virtual int Decompress(int len,const u8* src,u8* dst,int& dstpos)
	    {
		    int i;
		    
		    u8 val;
		    if(dst)
		    {
			    val = src[0];
			    for(i = 0; i < len; ++i, ++val)
				    dst[dstpos+i] = val;
		    }

		    dstpos += len;
            
		    return 1;
	    }
        
    // ==============================================
        
    };

#endif