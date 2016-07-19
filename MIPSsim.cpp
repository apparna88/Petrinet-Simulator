/*
	Course		: CDA5636-Embedded Systems
	Project 1	: MIPS simulator for petrinet
	Author 		: Aparna Hariyani	
	UFID   		: 69185846  
	"On my honor, I have neither given nor received unauthorized aid on this assignment"
	
*/

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

#define TOKEN_INB 0
#define TOKEN_ADB 1
#define TOKEN_SIB 2
#define TOKEN_AIB 3
#define TOKEN_REB 4
#define TOKEN_INM 5
#define TOKEN_PRB 6

using namespace std;

static int token_arr [6] = {0,0,0,0,0,1};
static int number_of_tokens=0;

/* Variable Declarations */
vector<string> INM;				// Instruction memory
vector<string> RGF;				// Register File
vector<string> DAM;				// Data Memory
vector<string> INB;				// Instruction Buffer
vector<string> SIB;				// Load Instruction Buffer
vector<string> ADB;				// Address Buffer
vector<string> REB;				// Result Buffer
vector<string> AIB;				// Arithmetic Instruction Buffer
vector<string> PRB;				// Arithmetic Instruction Buffer
string token[16][4];
int token_number = 0;
static int counter_step = 0;
string dummy_opcode;
string dummy_dest_reg;
string dummy_source_operand1;
string dummy_source_operand2;



void read_inputs();	
void write_output();
int read(string &source1 , string &source2 , string opcode); // Check for both the register source availablity , Returns 1 If both the sources are available else Returns 0
void decode(); // Fetch instruction from INM to INB
void issue1(); // If its a aritnmatic operation, fetches from INB to AIB
void issue2(); // If its a memory operation, fetches from INB to SIB
void add_sub_unit(string opcode, string dest_reg ,string source_operand1 , string source_operand2); // does arithmatic operations and stores result in REB
void addr(string dest_reg ,string source_operand1 , string source_operand2); // address calculation
void load(string dest_reg); // load from register file
void write(string dest_reg); //Write to RGF
void calculate(); // pointer calculations
void print_output(); //save in output file
void transitions(int value); 

void simulate( 		int ptr1 , int ptr2 , int ptr3 , int ptr4 ,	string opcode , string dest_reg ,
					string source_operand1 , string source_operand2, int length1 , int length2  );

void calculate_ptr(vector<string> pos);

//bool wayToSort(string i, string j) { return i < j; }

int main(){
	read_inputs();
	return 0;
}

void read_inputs(){
	ifstream input_stream;
	ofstream output_stream;
	string line;

	INM.reserve(16);				// Can store 16 tokens at a time 
	RGF.reserve(16);
	DAM.reserve(16);			
	// Reading From Instructions.txt	
	int data_count=0;
	input_stream.open("instructions.txt");
	INM.push_back("INM:");
	data_count=0;
	while(input_stream >> line)
	{
		//cout<< line;
		if(data_count<15){
			INM.push_back(line);		
		
			data_count++;
			
		}
		else{
			cout<<"\n Error! More then 16 tokens" << endl;
		}
		
	}

	input_stream.close();
	data_count=0;
	// Reading From Registers.txt	
	input_stream.open("registers.txt");
	RGF.push_back("RGF:");
	while(input_stream >> line)
	{
		if(data_count<15){
			RGF.push_back(line);
			//RGF.push_back(",");
			data_count++;
		}
		else{
			cout<<"\n Error! More then 16 registers" << endl;
		}
	}		
	input_stream.close();
	data_count=0;
	// Reading From DataMemory.txt	
	input_stream.open("datamemory.txt");
	DAM.push_back("DAM:");
	while(input_stream >> line)
	{
		if(data_count<15){
		DAM.push_back(line);
		//DAM.push_back(",");
		data_count++;
		}
		else{
			cout<<"\n Error! More then 16 registers" << endl;
		}
	}
	input_stream.close();
	//for output format
	INB.push_back("INB:");	
	AIB.push_back("AIB:");	
	SIB.push_back("SIB:");	
	REB.push_back("REB:");	
	ADB.push_back("ADB:");	
	PRB.push_back("PRB:");	
	write_output();
	//cout<<"INM"<<INM.size();
	print_output();
}

