#include <vector>
#include <windows.h>
enum{//ID's
	CHANGEROOM=0,//Loading levels and shit
	CLIPSTUFF,//BTS and slopes and shit
};

class clsTools{
public:
int scrVal;
int scrMax;
clsTools();
~clsTools();
std::vector<int>  LookUp;
std::vector<HWND> Set;
int Add(HWND hwnd,int ID);
int Switch(int ID);
HWND operator[] (int ID);
};