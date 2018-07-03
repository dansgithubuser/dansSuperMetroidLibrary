/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef			__SIZELIMITS_H__

    #define			__SIZELIMITS_H__
    
    class SIZELIMITS
    {
        public:

            SIZELIMITS(int blockSize = 0, int extraByte = 0, int lenMax = 0)
            {
                this->blocksize      = blockSize;
                this->extrabyteafter = extraByte;
                this->lenmax         = lenMax;
            }

	        int		blocksize;			// size of the header for the block
	        int		extrabyteafter;		// length after which an extra byte is added to block header
	        int		lenmax;				// maximum length of block
    };

#endif