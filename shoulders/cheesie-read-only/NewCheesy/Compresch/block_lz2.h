/*
Dan's SM short version:

class Block_LZ2 : public Block{
	public:
		virtual int GetBodySize(LittleEndianType t){ return 2; }
		virtual int GetBodySize(OneByteType t){ return 1; }
		virtual int GetBodySize(){ return GetBodySize(addrType()); }
		
		virtual int Decompress(int len, const u8 *src, u8 *dst, int &dstPos){
			int from = GetOffset(src, dstPos);
			DoDecompress(from, len, dst, dstPos);
			return GetBodySize();
		}
		
		static void DoDecompress(int from, int len, u8 *dst, int &dstPos){
			if(from < 0) return;
			if(from >= dstPos)
					// Fill with dummy data for debugging purposes.
					for(int i = 0; i < len; ++i) dst[dstPos + i] = 0xCC;
			else if(dst)
				for(int i = 0; i < len; ++i) dst[dstPos + i] = Operation(dst[from + i]);
			dstPos += len;
		}
		
		int GetAddr(const u8 *src, LittleEndianType t){ return ( (src[0]) | (src[1] << 8) ); }
		int GetAddr(const u8 *src, OneByteType t){ return src[0]; }		
		int GetAddr(const u8 *src){ return GetAddr( src, addrType() ); }

		int GetOffset(const u8 *src, int &destPos, AbsoluteType t){ return GetAddr(src); }
		int GetOffset(const u8 *src, int &destPos, RelativeType t){ return destPos - GetAddr(src); }
		int GetOffset(const u8 *src, int &destPos){ return GetOffset(src, destPos, offsetType() ); }        
		
		static u8 Operation(u8 input, NoneType t){ return input; }
		static u8 Operation(u8 input, XorType t){ return (input ^ 0xFF); }		
		static u8 Operation(u8 input){ return Operation(input, operationType()); }
};

*/





/*
 *	Compresch - compression library
 *  Copyright 2011 MathOnNapkins
 *  See license.txt for details
 */

    #include "block.h"

#ifndef __BLOCK_LZ2_H__
    
    #define __BLOCK_LZ2_H__
    
 // =================================================
    
    extern const u8 reverseBits[0x100];
    
    enum
    {
        LittleEndian,
        BigEndian,
        OneByte
    };
    
    template<int a>
    struct AddrType
    {
        static const int value = a;
    };
    
    typedef AddrType<LittleEndian> LittleEndianType;
    typedef AddrType<BigEndian> BigEndianType;
    typedef AddrType<OneByte> OneByteType;
    
 // =================================================
    
    enum
    {
        Absolute,
        Relative
    };
    
    template<int a>
    struct OffsetType
    {
        static const int value = a;
    };
    
    typedef OffsetType<Absolute> AbsoluteType;
    typedef OffsetType<Relative> RelativeType;
    
 // =================================================
    
    enum
    {
        Forward,
        Backward
    };
    
    template<int a>
    struct Direction
    {
        static const int value = a;
    };
    
    typedef Direction<Forward> ForwardType;
    typedef Direction<Backward> BackwardType;
    
 // =================================================
    
    enum
    {
        None,
        Xor,
        Reverse
    };
    
    template<int a>
    struct OperationType
    {
        static const int value = a;
    };
    
    typedef OperationType<None> NoneType;
    typedef OperationType<Xor> XorType;
    typedef OperationType<Reverse> ReverseType;
    
