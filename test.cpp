#include "sm.hpp"

#include <obvious.hpp>

using namespace sm;

//=====declarations of sm.cpp private pieces under test=====//
unsigned decompress(const Buffer& source, uint32_t offset, Buffer* destination);
void compress(const Buffer& source, Buffer& destination);

//=====main=====//
int main(){
	{
		Buffer incompressible;
		for(unsigned i=0; i<1025; i+=2){
			incompressible.push_back(i%128);
			incompressible.push_back(128+i/128);
		}
		Buffer compressed, decompressed;
		compress(incompressible, compressed);
		decompress(compressed, 0, &decompressed);
		EXPECT(decompressed.size(), incompressible.size())
		for(size_t i=0; i<decompressed.size(); ++i) EXPECT(decompressed[i], incompressible[i])
	}
	return 0;
}
