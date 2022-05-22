#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include<iomanip>
#include<math.h>
#include<vector>
#include<map>
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
	int exit = 1; //控制程式是否離開
	while (exit) {
		for (int i = 0; i < 32; i++) {
			Register[i] = 0;
		} //Register 初始化
		string all_state[8] = { "SN","WN1","WN2","WN3","WT1","WT2","WT3","ST" }; //3-bit history的初始狀態 2^3 參考[1] 3-bit history predictor status圖
		string all_state2[8] = { "N","N","N","N","T","T","T","T" };//對應要顯示的狀態
		string input; //用於存檔案內的每行字串
		ifstream fin1; //讀檔
		vector<vector<string>> instruction;//存放instruction的vector，會進行切割處理
		vector<string>temp;// 暫存使用
		map<string, int> label; // 用於存放Label之對應要跳的行數
		int position = 0;//切割字串紀錄位置用
		string file_name = "0";
		//輸入檔案名稱，若不存在則請使用者重新輸入
		while (true) {
			cout << "Please input the file name: ";
			cin >> file_name;
			fin1.open(file_name); //read files.
			if (!fin1) {
				cout << "The file does not exist, please try again." << endl;
			}
			else
				break;
		}
		//輸入entry，小於0則請使用者重新輸入
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
		predictor *predictor_table = new predictor[entries]; //根據entries數量進行動態分配每個entries的predictors
		int index = 0;//紀錄program counter的行數
		while (getline(fin1, input)) {//讀每一個instruction並放入vector中
			if (input[input.size() - 1] == ':') {//遇到Label要如此處理
				while (input.find(':') != -1) {
					input.erase(input.find(':'), 1);
				}
				label.insert(pair<string, int>(input, index));//把對應的Index放入
				index--;
			}
			else {//當作Instruction處理，先將\t處理掉，再根據找到的第一個空格切割，拿到operator
				while (input.find('\t') != -1) {
					input.erase(input.find('\t'), 1);
				}
				position = input.find_first_of(' ');
				temp.push_back(input.substr(0, position));//將operator放入
				input = input.substr(position + 1);
				input = deleteComment(input);//處理註解
				stringstream ss(input);//根據逗號的位置切割字串，並將每次切割出來的字串再存入該instruction的vector中，所以會根據這個順序依序拿到opcode rd rs1 rs2等等...
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
		string Actual = "0";//用於紀錄實際結果
		string Actual_2 = "0";//將0轉成N，1轉成T所用
		for (int i = 0; i < instruction.size(); i++) {
			cout << "----------------------------------------------------------------------" << endl;
			cout << "Address: 0x" << setfill('0') << setw(sizeof(int) * 2) << hex << (i * 4) << dec << endl;
			cout << "Instruction: " << instruction[i][0] << " ";
			for (int j = 1; j < instruction[i].size(); j++) {
				if (j + 1 != instruction[i].size())
					cout << instruction[i][j] << ", ";
				else
					cout << instruction[i][j];
			}
			cout << endl; //print出instruction
			//print entries
			cout << "Entries status: " << endl;
			for (int e = 0; e < entries; e++) {
				cout << "Entry: " << e << "\t ";
				cout << "History_miss: " << predictor_table[e].miss;
				//將資訊該印的印一印，接下來處理...prediction
				cout << "\t (" << predictor_table[e].initial;
				for (int e2 = 0; e2 < 8; e2++) {
					cout << ", " << all_state[predictor_table[e].state[e2]];
				}
				cout << ")"<<endl;
			}
			Actual = which_type(instruction[i]); //將目前指向的instruction帶入此function，此時會回傳0或1 0為not taken、1為taken的狀態
			//print entry
			if (Actual == "0") {
				Actual_2 = "N";
			}
			else {
				Actual_2 = "T";
			}
			predictor_table[i%entries].current = binaryToDecimal(predictor_table[i%entries].initial);//將二進位的結果轉成十進位放入current，決定要存取的狀態
			cout << "Predict status: " << endl;
			cout << "Used entry: " << i % entries << "\t Actual: " << Actual_2 << "\t Prediction: " << all_state2[predictor_table[i%entries].state[predictor_table[i%entries].current]];
			if (Actual_2 != all_state2[predictor_table[i%entries].state[predictor_table[i%entries].current]]) {//預測與實際不符合，該entry predictor的miss數增加
				predictor_table[i%entries].miss++;
			}
			cout << "\t Current_miss: " << predictor_table[i%entries].miss << endl;
			if (Actual == "1") {
				//Actual = 1的處理
				if (predictor_table[i%entries].state[predictor_table[i%entries].current] != 7) { //當==7時代表是ST，此時不要繼續增加
					predictor_table[i%entries].state[predictor_table[i%entries].current]++;
				}
				//根據實際結果變更initial
				predictor_table[i%entries].initial[0] = predictor_table[i%entries].initial[1];
				predictor_table[i%entries].initial[1] = predictor_table[i%entries].initial[2];
				predictor_table[i%entries].initial[2] = '1';
				i = label[instruction[i][3]] - 1; //branch to label taken了，所以要變更pc指向的位置
			}
			else {
				//Actual = 0的處理
				if (predictor_table[i%entries].state[predictor_table[i%entries].current] != 0) { //當==7時代表是SN，此時不要繼續減少
					predictor_table[i%entries].state[predictor_table[i%entries].current]--;
				}
				predictor_table[i%entries].initial[0] = predictor_table[i%entries].initial[1];
				predictor_table[i%entries].initial[1] = predictor_table[i%entries].initial[2];
				predictor_table[i%entries].initial[2] = '0';
			}
			cout << "----------------------------------------------------------------------" << endl;
		}
		fin1.close();
		cout << "Registers status: " << endl; //Register的狀態都印出來～供使用者參考
		for (int i = 0; i < 32; i++) {
			cout << "R" << i << ":" << Register[i] << "\t\t";
			if (i % 2 != 0)
				cout << endl;
		}
		cout << "----------------------------------------------------------------------" << endl;
		cout << "If you want to exit, please enter 0, if you're going to continue, you can enter any numbers(except 0): ";
		cin >> exit;
	}
	cout << "Thank you for using this program."<<endl;
	//system("PAUSE");
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
	else if (instruction[0] == "addi" || instruction[0] == "andi" || instruction[0] == "ori"
		|| instruction[0] == "slli" || instruction[0] == "slti" || instruction[0] == "sltiu"
		|| instruction[0] == "srai" || instruction[0] == "srli" || instruction[0] == "xori") {
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
		Register[rd] = (Register[rs1] < Register[rs2]) ? 1 : 0;
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