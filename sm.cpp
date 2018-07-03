#include "sm.hpp"

#include <fstream>
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace sm;

const uint32_t sm::VANILLA_ROOM_OFFSETS[VANILLA_ROOMS]={
	0x791F8, 0x792B3, 0x792FD, 0x793AA, 0x793D5, 0x793FE, 0x79461, 0x7948C,
	0x794CC, 0x794FD, 0x79552, 0x7957D, 0x795A8, 0x795D4, 0x795FF, 0x7962A,
	0x7965B, 0x7968F, 0x796BA, 0x7975C, 0x797B5, 0x79804, 0x79879, 0x798E2,
	0x7990D, 0x79938, 0x79969, 0x79994, 0x799BD, 0x799F9, 0x79A44, 0x79A90,
	0x79AD9, 0x79B5B, 0x79B9D, 0x79BC8, 0x79C07, 0x79C35, 0x79C5E, 0x79C89,
	0x79CB3, 0x79D19, 0x79D9C, 0x79DC7, 0x79E11, 0x79E52, 0x79E9F, 0x79F11,
	0x79F64, 0x79FBA, 0x79FE5, 0x7A011, 0x7A051, 0x7A07B, 0x7A0A4, 0x7A0D2,
	0x7A107, 0x7A130, 0x7A15B, 0x7A184, 0x7A1AD, 0x7A1D8, 0x7A201, 0x7A22A,
	0x7A253, 0x7A293, 0x7A2CE, 0x7A2F7, 0x7A322, 0x7A37C, 0x7A3AE, 0x7A3DD,
	0x7A408, 0x7A447, 0x7A471, 0x7A4B1, 0x7A4DA, 0x7A521, 0x7A56B, 0x7A59F,
	0x7A5ED, 0x7A618, 0x7A641, 0x7A66A, 0x7A6A1, 0x7A6E2, 0x7A70B, 0x7A734,
	0x7A75D, 0x7A788, 0x7A7B3, 0x7A7DE, 0x7A815, 0x7A865, 0x7A890, 0x7A8B9,
	0x7A8F8, 0x7A923, 0x7A98D, 0x7A9E5, 0x7AA0E, 0x7AA41, 0x7AA82, 0x7AAB5,
	0x7AADE, 0x7AB07, 0x7AB3B, 0x7AB64, 0x7AB8F, 0x7ABD2, 0x7AC00, 0x7AC2B,
	0x7AC5A, 0x7AC83, 0x7ACB3, 0x7ACF0, 0x7AD1B, 0x7AD5E, 0x7ADAD, 0x7ADDE,
	0x7AE07, 0x7AE32, 0x7AE74, 0x7AEB4, 0x7AEDF, 0x7AF14, 0x7AF3F, 0x7AF72,
	0x7AFA3, 0x7AFCE, 0x7AFFB, 0x7B026, 0x7B051, 0x7B07A, 0x7B0B4, 0x7B0DD,
	0x7B106, 0x7B139, 0x7B167, 0x7B192, 0x7B1BB, 0x7B1E5, 0x7B236, 0x7B283,
	0x7B2DA, 0x7B305, 0x7B32E, 0x7B37A, 0x7B3A5, 0x7B3E1, 0x7B40A, 0x7B457,
	0x7B482, 0x7B4AD, 0x7B4E5, 0x7B510, 0x7B55A, 0x7B585, 0x7B5D5, 0x7B62B,
	0x7B656, 0x7B698, 0x7B6C1, 0x7B6EE, 0x7B741, 0x7C98E, 0x7CA08, 0x7CA52,
	0x7CAAE, 0x7CAF6, 0x7CB8B, 0x7CBD5, 0x7CC27, 0x7CC6F, 0x7CCCB, 0x7CD13,
	0x7CD5C, 0x7CDA8, 0x7CDF1, 0x7CE40, 0x7CE8A, 0x7CED2, 0x7CEFB, 0x7CF54,
	0x7CF80, 0x7CFC9, 0x7D017, 0x7D055, 0x7D08A, 0x7D0B9, 0x7D104, 0x7D13B,
	0x7D16D, 0x7D1A3, 0x7D1DD, 0x7D21C, 0x7D252, 0x7D27E, 0x7D2AA, 0x7D2D9,
	0x7D30B, 0x7D340, 0x7D387, 0x7D3B6, 0x7D3DF, 0x7D408, 0x7D433, 0x7D461,
	0x7D48E, 0x7D4C2, 0x7D4EF, 0x7D51E, 0x7D54D, 0x7D57A, 0x7D5A7, 0x7D5EC,
	0x7D617, 0x7D646, 0x7D69A, 0x7D6D0, 0x7D6FD, 0x7D72A, 0x7D765, 0x7D78F,
	0x7D7E4, 0x7D81A, 0x7D845, 0x7D86E, 0x7D898, 0x7D8C5, 0x7D913, 0x7D95E,
	0x7D9AA, 0x7D9D4, 0x7D9FE, 0x7DA2B, 0x7DA60, 0x7DAAE, 0x7DAE1, 0x7DB31,
	0x7DB7D, 0x7DBCD, 0x7DC19, 0x7DC65, 0x7DCB1, 0x7DCFF, 0x7DD2E, 0x7DD58,
	0x7DDC4, 0x7DDF3, 0x7DE23, 0x7DE4D, 0x7DE7A, 0x7DEA7, 0x7DEDE, 0x7DF1B,
	0x7DF45, 0x7DF8D, 0x7DFD7, 0x7E021, 0x7E06B, 0x7E0B5, 0x7E82C
};

const uint16_t VANILLA_CERES_RIDLEY_ROOM_LAYER_HANDLING=0xC97Bu;

//=====SNES format 5 compression=====//
const unsigned MAX_BLOCK_LENGTH=1024;

unsigned lzDecompress(const Buffer& source, uint32_t offset, uint32_t length, unsigned bytes, uint8_t mask, bool absolute, Buffer* destination=NULL){
	if(!(bytes==1||bytes==2)) throw std::logic_error("bad bytes in call for lz decompress");
	if(!(mask==0||mask==0xFFu)) throw std::logic_error("bad mask in call for lz decompress");
	if(destination){
		int from=source[offset];
		if(bytes==2) from|=source[offset+1]<<8;
		if(!absolute) from=destination->size()-from;
		if(from>=0)
			for(unsigned i=0; i<length; ++i)
				destination->push_back((*destination)[from+i]^mask);
	}
	return bytes;
}

