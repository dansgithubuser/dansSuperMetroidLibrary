#include "sm.hpp"

#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>
#include <map>
#include <iostream>

using namespace std;
using namespace sm;

const unsigned MAX_BLOCK_LENGTH=1024;

const U32 sm::ROOM_OFFSETS[ROOMS]={
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

enum{
	STANDARD=0xE5E6u,
	DOORS=0xE5EBu,
	TOURIAN_BOSS_1=0xE5FFu,
	EVENTS=0xE612u,
	BOSSES=0xE629u,
	MORPH=0xE640u,
	MORPH_AND_MISSILES=0xE652u,
	POWER_BOMBS=0xE669u,
	SPEED_BOOSTER=0xE678u
};

unsigned lzDecompress(const Buffer& source, U32 offset, U32 length, unsigned bytes, U8 mask, bool absolute, Buffer& destination){
	assert(bytes==1||bytes==2);
	assert(mask==0||mask==0xFFu);
	int from=source[offset];
	if(bytes==2) from|=source[offset+1]<<8;
	if(!absolute) from=destination.size()-from;
	if(from>=0)
		for(unsigned i=0; i<length; ++i)
			destination.push_back(destination[from+i]^mask);
	return bytes;
}

unsigned decompress(const Buffer& source, U32 offset, Buffer& destination){
	unsigned initialOffset=offset;
	while(true){
		if(source[offset]==0xFF) break;//done
		U8 op;
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
					destination.push_back(source[offset]);
					++offset;
				}
				break;
			case 1://1 byte run length encoding
				for(unsigned i=0; i<length; ++i) destination.push_back(source[offset]);
				++offset;
				break;
			case 2://2 byte run length encoding
				for(unsigned i=0; i<length; ++i) destination.push_back(source[offset+i%2]);
				offset+=2;
				break;
			case 3://gradient run length encoding
				for(unsigned i=0; i<length; ++i) destination.push_back((source[offset]+i)%0x100u);
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

void putBlockHeader(Buffer& destination, U8 op, unsigned length){
	--length;
	if(length>0x1Fu||op==7u){
		destination.push_back(0xE0u|op<<2|(length&0x300u)>>8);
		destination.push_back(length&0xFFu);
	}
	else destination.push_back(op<<5|length);
}

void rleCompress(const Buffer& source, U32 offset, U32& length, U8 op, Buffer& destination){
	unsigned bytes=1, gradient=0;
	switch(op){
		case 1: break;
		case 2: bytes=2; break;
		case 3: gradient=1; break;
		default: assert(false);
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

template<class T> void eject(vector<T>& v, unsigned i){
	v[i]=v.back();
	v.pop_back();
}

class LzCompressor{
	public:
		LzCompressor(const Buffer& source): source(source) {
			for(unsigned i=0; i<source.size(); ++i)
				offsets[source[i]].push_back(i);
		}
		void compress(U32 offset, U32& length, U8 op, Buffer& destination){
			unsigned bytes=1;
			U8 mask=0;
			bool absolute=true;
			switch(op){
				case 4: bytes=2; break;
				case 5: bytes=2; mask=0xFFu; break;
				case 6: absolute=false; break;
				case 7: mask=0xFFu; absolute=false; break;
				default: assert(false);
			}
			U32 lowest=0, highest=offset;
			if(absolute){
				if(bytes==2) highest=min(0x10000u, highest);
				else highest=min(0x100u, highest);
			}
			else{
				if(bytes==2) lowest=max(int(offset-0xFFFFu), 0);
				else lowest=max(int(offset-0xFFu), 0);
			}
			//build Knuth–Morris–Pratt table
			int table[MAX_BLOCK_LENGTH];
			const unsigned wordLength=min(MAX_BLOCK_LENGTH, source.size()-offset);
			table[0]=-1;
			table[1]=0;
			unsigned i=2, j=0;
			while(i<wordLength){
				if(source[offset+i-1]==source[offset+j]){
					++j;
					table[i]=j;
					++i;
				}
				else if(j>0) j=table[j];
				else{
					table[i]=0;
					++i;
				}
			}
			//find longest match using Knuth–Morris–Pratt algorithm
			unsigned bestStart=0, bestLength=0, nextOffsetToTry=0;
			while(nextOffsetToTry<offsets[source[offset]^mask].size()){
				i=offsets[source[offset]^mask][nextOffsetToTry];
				if(i>=lowest) break;
				++nextOffsetToTry;
			}
			if(nextOffsetToTry>=offsets[source[offset]^mask].size()){
				length=0;
				return;
			}
			j=0;//offset into string being searched for
			while(i+j<highest||(j!=0&&i<offset&&i+j<highest+MAX_BLOCK_LENGTH&&i+j<source.size())){
				if(source[offset+j]==(source[i+j]^mask)){
					++j;
					if(j>bestLength){
						bestStart=i;
						bestLength=j;
					}
					if(j==wordLength) break;
				}
				else{
					i+=j-table[j];
					if(table[j]>=0) j=table[j];
					else{
						j=0;
						//advance i based on index of source
						while(true){
							++nextOffsetToTry;
							if(nextOffsetToTry>=offsets[source[offset]^mask].size()) break;
							if(offsets[source[offset]^mask][nextOffsetToTry]>=i) break;
						}
						if(nextOffsetToTry>=offsets[source[offset]^mask].size()) break;
						else i=offsets[source[offset]^mask][nextOffsetToTry];
					}
				}
			}
			//apply
			length=bestLength;
			putBlockHeader(destination, op, length);
			if(!absolute) bestStart=offset-bestStart;
			destination.push_back(bestStart);
			if(bytes==2) destination.push_back(bestStart>>8);
		}
	private:
		const Buffer& source;
		vector<unsigned> offsets[256];
};

void noCompress(const Buffer& source, U32 offset, U32 length, Buffer& destination){
	putBlockHeader(destination, 0, length);
	for(unsigned i=0; i<length; ++i)
		destination.push_back(source[offset-length+i]);
}

void compress(const Buffer& source, Buffer& destination){
	unsigned i=0;
	unsigned noCompressionLength=0;
	LzCompressor lzc(source);
	while(i<source.size()){
		//attempt all strategies greedily
		const unsigned stategies=7;
		Buffer block[stategies];
		unsigned length[stategies];
		rleCompress(source, i, length[0], 1, block[0]);
		rleCompress(source, i, length[1], 2, block[1]);
		rleCompress(source, i, length[2], 3, block[2]);
		lzc.compress(i, length[3], 4, block[3]);
		lzc.compress(i, length[4], 5, block[4]);
		lzc.compress(i, length[5], 6, block[5]);
		lzc.compress(i, length[6], 7, block[6]);
		//find best op to use
		unsigned bestOp=0, bestSourceLength=1, bestDestinationLength=1;
		for(unsigned j=0; j<stategies; ++j){
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
			if(i>=source.size()) noCompress(source, i, noCompressionLength, destination);
		}
		else{
			//actually stick in the no compression block first if it exists
			if(noCompressionLength!=0){
				noCompress(source, i, noCompressionLength, destination);

cerr<<"0 "<<noCompressionLength<<" "<<destination.size()<<" c\n";

				noCompressionLength=0;
			}
			//stick in the compressed block
			for(unsigned j=0; j<block[bestOp-1].size(); ++j)
				destination.push_back(block[bestOp-1][j]);

cerr<<(unsigned)bestOp<<" "<<length[bestOp-1]<<" "<<destination.size()<<" c\n";

			i+=length[bestOp-1];
		}
	}
	destination.push_back(0xFFu);//done
}

//convert cpu address to rom address
//not sure if values returned are correct when address&0x8000u==0x8000u
U32 cpuToRom(U32 address){ return (address&0x7F0000u)>>1|(address&0x7FFFu); }

U16 readU16(const Buffer& buffer, U32 offset){
	return buffer[offset]|buffer[offset+1]<<8;
}

void writeU16(Buffer& buffer, U16 value){
	buffer.push_back(value>>0&0xFFu);
	buffer.push_back(value>>8&0xFFu);
}

U32 readU24(const Buffer& buffer, U32 offset){
	return buffer[offset]|buffer[offset+1]<<8|buffer[offset+2]<<16;
}

void writeU24(Buffer& buffer, U32 value){
	buffer.push_back(value>> 0&0xFFu);
	buffer.push_back(value>> 8&0xFFu);
	buffer.push_back(value>>16&0xFFu);
}

U32 readPointer(const Buffer& buffer, U32 offset){
	return cpuToRom(readU24(buffer, offset));
}

void readU82D(const Buffer& buffer, U32 offset, Array2D<U8>& u82d){
	for(unsigned i=0; i<u82d.readISize(); ++i)
		for(unsigned j=0; j<u82d.readJSize(); ++j){
			u82d.at(i, j)=buffer[offset];
			++offset;
		}
}

string musicControlDescription(U8 musicControl){
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

string musicTrackDescription(U8 musicTrack){
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

void drawSubTile(const Buffer& subTiles, U16 tileInfo, const vector<Color>& palette, Array2D<Color>& destination, unsigned x, unsigned y){
	U8 xMask=(tileInfo&0x4000)?7:0;
	U8 yMask=(tileInfo&0x8000)?7:0;
	U8 hi=(tileInfo&0x1C00u)>>6;
	for(unsigned ty=0; ty<8; ++ty){
		for(unsigned tx=0; tx<8; ++tx){
			U8 lo=subTiles[(tileInfo&0x3FFu)*64+(tx^xMask)+(ty^yMask)*8];
			if(lo) destination.at(x+tx, y+ty)=palette[hi|lo];
			else destination.at(x+tx, y+ty)=Color();
		}
	}
}

//=====class Rom=====//
string Rom::open(string fileName){
	header.clear();
	buffer.clear();
	ifstream file(fileName.c_str(), ios::binary);
	char c;
	while(file.get(c)) buffer.push_back(c);
	file.close();
	if(buffer.size()%32768!=0){
		for(unsigned i=0; i<512; ++i) header.push_back(buffer[i]);
		buffer.erase(buffer.begin(), buffer.begin()+512);
	}
	if(buffer[0x7FD9u]>=2)
		return "ROM is PAL. This doesn't work on PAL ROMs.";
	return "";
}

void Rom::save(string fileName){
	ofstream file(fileName.c_str(), ios::binary);
	for(unsigned i=0; i<header.size(); ++i) file.put(header[i]);
	for(unsigned i=0; i<buffer.size(); ++i) file.put(buffer[i]);
	file.close();
}

//=====struct RoomHeader=====//
RoomHeader::RoomHeader(const Buffer& buffer, unsigned offset):
	index(buffer[offset]),
	region(buffer[offset+1]),
	x(buffer[offset+2]),
	y(buffer[offset+3]),
	width(buffer[offset+4]),
	height(buffer[offset+5]),
	upScroller(buffer[offset+6]),
	downScroller(buffer[offset+7]),
	graphicsFlags(buffer[offset+8]),
	doors(readU16(buffer, offset+9))
{}

//=====struct RoomState=====//
RoomState::RoomState(const Buffer& buffer, U32 offset):
	data(readU24(buffer, offset)),
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
{}

//=====struct TileLayer=====//
TileLayer::TileLayer(const Buffer& buffer, U32 offset):
	index(readU16(buffer, offset)&0x3FFu),
	flipH(buffer[offset+1]&4),
	flipV(buffer[offset+1]&8),
	property(buffer[offset+1]>>4)
{}

void TileLayer::write(Buffer& buffer){
	buffer.push_back(index&0xFFu);
	buffer.push_back(property<<4|(flipV?8:0)|(flipH?4:0)|index>>8);
}

//=====struct Door=====//
Door::Door(const Buffer& buffer, U32 offset):
	room(readU16(buffer, offset)),
	doorBitFlag(buffer[offset+3]),
	direction(buffer[offset+4]),
	illusionX(buffer[offset+5]),
	illusionY(buffer[offset+6]),
	x(buffer[offset+7]),
	y(buffer[offset+8]),
	distance(readU16(buffer, offset+9)),
	scrollData(readU16(buffer, offset+11))
{
	if(room!=0) room=cpuToRom(0x8F0000u|room);
}

//=====class Room=====//
Room::Room(Rom& rom): rom(rom) {}

bool Room::open(U32 offset, int stateIndex){
	//get room header
	header=RoomHeader(rom.buffer, offset);
	offset+=RoomHeader::SIZE;
	//get all room codes
	stateCodes.clear();
	bool foundState=false;
	U32 stateCodeOffset=0;
	for(int i=0; true; ++i){
		U16 stateCode=readU16(rom.buffer, offset);
		switch(stateCode){
			case STANDARD: offset+=2; break;
			case DOORS: offset+=6; break;
			case TOURIAN_BOSS_1: offset+=4; break;
			case EVENTS: offset+=5; break;
			case BOSSES: offset+=5; break;
			case MORPH: offset+=4; break;
			case MORPH_AND_MISSILES: offset+=4; break;
			case POWER_BOMBS: offset+=4; break;
			case SPEED_BOOSTER: offset+=4; break;
			default: return false;
		}
		stateCodes.push_back(stateCode);
		if(i==stateIndex||(stateCode==STANDARD&&!foundState)){
			foundState=true;
			stateCodeOffset=offset;
			if(stateCode==EVENTS||stateCode==BOSSES) stateCodeValue=rom.buffer[offset-3];
		}
		if(stateCode==STANDARD) break;
	}
	if(stateIndex>=0&&stateIndex<(int)stateCodes.size()&&stateCodes[stateIndex]!=STANDARD)
		stateCodeOffset=cpuToRom(0x8F0000u|readU16(rom.buffer, stateCodeOffset-2));
	//get room state
	state=RoomState(rom.buffer, stateCodeOffset);
	//get scroll data
	scroll.clear();
	scroll.resize(header.width, header.height);
	if(state.scroll>=0x8000u)
		readU82D(rom.buffer, cpuToRom(0x8F0000u|state.scroll), scroll);
	//get tile data
	Buffer buffer;
	originalCompressedTileDataSize=decompress(rom.buffer, cpuToRom(state.data), buffer);
	tiles.resize(header.width*CHUNK_SIZE, header.height*CHUNK_SIZE);
	U16 roomDataSize=readU16(buffer, 0);
	unsigned doorsInRoom=0;
	for(unsigned i=0; i<tiles.readISize(); ++i)
		for(unsigned j=0; j<tiles.readJSize(); ++j){
			tiles.at(i, j).layer1=TileLayer(buffer, 2+2*(i+j*tiles.readISize()));
			tiles.at(i, j).bts=buffer[2+roomDataSize+i+j*tiles.readISize()];
			if(buffer.size()>unsigned(2+roomDataSize+roomDataSize/2)){
				tiles.at(i, j).layer2=TileLayer(buffer, 2+roomDataSize+roomDataSize/2+2*(i+j*tiles.readISize()));
				tiles.at(i, j).hasLayer2=true;
			}
			else tiles.at(i, j).hasLayer2=false;
			if(tiles.at(i, j).layer1.property==9&&unsigned(tiles.at(i, j).bts+1)>doorsInRoom) doorsInRoom=tiles.at(i, j).bts+1;
		}
	//load doors
	doors.clear();
	for(unsigned i=0; i<doorsInRoom; ++i)
		doors.push_back(Door(
			rom.buffer,
			cpuToRom(0x830000u|readU16(
				rom.buffer,
				cpuToRom(0x8F0000u|header.doors)+2*i)
			)
		));
/*
    'get location for enemy data
    Dim EnPopBank As Byte   'finding bank for enemy population (A1 or by my code)
    Open needslash For Binary As #1
    'get the new EnPopBank before openning enemies
    Dim TempThreeVar As ThreeByte
    TempThreeVar.Byte1 = Val("&H" & Right$(Right$("0000" & Hex$(U1Pointer), 4), 2) & "&")
    TempThreeVar.Byte2 = Val("&H" & Left$(Right$("0000" & Hex$(U1Pointer), 4), 2) & "&")
    TempThreeVar.Byte3 = &H80&
    Get #1, ThreePoint2Offset(TempThreeVar) + ROM_HEADER + 1, MyRoomVar
    Get #1, &H100B5B + ROM_HEADER + 1, EnPopBank    'first pointer to EnPop Code
    If EnPopBank <> Val("&H20") Then    'if no JSR
        EnPopBank = Val("&HA1&")
    Else    'if JSR found, my code is in the ROM
        EnPopBank = MyRoomVar.MultiVar1 And Val("&H0020")
        If EnPopBank = &H20 Then
            Get #1, &H107FC3 + ROM_HEADER + 1, EnPopBank    'first instance of new bank listed in my code
        Else
            EnPopBank = Val("&HA1&")
        End If
    End If
    Close #1

    'Drewseph override for the old enpop coding
    If UCase$(Right$(App.EXEName, 1)) = "D" And Val("&H" & RoomHeader1.RoomIndexText1.Text & "&") >= &HA0& Then
        EnPopBank = Val("&HEF&")
    End If
    'Scyzer override for the old enpop coding
    If UCase$(Right$(App.EXEName, 1)) = "S" And Val("&H" & RoomHeader1.RoomIndexText1.Text & "&") >= &H80& Then
        EnPopBank = Val("&A2&")
    End If

    'get location for enemy data
    TempThree.Byte1 = Val("&H" & Right$(InHex(MyState.EnemyPopulation, 4), 2) & "&")
    TempThree.Byte2 = Val("&H" & Left$(InHex(MyState.EnemyPopulation, 4), 2) & "&")
    TempThree.Byte3 = EnPopBank    'A1, unless new bank
    Smile.EnemyLabel.Caption = Hex$(ThreePoint2Offset(TempThree))
    If TempThree.Byte1 = 0 And TempThree.Byte2 = 0 Then Smile.EnemyLabel.Caption = "000000"

    'get location for PLM data
    TempThree.Byte1 = Val("&H" & Right$(InHex(MyState.PLM, 4), 2) & "&")
    TempThree.Byte2 = Val("&H" & Left$(InHex(MyState.PLM, 4), 2) & "&")
    'TempThree.Byte3 = Val("&H8F&")
    TempThree.Byte3 = PLMBank   '8F by default
    Smile.PLMLabel.Caption = Hex$(ThreePoint2Offset(TempThree))
    If TempThree.Byte1 = 0 And TempThree.Byte2 = 0 Then Smile.PLMLabel.Caption = "000000"

    'refresh controls
    Smile.MenuPicture1.Refresh
    Smile.StateGraphicSetCombo1.Tag = 1
    States1.States1_Load_Again
    PointerForm1.RefreshPointerForm

    'load fx1 data and decide whether or not to display it
    FX1Displacement = 0
    Do
        DisplayFX1 = FigureFX1Data
        Select Case Right$("0000" & Hex$(TempFX1.Select), 4)
            Case "0000": Exit Do
            Case "FFFF": Exit Do
            Case Else:
        End Select
        'if we entered from the room list, accept any entry
        If EnteredRoomFromThisDoor = 0 Then Exit Do
        'if we entered from the door (and are finding door entries) see if this door matches
        If Right$("0000" & Hex$(TempFX1.Select), 4) = Right$("0000" & Hex$(EnteredRoomFromThisDoor), 4) Then Exit Do
        'if not, increase fx1pointer and try again
        FX1Displacement = FX1Displacement + &H10&
        FX1Pointer = FX1Pointer + FX1Displacement
    Loop
    If FX1Displacement > 0 Then MsgBox "You've entered a room that has door-dependent FX1 data." & vbCrLf & "The door you have entered is not the first door in the FX1 list." & vbCrLf & "If changing pointer data for this room with the pointer editor, do NOT move the data or it will get damaged." & vbCrLf & "If you want to change pointers for this room (and move the related data), you should refresh the room first by using the Room List instead of a door.", vbInformation
*/

	return true;
}

bool Room::save(U32 offset, int stateIndex){

	/*
	for(unsigned i=0; i<tiles.readISize(); ++i)
		for(unsigned j=0; j<tiles.readJSize(); ++j){
			tiles.at(i, j).layer1=TileLayer(buffer, 2+2*(i+j*tiles.readISize()));
			tiles.at(i, j).bts=buffer[2+roomDataSize+i+j*tiles.readISize()];
			if(buffer.size()>unsigned(2+roomDataSize+roomDataSize/2)){
				tiles.at(i, j).layer2=TileLayer(buffer, 2+roomDataSize+roomDataSize/2+2*(i+j*tiles.readISize()));
				tiles.at(i, j).hasLayer2=true;
			}
			else tiles.at(i, j).hasLayer2=false;
			if(tiles.at(i, j).layer1.property==9&&unsigned(tiles.at(i, j).bts+1)>doorsInRoom) doorsInRoom=tiles.at(i, j).bts+1;
		}
	*/

	//write tile data
	Buffer buffer;
	writeU16(buffer, 2*tiles.readISize()*tiles.readJSize());
	for(unsigned j=0; j<tiles.readJSize(); ++j)
		for(unsigned i=0; i<tiles.readISize(); ++i)
			tiles.at(i, j).layer1.write(buffer);
	for(unsigned j=0; j<tiles.readJSize(); ++j)
		for(unsigned i=0; i<tiles.readISize(); ++i)
			buffer.push_back(tiles.at(i, j).bts);
	if(tiles.at(0, 0).hasLayer2)
		for(unsigned j=0; j<tiles.readJSize(); ++j)
			for(unsigned i=0; i<tiles.readISize(); ++i)
				tiles.at(i, j).layer2.write(buffer);

	Buffer testbuf;
	decompress(rom.buffer, cpuToRom(state.data), testbuf);
	for(unsigned i=0; i<min(testbuf.size(), buffer.size()); ++i)
		if(testbuf[i]!=buffer[i]){
			cerr<<"difference at "<<i<<"\n";
			break;
		}

	Buffer compressed;
	compress(buffer, compressed);

	cerr<<"SNES compressed length: "<<originalCompressedTileDataSize<<"\n";
	cerr<<"My compressed length: "<<compressed.size()<<"\n";

	if(compressed.size()>originalCompressedTileDataSize) return false;
	for(unsigned i=0; i<compressed.size(); ++i)
		rom.buffer[cpuToRom(state.data)+i]=compressed[i];
	return true;
}

Tile Room::copy(unsigned x, unsigned y){
	if(mode7){
		x/=(TILE_SIZE/2);
		y/=(TILE_SIZE/2);
	}
	else{
		x/=TILE_SIZE;
		y/=TILE_SIZE;
	}
	if(x>=tiles.readISize()||y>=tiles.readJSize()) return Tile();
	return tiles.at(x, y);
}

void Room::paste(Tile t, unsigned x, unsigned y){
	if(mode7){
		x/=(TILE_SIZE/2);
		y/=(TILE_SIZE/2);
	}
	else{
		x/=TILE_SIZE;
		y/=TILE_SIZE;
	}
	if(x>=tiles.readISize()||y>=tiles.readJSize()) return;
	tiles.at(x, y)=t;
}

void Room::loadGraphics(){
	U32 tileSetPointer=0x7E6A2u+U32(state.tileSet)*9;
	Buffer buffer;
	decompress(rom.buffer, readPointer(rom.buffer, tileSetPointer+6), buffer);
	vector<Color> palette;
	for(unsigned i=0; i<buffer.size(); i+=2){
		U16 p=buffer[i+1]<<8|buffer[i];
		palette.push_back(Color(
			1.0f*(p>> 0&0x1Fu)/0x1Fu,
			1.0f*(p>> 5&0x1Fu)/0x1Fu,
			1.0f*(p>>10&0x1Fu)/0x1Fu,
			1.0f
		));
	}
	buffer.clear();
	decompress(rom.buffer, readPointer(rom.buffer, tileSetPointer+3), buffer);
	if(17<=state.tileSet&&state.tileSet<=20){//These tile sets use SNES mode 7 -- they are used in 2 Ceres rooms
		mode7=true;
		tileSet.clear();
		tileSet.resize(256);
		for(unsigned i=0; i<tileSet.size(); ++i){
			tileSet[i].resize(TILE_SIZE/2, TILE_SIZE/2);
			for(unsigned x=0; x<TILE_SIZE/2; ++x)
				for(unsigned y=0; y<TILE_SIZE/2; ++y)
					tileSet[i].at(x, y)=palette[buffer[2*(TILE_SIZE/2*TILE_SIZE/2*i+8*y+x)+1]];
		}
		Array2D<Tile> tilesCopy=tiles;
		tiles.clear();
		tiles.resize(128, 128);
		for(unsigned i=0; i<tiles.readISize(); ++i)
			for(unsigned j=0; j<tiles.readJSize(); ++j){
				if(i/2<tilesCopy.readISize()&&j/2<tilesCopy.readJSize()) tiles.at(i, j)=tilesCopy.at(i/2, j/2);
				tiles.at(i, j).layer2.index=buffer[2*(j*tiles.readISize()+i)];
			}
	}
	else{
		mode7=false;
		if(state.tileSet==26) buffer.resize(0x8000u);
		else buffer.resize(0x5000u);
		Buffer furtherBuffer;
		decompress(rom.buffer, 0x1C8000u, furtherBuffer);
		for(unsigned i=0; i<furtherBuffer.size(); ++i) buffer.push_back(furtherBuffer[i]);
		for(unsigned i=0; i<buffer.size(); i+=32){
			U8 copy[32];
			for(unsigned j=0; j<32; ++j){
				copy[j]=buffer[i+j];
				buffer[i+j]=0;
			}
			for(unsigned y=0; y<8; y++){
				U8 line[4];
				line[0]=copy[y*2];
				line[1]=copy[y*2+1];
				line[2]=copy[y*2+16];
				line[3]=copy[y*2+17];
				for(unsigned x=0; x<8; x++){
					unsigned shift=(7-x)*4;
					U32 word=0;
					for(unsigned j=0; j<4; ++j) word+=(line[j]&1)<<(shift+j);
					for(unsigned j=0; j<4; ++j){
						buffer[i+y*4+j]|=word>>(8*j)&0xFFu;
						line[j]>>=1;
					}
				}
			}
		}
		Buffer subTiles;
		for(unsigned i=0; i<buffer.size(); ++i){
			subTiles.push_back(buffer[i]&0xFu);
			subTiles.push_back(buffer[i]>>4);
		}
		vector<TileAssembler> tileAssemblers;
		buffer.clear();
		if(header.region!=6) decompress(rom.buffer, 0x1CA09Du, buffer);
		furtherBuffer.clear();
		decompress(rom.buffer, readPointer(rom.buffer, tileSetPointer), furtherBuffer);
		for(unsigned i=0; i<furtherBuffer.size(); ++i) buffer.push_back(furtherBuffer[i]);
		for(unsigned i=0; i<buffer.size(); i+=8){
			tileAssemblers.push_back(TileAssembler(
				buffer[i+1]<<8|buffer[i+0],
				buffer[i+3]<<8|buffer[i+2],
				buffer[i+5]<<8|buffer[i+4],
				buffer[i+7]<<8|buffer[i+6]
			));
		}
		tileSet.clear();
		tileSet.resize(tileAssemblers.size());
		for(unsigned i=0; i<tileAssemblers.size(); ++i){
			tileSet[i].resize(TILE_SIZE, TILE_SIZE);
			drawSubTile(subTiles, tileAssemblers[i].ul, palette, tileSet[i], 0          , 0);
			drawSubTile(subTiles, tileAssemblers[i].ur, palette, tileSet[i], TILE_SIZE/2, 0);
			drawSubTile(subTiles, tileAssemblers[i].dl, palette, tileSet[i], 0          , TILE_SIZE/2);
			drawSubTile(subTiles, tileAssemblers[i].dr, palette, tileSet[i], TILE_SIZE/2, TILE_SIZE/2);
		}
	}
}

void Room::drawTileSet(Array2D<Color>& destination, unsigned tilesWide) const{
	if(mode7){
		tilesWide*=2;//tiles in this mode are half as big as usual, so compensate
		destination.resize(tilesWide*TILE_SIZE/2, (tileSet.size()/(tilesWide/2)+1)*TILE_SIZE/2);
		for(unsigned i=0; i<tileSet.size(); ++i)
			for(unsigned x=0; x<TILE_SIZE/2; ++x)
				for(unsigned y=0; y<TILE_SIZE/2; ++y)
					destination.at(i%tilesWide*TILE_SIZE/2+x, i/tilesWide*TILE_SIZE/2+y)=tileSet[i].at(x, y);
	}
	else{
		destination.resize(tilesWide*TILE_SIZE, (tileSet.size()/tilesWide+1)*TILE_SIZE);
		for(unsigned i=0; i<tileSet.size(); ++i)
			for(unsigned x=0; x<TILE_SIZE; ++x)
				for(unsigned y=0; y<TILE_SIZE; ++y)
					destination.at(i%tilesWide*TILE_SIZE+x, i/tilesWide*TILE_SIZE+y)=tileSet[i].at(x, y);
	}
}

void Room::getQuadsVertexArray(vector<Vertex>& vertices, unsigned tilesWide) const{
	if(mode7){
		tilesWide*=2;//tiles in this mode are half as big as usual, so compensate
		for(unsigned i=0; i<tiles.readISize(); ++i)
			for(unsigned j=0; j<tiles.readJSize(); ++j){
				const Tile& tile=tiles.at(i, j);
				unsigned tileX=tile.layer2.index%tilesWide*TILE_SIZE/2;
				unsigned tileY=tile.layer2.index/tilesWide*TILE_SIZE/2;
				unsigned txi=tileX, txf=tileX+TILE_SIZE/2-1, tyi=tileY, tyf=tileY+TILE_SIZE/2-1;
				vertices.push_back(Vertex((i+0)*TILE_SIZE/2, (j+0)*TILE_SIZE/2, txi, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE/2, (j+0)*TILE_SIZE/2, txf, tyi));
				vertices.push_back(Vertex((i+1)*TILE_SIZE/2, (j+1)*TILE_SIZE/2, txf, tyf));
				vertices.push_back(Vertex((i+0)*TILE_SIZE/2, (j+1)*TILE_SIZE/2, txi, tyf));
			}
	}
	else{
		//layer 2
		for(unsigned i=0; i<tiles.readISize(); ++i)
			for(unsigned j=0; j<tiles.readJSize(); ++j){
				const Tile& tile=tiles.at(i, j);
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
		//layer 1
		for(unsigned i=0; i<tiles.readISize(); ++i)
			for(unsigned j=0; j<tiles.readJSize(); ++j){
				const Tile& tile=tiles.at(i, j);
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
}

const Door* Room::readDoor(unsigned x, unsigned y) const{
	if(mode7){
		x/=(TILE_SIZE/2);
		y/=(TILE_SIZE/2);
	}
	else{
		x/=TILE_SIZE;
		y/=TILE_SIZE;
	}
	if(x>=tiles.readISize()||y>=tiles.readJSize()) return NULL;
	if(tiles.at(x, y).layer1.property==9) return &doors[tiles.at(x, y).bts];
	return NULL;
}

//=====functions=====//
string codeDescription(U16 code){
	switch(code){
		case STANDARD: return "STANDARD";
		case DOORS: return "Doors";
		case TOURIAN_BOSS_1: return "Tourian Boss 1";
		case EVENTS: return "Events";
		case BOSSES: return "Bosses";
		case MORPH: return "Morph";
		case MORPH_AND_MISSILES: return "Morph & Missiles";
		case POWER_BOMBS: return "Power Bombs";
		case SPEED_BOOSTER: return "Speed Booster";
		default: break;
	}
	return "";
}
