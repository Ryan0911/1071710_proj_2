#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include <iomanip>
#include<math.h>
using namespace std;

struct inst_state {
	int current = 0;
	int state[8] = {0,0,0,0,0,0,0,0};
};
int Register[32] = {}; // register
int binaryToDecimal(string binary);
int main() {
	string all_state[8] = { "SN","WN1","WN2","WN3","WT1","WT2","WT3","ST" };
	string all_state2[8] = { "N","N","N","N","T","T","T","T" };
	string input;
	ifstream fin1;
	fin1.open("input.txt");
	int miss = 0;
	while (getline(fin1, input));
	string initial = "000";
	inst_state test;
	for (int i = 0; i < input.size(); i++) {
		test.current = binaryToDecimal(initial);
		cout << "(" << initial;
		for (int i2 = 0; i2 < 8;i2++) {
			cout <<", "<< all_state[test.state[i2]];
		}
		cout << ")";
		cout << "\tPrediction: "<< all_state2[test.state[test.current]];
		cout << "\tActual: " << input[i];
		if (input[i] == 'N') {
			if(test.state[test.current]!=0)
				test.state[test.current]--;
		}
		else if (input[i] == 'T') {
			if (test.state[test.current] != 7)
				test.state[test.current]++;
		}
		if (input[i] != all_state2[test.state[test.current]][0]) {
			miss++;
		}
		cout << "\tMiss: " << miss;
		cout << endl;
		initial[0] = initial[1];
		initial[1] = initial[2];
		if (input[i] == 'N') {
			initial[2] = '0';
		}
		else {
			initial[2] = '1';
		}
	}
	
	system("PAUSE");	
	return 0;
}
int binaryToDecimal(string binary) {
	int base = 0;
	int outcome = 0;
	for (int i = binary.size() - 1; i >= 0; i--) {
		outcome += pow(2, base) * (binary[i] - '0');
		base++;
	}
	return outcome;
}