//returns size of compressed data
unsigned decompress(const Buffer& source, uint32_t offset, Buffer* destination){
	unsigned initialOffset=offset;
	while(true){
		if(source[offset]==0xFF) break;//done
		uint8_t op;
		unsigned length;
		if((source[offset]&0xE0)==0xE0){//long length block
			length=(source[offset]&3)<<8|source[offset+1];
			op=source[offset]>>2&7;
			++offset;
		}
		else{//short length block
			length=source[offset]&0x1F;
			op=source[offset]>>5;
		}
		++length;
		++offset;
		switch(op){
			case 0://no compression
				for(unsigned i=0; i<length; ++i){
					if(destination) destination->push_back(source[offset]);
					++offset;
				}
				break;
			case 1://1 byte run length encoding
				if(destination) for(unsigned i=0; i<length; ++i) destination->push_back(source[offset]);
				++offset;
				break;
			case 2://2 byte run length encoding
				if(destination) for(unsigned i=0; i<length; ++i) destination->push_back(source[offset+i%2]);
				offset+=2;
				break;
			case 3://gradient run length encoding
				if(destination) for(unsigned i=0; i<length; ++i) destination->push_back((source[offset]+i)%0x100u);
				++offset;
				break;
			case 4:
				offset+=lzDecompress(source, offset, length, 2, 0, true, destination);
				break;
			case 5:
				offset+=lzDecompress(source, offset, length, 2, 0xFFu, true, destination);
				break;
			case 6:
				offset+=lzDecompress(source, offset, length, 1, 0, false, destination);
				break;
			case 7:
				offset+=lzDecompress(source, offset, length, 1, 0xFFu, false, destination);
				break;
			default: break;
		}
	}
	return offset+1-initialOffset;
}

void putBlockHeader(Buffer& destination, uint8_t op, unsigned length){
	if(length==0||length>MAX_BLOCK_LENGTH){
		std::stringstream ss;
		ss<<"bad length "<<length<<" in compression block header";
		throw std::logic_error(ss.str());
	}
	--length;
	if(length>0x1Fu||op==7u){
		destination.push_back(0xE0u|op<<2|(length&0x300u)>>8);
		destination.push_back(length&0xFFu);
	}
	else destination.push_back(op<<5|length);
}

void noCompress(const Buffer& source, uint32_t offset, uint32_t length, Buffer& destination){
	putBlockHeader(destination, 0, length);
	for(unsigned i=0; i<length; ++i)
		destination.push_back(source[offset-length+i]);
}

void rleCompress(const Buffer& source, uint32_t offset, uint32_t& length, uint8_t op, Buffer& destination){
	unsigned bytes=1, gradient=0;
	switch(op){
		case 1: break;
		case 2: bytes=2; break;
		case 3: gradient=1; break;
		default: throw std::logic_error("bad op in call for rle compress");
	}
	length=1;
	for(unsigned i=1; offset+i<source.size()&&length<MAX_BLOCK_LENGTH; ++i){
		if(source[offset+i]==(source[offset+i%bytes]+gradient*i)%0x100u) ++length;
		else break;
	}
	putBlockHeader(destination, op, length);
	destination.push_back(source[offset]);
	if(bytes==2) destination.push_back(source[offset+1]);
}

class LzCompressor{
	public:
		LzCompressor(const Buffer& source): _source(source) {
			for(unsigned i=0; i<_source.size(); ++i)
				_offsets[_source[i]].push_back(i);
		}
		void compress(uint32_t offset, uint32_t& length, uint8_t op, Buffer& destination){
			unsigned bytes=1;//bytes allocated to specify where to decompress from
			uint8_t mask=0;//the range being decompressed shall be xor with this value
			bool absolute=true;//whether the place to decompress from specified absolutely or relatively
			unsigned maxBlockLength=1024;
			switch(op){
				case 4: bytes=2; break;
				case 5: bytes=2; mask=0xFFu; break;
				case 6: absolute=false; break;
				case 7: mask=0xFFu; absolute=false; maxBlockLength=768; break;
				default: throw std::logic_error("bad op in call for lz compress");
			}
			uint32_t lowest=0, highest=offset;//extreme possible indices to start reading from while decompressing
			if(absolute) highest=min(bytes==2?0x10000u:0x100u, offset);
			else lowest=max(int(offset-(bytes==2?0xFFFFu:0xFFu)), 0);
			//build Knuth–Morris–Pratt table
			//search word is source[offset..length]
			//table tells us what index into the search word to start from when we hit a mismatch
			int table[MAX_BLOCK_LENGTH];
			const unsigned wordLength=min(maxBlockLength, uint32_t(_source.size()-offset));//length of search word
			table[0]=-1;//search word mismatches on 1st char: negative backtrack by one computes as move to next position in text
			table[1]= 0;//search word mismatches on 2nd char: nothing else to do but go back to beginning of search word and try again
			{
				unsigned i=2,//index pointing to part of table being computed
					j=0;//the value going into table[i]
				while(i<wordLength){//size of table is size of search word
					if(_source[offset+i-1]==_source[offset+j]){//search word continues to match search word prefix
						++j;
						table[i]=j;
						++i;
					}
					else if(j>0) j=table[j];//mismatch -- backtrack j using table defined so far
					else{//mismatch, nothing more to backtrack to
						table[i]=0;//j is 0
						++i;
					}
				}
			}
			//find longest match using Knuth–Morris–Pratt algorithm
			unsigned bestStart=0, bestLength=0, nextOffsetToTry=0,
				i,//i is where in source current match started
				j=0;//j is index into search word; i+j is index into source currently being inspected
			const auto offsets=_offsets[_source[offset]^mask];//the particular offsets into source that we will try
			while(true){//skip to the lowest offset we can use
				if(nextOffsetToTry>=offsets.size()){
					length=0;
					return;
				}
				i=offsets[nextOffsetToTry];
				if(i>=lowest) break;
				++nextOffsetToTry;
			}
			while(
				i+j<highest//obvious condition for condition for continuing
				||(//we may want to allow decompression by reading what we've just decompressed, but we must fulfill
					j!=0//currently matching
					&&i<offset//start of match is before where we're trying to compress
					&&i+j<highest+maxBlockLength//we are capable of specifying how to decompress when limited by absolute offset
					&&i+j<_source.size()//reading valid data at all
				)
			){
				if(_source[offset+j]==(_source[i+j]^mask)){//match
					++j;//increase length of match (which is index into search word)
					if(j>bestLength){//keep track of best
						bestStart=i;
						bestLength=j;
					}
					if(j==wordLength) break;//early exit if we found the entire search word
				}
				else{//mismatch
					i+=j-table[j];
					if(table[j]>=0) j=table[j];//don't have to start over; i+j remains constant
					else{
						j=0;//have to start over; i+j increases by 1
						//advance i based on index of source
						while(true){
							++nextOffsetToTry;
							if(nextOffsetToTry>=offsets.size()) break;
							if(offsets[nextOffsetToTry]>=i) break;
						}
						if(nextOffsetToTry>=offsets.size()) break;
						else i=offsets[nextOffsetToTry];
					}
				}
			}
			if(bestLength==0){
				length=0;
				return;
			}
			//apply
			length=bestLength;
			putBlockHeader(destination, op, length);
			if(!absolute) bestStart=offset-bestStart;
			destination.push_back(bestStart);
			if(bytes==2) destination.push_back(bestStart>>8);
		}
	private:
		const Buffer& _source;
		vector<unsigned> _offsets[256];//map byte to vector of offsets into source equal to that byte, sorted low-to-high
};

