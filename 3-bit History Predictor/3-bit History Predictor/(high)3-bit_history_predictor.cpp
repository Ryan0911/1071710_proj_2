#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include <iomanip>
#include<math.h>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
using namespace std;
struct predictor {
	int current = 0;
	int state[8] = { 0,0,0,0,0,0,0,0 };
	string initial = "000";
	int miss = 0;
};
int Register[32] = {};
int binaryToDecimal(string binary); //二進位轉十進位
string deleteComment(string instruction); //把註解除掉
string which_type(vector<string> instruction);
string type_R(vector<string> instruction);
string type_I(vector<string> instruction);
string type_SB(vector<string> instruction);
int main() {
	for (int i = 0; i < 32; i++) {
		Register[i] = 0;
	}
	string all_state[8] = { "SN ","WN1","WN2","WN3","WT1","WT2","WT3","ST " };
	string all_state2[8] = { "N","N","N","N","T","T","T","T" };
	string input;
	ifstream fin1;
	vector<vector<string>> instruction;
	vector<string>temp;
	map<string, int> label;
	int position = 0;
	fin1.open("input2.txt");
	int entries = 0;
	while (true) {
		cout << "Please input entry (entry > 0): ";
		cin >> entries;
		if (entries <= 0) {
			cout << "Please try again." << endl;
		}
		else
			break;
	} 
	//get entries.
	predictor *all_predictor = new predictor[entries]; //動態分配每個entries的predictors
	int index = 0;
	while (getline(fin1, input)) {//讀每一個instruction
		if (input[input.size()-1] == ':') {
			while (input.find(':') != -1) {
				input.erase(input.find(':'), 1);
			}
			label.insert(pair<string, int>(input, index));
			index--;
		}
		else {
			replace(input.begin(), input.end(), '\t', ' '); //replace all \t to space
			position = input.find_first_of(' ');
			if (position == 0) {
				input = input.substr(1);
				position = input.find(' ');
				temp.push_back(input.substr(0, position));
				input = input.substr(position + 1);
			}
			else if (position != -1) {
				temp.push_back(input.substr(0, position));
				input = input.substr(position + 1);
			}
			input = deleteComment(input);
			stringstream ss(input);
			while (getline(ss, input, ',')) {
				if (!input.empty()) {
					temp.push_back(input);
				}
			}
			instruction.push_back(temp);
			temp.clear();
		}
		index++;
	}
	//處理完instruction!!! 接下來就是處理每一個prediction
	string Actual = "0";
	string Actual_2 = "0";
	for (int i = 0; i < instruction.size(); i++) {
		cout << "----------------------------------------------------------------------" << endl;
		all_predictor[i%entries].current = binaryToDecimal(all_predictor[i%entries].initial);
		cout << "Entry: " << i % entries<<"\t\t";
		//該印的印一印，接下來處理...prediction???
		Actual = which_type(instruction[i]);
		cout << "(" << all_predictor[i%entries].initial;
		for (int i2 = 0; i2 < 8; i2++) {
			cout << ", " << all_state[all_predictor[i%entries].state[i2]];
		}
		cout << ")"<<endl;
		cout << "Instruction: " << instruction[i][0] << " ";
		for (int j = 1; j < instruction[i].size(); j++) {
			if (j + 1 != instruction[i].size())
				cout << instruction[i][j] << ", ";
			else
				cout << instruction[i][j];
		}
		if (Actual == "0") {
			Actual_2 = "N";
		}
		else {
			Actual_2 = "T";
		}
		cout << endl << "Actual: " << Actual_2 << "\t\t Prediction: " << all_state2[all_predictor[i%entries].current] << endl;
		if (Actual_2 != all_state2[all_predictor[i%entries].current]) {
			all_predictor[i%entries].miss++;
		}
		cout << "Miss: " << all_predictor[i%entries].miss<<endl;
		if (Actual == "1") {
			//Actual 1
			if (all_predictor[i%entries].state[all_predictor[i%entries].current] != 7) {
				all_predictor[i%entries].state[all_predictor[i%entries].current]++;
			}
			all_predictor[i%entries].initial[0] = all_predictor[i%entries].initial[1];
			all_predictor[i%entries].initial[1] = all_predictor[i%entries].initial[2];
			all_predictor[i%entries].initial[2] = '1';
			i = label[instruction[i][3]] -1; //branch to label
		}
		else {
			//Actual 0
			if (all_predictor[i%entries].state[all_predictor[i%entries].current] != 0) {
				all_predictor[i%entries].state[all_predictor[i%entries].current]--;
			}
			all_predictor[i%entries].initial[0] = all_predictor[i%entries].initial[1];
			all_predictor[i%entries].initial[1] = all_predictor[i%entries].initial[2];
			all_predictor[i%entries].initial[2] = '0';
		}
		cout <<"----------------------------------------------------------------------"<< endl;
	}
	fin1.close();
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
string deleteComment(string instruction) {
	replace(instruction.begin(), instruction.end(), '\t', ' '); //replace all \t to space
	int position = 0;
	position = instruction.find(";");
	if (position != -1) {
		instruction.erase(position);
	}
	position = instruction.find("//");
	if (position != -1) {
		instruction.erase(position);
	}
	while (instruction.find(' ') != -1) {
		instruction.erase(instruction.find(' '), 1);
	}
	return instruction;
}
string which_type(vector<string> instruction) {
	if (instruction[0] == "li") {
		int position = stoi(instruction[1].substr(1));
		Register[position] = stoi(instruction[2]);
		return "0";
	}
	else if (instruction[0] == "add" || instruction[0] == "and" || instruction[0] == "or"
		|| instruction[0] == "sll" || instruction[0] == "slt" || instruction[0] == "sltu"
		|| instruction[0] == "sra" || instruction[0] == "srl" || instruction[0] == "sub" || instruction[0] == "xor") {
		return type_R(instruction);
	}
	else if (instruction[0]=="addi"||instruction[0]=="andi"	 || instruction[0] == "ori" 
		||instruction[0] == "slli" || instruction[0] == "slti" || instruction[0] == "sltiu" 
		|| instruction[0] == "srai" || instruction[0] == "srli"|| instruction[0] == "xori") {
		return type_I(instruction);
	}
	else if (instruction[0] == "beq" || instruction[0] == "bge" || instruction[0] == "bgeu"
		|| instruction[0] == "blt" || instruction[0] == "bltu" || instruction[0] == "bne") {
		return type_SB(instruction);
	}
	
}
string type_R(vector<string> instruction) {
	int rd = stoi(instruction[1].substr(1));
	int rs1 = stoi(instruction[2].substr(1));
	int rs2 = stoi(instruction[3].substr(1));
	if (instruction[0] == "add") {
		Register[rd] = Register[rs1] + Register[rs2];
	}
	else if (instruction[0] == "and") {
		Register[rd] = Register[rs1] & Register[rs2];
	}
	else if (instruction[0] == "or") {
		Register[rd] = Register[rs1] | Register[rs2];
	}
	else if (instruction[0] == "sll") {
		Register[rd] = Register[rs1] << Register[rs2];
	}
	else if (instruction[0] == "slt") {
		Register[rd] = (Register[rs1] < Register[rs2])? 1 : 0;
	}
	else if (instruction[0] == "sltu") {
		Register[rd] = (Register[rs1] < Register[rs2]) ? 1 : 0;
	}
	else if (instruction[0] == "sra") {
		Register[rd] = Register[rs1] >> Register[rs2];
	}
	else if (instruction[0] == "srl") {
		Register[rd] = Register[rs1] >> Register[rs2];
	}
	else if (instruction[0] == "sub") {
		Register[rd] = Register[rs1] - Register[rs2];
	}
	else if (instruction[0] == "xor") {
		Register[rd] = Register[rs1] ^ Register[rs2];
	}
	return "0";
}
string type_I(vector<string> instruction) {
	int rd = stoi(instruction[1].substr(1));
	int rs1 = stoi(instruction[2].substr(1));
	int imm = stoi(instruction[3]);
	if (instruction[0] == "addi") {
		Register[rd] = Register[rs1] + imm;
	}
	else if (instruction[0] == "andi") {
		Register[rd] = Register[rs1] & imm;
	}
	else if (instruction[0] == "ori") {
		Register[rd] = Register[rs1] | imm;
	}
	else if (instruction[0] == "slli") {
		Register[rd] = Register[rs1] << imm;
	}
	else if (instruction[0] == "slti") {
		Register[rd] = (Register[rs1] < imm) ? 1 : 0;
	}
	else if (instruction[0] == "sltiu") {
		Register[rd] = (Register[rs1] < imm) ? 1 : 0;
	}
	else if (instruction[0] == "srai") {
		Register[rd] = Register[rs1] >> imm;
	}
	else if (instruction[0] == "srli") {
		Register[rd] = Register[rs1] >> imm;
	}
	else if (instruction[0] == "xori") {
		Register[rd] = Register[rs1] ^ imm;
	}
	return "0";
}
string type_SB(vector<string> instruction) {
	int rs1 = stoi(instruction[1].substr(1));
	int rs2 = stoi(instruction[2].substr(1));
	string branch_label = instruction[3];
	if (instruction[0] == "beq") {
		if (Register[rs1] == Register[rs2])
			return "1";
		return "0";
	}
	else if (instruction[0] == "bge") {
		if (Register[rs1] >= Register[rs2])
			return "1";
		return "0";
	}
	else if (instruction[0] == "bgeu") {
		if (Register[rs1] >= Register[rs2])
			return "1";
		return "0";
	}
	else if (instruction[0] == "blt") {
		if (Register[rs1] < Register[rs2])
			return "1";
		return "0";
	}
	else if (instruction[0] == "bltu") {
		if (Register[rs1] < Register[rs2])
			return "1";
		return "0";
	}
	else if (instruction[0] == "bne") {
		if (Register[rs1] != Register[rs2])
			return "1";
		return "0";
	}
}

/*
string type_UJ(vector<string> instruction) {

}
*/