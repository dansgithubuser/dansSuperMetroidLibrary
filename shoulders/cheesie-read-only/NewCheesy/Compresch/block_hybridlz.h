/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_HYBRIDLZ_H__
#define __BLOCK_HYBRIDLZ_H__

#include "block_lz.h"

/*
 *	LZ -- Basic LZ copy.  Hybrid data
 *
 *   if high bit of 1st data byte is clear, normal 2 byte LZ block (Block_LZ)
 *   if high bit of 1st data byte is set, 1 byte LZ block, byte is not an absolute
 *  address, but rather, is a relative address from the starting point.  ie:  85 would
 *  start 5 bytes before dest
 *
 *  if @ 0816:  16 18 EA 6F 3F 67 11 09
 *  and if current output at 081E
 *
 *  dat=0818  len=4:   EA 6F 3F 67
 *  dat=82    len=2:   67 11         (2 bytes back from last byte output)
 */

    class Block_HybridLZ : public Block_LZ
    {
        
    // ==============================================
        
    public:
        
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
        
	    virtual Block* Dup()	{	return (new Block_HybridLZ(*this));		}
        
	    static void Build(u8 type,BlockList& lst,const u8* src,int srclen)
	    {
		    Block_HybridLZ tmp;
		    tmp.BuildBlocks(type,lst,src,srclen,2,srclen);
	    }
        
     // =================================================
        
        virtual int Decompress(int len,const u8 *src, u8* dst, int &dstpos)
	    {
            int from;
            
            if(src[0] & 0x80)
            {
                // Relative position
                from = dstpos - (src[0] & 0x7F) - 1;
                DoDecompress(from,len,dst,dstpos);
                
                return 1;
            }
            
            // Absolute position
            from = (src[0] << 8) | src[1];
            DoDecompress(from,len,dst,dstpos);
            
            return 2;
        }
        
    // ==============================================
        
    protected:
        
	    virtual Block_LZ* InnerDup()	{	return (new Block_HybridLZ(*this));		}
    	
    // ==============================================
        
    };

#endif