void compress(const Buffer& source, Buffer& destination){
	unsigned i=0;
	unsigned noCompressionLength=0;
	LzCompressor lzc(source);
	while(i<source.size()){
		//attempt all strategies greedily
		const unsigned strategies=7;
		Buffer block[strategies];
		unsigned length[strategies];
		rleCompress(source, i, length[0], 1, block[0]);
		rleCompress(source, i, length[1], 2, block[1]);
		rleCompress(source, i, length[2], 3, block[2]);
		lzc.compress(i, length[3], 4, block[3]);
		lzc.compress(i, length[4], 5, block[4]);
		lzc.compress(i, length[5], 6, block[5]);
		lzc.compress(i, length[6], 7, block[6]);
		//find best op to use
		unsigned bestOp=0, bestSourceLength=1, bestDestinationLength=1;
		for(unsigned j=0; j<strategies; ++j){
			unsigned sourceLength=length[j];
			if(sourceLength==0) continue;
			unsigned destinationLength=block[j].size();
			float bestRatio=1.0f*bestDestinationLength/bestSourceLength;
			float ratio=1.0f*destinationLength/sourceLength;
			if(bestSourceLength>8&&sourceLength>8){
				if(sourceLength<bestSourceLength) ratio=1.0f*(destinationLength+2)/sourceLength;
				else if(sourceLength>bestSourceLength) bestRatio=1.0f*(bestDestinationLength+2)/bestSourceLength;
			}
			if(ratio<bestRatio){
				bestOp=j+1;
				bestSourceLength=length[j];
				bestDestinationLength=block[j].size();
			}
		}
		//apply best op
		if(bestOp==0){
			++noCompressionLength;
			++i;
			if(i>=source.size()||noCompressionLength==MAX_BLOCK_LENGTH){
				noCompress(source, i, noCompressionLength, destination);
				noCompressionLength=0;
			}
		}
		else{
			//actually stick in the no compression block first if it exists
			if(noCompressionLength!=0){
				noCompress(source, i, noCompressionLength, destination);
				noCompressionLength=0;
			}
			//stick in the compressed block
			for(unsigned j=0; j<block[bestOp-1].size(); ++j)
				destination.push_back(block[bestOp-1][j]);
			i+=length[bestOp-1];
		}
	}
	destination.push_back(0xFFu);//done
}

//=====translation=====//
uint32_t loRomToOffset(uint32_t address){ return (address&0x7F0000u)>>1|(address&0x7FFFu); }
uint32_t loRomToOffset(uint8_t bank, uint16_t address){ return (bank&0x7Fu)<<15|(address&0x7FFFu); }
uint32_t offsetToLoRom(uint32_t offset){ return 0x800000u|(offset&0x3F8000u)<<1|0x8000u|(offset&0x7FFFu); }
uint16_t offsetToLoRom16(uint32_t offset){ return 0x8000u|(offset&0x7FFFu); }

uint16_t readU16(const Buffer& buffer, uint32_t offset){
	return buffer[offset]|buffer[offset+1]<<8;
}

void writeU16(Buffer& buffer, uint32_t offset, uint16_t value){
	buffer[offset+0]=value>>0&0xFFu;
	buffer[offset+1]=value>>8&0xFFu;
}

uint32_t readU24(const Buffer& buffer, uint32_t offset){
	return buffer[offset]|buffer[offset+1]<<8|buffer[offset+2]<<16;
}

void writeU24(Buffer& buffer, uint32_t offset, uint32_t value){
	buffer[offset+0]=value>> 0&0xFFu;
	buffer[offset+1]=value>> 8&0xFFu;
	buffer[offset+2]=value>>16&0xFFu;
}

void readU82D(const Buffer& buffer, uint32_t offset, Array2D<uint8_t>& u82d){
	for(unsigned j=0; j<u82d.readJSize(); ++j)
		for(unsigned i=0; i<u82d.readISize(); ++i){
			u82d.at(i, j)=buffer[offset];
			++offset;
		}
}

void writeU82D(Buffer& buffer, uint32_t offset, const Array2D<uint8_t>& u82d){
	for(unsigned i=0; i<u82d.readISize(); ++i)
		for(unsigned j=0; j<u82d.readJSize(); ++j){
			buffer[offset]=u82d.at(i, j);
			++offset;
		}
}

void drawSubtile(const Buffer& subtiles, uint16_t tileInfo, const vector<Color>& palette, Array2D<Color>& destination, unsigned x, unsigned y){
	uint8_t xMask=(tileInfo&0x4000)?7:0;
	uint8_t yMask=(tileInfo&0x8000)?7:0;
	uint8_t hi=(tileInfo&0x1C00u)>>6;
	for(unsigned ty=0; ty<8; ++ty){
		for(unsigned tx=0; tx<8; ++tx){
			uint8_t lo=subtiles[(tileInfo&0x3FFu)*64+(tx^xMask)+(ty^yMask)*8];
			if(lo) destination.at(x+tx, y+ty)=palette[hi|lo];
			else destination.at(x+tx, y+ty)=Color();
		}
	}
}

uint32_t tileSetOffset(uint8_t tileSet){ return 0x7E6A2u+uint32_t(tileSet)*9; }

//=====class Rom=====//
string Rom::open(string fileName){
	header.clear();
	buffer.clear();
	ifstream file(fileName.c_str(), ios::binary);
	if(!file.good()) return "Couldn't open ROM.";
	char c;
	while(file.get(c)) buffer.push_back(c);
	file.close();
	//handle header
	if(buffer.size()%32768!=0){
		for(unsigned i=0; i<512; ++i) header.push_back(buffer[i]);
		buffer.erase(buffer.begin(), buffer.begin()+512);
	}
	//check for PAL
	if(buffer[0x7FD9u]>=2) return "ROM is PAL. This doesn't work on PAL ROMs.";
	//finish
	return "";
}

bool Rom::indexVanilla(){
	//saves
	Save::index(index);
	//transitions from saves
	for(unsigned offset=Save::START; offset<Save::END; offset+=Save::SIZE){
		Save s(*this);
		s.open(offset);
		Transition t(*this);
		t.index(s.transition, index);
	}
	//mode 7 graphics
	Mode7 mode7(*this);
	for(uint8_t i=Mode7::FIRST_TILE_SET; i<=Mode7::LAST_TILE_SET; ++i)
		mode7.index(i, index);
	//rooms
	Room room(*this);
	for(unsigned i=0; i<VANILLA_ROOMS; ++i)
		if(!room.index(sm::VANILLA_ROOM_OFFSETS[i], index))
			return false;
	//finish
	return true;
}

bool Rom::takeSpace(uint8_t bank, uint16_t size, uint32_t& offset){
	uint32_t bankStart=loRomToOffset(bank, 0);
	offset=bankStart;
	for(unsigned i=bankStart, sizeSoFar=0; i<bankStart+0x8000u; ++i){
		if(index[i]==HACKABLE){
			++sizeSoFar;
			if(sizeSoFar==size){
				index.set(offset, size, HACKED);
				return true;
			}
		}
		else{
			offset=i+1;
			sizeSoFar=0;
		}
	}
	return false;
}

bool Rom::takeSpace(uint8_t minBank, uint8_t maxBank, uint16_t size, uint32_t& offset){
	for(uint8_t bank=minBank; bank<=maxBank; ++bank)
		if(takeSpace(bank, size, offset)) return true;
	return false;
}

