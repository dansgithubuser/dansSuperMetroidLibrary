/*
 *	Compresch - compression library
 *  Copyright 2011 MathOnNapkins
 *  See license.txt for details
 */

    #include "compresch_stdblock.h"
    
    #include "block_lz2.h"
    
#ifndef Compresch_Metroid_H

    #define Compresch_Metroid_H

    class Compresch_Metroid : public Compresch_StdBlock
    {
        protected:
            
            typedef Block_LZ2<LittleEndian, Forward, None, Absolute> LZ_LE_Abs;
            typedef Block_LZ2<LittleEndian, Forward, Xor,  Absolute> LZ_LE_Xor_Abs;
            typedef Block_LZ2<OneByte,      Forward, None, Relative> LZ_Byte_Rel;
            typedef Block_LZ2<OneByte,      Forward, Xor,  Relative> LZ_Byte_Xor_Rel;
            
        public:
           
            static int Compress(const u8 *src, int srcLen, u8 *dst);
            
            static int WorstCompressSize(int srcLen);
            
            static int Decompress(const u8 *src, int srcLen, u8 *dst);
    };

#endif