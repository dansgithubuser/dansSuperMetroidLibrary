#include <stdio.h>
#include <String>
#include <vector>
struct strProb{
	unsigned int offset;
	std::string problem;
	unsigned int src;//Good file
	unsigned int dst;//Our file
	std::string srcF;
	std::string dstF;
	std::vector<unsigned char> outBuff;
	std::vector<unsigned char> inBuff;
	long dstpos;
};

int CompareFiles(strProb* TheFiles);