bool Rom::save(string fileName){
	ofstream file(fileName.c_str(), ios::binary);
	for(unsigned i=0; i<header.size(); ++i) file.put(header[i]);
	for(unsigned i=0; i<buffer.size(); ++i) file.put(buffer[i]);
	file.close();
	return true;
}

void Rom::dummify(){
	for(unsigned i=0; i<index.size(); ++i)
		if(index[i]==HACKABLE)
			buffer[i]=0xD0u;
}

//=====class Transition=====//
void Transition::index(uint32_t offset, Rom::Index& index){
	index.set(offset, SIZE, Rom::HACKABLE);
}

void Transition::open(uint32_t offset){
	room=readU16(rom->buffer, offset);
	if(room!=0) room=loRomToOffset(Room::BANK, room);
	flags=rom->buffer[offset+2];
	direction=rom->buffer[offset+3];
	animationX=rom->buffer[offset+4];
	animationY=rom->buffer[offset+5];
	x=rom->buffer[offset+6];
	y=rom->buffer[offset+7];
	distance=readU16(rom->buffer, offset+8);
	scroll=readU16(rom->buffer, offset+10);
}

bool Transition::save(uint32_t& offset){
	if(!rom->takeSpace(BANK, SIZE, offset)) return false;
	writeU16(rom->buffer, offset, room==0?0:offsetToLoRom16(room));
	rom->buffer[offset+2]=flags;
	rom->buffer[offset+3]=direction;
	rom->buffer[offset+4]=animationX;
	rom->buffer[offset+5]=animationY;
	rom->buffer[offset+6]=x;
	rom->buffer[offset+7]=y;
	writeU16(rom->buffer, offset+8, distance);
	writeU16(rom->buffer, offset+10, scroll);
	return true;
}

//=====class Save=====//
void Save::index(Rom::Index& index){
	index.set(START, END-START, Rom::HACKABLE);
}

uint32_t Save::readRegionTable(Region region){
	return loRomToOffset(BANK, readU16(rom->buffer, REGION_TABLES+2*region));
}

void Save::setRegionTable(Region region, uint32_t offset){
	writeU16(rom->buffer, REGION_TABLES+2*region, offsetToLoRom16(offset));
}

void Save::open(uint32_t offset){
	room=loRomToOffset(Room::BANK, readU16(rom->buffer, offset));
	transition=loRomToOffset(Transition::BANK, readU16(rom->buffer, offset+2));
	unknown=readU16(rom->buffer, offset+4);
	scrollX=readU16(rom->buffer, offset+6);
	scrollY=readU16(rom->buffer, offset+8);
	samusY=readU16(rom->buffer, offset+10);
	samusX=readU16(rom->buffer, offset+12);
}

bool Save::save(uint32_t& offset){
	if(!rom->takeSpace(BANK, SIZE, offset)) return false;
	writeU16(rom->buffer, offset, offsetToLoRom16(room));
	writeU16(rom->buffer, offset+2, offsetToLoRom16(transition));
	writeU16(rom->buffer, offset+4, unknown);
	writeU16(rom->buffer, offset+6, scrollX);
	writeU16(rom->buffer, offset+8, scrollY);
	writeU16(rom->buffer, offset+10, samusY);
	writeU16(rom->buffer, offset+12, samusX);
	return true;
}

//=====class Mode7=====//
void Mode7::index(uint8_t tileSet, Rom::Index& index){
	unsigned offset=dataOffset(tileSet);
	index.set(offset, decompress(rom->buffer, offset, NULL), Rom::HACKABLE);
}

void Mode7::open(uint8_t tileSet){
	data.clear();
	tiles.clear();
	decompress(rom->buffer, dataOffset(tileSet), &data);
	tiles.resize(128, 128);
	for(unsigned i=0; i<tiles.readISize(); ++i)
		for(unsigned j=0; j<tiles.readJSize(); ++j)
			tiles.at(i, j)=data[2*(j*tiles.readISize()+i)];
}

bool Mode7::save(uint8_t tileSet){
	for(unsigned i=0; i<tiles.readISize(); ++i)
		for(unsigned j=0; j<tiles.readJSize(); ++j)
			data[2*(j*tiles.readISize()+i)]=tiles.at(i, j);
	Buffer compressed;
	compress(data, compressed);
	uint32_t offset;
	if(!rom->takeSpace(Mode7::FIRST_BANK, Mode7::LAST_BANK, (uint16_t)compressed.size(), offset))
		return false;
	writeU24(rom->buffer, tileSetOffset(tileSet)+3, offsetToLoRom(offset));
	for(unsigned i=0; i<compressed.size(); ++i) rom->buffer[offset+i]=compressed[i];
	return true;
}

void Mode7::clear(){
	data.clear();
	tiles.clear();
}

uint32_t Mode7::dataOffset(uint8_t tileSet){
	return loRomToOffset(readU24(rom->buffer, tileSetOffset(tileSet)+3));
}

//=====class Header=====//
string Header::codeDescription(Code code){
	switch(code){
		case STANDARD          : return "standard";
		case DOORS             : return "doors";
		case TOURIAN_BOSS      : return "Tourian boss";
		case EVENTS            : return "events";
		case BOSSES            : return "bosses";
		case MORPH             : return "morph";
		case MORPH_AND_MISSILES: return "morph & missiles";
		case POWER_BOMBS       : return "power bombs";
		case SPEED_BOOSTER     : return "speed booster";
		default: break;
	}
	return "";
}

Header::Header(const Buffer& buffer, unsigned offset):
	index(buffer[offset]),
	region(Region(buffer[offset+1])),
	x(buffer[offset+2]),
	y(buffer[offset+3]),
	width(buffer[offset+4]),
	height(buffer[offset+5]),
	upScroll(buffer[offset+6]),
	downScroll(buffer[offset+7]),
	graphicsFlags(buffer[offset+8]),
	doors(loRomToOffset(DOOR_BANK, readU16(buffer, offset+9)))
{
	offset+=MAIN_SIZE;
	while(true){
		stateInfo.push_back(StateInfo());
		stateInfo.back().code=Code(readU16(buffer, offset));
		offset+=2;
		for(unsigned i=0; i<fieldSize(stateInfo.back().code); ++i){
			stateInfo.back().fields.push_back(buffer[offset]);
			++offset;
		}
		if(stateInfo.back().code==STANDARD){
			stateInfo.back().state=offset;
			break;
		}
		stateInfo.back().state=loRomToOffset(State::BANK, readU16(buffer, offset));
		offset+=2;
	}
}

unsigned Header::size(){
	unsigned result=MAIN_SIZE;
	for(unsigned i=0; i<stateInfo.size(); ++i) result+=stateInfoSize(stateInfo[i].code);
	return result;
}

