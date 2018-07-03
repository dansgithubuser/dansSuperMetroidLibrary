/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  Copyright 2009 MathOnNapkins
 *  See license.txt for details
 */

#ifndef __BLOCK_COPY_PADDED_H__
    
    #define __BLOCK_COPY_PADDED_H__

    /*
     *	Copy Two -- Extremely simple type of compression
     *              This is a variant of Block_Copy that alternates
     *              between writing zeroes and the bytes in the source buffer.
     *  Example: src FF FE FD F0 -> 00 FF 00 FE 00 FD 00 F0
     */

    class Block_CopyPadded : public Block
    {
        u8 dat;
        
    public:
        
    // ==============================================
        
        virtual int GetBodySize() { return 1; }
        
    // ==============================================
        
        virtual int GetMinLength()
        {
            return 1;
        }
        
    // ==============================================
        
        virtual int Output(u8* buf) { buf[0] = dat; return 1; }
        
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
            return new Block_CopyPadded(*this);
        }
        
    // ==============================================
        
    /* Build all possible blocks */

        static void Build(u8 type, BlockList &lst, const u8* src, int srcLen)
        {
            int i = 0, j = 0, len  = 0;

            Block_CopyPadded *bk;

            // ------------------------

            // First we go through the even starting points
            for(i = 0; i < srcLen; i += 2)
            {
                // looking for a sequence that starts with zero.
                for(j = i, len = 0; j < srcLen; j += 2, ++len)
                {
                    if(src[j] != 0x00)
                        break;
                }

                // no savings occurs below a length of 3
                if(len >= 3)
                {
                    bk = new Block_CopyPadded;
                    bk->type  =     type;
                    bk->start =     i;
                    bk->stop  =     j;
                    bk->len   =     len;

                    lst.InsertBlock(bk);
                }

                i = j;
            }

            // Now check all the odd starting points
            for(i = 1; i < srcLen; i += 2)
            {
                // looking for a sequence that starts with zero.
                
                for(j = i, len = 0; j < srcLen; j += 2, ++len)
                {
                    if(src[j] != 0x00)
                        break;
                }

                // no savings occurs below a length of 3
                if(len >= 3)
                {
                    bk = new Block_CopyPadded;
                    bk->type  =     type;
                    bk->start =     i;
                    bk->stop  =     j;
                    bk->len   =     len;

                    lst.InsertBlock(bk);
                }

                i = j;
            }
        }
            
    // ***********************************

	    /* Decompression */
	    virtual int Decompress(int len, const u8 *src, u8* dst, int &dstPos)
	    {
            int i = 0;

            // ---------------------------

		    if(dst)
            {
                // Alternate between writing zeroes and the each byte in the source buffer
                for(i = 0; i < len; ++i)
                {
				    dst[dstPos++] = 0x00;
                    dst[dstPos++] = src[i];
                }
            }
		    else
			    dstPos += (len * 2);

            // The data is directly copied from the source buffer so we must advance
            // by the length of the data that's copied.
		    return len; 
	    }
};

// ***********************************************************************

#endif