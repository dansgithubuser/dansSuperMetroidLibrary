/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_RLE_H__
#define __BLOCK_RLE_H__

/*
 *	RLE -- Basic RLE.  1-byte value to repeat
 *
 *  dat=61  len=5:  61 61 61 61 61
 */


    class Block_RLE : public Block
    {   
        
    // ==============================================
        
    protected:
        
        u8 dat;
        
    public:
        
    // ==============================================
        
	virtual int GetBodySize() { return 1; }
	    
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
    	virtual int Output(u8* buf)
    	{
    	    buf[0] = dat;
    	    
    	    return 1;
    	}
        
    // ==============================================
        
        virtual int Shrink(int newstart,int newstop)
	    {
		    start = newstart;
		    stop = newstop;
		    len = stop - start;
		    return 0;
	    }
	    
    // ==============================================
        
	    virtual Block* Dup()
	    {
	        return new Block_RLE(*this);
	    }
	    
    // ==============================================
        
	/*
	 *	Build all possible blocks
	 *	  RLE (1 byte repeated:  13 13 13 13 13 13)
	 *    minimum length:  2
	 */
        
        static void	Build(u8 type, BlockList &lst, const u8* src, int srcLen)
	    {
	        Block_RLE tmp;
	        
	        tmp.BuildBlocks(type, lst, src, srcLen);
	    }
	    
	    void BuildBlocks(u8 type, BlockList &lst, const u8 *src, int srcLen){
		    u8 run = 0;
		    int i = 0, j = 0;
		    int len = 0;
        Block_RLE* bk;
        while(i < srcLen){
			    run = src[i];
					for(j = i + 1; j < srcLen; ++j)
				    if(src[j] != run)
							break;
			    len = j - i;
          // RLE isn't worth it unless the run length is 2 or more
          if(len >= GetMinLength() ){
				    bk = new Block_RLE;
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
        
	    /* Decompression */
        virtual int Decompress(int len, const u8 *src, u8 *dst,int &dstpos)
        {
            if(dst)
            {
                while(len > 0)
                {
                    dst[dstpos++] = src[0];
                    --len;
                }
            }
            else
                dstpos += len;
            
            return 1;
        }
        
    // ==============================================
        
    };

#endif