void Header::write(Buffer& buffer, uint32_t offset) const{
	buffer[offset+0]=index;
	buffer[offset+1]=region;
	buffer[offset+2]=x;
	buffer[offset+3]=y;
	buffer[offset+4]=width;
	buffer[offset+5]=height;
	buffer[offset+6]=upScroll;
	buffer[offset+7]=downScroll;
	buffer[offset+8]=graphicsFlags;
	writeU16(buffer, offset+9, offsetToLoRom16(doors));
	offset+=MAIN_SIZE;
	for(unsigned i=0; i<stateInfo.size(); ++i){
		writeU16(buffer, offset, stateInfo[i].code);
		offset+=2;
		for(unsigned j=0; j<stateInfo[i].fields.size(); ++j){
			buffer[offset]=stateInfo[i].fields[j];
			++offset;
		}
		if(stateInfo[i].code!=STANDARD){
			writeU16(buffer, offset, offsetToLoRom16(stateInfo[i].state));
			offset+=2;
		}
	}
}

unsigned Header::fieldSize(Code code){
	switch(code){
		case DOORS : return 2;
		case EVENTS: return 1;
		case BOSSES: return 1;
		default: return 0;
	}
}

unsigned Header::stateInfoSize(Code code){
	unsigned result=2;//for the code
	if(code!=STANDARD) result+=fieldSize(code)+2;//for the fields and state pointer
	return result;
}

//=====struct State=====//
State::State(const Buffer& buffer, uint32_t offset):
	tiles(loRomToOffset(readU24(buffer, offset))),
	tileSet(buffer[offset+3]),
	musicTrack(buffer[offset+4]),
	musicControl(buffer[offset+5]),
	fx1(readU16(buffer, offset+6)),
	enemies(readU16(buffer, offset+8)),
	enemySet(readU16(buffer, offset+10)),
	layer2(readU16(buffer, offset+12)),
	scroll(readU16(buffer, offset+14)),
	unknown(readU16(buffer, offset+16)),
	fx2(readU16(buffer, offset+18)),
	plm(readU16(buffer, offset+20)),
	background(readU16(buffer, offset+22)),
	layerHandling(readU16(buffer, offset+24))
{
	if(enemies) enemies=loRomToOffset(Enemy::BANK, enemies);
	if(scroll>=0x8000u) scroll=loRomToOffset(SCROLL_BANK, scroll);
	if(plm) plm=loRomToOffset(Plm::BANK, plm);
}

void State::write(Buffer& buffer, uint32_t offset) const{
	writeU24(buffer, offset, offsetToLoRom(tiles));
	buffer[offset+3]=tileSet;
	buffer[offset+4]=musicTrack;
	buffer[offset+5]=musicControl;
	writeU16(buffer, offset+6, fx1);
	writeU16(buffer, offset+8, enemies?offsetToLoRom16(enemies):0);
	writeU16(buffer, offset+10, enemySet);
	writeU16(buffer, offset+12, layer2);
	writeU16(buffer, offset+14, scroll>=0x8000u?offsetToLoRom16(scroll):scroll);
	writeU16(buffer, offset+16, unknown);
	writeU16(buffer, offset+18, fx2);
	writeU16(buffer, offset+20, plm?offsetToLoRom16(plm):0);
	writeU16(buffer, offset+22, background);
	writeU16(buffer, offset+24, layerHandling);
}

//=====struct TileLayer=====//
TileLayer::TileLayer(const Buffer& buffer, uint32_t offset):
	index(readU16(buffer, offset)&0x3FFu),
	flipH((buffer[offset+1]&4)!=0),
	flipV((buffer[offset+1]&8)!=0),
	property(buffer[offset+1]>>4)
{}

void TileLayer::write(Buffer& buffer) const{
	buffer.push_back(index&0xFFu);
	buffer.push_back(property<<4|(flipV?8:0)|(flipH?4:0)|index>>8);
}

//=====struct Enemy=====//
Enemy::Enemy(const Buffer& buffer, uint32_t offset):
	species(readU16(buffer, offset)),
	x(readU16(buffer, offset+2)),
	y(readU16(buffer, offset+4)),
	field1(readU16(buffer, offset+6)),
	field2(readU16(buffer, offset+8)),
	field3(readU16(buffer, offset+10)),
	field4(readU16(buffer, offset+12)),
	field5(readU16(buffer, offset+14))
{}

void Enemy::write(Buffer& buffer, uint32_t offset) const{
	writeU16(buffer, offset, species);
	writeU16(buffer, offset+2, x);
	writeU16(buffer, offset+4, y);
	writeU16(buffer, offset+6, field1);
	writeU16(buffer, offset+8, field2);
	writeU16(buffer, offset+10, field3);
	writeU16(buffer, offset+12, field4);
	writeU16(buffer, offset+14, field5);
}

//=====struct Plm=====//
Plm::Plm(const Buffer& buffer, uint32_t offset):
	type(readU16(buffer, offset)),
	x(buffer[offset+2]),
	y(buffer[offset+3]),
	field1(buffer[offset+4]),
	field2(buffer[offset+5])
{}

void Plm::write(Buffer& buffer, uint32_t offset) const{
	writeU16(buffer, offset, type);
	buffer[offset+2]=x;
	buffer[offset+3]=y;
	buffer[offset+4]=field1;
	buffer[offset+5]=field2;
}

//=====class Room=====//
bool Room::index(uint32_t offset, Rom::Index& index){
	//header
	Header indexingHeader=Header(rom->buffer, offset);
	index.set(offset, indexingHeader.size(), Rom::HACKABLE);
	offset+=indexingHeader.size();
	//state dependent data
	unsigned doorsInRoom=0;
	for(unsigned i=0; i<indexingHeader.stateInfo.size(); ++i){
		State state=State(rom->buffer, indexingHeader.stateInfo[i].state);
		//state
		index.set(indexingHeader.stateInfo[i].state, State::SIZE, Rom::HACKABLE);
		//scroll data
		if(state.scroll>=0x8000u)
			index.set(state.scroll, indexingHeader.width*indexingHeader.height, Rom::HACKABLE);
		else if(state.scroll>=2)
			return false;
		//tile data
		Buffer buffer;
		index.set(state.tiles, decompress(rom->buffer, state.tiles, &buffer), Rom::HACKABLE);
		uint16_t roomSize=readU16(buffer, 0);
		for(unsigned i=0; i<indexingHeader.width*SCREEN_SIZE; ++i)
			for(unsigned j=0; j<indexingHeader.height*SCREEN_SIZE; ++j){
				TileLayer layer1=TileLayer(buffer, 2+2*(i+j*indexingHeader.width*SCREEN_SIZE));
				if(layer1.property==9){
					uint8_t bts=buffer[2+roomSize+i+j*indexingHeader.width*SCREEN_SIZE];
					doorsInRoom=max(doorsInRoom, unsigned(bts)+1);
				}
			}
		//enemies
		if(state.enemies){
			offset=state.enemies;
			while(readU16(rom->buffer, offset)!=Enemy::SENTINEL){
				index.set(offset, Enemy::SIZE, Rom::HACKABLE);
				offset+=Enemy::SIZE;
			}
			index.set(offset, 2, Rom::HACKABLE);
		}
		//post load modifications
		if(state.plm){
			offset=state.plm;
			while(readU16(rom->buffer, offset)!=Plm::SENTINEL){
				index.set(offset, Plm::SIZE, Rom::HACKABLE);
				offset+=Plm::SIZE;
			}
			index.set(offset, 2, Rom::HACKABLE);
		}
	}
	//doors
	index.set(indexingHeader.doors, 2*doorsInRoom, Rom::HACKABLE);
	for(unsigned i=0; i<doorsInRoom; ++i)
		index.set(
			loRomToOffset(Transition::BANK, readU16(rom->buffer, indexingHeader.doors+2*i)),
			Transition::SIZE,
			Rom::HACKABLE
		);
	//finish
	return true;
}

