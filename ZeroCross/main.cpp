#include <Windows.h>
#include <iostream>
#include <random>
#include <vector>
#include <ctime>
using namespace std;

char parse(char c){
	if(c==0) return '_';
	if(c==1) return 'X';
	if(c==2) return 'O';
	return 0;
}

int check(char *f){
	if(f[0]==f[1] && f[1]==f[2] && f[0]) return f[0];
	if(f[3]==f[4] && f[4]==f[5] && f[3]) return f[3];
	if(f[6]==f[7] && f[7]==f[8] && f[6]) return f[6];
	if(f[0]==f[3] && f[3]==f[6] && f[0]) return f[0];
	if(f[1]==f[4] && f[4]==f[7] && f[1]) return f[1];
	if(f[2]==f[5 ]&& f[5]==f[8] && f[2]) return f[2];
	if(f[0]==f[4] && f[4]==f[8] && f[0]) return f[0];
	if(f[2]==f[4] && f[4]==f[6] && f[2]) return f[2];
	return 0;
}

void printfield(char *field){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++)
			cout << parse(field[3*i+j]);
		cout << endl;
	}
	cout << endl;
}

int strategy(char *field, int player){
	static char* testfield = new char[9];
	vector<int> free;
	for(int i=0;i<9;i++)
		testfield[i] = field[i];
	int n=0;
	for(int i=0;i<9;i++){
		if(!testfield[i]){
			n++;
			testfield[i] = player + 1;
			if(check(testfield)) return i;
			testfield[i] = !player + 1;
			if(check(testfield)) return i;
			testfield[i] = 0;
			free.push_back(i);
		}
	}
	if(n==0)
		return -1;
	if(n==9) return 4;
	n = rand()%free.size();
	return free[n];
}

int main(){
	srand(time(0));
	int player = -1;
	HANDLE hMap = CreateFileMappingA(NULL,NULL, PAGE_READWRITE,0,11*sizeof(char),"ZeroCross");
	if(!hMap){
		cout << "Internal game error!" << endl;
		return 0;
	}
	LPVOID ptr = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	char *status = (char*) ptr;
	char *field = status + 2;
	if(!status[0]){
		player = 0;
		status[0] = 1;
		cout << "Waiting for other player.." << endl;
		while(!status[1])
			Sleep(1000);
		cout << "Game is found!" << endl;
		status[player] = 'h';
	}
	else if(status[1]){		
		cout << "Game has already been started!" << endl;
		return 0;
	}
	else
	{
		player = 1;
		cout << "Game is found!" << endl;
		status[1] = 1;
	}
	while(1){
		if(status[player] == 'h'){
			printfield(field);
			int k = strategy(field,player);
			if(k==-1){
				status[0]=status[1]='d';
			}
			else{
				field[k] = player + 1;
				if(check(field))
					status[player] = 'w';
				else{
					status[player] = 1;
					status[!player] = 'h';
				}
				printfield(field);
			}
		}
		if(status[player] == 'w'){
			cout << "I win!" << endl;
			break;
		}
		if(status[!player] == 'w'){
			printfield(field);
			cout << "I loose!" << endl;
			break;
		}
		if(status[player] == 'd'){
			cout << "Round draw!" << endl;
			break;
		}
		Sleep(500);
	}
	return 0;
}