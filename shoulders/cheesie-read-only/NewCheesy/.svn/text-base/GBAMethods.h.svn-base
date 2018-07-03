#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long

#define s8 signed char
#define s16 signed short
#define s32 signed long
#define DIFF_CODE_HEADER        (0x80)
#define RL_CODE_HEADER          (0x30)
#define LZ_CODE_HEADER          (0x10)
#define HUFF_CODE_HEADER        (0x20)
#define CODE_HEADER_MASK        (0xF0)
class GBAMethods
{
public: 
	char  FileName[1024];   
	GBAMethods();
	~GBAMethods();
	void  Reopen();
	u32   RawWrite(       u8 *srcp, u32 size, u8 *dstp);
	u32   RLCompWrite(    u8 *srcp, u32 size, u8 *dstp);
	u32   LZCompWrite(    u8 *srcp, u32 size, u8 *dstp, u8 lzSearchOffset);
	s32   RawRead(        u8 *srcp, u32 size, u8 *dstp);
	s32   RLCompRead(     u8 *srcp, u32 size, u8 *dstp);
	s32   LZ77UnComp(u8 *source, u8 *dest);
	u32   LZ77Comp(u32 decmpsize,  u8 *source, u8 *dest) ;
	long  ZMUncompRle(u16 arg0, u8 *arg1, u8*arg2);
	void* DecodePal(long offset, long* dstPal, int numPals, char palpos);
	void* DecodePal(short *palGBA,long* palPC,int numpals, char palpos);
	int   ZMCompress(FILE* fp);
	int   ReturnFileName(char* Filter,char*FilePath,int sofn);
	void* EncodePal(short *palGBA,long* palPC,int numpals, char palpos);
	long  RLComp(u32 size , u8* srcp, u8* dstp);
	long  compress(u8 cmp[], u8 uncmp[], unsigned long sze);
	int   RLEComp(u8* cmp, u8 *uncmp);
	long  FindFreeSpace(long bNo, u8 bConst);
	int ReturnFileName(char* Filter,char*FilePath,int sofn,u8 Existance);
	int ReturnEOF(FILE* fp);
	int SMDecomp(u32 offset);//Auto writes to CompBuffer
	int SMDecomp(u32 offset, u8* buffer);//Writes to buffer
	void Blit(HDC hdcDest, RECT* dstRect,  HDC hdcSrc, RECT* srcRect, float magnify);
	FILE* ROM;
	FILE* REDIT;
	unsigned char  CompBuffer[64691];
	short GBAPal[1024];
	u32 DefAddress;
	char  FileLoc[512];


};