bool Room::open(uint32_t offset){
	//header
	header=Header(rom->buffer, offset);
	//states
	states.clear();
	for(unsigned i=0; i<header.stateInfo.size(); ++i)
		states.push_back(State(rom->buffer, header.stateInfo[i].state));
	//stuff that can be shared between states
	scroll.clear();
	tiles.clear();
	enemies.clear();
	plm.clear();
	unsigned s, doorsInRoom=0;
	for(s=0; s<states.size(); ++s){
		State& state=states[s];
		//scroll data
		if(state.scroll>=0x8000u){
			if(scroll.find(state.scroll)==scroll.end()){
				scroll[state.scroll].resize(header.width, header.height);
				readU82D(rom->buffer, state.scroll, scroll[state.scroll]);
			}
		}
		else if(state.scroll>=2) return false;
		//tile data
		if(tiles.find(state.tiles)==tiles.end()){
			Buffer buffer;
			decompress(rom->buffer, state.tiles, &buffer);
			Array2D<Tile>& stateTiles=tiles[state.tiles];
			stateTiles.resize(header.width*SCREEN_SIZE, header.height*SCREEN_SIZE);
			uint16_t roomSize=readU16(buffer, 0);
			for(unsigned i=0; i<stateTiles.readISize(); ++i)
				for(unsigned j=0; j<stateTiles.readJSize(); ++j){
					stateTiles.at(i, j).layer1=TileLayer(buffer, 2+2*(i+j*stateTiles.readISize()));
					stateTiles.at(i, j).bts=buffer[2+roomSize+i+j*stateTiles.readISize()];
					if(buffer.size()>unsigned(2+roomSize+roomSize/2)){
						stateTiles.at(i, j).layer2=TileLayer(buffer, 2+roomSize+roomSize/2+2*(i+j*stateTiles.readISize()));
						stateTiles.at(i, j).hasLayer2=true;
					}
					else stateTiles.at(i, j).hasLayer2=false;
					if(stateTiles.at(i, j).layer1.property==9)
						doorsInRoom=max(doorsInRoom, unsigned(stateTiles.at(i, j).bts+1));
				}
		}
		//enemies
		if(state.enemies&&enemies.find(state.enemies)==enemies.end()){
			offset=state.enemies;
			while(readU16(rom->buffer, offset)!=Enemy::SENTINEL){
				enemies[state.enemies].push_back(Enemy(rom->buffer, offset));
				offset+=Enemy::SIZE;
			}
		}
		//post load modifications
		if(state.plm&&plm.find(state.plm)==plm.end()){
			offset=state.plm;
			while(readU16(rom->buffer, offset)!=Plm::SENTINEL){
				plm[state.plm].push_back(Plm(rom->buffer, offset));
				offset+=Plm::SIZE;
			}
		}
	}
	//doors
	doors.clear();
	for(unsigned i=0; i<doorsInRoom; ++i)
		doors.push_back(loRomToOffset(Transition::BANK, readU16(rom->buffer, header.doors+2*i)));
	//finish
	stateIndex=s-1;
	return true;
}

bool Room::save(uint32_t& offset){
	//map identifier to written location
	map<uint32_t, uint32_t> scrollHacks;
	map<uint32_t, uint32_t> tileHacks;
	map<uint32_t, uint32_t> enemyHacks;
	map<uint32_t, uint32_t> plmHacks;
	//stuff that can be shared between states
	for(unsigned s=0; s<states.size(); ++s){
		State& state=states[s];
		//scroll data
		if(state.scroll>=0x8000u){
			if(scrollHacks.find(state.scroll)==scrollHacks.end()){
				if(!rom->takeSpace(State::SCROLL_BANK, scroll[state.scroll].size(), offset))
					return false;
				scrollHacks[state.scroll]=offset;
				writeU82D(rom->buffer, offset, scroll[state.scroll]);
			}
			state.scroll=scrollHacks[state.scroll];
		}
		//tile data
		if(tileHacks.find(state.tiles)==tileHacks.end()){
			Buffer buffer;
			buffer.resize(2);
			Array2D<Tile>& stateTiles=tiles[state.tiles];
			writeU16(buffer, 0, 2*stateTiles.readISize()*stateTiles.readJSize());
			for(unsigned j=0; j<stateTiles.readJSize(); ++j)
				for(unsigned i=0; i<stateTiles.readISize(); ++i)
					stateTiles.at(i, j).layer1.write(buffer);
			for(unsigned j=0; j<stateTiles.readJSize(); ++j)
				for(unsigned i=0; i<stateTiles.readISize(); ++i)
					buffer.push_back(stateTiles.at(i, j).bts);
			if(stateTiles.at(0, 0).hasLayer2)
				for(unsigned j=0; j<stateTiles.readJSize(); ++j)
					for(unsigned i=0; i<stateTiles.readISize(); ++i)
						stateTiles.at(i, j).layer2.write(buffer);
			Buffer compressed;
			compress(buffer, compressed);
			if(!rom->takeSpace(Tile::FIRST_BANK, Tile::LAST_BANK, (uint16_t)compressed.size(), offset))
				return false;
			tileHacks[state.tiles]=offset;
			for(unsigned i=0; i<compressed.size(); ++i) rom->buffer[offset+i]=compressed[i];
		}
		state.tiles=tileHacks[state.tiles];
		//enemies
		if(enemies[state.enemies].size()){
			if(enemyHacks.find(state.enemies)==enemyHacks.end()){
				if(!rom->takeSpace(Enemy::BANK, uint16_t(enemies[state.enemies].size()*Enemy::SIZE+2), offset))
					return false;
				enemyHacks[state.enemies]=offset;
				for(unsigned i=0; i<enemies[state.enemies].size(); ++i){
					enemies[state.enemies][i].write(rom->buffer, offset);
					offset+=Enemy::SIZE;
				}
				writeU16(rom->buffer, offset, Enemy::SENTINEL);
			}
			state.enemies=enemyHacks[state.enemies];
		}
		else state.enemies=0;
		//post load modifications
		if(plm[state.plm].size()){
			if(plmHacks.find(state.plm)==plmHacks.end()){
				if(!rom->takeSpace(Plm::BANK, uint16_t(plm[state.plm].size()*Plm::SENTINEL+2), offset))
					return false;
				plmHacks[state.plm]=offset;
				for(unsigned i=0; i<plm[state.plm].size(); ++i){
					plm[state.plm][i].write(rom->buffer, offset);
					offset+=Plm::SIZE;
				}
				writeU16(rom->buffer, offset, Plm::SENTINEL);
			}
			state.plm=plmHacks[state.plm];
		}
		else state.plm=0;
		//state
		if(header.stateInfo[s].code!=Header::STANDARD){
			if(!rom->takeSpace(State::BANK, State::SIZE, header.stateInfo[s].state))
				return false;
			state.write(rom->buffer, header.stateInfo[s].state);
		}
	}
	//doors
	if(!rom->takeSpace(Header::DOOR_BANK, uint16_t(2*doors.size()), header.doors))
		return false;
	for(unsigned i=0; i<doors.size(); ++i)
		writeU16(rom->buffer, header.doors+2*i, offsetToLoRom16(doors[i]));
	//header and default state
	if(!rom->takeSpace(Header::BANK, header.size()+State::SIZE, offset))
		return false;
	header.write(rom->buffer, offset);
	states.back().write(rom->buffer, offset+header.size());
	//finish
	rekey(scroll, scrollHacks);
	rekey(tiles, tileHacks);
	rekey(enemies, enemyHacks);
	rekey(plm, plmHacks);
	return true;
}

