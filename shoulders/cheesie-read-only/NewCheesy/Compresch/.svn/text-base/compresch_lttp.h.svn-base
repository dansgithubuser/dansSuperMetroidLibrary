/*
 *	Compresch - compression library
 *  Copyright 2011 MathOnNapkins
 *  See license.txt for details
 */

    #include "compresch_stdblock.h"
    
#ifndef COMPRESCH_LTTP_H

    #define COMPRESCH_LTTP_H

    class Compresch_LTTP : public Compresch_StdBlock
    {
        public:
           
            static int  Compress(const u8* src,int srclen,u8* dst, bool littleEndian = false);
            static int  WorstCompressSize(int srclen);
            
            int         Decompress(const u8* src,int srclen,u8* dst, bool littleEndian = false);
    };

#endif