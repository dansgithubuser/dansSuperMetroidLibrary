#include "Tools.h"
clsTools::clsTools(){
scrVal=0;
}
clsTools::~clsTools(){}
int clsTools::Add(HWND hwnd,int ID){
Set.push_back(hwnd);
LookUp.push_back(ID);
scrMax=Set.size()-1;//Should we auto update the scroll? nawwwww maybe later leave this comment here. 
return 0;
}

int clsTools::Switch(int ID){
int i=0; 

	for(i=0;i<Set.size();i++){
		if(LookUp[i]==ID){
			  ShowWindow(Set[LookUp[i]],SW_SHOW);
		}else{
			  ShowWindow(Set[LookUp[i]],SW_HIDE);
		}
	}
return  0;

}
HWND clsTools::operator[] (int ID){
//find ID
int index=0;
for(index=0;index<Set.size();index++){
	if(LookUp[index]==ID){ 
		break;
	}
}
return Set[index];
}
