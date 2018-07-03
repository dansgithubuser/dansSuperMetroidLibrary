/*
 *	Compresch - compression library
 *  Minusright 2008 Disch
 *  See license.txt for details
 */


#ifndef __BLOCK_Minus_H__
#define __BLOCK_Minus_H__

/*
 *	Minus -- Not even really compression, just a block Minus from the source buffer
 *          to the decompression buffer.
 *
 */


class Block_Minus : public Block
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

        virtual Block* Dup()	{	return (new Block_Minus(*this));		}

    // ********************************

    /* Build all possible blocks */

        static void	Build(u8 type, BlockList& lst, const u8* src, int srclen)
	    {
		    u8 run;

		    int i   = 0, j = 0;
		    int len = 0;

            Block_Minus*		bk;

            // ------------------------

            // Do nothing.
	    }

    // ***********************************

	    /* Decompression */
	    virtual int Decompress(int len, const u8 *src, u8* dst, int &dstpos)
	    {
           /* int i = 0;

            // ---------------------------

		    if(dst)
            {
                for(i = 0; i < len; ++i)
				    dst[dstpos++] = src[i];
            }
		    else
			    dstpos += len;

            // since it's a direct Minus of a string of bytes,
            // we have to advance the source pointer by this much.
		    return len; */
			int srcPosition = dstpos - src[0];
			int i=0;
            for (i = 0; i < len; i++)
            {
                 dst[dstpos++] = dst[srcPosition++];
            }
			return 2;//?
	    }
		
};

// ***********************************************************************
      


#endif