// ==============================================
    
    template<int addr      = LittleEndian,
             int direction = Forward,
             int operation = None,
             int offset    = Absolute>
    
    class Block_LZ2 : public Block
    {
        
    // ==============================================
        
    protected:
        
        typedef AddrType<addr> addrType;
        typedef Direction<direction> directionType;
        typedef Direction<direction ^ 0x01> oppositeDirectionType;
        
        typedef OperationType<operation> operationType;
        
        typedef OffsetType<offset> offsetType;
        
        typedef Block_LZ2<addr, direction, operation, offset> SameType;
        
        int dat;
        
        static u8 reverseBits[0x100];
        
    // ==============================================
        
    public:
        
        virtual int GetBodySize(LittleEndianType t)
        {
            return 2;
        }
        
        virtual int GetBodySize(BigEndianType t)
        {
            return 2;
        }
        
        virtual int GetBodySize(OneByteType t)
        {
            return 1;
        }
        
        virtual int GetBodySize()
        {
            // Type 7 blocks unfortunately must use the the longer
            // address to work properly.
            // We compensate for that with the conditional statement.
            return GetBodySize(addrType()); // + (type == 7) ? 1 : 0;
        }
        
    // ==============================================
        
        virtual int GetMinLength()
        {
            return GetBodySize() + 2;
        }
        
    // ==============================================
        
        virtual int GetMinStart(int pos, AbsoluteType t)
        {
            return 0;
        }
        
        virtual int GetMinStart(int pos, RelativeType t)
        {
            int minStart = pos - GetMaxDataSize();
            
            return (minStart < 0) ? 0 : minStart;
        }
        
        virtual int GetMinStart(int pos)
        {
            return GetMinStart(pos, offsetType());
        }
        
    // ==============================================
        
        virtual int GetMaxStart(int srcLen, AbsoluteType t)
        {
            return GetMaxDataSize();
        }
        
        virtual int GetMaxStart(int srcLen, RelativeType t)
        {
            return srcLen;
        }
        
        virtual int GetMaxStart(int srcLen)
        {
            return GetMaxStart(srcLen, offsetType());
        }
        
    // ==============================================
        
        virtual int Decompress(int len, const u8 *src, u8 *dst, int &dstPos)
        {
            int from = GetOffset(src, dstPos);
            
            DoDecompress(from, len, dst, dstPos);
            
            return GetBodySize();
        }
        
    // ==============================================
        
        static void DoDecompress(int from, int len, u8 *dst, int &dstPos)
        {
            if(from < 0)
            {
                return;
            }
            
            if(from >= dstPos)
            {
                // Fill with dummy data for debugging purposes.
                // Yes this is imitating Microsoft's C Runtime.
                for(int i = 0; i < len; ++i)
                {
                    dst[dstPos + i] = 0xCC;
                }
            }
            else if(dst)
            {
                for(int i = 0; i < len; ++i)
                {
                    dst[dstPos + i] = Operation(dst[from + i]);
                }
            }
            
            dstPos += len;
        }
        
    // ==============================================
        
        static void Build(u8 type, BlockList &lst, const u8 *src, int srcLen)
        {
            SameType tmp;
            
            tmp.BuildBlocks(type, lst, src, srcLen);
        }
        
    // ==============================================
        
        void BuildBlocks(u8 type,
                         BlockList &lst,
                         const u8 *src,
                         int srcLen)
        {
            SameType *bk;
            
            int i   = 1;
            int j   = 0;
            int len = 0;
            
            int bestLen   = 0;
            int bestStart = 0;
            int maxStart  = GetMaxStart(srcLen);
            
            int justBlocked = 0;
            int adj         = 0;
            int bestAdj     = 0;
            
            // --------------------------------
            
            while(i < srcLen)
            {
                bestAdj = 0;
                bestLen = GetMinLength() - 1;
                
                for( AuxStart(i, j); AuxTest(i, j); AuxNext(j) )
                {
                    if(j > maxStart)
                        break;
                    
                    for(len = 0; ; ++len)
                    {
                        int mainPos = (i + len);
                        int auxPos  = AuxPos(j, len);
                        
                        // -------------------------
                        
                        if( (mainPos < 0) || (mainPos >= srcLen))
                            break;
                        
                        if( (auxPos < 0) || (auxPos >= srcLen))
                            break;
                        
                        if( src[mainPos] != Operation(src[auxPos]) )
                            break;
                    }
                    
                    adj = 0;
                    
                    if(justBlocked && len)
                    {
                        for( ; ; ++adj)
                        {
                            int mainPos = (i - adj);
                            int auxPos  = InvertedAuxPos(j, adj);
                            
                            // ---------------------------------------
                            
                            if( (mainPos < 0) || (mainPos >= srcLen) )
                                break;
                            
                            if( (auxPos < 0) || (auxPos >= srcLen) )
                                break;
                            
                            // This is primarily here for the Backward directed
                            // LZ blocks, but this should never happen for Forward
                            // directed blocks anyways.
                            if(auxPos >= mainPos)
                                break;
                            
                            if( src[mainPos] != Operation(src[auxPos]) )
                                break;
                        }
                        
                        --adj;
                        
                        len += adj;
                        
                        if( len < GetMinLength() )
                            continue;
                    }
                    
                    if( ( (adj == bestAdj) && (len > bestLen) ) || (adj > bestAdj) )
                    {
                        bestLen   = len;
                        bestStart = InvertedAuxPos(j, adj);
                        bestAdj   = adj;
                    }
                }
                
                if( bestLen >= GetMinLength() )
                {
                    bk        = InnerDup();
                    bk->type  = type;
                    bk->start = i - bestAdj;
                    bk->stop  = i - bestAdj + bestLen;
                    bk->len   =	bestLen;
                    bk->dat   = bestStart;
                    
                    if(bk->Shrink(bk->start, bk->stop))
                    {
                        delete bk;
                        
                        ++i;
                        
                        justBlocked = 0;
                    }
                    else
                    {
                        lst.InsertBlock(bk);
                        
                        i = bk->stop;
                        
                        justBlocked = 1;
                    }
                }
                else
                {
                    ++i;
                    
                    justBlocked = 0;
                }
            }
        }
        
    // ==============================================
        
    protected:
        
        void SetAddr(u8 *buf, int arg, LittleEndianType t)
        {
            buf[0] = (u8) (arg);
            buf[1] = (u8) (arg >> 8);
        }
        
        void SetAddr(u8 *buf, int arg, BigEndianType t)
        {
            buf[0] = (u8) (arg >> 8);
            buf[1] = (u8) (arg);
        }
        
        void SetAddr(u8 *buf, int arg, OneByteType t)
        {
            buf[0] = (u8) (arg);
        }
        
        void SetAddr(u8 *buf, int arg)
        {
            SetAddr(buf, arg, addrType() );
        }
        
    // ==============================================
        
        void SetOffset(u8 *buf, AbsoluteType t)
        {
            SetAddr(buf, dat);
        }
        
        void SetOffset(u8 *buf, RelativeType t)
        {
            SetAddr(buf, start - dat);
            
            if( (start - dat) < 0)
            {
                int b = 0;
            }
        }
        
        void SetOffset(u8 *buf)
        {
            SetOffset( buf, offsetType() );
        }
        
    // ==============================================
        
        int GetAddr(const u8 *src, LittleEndianType t)
        {
            return ( (src[0]) | (src[1] << 8) );
        }
        
        int GetAddr(const u8 *src, BigEndianType t)
        {
            return ( (src[0] << 8) | (src[1]) );
        }
        
        int GetAddr(const u8 *src, OneByteType t)
        {
            return src[0];
        }
        
        int GetAddr(const u8 *src)
        {
            return GetAddr( src, addrType() );
        }
        
    // ==============================================
        
        int GetOffset(const u8 *src, int &destPos, AbsoluteType t)
        {
            return GetAddr(src);
        }
        
        int GetOffset(const u8 *src, int &destPos, RelativeType t)
        {
            return destPos - GetAddr(src);
        }
        
        int GetOffset(const u8 *src, int &destPos)
        {
            int a = GetOffset(src, destPos, offsetType() );
            
            if(a == 0)
            {
                a = 0;
            }
            
            return a;
        }        
        
    // ==============================================
        
        virtual int GetMaxDataSize()
        {
            int ret = 0;
            
            // ----------------------------------
            
            // The range of our data is allowed to be
            // 0x00 to 0xFF, or 0x0000 to 0xFFFF
            ret  = ( 1 << ( 8 * GetBodySize() ) );
            ret -= 1;
            
            return ret;
        }
        
    // ==============================================
        
        virtual int Output(u8* buf)
        { 
            // Some games use little endian representations of the LZ pointer
            SetOffset(buf);
            
            return GetBodySize();
        }
        
    // ==============================================
        
        virtual int Shrink(int newstart, int newstop)
        {
            int newdat = (dat + newstart - start);
            
            if( newdat > GetMaxDataSize() )
            {
                return 1;
            }
            
            dat   = newdat;
            start = newstart;
            stop  = newstop;
            len   = stop - start;
            
            return 0;
        }
        
    // ==============================================
        
        virtual Block* Dup()
        {
            return new SameType(*this);
        }
        
    // ==============================================
        
        virtual SameType* InnerDup()
        {
            return new SameType(*this);
        }
        
    // ==============================================
        
        void AuxStart(int &main, int &aux, ForwardType t)
        {
            aux = GetMinStart(main);
        }
        
        void AuxStart(int &main, int &aux, BackwardType t)
        {
            aux = main - 1;
        }
        
        void AuxStart(int &main, int &aux)
        {
            AuxStart(main, aux, directionType());
        }
        
    // ==============================================
        
        bool AuxTest(int &main, int &aux, ForwardType t)
        {
            return (aux < main);
        }
        
        bool AuxTest(int &main, int &aux, BackwardType t)
        {
            return (aux >= 0);
        }
        
        bool AuxTest(int &main, int &aux)
        {
            return AuxTest(main, aux, directionType());
        }
        
    // ==============================================
        
        void AuxNext(int &aux, ForwardType t)
        {
            ++aux;
        }
        
        void AuxNext(int &aux, BackwardType t)
        {
            --aux;
        }
        
        void AuxNext(int &aux)
        {
            AuxNext(aux, directionType());
        }
        
    // ==============================================
        
        int AuxPos(int aux, int len, ForwardType t)
        {
            return (aux + len);
        }
        
        int AuxPos(int aux, int len, BackwardType t)
        {
            return (aux - len);
        }     
        
        int AuxPos(int aux, int len)
        {
            return AuxPos(aux, len, directionType());
        }
        
    // ==============================================
        
        int InvertedAuxPos(int aux, int len)
        {
            // Use the opposite direction's AuxPos calculation
            return AuxPos(aux, len, oppositeDirectionType());
        }
        
    // ==============================================
        
        static u8 Operation(u8 input, NoneType t)
        {
            return input;
        }
        
        static u8 Operation(u8 input, XorType t)
        {
            return (input ^ 0xFF);
        }
        
        static u8 Operation(u8 input, ReverseType t)
        {
            return reverseBits[input];
        }
        
        static u8 Operation(u8 input)
        {
            return Operation(input, operationType());
        }
    };

// ===========================================================

#endif