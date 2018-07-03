/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_HYBRIDLZBITREV_H__
#define __BLOCK_HYBRIDLZBITREV_H__

#include "block_lzbitrev.h"

/*
 *	LZ -- Bit reversed -- hybrid data
 *
 *   see LZBitRev for bit reversed explanation
 *   see HybridLZ for hybird explanation
 */


    class Block_HybridLZBitRev : public Block_LZBitRev
    {
        
    public:
        
    // ==============================================
        
	    virtual int GetBodySize()
	    {
		    if((start - dat) > 0x80)		return 2;
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
		    if((start - dat) > 0x80)
		    {
			    buf[0] = (u8)(dat >> 8);
			    buf[1] = (u8)dat;
			    return 2;
		    }
		    
		    buf[0] = (u8)(start - dat - 1) | 0x80;
		    
		    return 1;
	    }
        
    // ==============================================
        
	    virtual int Shrink(int newstart,int newstop)
	    {
		    int newdat = (dat + newstart - start);
		    if((newstart - newdat) > 0)
		    {
			    if(newdat > 0x7FFF)
				    return 1;
		    }

		    dat = newdat;
		    start = newstart;
		    stop = newstop;
		    len = stop - start;
		    return 0;
	    }
	    
    // ==============================================
        
	    virtual Block* Dup()
	    {
	        return new Block_HybridLZBitRev(*this);
	    }
        
    // ==============================================
        
	    static void Build(u8 type,BlockList& lst,const u8* src,int srclen)
	    {
		    Block_HybridLZBitRev tmp;
		    
		    tmp.BuildBlocks(type,lst,src,srclen,2,srclen);
	    }
    	
    // ==============================================
        
        virtual int Decompress(int len,const u8* src,u8* dst,int& dstpos)
        {
            int from;
            
            if(src[0] & 0x80)
            {
                from = dstpos - (src[0] & 0x7F) - 1;
                DoDecompress(from,len,dst,dstpos);
                
                return 1;
            }
            
            from = (src[0] << 8) | src[1];
            DoDecompress(from,len,dst,dstpos);
            
            return 2;
	    }
        
    // ==============================================
        
    protected:
        
	    virtual Block_LZBitRev* InnerDup()	{	return (new Block_HybridLZBitRev(*this));		}
	    
    // ==============================================
        
    };

#endif