//output the step-wise simulation to a file 
void write_output()
{
	//cout<<"counter_step"<<counter_step<<endl;
	ofstream output_stream;
	
	if(counter_step==0)
	{
		output_stream.open("simulation.txt");
		output_stream<< "STEP "<<counter_step<<":"<<"\n";
		output_stream.close();
	}
	else if(counter_step<=16)
	{
		output_stream.open("simulation.txt" , ios::app);
		output_stream<<"\n"<< "\nSTEP "<<counter_step<<":"<<"\n";
		output_stream.close();
	}
	if(counter_step<=16){
		output_stream.open("simulation.txt", ios :: app);
		for( vector<string>::const_iterator i = INM.begin(); i != INM.end() ; i++) // Check Segmentation Fault due to -1
		{
			if(i!= INM.begin() && i!= INM.end()-1){
				output_stream<< *i << ",";
				//cout<< *i <<",";
			}
			else{
				output_stream<< *i;
				//cout<< *i;
			}
		}
		output_stream<<"\n";
	
		for( vector<string>::const_iterator i = INB.begin(); i != INB.end(); i++)
		{
			if(i!= INB.begin() && i!= INB.end()-1)
				output_stream<< *i<< ",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
		
		for( vector<string>::const_iterator i = AIB.begin(); i != AIB.end(); i++)
		{
			if(i!= AIB.begin() && i!= AIB.end()-1)
				output_stream<< *i<< ",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
	
		
		for( vector<string>::const_iterator i = SIB.begin(); i != SIB.end(); i++)
		{
			if(i!= SIB.begin() && i!= SIB.end()-1)
				output_stream<< *i<< ",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
		
		for( vector<string>::const_iterator i = PRB.begin(); i != PRB.end(); i++)
		{
			if(i!= PRB.begin() && i!= PRB.end()-1)
				output_stream<< *i<< ",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
	
		
		for( vector<string>::const_iterator i = ADB.begin(); i != ADB.end(); i++)
		{
			if(i!= ADB.begin() && i!= ADB.end()-1)
				output_stream<< *i<<",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
	
		
		for( vector<string>::const_iterator i = REB.begin(); i != REB.end(); i++)
		{
			if(i!= REB.begin() && i!= REB.end()-1)
				output_stream<< *i<<",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
	
		
		for( vector<string>::const_iterator i = RGF.begin(); i != RGF.end(); i++)  // Check Segmentation Fault due to -1
		{
			if(i!= RGF.begin() && i!= RGF.end()-1)
				output_stream<< *i<<",";
			else
				output_stream<< *i;
		}
		output_stream<<"\n";
	
		for( vector<string>::const_iterator i = DAM.begin(); i != DAM.end(); i++)  // Check Segmentation Fault due to -1
		{
			if(i!= DAM.begin() && i!= DAM.end()- 1)
				output_stream<< *i <<",";
			else
				output_stream<< *i;
		}
		//output_stream<<"\n";
		
		

		counter_step +=1;
		if(counter_step == 16)
			output_stream.close();
	}
}

void print_output(){
		
	int temp_arr[6];
	for(int i=0;i<6;i++){
		//cout<<" Lock Array" << token_arr[i] << endl;
		temp_arr[i]=token_arr[i];
	}	
	
	for(int i=0;i<6;i++)	
	{
		/*if(temp_arr[i]>1)
			token_arr[TOKEN_REB] -= 1;*/
		//cout << "in for" << "INB.size() " << INB.size() << endl;
		if(temp_arr[i]==1){//} || temp_arr[i]>1){
			if((i==5) && (INM.size()!=1))
			{
				//cout<<"heree";
				transitions(i);
			}
			
			else
				transitions(i);
		}
	}

	write_output();
	if(REB.size()==1 && INM.size()==1 && INB.size()==1)
		exit(0);
	else
		print_output();
}

int read(string &source1 , string &source2 , string opcode){

	int flag1 = 0 , flag2 = 0;
	int ptr1 , ptr2 , ptr3;
	if(opcode=="LD" | opcode=="ST"){
		flag2=1;
		for(int i=0; i< RGF.size() ; i++){
		if(RGF[i].find(source1)==1)
		{
			ptr1=RGF[i].find(source1);
			ptr2=RGF[i].find(",");
			ptr3=RGF[i].find(">");
			//cout<<"\n"<<RGF[i].substr(ptr2+1 , ptr3-ptr2-1)<<"\n";
			source1 = RGF[i].substr(ptr2+1 , ptr3-ptr2-1);		// Actually Changes the value of operands (Call by Reference)
			flag1=1;
		}
	 }
	}
	else{
		
		//cout << "yaaaaaaaaaaaaaaa" << endl;
		for(int i=0; i< RGF.size() ; i++){

		if(RGF[i].find(source1)==1)
		{
			//cout << "yaaaaaaaaaaaaaaa if" << endl;
			ptr1=RGF[i].find(source1);
			ptr2=RGF[i].find(",");
			ptr3=RGF[i].find(">");
			//cout<<"\n"<<RGF[i].substr(ptr2+1 , ptr3-ptr2-1)<<"\n";
			source1 = RGF[i].substr(ptr2+1 , ptr3-ptr2-1);		// Actually Changes the value of operands (Call by Reference)
			//cout << source1 << endl;
			flag1=1;
		}	
		if(RGF[i].find(source2)==1){
			//cout << "yaaaaaaaaaaaaaaa if  1111" << endl;
			ptr1=RGF[i].find(source1);
			ptr2=RGF[i].find(",");
			ptr3=RGF[i].find(">");
			source2 = RGF[i].substr(ptr2+1 , ptr3-ptr2-1);		// Actually changes the value of operands (Call by reference)
			//cout << source2 << endl;
			flag2=1;
		}
	}
}
	//cout << flag1 << flag2 << endl;
	if(flag1 && flag2)
		return 1;
	else
		return 0;
}

void calculate(){
	
	if(INM.size() != 1){
		int ptr1 , ptr2 , ptr3 , ptr4 ; 	// To seperate operands
		ptr1 = INM[1].find(",");			// position of 1st comma
		ptr2 = INM[1].find(",", ptr1+1);	// position of 2nd Comma
		ptr3 = INM[1].find("," ,ptr2+1);	// position of 3rd comma
		ptr4 = INM[1].find(">");
		
		
		string opcode;
		string dest_reg;
		string source_operand1;

		string source_operand2;
		int length1;
		int length2;

		opcode = INM[1].substr(1,ptr1-1);
		dest_reg = INM[1].substr(ptr1+1,2); //2 tells how many characters to be there in sub
		source_operand1 = INM[1].substr(ptr2+1,2);
		source_operand2 = INM[1].substr(ptr3+1, ptr4-ptr3-1); 	
		length1 = source_operand1.length();
		length2 = source_operand2.length();
		//cout << opcode << endl;
		simulate(	ptr1 ,  ptr2 ,  ptr3 ,  ptr4 ,
					opcode , dest_reg , source_operand1 , 
					source_operand2, length1 ,  length2  );
	}
}

void simulate( 		int ptr1 , int ptr2 , int ptr3 , int ptr4 , string opcode , string dest_reg ,
					string source_operand1 , string source_operand2, int length1 , int length2  ){
	
	int read_check;
	read_check = read(source_operand1 , source_operand2 , opcode);
		//read_check = 1;
		if(read_check==1)
		{
            if  (stoi(source_operand1) < 10){
                INM[1].replace(ptr2+1,length1,source_operand1);
                INM[1].replace(ptr3,length2,source_operand2); 
            }
            else
            {
                INM[1].replace(ptr2+1,length1,source_operand1);
                INM[1].replace(ptr3+1,length2,source_operand2);
            }
			decode(); 	// Now when you remove the Add operand , then you need to check the INB back to see if other operation are ready
			//number_of_tokens++;
		}
		else{
            cout<<"Error!! Read_check=0";
			//token_arr[TOKEN_INM]=0;
		}
}
void decode(){
	INB.push_back(INM[1]);
	INM.erase(INM.begin() + 1);
	//cout<<"Decoded";
	token_arr[TOKEN_INB]=1;
	token_arr[TOKEN_INM]=1;
	//token_arr[TOKEN_AIB]=0;
	//token_number++;
}


void issue1(){
	//cout<<"issue1" << endl;
	//cout << "Issue1 1 " << "INB.size" << INB.size() << endl;
	AIB.push_back(INB[1]);
	INB.erase(INB.begin() + 1);
	//cout << "Issue1 2 " << "INB.size" << INB.size() << endl;
	token_arr[TOKEN_INB]=0;
	token_arr[TOKEN_INM]=1;
	token_arr[TOKEN_AIB]=1;
	//token_arr[TOKEN_SIB]=0;
}


void issue2(){
	SIB.push_back(INB[1]);
	//cout << "Issue2 2 " << "INB.size" << INB.size() << endl;
	INB.erase(INB.begin() + 1);
	//cout << "Issue2 2 " << "INB.size" << INB.size() << endl;
	token_arr[TOKEN_INB]=0;
	token_arr[TOKEN_INM]=1;
	token_arr[TOKEN_SIB]=1;
	//token_arr[TOKEN_AIB]=0;
}

void add_sub_unit(string opcode, string dest_reg ,string source_operand1 , string source_operand2){
	int temp;
	if(opcode=="ADD"){
		temp = stoi(source_operand1,nullptr) + stoi(source_operand2,nullptr);
	}
	if(opcode=="SUB"){
		temp = stoi(source_operand1,nullptr) - stoi(source_operand2,nullptr);
	}
	if(opcode=="MUL"){
		temp = stoi(source_operand1,nullptr) * stoi(source_operand2,nullptr);
	}
	REB.push_back("<"+dest_reg+","+to_string(temp)+">");
	AIB.erase(AIB.begin() + 1);
	//INB.erase(INB.begin() + 1);
	token_arr[TOKEN_REB]+=1;
	token_arr[TOKEN_AIB]=0;
}

void addr(string dest_reg ,string source_operand1 , string source_operand2){
	int temp;
	temp = stoi(source_operand1,nullptr) + stoi(source_operand2,nullptr);
	ADB.push_back("<"+dest_reg+","+to_string(temp)+">");
	SIB.erase(SIB.begin() + 1);
	//ADB.erase(ADB.begin() + 1); ////////////////////
	token_arr[TOKEN_ADB]=1;
	token_arr[TOKEN_SIB]=0;
	token_arr[TOKEN_REB]-=1;
}

void load(string dest_reg){
	int ptr1 , ptr2;
	string temp, temp1;
	ptr1 = ADB[1].find(",");
	ptr2 = ADB[1].find(">");
	temp = ADB[1].substr(4,ptr2-ptr1-1);
	
	int temp_p1,temp_p2;
	for(int i=1;i<DAM.size();i++){
		temp_p1 = DAM[i].find(",");
		
		if(((DAM[i].substr(1,temp_p1 - 1)) == temp)){
			//cout << "in if" << endl;
			ptr1 = DAM[i].find(",");
			ptr2 = DAM[i].find(">");
			temp = DAM[i].substr(ptr1+1,ptr2-ptr1-1);
			REB.push_back("<"+dest_reg+","+temp+">");
			ADB.erase(ADB.begin() + 1);
			REB.erase(REB.begin() + 1); ///////////////////////////////
		}
	}
	token_arr[TOKEN_REB]+=1;
	token_arr[TOKEN_ADB]=0;
}
void write(string dest_reg){
	// Over-Writes if the value if already there
	int flag1 = 0;
	int ptr3 ,ptr2 , ptr1;
	string temp;
	for(int i=0; i< RGF.size() ; i++){
		
		if(RGF[i].find(dest_reg)==1)
		{
			RGF.erase(RGF.begin() + i);
			RGF.push_back(REB[1]);
			REB.erase(REB.begin() + 1);
			sort(RGF.begin()+1,RGF.end());//,wayToSort);
			flag1=1;
			number_of_tokens--;
			
			break;

		}
		//token_arr[TOKEN_REB] -= 1;
		//token_arr[TOKEN_ADB]=0;
	}
		if(flag1 == 0){
			RGF.push_back(REB[1]);
			REB.erase(REB.begin() + 1);
			sort(RGF.begin()+1,RGF.end());//,wayToSort);
			number_of_tokens--;
		}
		token_arr[TOKEN_REB] -= 1;
		
		
}

void calculate_ptr(vector<string> pos){
	
	if(pos[0]=="INB:" || pos[0]=="AIB:" || pos[0]=="SIB:" || pos[0]=="PRB:"){
			
			//cout<<"calculate_ptr"<<"if"<<endl;
			int ptr1 , ptr2 , ptr3 , ptr4 ; 		// To find position of commas and separate out operands
				ptr1 = pos[1].find(",");			// position of 1st comma
				ptr2 = pos[1].find(",", ptr1+1);	// position of 2nd Comma
				ptr3 = pos[1].find("," ,ptr2+1);	// position of 3rd comma
				ptr4 = pos[1].find(">");
				
			
				int length1;
				int length2;

				dummy_opcode = pos[1].substr(1,ptr1-1);
				dummy_dest_reg = pos[1].substr(ptr1+1,2); //2 tells how many characters to be there in sub
				dummy_source_operand1 = pos[1].substr(ptr2+1,2);
				dummy_source_operand2 = pos[1].substr(ptr3+1, ptr4-ptr3-1); 
	}
	else
	{
		//cout<<"calculate_ptr"<<"else"<<endl;
		int ptr1 , ptr2;
		ptr1 = pos[1].find(",");
		ptr2 = pos[1].find(">");
		dummy_dest_reg = pos[1].substr(1,ptr1-1);;
	}
}

void transitions(int value){

	switch(value){
		

		case 0: 
				if (INB.size()>1) {
					//cout << "in CASE0 " <<" INB.size " << INB.size() << endl;
					calculate_ptr(INB);
					if((dummy_opcode=="ADD") || (dummy_opcode=="SUB") || (dummy_opcode=="MUL")){
						issue1();
					}
					else if((dummy_opcode=="LD") || (dummy_opcode=="ST")){
						issue2();
					}
				}
            

		break;

		case 1: 
				//cout << "in CASE1 " << endl;
				calculate_ptr(ADB);
				load(dummy_dest_reg);
		break;

		

		case 2:	
				//cout << "in CASE2 " << endl;
				calculate_ptr(SIB);
				addr(dummy_dest_reg ,dummy_source_operand1 , dummy_source_operand2);

		break;

		case 3:		
				//cout << "in CASE3 " << endl;
				calculate_ptr(AIB);
				add_sub_unit(dummy_opcode, dummy_dest_reg ,dummy_source_operand1 , dummy_source_operand2);

		break;
		
		case 4: 			
				//cout << "in CASE4 " << endl;
				if (REB.size()>1)
				{
					calculate_ptr(REB);
					write(dummy_dest_reg);
				}
				break;
				
		case 5:				
				//cout << "in CASE5 " << endl;
				if(INB.size()>0){
					calculate();
					token_arr[TOKEN_INB] = 1;
				}
		break;

		case 6: 			
				//cout << "in CASE6 " << endl;
				calculate_ptr(PRB);
				add_sub_unit(dummy_opcode, dummy_dest_reg ,dummy_source_operand1 , dummy_source_operand2);
				calculate_ptr(REB);
				write(dummy_dest_reg);
		break;

	}

}