void Room::loadGraphics(){
	uint32_t tileSetPointer=tileSetOffset(states[stateIndex].tileSet);
	//get palette
	Buffer buffer;
	decompress(rom->buffer, loRomToOffset(readU24(rom->buffer, tileSetPointer+6)), &buffer);
	vector<Color> palette;
	for(unsigned i=0; i<buffer.size(); i+=2){
		uint16_t p=buffer[i+1]<<8|buffer[i];
		palette.push_back(Color(
			1.0f*(p>> 0&0x1Fu)/0x1Fu,
			1.0f*(p>> 5&0x1Fu)/0x1Fu,
			1.0f*(p>>10&0x1Fu)/0x1Fu,
			1.0f
		));
	}
	//get tiles
	tileSet.clear();
	mode7TileSet.clear();
	buffer.clear();
	//handle mode 7
	decompress(rom->buffer, loRomToOffset(readU24(rom->buffer, tileSetPointer+3)), &buffer);
	if(Mode7::FIRST_TILE_SET<=states[stateIndex].tileSet&&states[stateIndex].tileSet<=Mode7::LAST_TILE_SET){
		mode7.open(states[stateIndex].tileSet);
		mode7TileSet.resize(256);
		for(unsigned i=0; i<mode7TileSet.size(); ++i){
			mode7TileSet[i].resize(TILE_SIZE/2, TILE_SIZE/2);
			for(unsigned x=0; x<TILE_SIZE/2; ++x)
				for(unsigned y=0; y<TILE_SIZE/2; ++y)
					mode7TileSet[i].at(x, y)=palette[buffer[2*(TILE_SIZE/2*TILE_SIZE/2*i+8*y+x)+1]];
		}
		buffer.clear();
		if(states[stateIndex].layerHandling==VANILLA_CERES_RIDLEY_ROOM_LAYER_HANDLING)
			for(unsigned i=0; i<0x2000u; ++i)
				buffer.push_back(rom->buffer[0x182000u+i]);
	}
	else mode7.clear();
	//get subtiles
	if(states[stateIndex].tileSet==26) buffer.resize(0x8000u);//Kraid room
	else buffer.resize(0x5000u);
	Buffer furtherBuffer;
	bool loadCommonRoomElements=header.region!=6&&!mode7TileSet.size();
	if(loadCommonRoomElements) decompress(rom->buffer, 0x1C8000u, &furtherBuffer);//common room elements
	for(unsigned i=0; i<furtherBuffer.size(); ++i) buffer.push_back(furtherBuffer[i]);
	for(unsigned i=0; i<buffer.size(); i+=32){
		uint8_t copy[32];
		for(unsigned j=0; j<32; ++j){
			copy[j]=buffer[i+j];
			buffer[i+j]=0;
		}
		for(unsigned y=0; y<8; y++){
			uint8_t line[4];
			line[0]=copy[y*2];
			line[1]=copy[y*2+1];
			line[2]=copy[y*2+16];
			line[3]=copy[y*2+17];
			for(unsigned x=0; x<8; x++){
				unsigned shift=(7-x)*4;
				uint32_t word=0;
				for(unsigned j=0; j<4; ++j) word+=(line[j]&1)<<(shift+j);
				for(unsigned j=0; j<4; ++j){
					buffer[i+y*4+j]|=word>>(8*j)&0xFFu;
					line[j]>>=1;
				}
			}
		}
	}
	Buffer subtiles;
	for(unsigned i=0; i<buffer.size(); ++i){
		subtiles.push_back(buffer[i]&0xFu);
		subtiles.push_back(buffer[i]>>4);
	}
	//get tile assemblers
	vector<TileAssembler> tileAssemblers;
	buffer.clear();
	if(loadCommonRoomElements) decompress(rom->buffer, 0x1CA09Du, &buffer);//common room elements
	furtherBuffer.clear();
	decompress(rom->buffer, loRomToOffset(readU24(rom->buffer, tileSetPointer)), &furtherBuffer);
	for(unsigned i=0; i<furtherBuffer.size(); ++i) buffer.push_back(furtherBuffer[i]);
	for(unsigned i=0; i<buffer.size(); i+=8){
		tileAssemblers.push_back(TileAssembler(
			buffer[i+1]<<8|buffer[i+0],
			buffer[i+3]<<8|buffer[i+2],
			buffer[i+5]<<8|buffer[i+4],
			buffer[i+7]<<8|buffer[i+6]
		));
	}
	//assemble subtiles into tiles
	tileSet.resize(tileAssemblers.size());
	for(unsigned i=0; i<tileAssemblers.size(); ++i){
		tileSet[i].resize(TILE_SIZE, TILE_SIZE);
		drawSubtile(subtiles, tileAssemblers[i].ul, palette, tileSet[i], 0          , 0);
		drawSubtile(subtiles, tileAssemblers[i].ur, palette, tileSet[i], TILE_SIZE/2, 0);
		drawSubtile(subtiles, tileAssemblers[i].dl, palette, tileSet[i], 0          , TILE_SIZE/2);
		drawSubtile(subtiles, tileAssemblers[i].dr, palette, tileSet[i], TILE_SIZE/2, TILE_SIZE/2);
	}
}

void Room::drawTileSet(Array2D<Color>& destination, unsigned tilesWide) const{
	destination.resize(tilesWide*TILE_SIZE, (tileSet.size()/tilesWide+1)*TILE_SIZE+(mode7TileSet.size()/(tilesWide/2)+1)*TILE_SIZE/2);
	for(unsigned i=0; i<tileSet.size(); ++i)
		for(unsigned x=0; x<TILE_SIZE; ++x)
			for(unsigned y=0; y<TILE_SIZE; ++y)
				destination.at(i%tilesWide*TILE_SIZE+x, i/tilesWide*TILE_SIZE+y)=tileSet[i].at(x, y);
	if(mode7.tiles.readISize())
		for(unsigned i=0; i<mode7TileSet.size(); ++i)
			for(unsigned x=0; x<TILE_SIZE/2; ++x)
				for(unsigned y=0; y<TILE_SIZE/2; ++y)
					destination.at(i%(tilesWide*2)*TILE_SIZE/2+x, (tileSet.size()/tilesWide+1)*TILE_SIZE+i/(tilesWide*2)*TILE_SIZE/2+y)=mode7TileSet[i].at(x, y);
}

