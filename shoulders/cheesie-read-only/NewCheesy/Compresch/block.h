/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_H__
    
    #define __BLOCK_H__
    
    class Block
    {
        
    public:
        
	    virtual ~Block() { }
	    virtual Block* Dup() = 0;
	    
        virtual int GetMinLength() = 0;
	    virtual int GetBodySize() = 0;
	    virtual int Output(unsigned char* buf) = 0;
	    virtual int Shrink(int newstart, int newstop) = 0;
	    virtual int Decompress(int len, const unsigned char *src, unsigned char *dst, int &dstpos) = 0;
	    
	    virtual int CanShrink(int newstart, int newstop)
	    {
		    Block* blk = Dup();
		    
		    int ret = !blk->Shrink(newstart,newstop);
		    
		    delete blk;
		    
		    return ret;
	    }
	    
	    virtual int HasExtraByte()
	    {
	        return (type == 7) ? 1 : 0;
	    }
    	
	    virtual void DropLen(int newlen)
	    {
	        Shrink(start, start + newlen);
	    }
        
	    unsigned char type;
	    
	    int start;
	    int stop;
	    int len;
        int param1; // param
        int param2;   
        
	    int tradeoff; // only used for fast compression
    };


#endif
