#include "sm.hpp"

#include <obvious.hpp>

using namespace sm;

//=====declarations of sm.cpp private pieces under test=====//
unsigned decompress(const Buffer& source, uint32_t offset, Buffer* destination);
void compress(const Buffer& source, Buffer& destination);

//=====helpers=====//
#define COMPDECOMP(UNCOMPRESSED){\
	Buffer compressed, decompressed;\
	compress(UNCOMPRESSED, compressed);\
	decompress(compressed, 0, &decompressed);\
	EXPECT(decompressed.size(), UNCOMPRESSED.size())\
	for(size_t i=0; i<decompressed.size(); ++i) EXPECT(decompressed[i], UNCOMPRESSED[i])\
}

void pushIncompressible(Buffer& b, unsigned i, uint8_t mask=0){
	b.push_back((    i%128)^mask);
	b.push_back((128+i/128)^mask);
}

//=====main=====//
int main(){
	{
		Buffer incompressible;
		for(unsigned i=0; i<1025; i+=2) pushIncompressible(incompressible, i);
		COMPDECOMP(incompressible);
	}
	{
		Buffer uncompressed;
		for(unsigned i=0; i<0x10001; ++i) uncompressed.push_back(0);
		for(unsigned i=0; i<4; i+=2) pushIncompressible(uncompressed, i);
		for(unsigned i=0; i<1024; ++i) uncompressed.push_back(uncompressed[0x10001+i%8]^0xFFu);
		COMPDECOMP(uncompressed);
	}
	return 0;
}
