#include "main.h"

long FindFreeSpace(long bNo, u8 bConst)
{ // bNo is how many bytes, bConst is what
	// value to look for 

	// returns offset of "free space" block
	// or -1 if no such block found
	/*

if (!ROM)
return -1;

bNo += 0x100; // Look for an extra free 256 bytes. -- still not sure what
		// this does,
// but whatever

int             ch;
long            runstart = -1;
long            runcount = 0;

fseek(ROM, 0, SEEK_SET);
checkagain:
while (1)
{
ch = fgetc(ROM); // get character (byte)

if (ch == EOF) // if end of ROM

{
	runstart = -1; // eof reached before we found block

	break;

}
if ((u8) ch == bConst)
{
	if (!runcount) // start of run

	runstart = (long) ftell(ROM) - 1;

	++runcount;
	if (runcount >= bNo) // block found

	break;
} else
{
	runstart = -1;
	runcount = 0;
}

}
if (runstart == -1)
{
fseek(ROM, 0, SEEK_SET);

goto checkagain;
}
if (runstart != 1)
while (runstart % 4)
	runstart++;*/
	return 0;
}