void Room::getQuadsVertexArray(vector<Vertex>& vertices, unsigned tilesWide, bool showLayer1, bool showLayer2, bool showMode7) const{
	if(showMode7)
		for(unsigned i=0; i<mode7.tiles.readISize(); ++i)
			for(unsigned j=0; j<mode7.tiles.readJSize(); ++j){
				unsigned tileX=mode7.tiles.at(i, j)%(tilesWide*2)*TILE_SIZE/2;
				unsigned tileY=mode7.tiles.at(i, j)/(tilesWide*2)*TILE_SIZE/2+(tileSet.size()/tilesWide+1)*TILE_SIZE;
				unsigned txi=tileX, txf=tileX+TILE_SIZE/2-1, tyi=tileY, tyf=tileY+TILE_SIZE/2-1;
				vertices.push_back(Vertex((i+0)*TILE_SIZE/2, (j+0)*TILE_SIZE/2, txi, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE/2, (j+0)*TILE_SIZE/2, txf, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE/2, (j+1)*TILE_SIZE/2, txf, tyf));
				vertices.push_back(Vertex((i+0)*TILE_SIZE/2, (j+1)*TILE_SIZE/2, txi, tyf));
			}
	if(showLayer2)
		for(unsigned i=0; i<readStateTilesConst().readISize(); ++i)
			for(unsigned j=0; j<readStateTilesConst().readJSize(); ++j){
				const Tile& tile=readStateTilesConst().at(i, j);
				if(!tile.hasLayer2) continue;
				unsigned tileX=tile.layer2.index%tilesWide*TILE_SIZE;
				unsigned tileY=tile.layer2.index/tilesWide*TILE_SIZE;
				unsigned txi=tileX, txf=tileX+TILE_SIZE-1, tyi=tileY, tyf=tileY+TILE_SIZE-1;
				if(tile.layer2.flipH){
					txi=tileX+TILE_SIZE-1;
					txf=tileX;
				}
				if(tile.layer2.flipV){
					tyi=tileY+TILE_SIZE-1;
					tyf=tileY;
				}
				vertices.push_back(Vertex((i+0)*TILE_SIZE, (j+0)*TILE_SIZE, txi, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE, (j+0)*TILE_SIZE, txf, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE, (j+1)*TILE_SIZE, txf, tyf));
				vertices.push_back(Vertex((i+0)*TILE_SIZE, (j+1)*TILE_SIZE, txi, tyf));
			}
	if(showLayer1)
		for(unsigned i=0; i<readStateTilesConst().readISize(); ++i)
			for(unsigned j=0; j<readStateTilesConst().readJSize(); ++j){
				const Tile& tile=readStateTilesConst().at(i, j);
				unsigned tileX=tile.layer1.index%tilesWide*TILE_SIZE;
				unsigned tileY=tile.layer1.index/tilesWide*TILE_SIZE;
				unsigned txi=tileX, txf=tileX+TILE_SIZE-1, tyi=tileY, tyf=tileY+TILE_SIZE-1;
				if(tile.layer1.flipH){
					txi=tileX+TILE_SIZE-1;
					txf=tileX;
				}
				if(tile.layer1.flipV){
					tyi=tileY+TILE_SIZE-1;
					tyf=tileY;
				}
				vertices.push_back(Vertex((i+0)*TILE_SIZE, (j+0)*TILE_SIZE, txi, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE, (j+0)*TILE_SIZE, txf, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE, (j+1)*TILE_SIZE, txf, tyf));
				vertices.push_back(Vertex((i+0)*TILE_SIZE, (j+1)*TILE_SIZE, txi, tyf));
			}
}

bool Room::readDoorWithPixelPosition(unsigned x, unsigned y, Transition& transition){
	if(!convertScreenToTile(x, y)) return false;
	if(readStateTiles().at(x, y).layer1.property!=9) return false;
	transition=Transition(*rom);
	transition.open(doors[readStateTiles().at(x, y).bts]);
	return true;
}

Tile& Room::getTile(unsigned x, unsigned y){
	return readStateTiles().at(x, y);
}

uint8_t& Room::getMode7Tile(unsigned x, unsigned y){
	return mode7.tiles.at(x, y);
}

Header::Code Room::readStateCode(unsigned i) const{
	return header.stateInfo[i].code;
}

bool Room::convertScreenToTile(unsigned& x, unsigned& y) const{
	x/=TILE_SIZE;
	y/=TILE_SIZE;
	return x<readStateTilesConst().readISize()&&y<readStateTilesConst().readJSize();
}

bool Room::visible(unsigned x, unsigned y) const{
	if(x>=readTilesWide()||y>=readTilesHigh()) return false;
	if(states[stateIndex].scroll<0x8000u) return states[stateIndex].scroll!=0;
	return scroll.find(states[stateIndex].scroll)->second.at(x/SCREEN_SIZE, y/SCREEN_SIZE)!=0;
}

//=====functions=====//
string sm::musicControlDescription(uint8_t musicControl){
	switch(musicControl){
		case 0: return "No Change";
		case 1: return "Samus appear";
		case 2: return "Aquire item";
		case 3: return "Elevator";
		case 4: return "Hall before statues";
		case 5: return "No change/Song One";
		case 6: return "Song Two";
		case 7: return "Mute";
		default: break;
	}
	stringstream ss;
	ss<<musicControl;
	string s;
	ss>>s;
	return s;
}

string sm::musicTrackDescription(uint8_t musicTrack){
	switch(musicTrack){
		case 0x00u: return "None";
		case 0x03u: return "Title Screen";
		case 0x06u: return "Empty Crateria";
		case 0x09u: return "Space Pirate";
		case 0x0Cu: return "Samus Theme";
		case 0x0Fu: return "Green Brinstar";
		case 0x12u: return "Red Brinstar";
		case 0x15u: return "Upper Norfair";
		case 0x18u: return "Lower Norfair";
		case 0x1Bu: return "Maridia";
		case 0x1Eu: return "Tourian";
		case 0x21u: return "Mother Brain";
		case 0x24u: return "Boss Fight 1(04,05,06,80)";
		case 0x27u: return "Boss Fight 2(04,05,06)";
		case 0x2Au: return "Miniboss Fight";
		case 0x2Du: return "Ceres";
		case 0x30u: return "Wrecked Ship";
		case 0x33u: return "Zebes Boom";
		case 0x36u: return "Intro";
		case 0x39u: return "Death";
		case 0x3Cu: return "Credits";
		case 0x3Fu: return "The last metroid is in captivity";
		case 0x42u: return "The galaxy is at peace";
		case 0x45u: return "Large Metroid";
		case 0x48u: return "Samus Theme";
		default: break;
	}
	stringstream ss;
	ss<<musicTrack;
	string s;
	ss>>s;
	return s;
}
