/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
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
    protected:
	    u8				dat;

    public:

    // ********************************

        virtual int GetBodySize() { return 1; }

    // ********************************

        virtual int Output(u8* buf) { buf[0] = dat; return 1; }
    
    // ********************************

        virtual int Shrink(int newstart,int newstop)
	    {
		    start = newstart;
		    stop = newstop;
		    len = stop - start;
		    return 0;
	    }
    
    // ********************************

        virtual Block* Dup()	{	return (new Block_CopyPadded(*this));		}

    // ********************************

    /* Build all possible blocks */

        static void	Build(u8 type, BlockList& lst, const u8* src, int srclen)
	    {
		    u8 run;

		    int i   = 0, j = 0;
		    int len = 0;

            Block_CopyPadded *bk;

            // ------------------------

            while(i < srclen)
		    {
			    run = src[i];

                for(j = i + 1; j < srclen; ++j)
				    if(src[j] != run)
                        break;
			    
			    len = j - i;

                if(len >= 2)
			    {
				    bk = new Block_CopyPadded;
				    bk->type =		type;
				    bk->start =		i;
				    bk->stop =		j;
				    bk->len =		len;
				    bk->dat =		run;

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
                for(i = 0, i < len; ++i)
                {
				    dst[dstPos++] = 0x00;
                    dst[dstPos++] = src[i]
                }
            }
		    else
			    dstpos += (len * 2);

            // The data is directly copied from the source buffer so we must advance
            // by the length of the data that's copied.
		    return len; 
	    }
};

// ***********************************************************************
      


#endif