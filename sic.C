#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include<iomanip>
#include<sstream>
using namespace std;
class ASSEMBLER
{
	class OPTAB
	{
		struct node
		{
			char name[8],code[2];
			node *link;
		}*hash_table[29];
		public:
		OPTAB();
		bool search(const char*);
		void insert(const char*,const char*);
		char* retrieve(const char*);
		~OPTAB()
		{
			int i;
			node *temp;
			for(i=0;i<29;i++)
			{
				for(;hash_table[i]!=NULL;)
				{
					temp=hash_table[i];
					hash_table[i]=temp->link;
					delete temp;
				}
			}
		}	
	}optab;
	class SYMTAB
	{
		struct node
		{
			char name[16];
			int address;
			node* link;
		}*hash_table[29];
		public:
		SYMTAB()
		{
			for(int i=0;i<29;i++)
				hash_table[i]=NULL;
		}
		bool search(const char*);
		void insert(const char*,const int);
		int retrieve(const char*);
		~SYMTAB()
		{
			int i;
                        node *temp;
                        for(i=0;i<29;i++)
                        {
                                for(;hash_table[i]!=NULL;)
                                {
                                        temp=hash_table[i];
                                        hash_table[i]=temp->link;
                                        delete temp;
                                }
                        }
		}
	}symtab;
	int LOCCTR,starting_address,program_length,error_redefined,error_undefined,error_invalid_opcode;
	char program_name[16],input_file[32],buffer[65536];
	ifstream fin;
	ofstream fout,object_program;
	public:
	ASSEMBLER(char* file)
	{
		error_redefined=0;
		error_undefined=0;
		error_invalid_opcode=0;
		strcpy(input_file,file);
		fin.open(input_file);
		fin.getline(buffer,65536,EOF);
		cout<<"<---INPUT FILE--->\n"<<buffer<<"\n";
		fin.close();
		fin.open(input_file);
		fout.open("intermediate.txt");
	}
	void pass1();
	void pass2();
	~ASSEMBLER()
	{
		fin.open("assembly_listing.txt");
		fin.getline(buffer,65536,EOF);
		cout<<"<---ASSEMBLY LISTING--->\n"<<buffer<<"\n";
		fin.close();
		fin.open(program_name);
		fin.getline(buffer,65536,EOF);
		cout<<"<---OBJECT PROGRAM--->\n"<<buffer<<"\n";
		fin.close();
		cout<<"\nOBJECT PROGRAM SIZE  : "<<program_length<<" Bytes";
		cout<<"\n\nERRORS:\n";
		cout<<"REDEFINED SYMBOLS : "<<error_redefined;
		cout<<"\nUNDEFINED SYMBOLS : "<<error_undefined;
		cout<<"\nINVALID OPCODES   : "<<error_invalid_opcode<<"\n";
	}
};
int hash_function(const char*);
int hextodec(char x[])
{
	int n=0,i,j=1;
	for(i=strlen(x)-1;i>=0;i--)
	{
		if(x[i]>=48 && x[i]<=57)
			n+=(x[i]-'0')*j;
		else
		{
			switch(x[i])
			{
				case 'A':
				case 'a':
				n+=10*j;
				break;
				case 'B':
				case 'b':
				n+=11*j;
				break;
				case 'C':
				case 'c':
				n+=12*j;
				break;
				case 'D':
				case 'd':
				n+=13*j;
				break;
				case 'E':
				case 'e':
				n+=14*j;
				break;
				case 'F':
				case 'f':
				n+=15*j;
				break;
			}
		}
		j*=16;
	}
	return n;
}
int main(int argc,char* argv[])
{
	char file_name[32];
	if(argc==1)
	{
		cout<<"PLEASE ENTER FILE NAME\n";
		cin>>file_name;
	}
	else
		strcpy(file_name,argv[1]);
	ASSEMBLER A(file_name);
	A.pass1();
	A.pass2();
	return 0;
}
int hash_function(const char name[])
{
	int i,sum=0;
	for(i=0;name[i]!='\0';i++)
		sum+=(int)name[i];
	return sum%29;
}
void ASSEMBLER::pass1()
{
	char label[16],opcode[8],operand[16];
	int loc;
	fin.getline(label,16,'\t');
	fin.getline(opcode,8,'\t');
	fin.getline(operand,16,'\n');
	if(!strcmp(opcode,"START"))
	{
		strcpy(program_name,label);
		starting_address=hextodec(operand);
		LOCCTR=starting_address;
		fout<<setw(4)<<setfill('0')<<hex<<LOCCTR<<"\t";
		fout<<label<<"\t";
		fout<<opcode<<"\t";
		fout<<operand<<"\n";
		fin.getline(label,16,'\t');
	        fin.getline(opcode,8,'\t');
       		fin.getline(operand,16,'\n');
	}
	else
	{
		starting_address=0;
		LOCCTR=0;
		strcpy(program_name,"UNTITLED");
	}
	while(strcmp(opcode,"END"))
	{
		loc=LOCCTR;
		if(strcmp(label,"."))
		{
			if(strlen(label)!=0)
			{
				if(!symtab.search(label))
					symtab.insert(label,LOCCTR);
				else
				{
					cout<<"ERROR: |"<<label<<"| REDEFINED\n";
					error_redefined++;
				}
			}
			if(optab.search(opcode))
				LOCCTR+=3;
			else if(!strcmp(opcode,"WORD"))
				LOCCTR+=3;
			else if(!strcmp(opcode,"RESW"))
				LOCCTR+=(3*atoi(operand));
			else if(!strcmp(opcode,"RESB"))
				LOCCTR+=atoi(operand);
			else if(!strcmp(opcode,"BYTE"))
			{
				if(operand[0]=='X')
					LOCCTR+=(strlen(operand)-3)/2;
				else
					LOCCTR+=(strlen(operand)-3);
			}
			else
			{
				cout<<"ERROR: INVALID OPCODE |"<<opcode<<"|\n";
				error_invalid_opcode++;
			}
			fout<<setw(4)<<setfill('0')<<hex<<loc;
		}
		fout<<"\t";
                fout<<label<<"\t";
                fout<<opcode<<"\t";
                fout<<operand<<"\n";
		fin.getline(label,16,'\t');
                fin.getline(opcode,8,'\t');
                fin.getline(operand,16,'\n');
	}
	fout<<"\t";
        fout<<label<<"\t";
        fout<<opcode<<"\t";
        fout<<operand<<"\n";
	fin.close();
	fout.close();
	fin.open("intermediate.txt");
	fin.getline(buffer,65536,EOF);
	cout<<"<---INTERMEDIATE FILE--->\n"<<buffer<<"\n";
	fin.close();
	program_length=LOCCTR-starting_address;
}
void ASSEMBLER::pass2()
{
	char address[8],label[16],opcode[8],operand[16],first_address[4];
	int text_record_length=0,incr=0;
	bool relative,RES;
	char object_code[6],text_record[64];
	stringstream T(text_record),O(object_code);
	fin.open("intermediate.txt");
	fout.open("assembly_listing.txt");
	object_program.open(program_name);
	fin.getline(address,8,'\t');
	fin.getline(label,16,'\t');
	fin.getline(opcode,8,'\t');
	fin.getline(operand,16,'\n');
	if(!strcmp(opcode,"START"))
	{
		fout<<address<<"\t";
		fout<<label<<"\t";
		fout<<opcode<<"\t";
		fout<<operand<<"\t";
		fout<<"\t\n";
		fin.getline(address,8,'\t');
		fin.getline(label,16,'\t');
        	fin.getline(opcode,8,'\t');
	        fin.getline(operand,16,'\n');
	}
	object_program<<"H."<<program_name;
	for(int i=0;i<6-strlen(program_name);i++)
		object_program<<" ";
	object_program<<"."<<setw(6)<<setfill('0')<<hex<<starting_address<<".";
	object_program<<setw(6)<<setfill('0')<<hex<<program_length<<"\n";
	strcpy(first_address,address);
	while(strcmp(opcode,"END"))
	{
		O.str("");
		if(strcmp(label,"."))
		{
			if(optab.search(opcode))
			{
				if(strlen(operand)!=0)
				{
					char actual_operand[16]="";
					relative=false;
					for(int i=0;i<strlen(operand)-2;i++)
					{
						actual_operand[i]=operand[i];
						if(operand[i+1]==',' && operand[i+2]=='X')
						{
							relative=true;
							break;
						}
					}
					if(relative)
					{
						if(symtab.search(actual_operand))
						{
							O<<optab.retrieve(opcode)<<setw(4)<<setfill('0')<<hex<<symtab.retrieve(actual_operand)+32768;
						}
						else
						{
                                	                O<<optab.retrieve(opcode)<<"0000";
                        	                        cout<<"ERROR: |"<<operand<<"| UNDEFINED\n";
                	                                error_undefined++;
        	                                }
					
					}
					else if(symtab.search(operand))
						O<<optab.retrieve(opcode)<<setw(4)<<setfill('0')<<hex<<symtab.retrieve(operand);
					else
					{
						O<<optab.retrieve(opcode)<<"0000";
						cout<<"ERROR: |"<<operand<<"| UNDEFINED\n";
						error_undefined++;
					}
				}
				else
					O<<optab.retrieve(opcode)<<"0000";
				incr=3;
			}
			else if(!(strcmp(opcode,"BYTE")&&strcmp(opcode,"WORD")))
			{
				if(operand[0]=='C')
				{
					for(int i=2;i<strlen(operand)-1;i++)
						O<<hex<<(int)operand[i];
					incr=strlen(operand)-3;
				}
				else if(operand[0]=='X')
				{
					for(int i=2;i<strlen(operand)-1;i++)
						O<<(char)tolower(operand[i]);
					incr=(strlen(operand)-3)/2;
				}
				else
				{
					O<<setw(6)<<setfill('0')<<hex<<atoi(operand);
					incr=3;
				}
			}
			text_record_length+=incr;
			if(text_record_length>30)
			{
				object_program<<"T."<<setw(6)<<setfill('0')<<first_address<<"."<<setw(2)<<setfill('0')<<hex<<text_record_length-incr<<T.str()<<"\n";
				text_record_length=incr;
				strcpy(first_address,address);
				T.str("");
			}
			T<<"."<<O.str();
		}
		fout<<address<<"\t";
		fout<<label<<"\t";
		fout<<opcode<<"\t";
		fout<<operand<<"\t";
		fout<<O.str()<<"\n";
		fin.getline(address,8,'\t');
		fin.getline(label,16,'\t');
		fin.getline(opcode,8,'\t');
		fin.getline(operand,16,'\n');
		RES=false;
		if((!strcmp(opcode,"RESW"))||(!strcmp(opcode,"RESB")))
		{
			object_program<<"T."<<setw(6)<<setfill('0')<<first_address<<"."<<setw(2)<<setfill('0')<<hex<<text_record_length<<T.str()<<"\n";
			text_record_length=0;
			T.str("");
		}
		while((!strcmp(opcode,"RESW"))||(!strcmp(opcode,"RESB")))
		{
			fout<<address<<"\t";
                        fout<<label<<"\t";
                        fout<<opcode<<"\t";
                        fout<<operand<<"\n";
			fin.getline(address,8,'\t');
	                fin.getline(label,16,'\t');
        	        fin.getline(opcode,8,'\t');
                	fin.getline(operand,16,'\n');
			RES=true;
		}
		if(RES)
			strcpy(first_address,address);
	}
	if(!RES)
		object_program<<"T."<<setw(6)<<setfill('0')<<first_address<<"."<<setw(2)<<setfill('0')<<hex<<text_record_length<<T.str()<<"\n";
	object_program<<"E."<<setw(6)<<setfill('0')<<hex<<starting_address;
		fout<<address<<"\t";
                fout<<label<<"\t";
                fout<<opcode<<"\t";
                fout<<operand<<"\t";
		fin.close();
		fout.close();
		object_program.close();
}
bool ASSEMBLER::SYMTAB::search(const char name[])
{
	int i=hash_function(name);
	node *r;
	for(r=hash_table[i];r!=NULL;r=r->link)
		if(!strcmp(name,r->name))
			return true;
	return false;
}
void ASSEMBLER::SYMTAB::insert(const char name[],int address)
{
	int i=hash_function(name);
	node *temp=new node;
	strcpy(temp->name,name);
	temp->address=address;
	if(hash_table[i]==NULL)
		temp->link=NULL;
	else
		temp->link=hash_table[i];
	hash_table[i]=temp;
}
int ASSEMBLER::SYMTAB::retrieve(const char name[])
{
	int i=hash_function(name);
	node *r;
	for(r=hash_table[i];r!=NULL;r=r->link)
		if(!strcmp(name,r->name))
			return r->address;
	return 0;
}
ASSEMBLER::OPTAB::OPTAB()
{
	for(int i=0;i<29;i++)
		hash_table[i]=NULL;
	insert("ADD","18");
	insert("AND","40");
	insert("COMP","28");
	insert("DIV","24");
	insert("J","3c");
	insert("JEQ","30");
	insert("JGT","34");
	insert("JLT","38");
	insert("JSUB","48");
	insert("LDA","00");
	insert("LDCH","50");
	insert("LDL","08");
	insert("LDX","04");
	insert("MUL","20");
	insert("OR","44");
	insert("RD","d8");
	insert("RSUB","4c");
	insert("STA","0c");
	insert("STCH","54");
	insert("STL","14");
	insert("STSW","e8");
	insert("STX","10");
	insert("SUB","1c");
	insert("TD","e0");
	insert("TIX","2c");
	insert("WD","dc");
}
bool ASSEMBLER::OPTAB::search(const char name[])
{
	int i=hash_function(name);
        node *r;
        for(r=hash_table[i];r!=NULL;r=r->link)
                if(!strcmp(name,r->name))
                        return true;
        return false;

}
void ASSEMBLER::OPTAB::insert(const char name[],const char code[])
{
	int i=hash_function(name);
	node *temp=new node;
	strcpy(temp->name,name);
	strcpy(temp->code,code);
	if(hash_table[i]==NULL)
		temp->link=NULL;
	else
		temp->link=hash_table[i];
	hash_table[i]=temp;
}
char* ASSEMBLER::OPTAB::retrieve(const char name[])
{
        int i=hash_function(name);
        node *r;
        for(r=hash_table[i];r!=NULL;r=r->link)
                if(!strcmp(name,r->name))
                        return r->code;
        return NULL;
}

