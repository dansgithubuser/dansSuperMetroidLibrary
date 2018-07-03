#include "Compare.h"

unsigned long GetFileLen(FILE* fp){
	unsigned long offset=ftell(fp);
	unsigned long len;
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	fseek(fp,offset,SEEK_SET);
	return len;
}

int CompareFiles(strProb* TheFiles){
	
	

	int i =0;
	unsigned char* srcp=&TheFiles->inBuff[0];
	unsigned char* dstp=&TheFiles->outBuff[0];
	for(;i<TheFiles->dstpos;i++){

		if(dstp[i]!=srcp[i]){

			//PROBLEMLC_LZ5
			TheFiles->dst=dstp[i];
			TheFiles->src=srcp[i];
			TheFiles->offset=i;
			TheFiles->problem="outBuff does not match inBuff or vice versa";
			return 1;
		}

	}
	return 0;

}