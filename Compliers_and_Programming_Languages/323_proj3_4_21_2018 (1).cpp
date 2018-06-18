#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <stack>
using namespace std;
/////////////////////////////////////////////////////////////////////////////////////////

/*PART 3
Here, we are outputting the assembly instructions for a target machine.  We utilize a
structure for both the assembly instructions table as well as the symbol table.  We
use a jumpstack to track the beginning of our loops to write an assembly instruction and
determine the location of the label.  These instructions are generated during the recursive
descent parser.
*/

//ASSEMBLY INSTRUCTION TABLE
struct assembly_instruc_Table {
	string op[1000];
	int operand[1000];
	int instr_row_num = 0;
	int instr_line[1000];
};
//to track address line
int instr_address = 1;
string temp_op;

//creating an instance of the structure
assembly_instruc_Table instruct_Table;

//generating instructions
void gen_instr(string op, int oprand)
{
	instruct_Table.instr_line[instruct_Table.instr_row_num] = instr_address;
	instruct_Table.op[instruct_Table.instr_row_num] = op;
	instruct_Table.operand[instruct_Table.instr_row_num] = oprand;
	instruct_Table.instr_row_num++;
	instr_address++;
}

//function to print the instruction table
void print_instr_t(ofstream& ofs) {
	for (int i = 0; i < instruct_Table.instr_row_num; i++) {
		ofs << instruct_Table.instr_line[i] << "\t"
			<< instruct_Table.op[i] << "\t"
			<< instruct_Table.operand[i] << endl;
	}
}

//the jumpstack helps us determine which line our loops starts and helps
//fill the instructions table
stack <int> jumpstack;

//tells us when the loops start uses stack to track changes.
void back_patch(int jump_address) {
	//go back to line number containing jumpz
	instruct_Table.instr_row_num = jumpstack.top() - 1;
	jumpstack.pop();
	//insert the end of the loop line number
	instruct_Table.operand[instruct_Table.instr_row_num] = jump_address;
	//set instruct table row num back to original location
	instruct_Table.instr_row_num = instr_address - 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
//SYMBOL TABLE:
struct SYMBOL_TABLE {
	string id_array[100];
	string id_type[100];
	int mem_address[100];
};
//instances of symbol table
SYMBOL_TABLE symbol_table;

//counter for symbol table
int st_row_num = 0;
int mem_addr = 2000;
int pop_mem_addr;
string id_type; //save
string id_lexeme;


//Symbol Table Functions
bool lookupSymbolTable(string token);

//function to insert into symbol table
void addIdToSymTable(string token) {
	//lookup sysmbol table and if id not present add it.
	if (!(lookupSymbolTable(token))) {
		symbol_table.id_array[st_row_num] = token;
		symbol_table.id_type[st_row_num] = id_type;
		symbol_table.mem_address[st_row_num] = mem_addr;
		st_row_num++;
		mem_addr++;
	}
}
//function to lookup id into symbol table
bool lookupSymbolTable(string token) {
	for (int i = 0; i < (mem_addr - 2000); i++) {
		if (token == symbol_table.id_array[i]) {
			pop_mem_addr = symbol_table.mem_address[i];
			return true;
		}
	}
	return false;
}
//function to print the symbol table
void printSymbolTable(ofstream& ofs) {
	for (int i = 0; i < st_row_num; i++) {
		ofs << symbol_table.id_array[i] << "\t"
			<< symbol_table.id_type[i] << "\t"
			<< symbol_table.mem_address[i] << endl;
	}
}
//end part 3
/////////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS TO WRITE TO FILE
template <typename T>
void writetofile(ofstream& ofs, T& x, string token, LA_output & val) {
	val.lexeme = x;
	val.token = token;
	ofs << setw(10) << x << "\t\t\t" << token << endl;
}
template <typename T>
void writetofile(T* x, string token, LA_output & val) {
	val.lexeme = x;
	val.token = token;
	writetofile(*x, token);
}

void SA_output(ofstream & ofs, string production) {
	ofs << production;
}

void SA_output_error(ofstream & ofs, string lexeme, int line_counter) {
	ofs << "ERROR: line " << line_counter << "\t" << lexeme << endl;
}
///////////////////////////////////////////////////////////////////////////////////
/* PART 1: Lexical Analyzer
To create the lexical analyzer we converted the RE (regular expressions) to a
NFSM (non-finite state machine) using thompsons; and finally, we simplified
the NFSM using subsets to construct a FSM.  This parses the source code and
returns tokens and the lexeme in a structure.
*/

//for error printing
int line_counter = 1;

bool end_file = false;

//struct for holding LA record
struct LA_output {
    string token;   //category
    string lexeme;  //actual
};


void m_id_key(ofstream& ofs, char *&p, string * keyword, LA_output & val) {
	//since starting state is 1 and entering this function requires a letter input
	int state = 2;
	bool found_keyword = false;
	string word = "";
	/*
	A finite state machine for identifiers and keywords;
	will continue checking character inputs as long as
	they are valid inputs (letter, digit, $).
	Accepting states 2, 3, 5
	*/
	int id_FSM[6][4]{
		//state   L   D   $
		1,        2,  6,  6,
		2,        3,  4,  5,
		3,        3,  4,  5,
		4,        3,  4,  5,
		5,        6,  6,  6,
		6,        6,  6,  6

	};
	//while valid input
	while (((isalpha(*p)) || (isdigit(*p))) || (*p == '$')) {
		if (isalpha(*p))
			state = id_FSM[state - 1][1];
		else if (isdigit(*p))
			state = id_FSM[state - 1][2];
		else
			state = id_FSM[state - 1][3];
		word += *p;
		++p;
	}
	//if accepting state
	if (((state == 2) || (state == 3)) || (state == 5)) {
		//check if keyword
		for (int i = 0; i < 13; i++) {
			if (word == keyword[i]) {
				found_keyword = true;
				writetofile(ofs, word, "keyword", val);
			}
		}
		//otherwise its an identifier
		if (!found_keyword)
			writetofile(ofs, word, "identifier", val);
	}
	//not accepting state
	else
		writetofile(ofs, word, "unknown", val);
}
//determines whether the input characters are integers or real numbers
void m_int_real(ofstream& ofs, char*&p, LA_output & val) {
	int state = 2; //set to 2 since input char was a digit
    /*
    combination of integer and real number FSM
    states 2 is accepting states for integer numbers
    state 4 is the accepting state for real numbers
    state 5 is an error state.
    */
	int int_real_FSM[5][3]{
		//state     D   .
		1,          2,  5,
		2,          2,  3,
		3,          4,  5,
		4,          4,  5,
		5,          5,  5,
	};
	string word = "";
	//while valid input
	while (isdigit(*p) || (*p) == '.') {
		if (isdigit(*p))
			state = int_real_FSM[state - 1][1];
		else
			state = int_real_FSM[state - 1][2];
		word += *p;
		++p;
	}
	if (state == 2)
		writetofile(ofs, word, "integer", val);
	else if (state == 4)
		writetofile(ofs, word, "real", val);
	else
		writetofile(ofs, word, "unknown", val);
}

//determines if the character is a separator
bool is_sep(ofstream& ofs, char *&p, char * sep, LA_output & val) {
	for (int i = 0; i < 10; i++) {
		if (*p == sep[i]) {
			if (*p == '%') {
				//if next char is also %
				if (*(++p) == '%') {
					writetofile(ofs, "%%", "separator", val);
					++p;
					return true;
				}
				else {
					//decrement to original p for error message
					--p;
					char tmp = p[0];
					writetofile(ofs, tmp, "unknown", val);
					++p;
					return true;
				}
			}
			else {
				char tmp = p[0];
				writetofile(ofs, tmp, "separator", val);
				++p;
				return true;
			}
		}
	}
	return false;
}
//determines if the current character and next char are operators
bool is_op(ofstream& ofs, char *&p, char * ope, LA_output & val) {
	string two_char_operator[4] = { "==", "=>", "=<", "^=" };

	string word_ope = "";
	for (int i = 0; i < 8; i++) {
		if (*p == ope[i]) {
			word_ope += *p;
			++p;
			//second loop to check if next char is part of operator
			for (int k = 0; k < 8; k++) {
				if (*p == ope[k]) {
					word_ope += *p;
					for (int j = 0; j < 4; j++) {
						if (word_ope == two_char_operator[j]) {
							writetofile(ofs, word_ope, "operator", val);
							//cout << word_ope << " is valid" << endl;
							++p;
							return true;
						}//end if two_char operator
					}//end for checking valid two_char operator
					 //if second char isn't a valid operator
					++p;
					writetofile(ofs, word_ope, "unknown", val);
					//cout << word_ope << " is not valid" << endl;
					return true;
				}//end if second char is an operator
			}//end for k
			writetofile(ofs, word_ope, "operator", val);
			//cout << word_ope << " is valid" << endl;
			return true;
		}//end if first char is an operator
	}
	//cout << word_ope << " is not valid" << endl;
	return false;
}


//checks for comments
bool is_comment(char *&p, fstream & fin, string & x) {
	if (*p == '!') {
		++p;
		while (*p != '!')
		{
			if (p != &x[x.length()])
				++p;
			else
			{
				getline(fin, x);
				p = &x[0];
				line_counter++;
			}
		}
		++p;
		return true;
	}
	return false;
}

//the lexer function considers all valid inputs
LA_output lexer(char *& p, ofstream & outfile, fstream & fin, string & x) {
	//valid keywords, separators, and operators
	string keywords[13] = { "int","boolean","real","function","return","true",
		"false","if","else","endif","while","get","put" };
	char separators[10] = { '(',')',':',',','{','}', ']', '[', ';', '%' };
	char operators[8] = { '<','>','=','^','+','-','*','/' };

	LA_output ex;
	if (isalpha(*p))m_id_key(outfile, p, keywords, ex);
	else if (isdigit(*p))m_int_real(outfile, p, ex);
	else if (is_op(outfile, p, operators, ex)) {}
	else if (is_sep(outfile, p, separators, ex)) {}
	else if ((int)*p <= 32) {
		if ((int)(*p) == 13) ex.token = "return carridge";
		else if ((int)*p <= 0) ex.token = "error";
		else {
			//cout << (int)(*p) << endl;
			ex.token = "undefined space";
		}
		++p;
	}
	else if (*p == '!') {
		++p;
		while (*p != '!')
		{
			if (p != &x[x.length()])
				++p;
			else
			{
				getline(fin, x);
				line_counter++;
				p = &x[0];
			}
		}
		ex.lexeme = "";
		++p;
	}
	else {
		++p;
		ex.token = "unknown";
	}
	return ex;
}
//end LA (Lexical Analyzer)
/////////////////////////////////////////////////////////////////////////////////////////

//Headers
void SA(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void new_line(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void get_token(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void functiondefinitions(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void statementlistP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void declarationlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void parameterlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void term(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void expression(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void qualifier(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void statementlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void statement(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void ids_SCA(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void ids_PRI(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);
void ids_DEC(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code);

///////////////////////////////////////////////////////////////////////////////////////////////////
/*PART 2: using recursive descent parser to check grammer.
Here, we are using a recursive descent parser to parse the source code and determine whether the
syntax is correct.  Using a top-down parse we removed our grammar's left recursion and back-tracking.
We then constructed a function for each rule and checked the tokens to ensure they are consistent
with the grammar.
*/

//if we find an error we exit
void error(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output_error(outfile, record.lexeme, line_counter - 1);

	cout << "symbol table and instruction table before error: \n";
	printSymbolTable(outfile);
	print_instr_t(outfile);

	exit(0);
}

//the end of the line doesn't mean the end of SA
void get_token(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	record = lexer(p, outfile, fin, line_code);
	bool prob = true;

	while (prob) {
		if (fin.eof()) {
			end_file = true;
			//break;
			prob = false;
		}
		else if (record.token == "unknown") {
			if (getline(fin, line_code)) {
				p = &line_code[0];
				line_counter++;
			}
			record = lexer(p, outfile, fin, line_code);
		}
		else if ((record.token == "return carridge") || (record.token == "error")) {
			new_line(p, outfile, fin, record, line_code);
			record = lexer(p, outfile, fin, line_code);
		}
		else if (record.lexeme.length() == 0) {
			record = lexer(p, outfile, fin, line_code);
		}
		else {
			prob = false;
		}
	}
}

//when we run into an error, we can call new_line
void new_line(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	if (getline(fin, line_code)) {
		p = &line_code[0];
		line_counter++;
	}
}

//r29
void empty(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Empty>\n");
}

//r28p
void primaryP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Primary Prime> -> (<Ids>) | Empty\n");
	if (record.lexeme == "(") {
		get_token(p, outfile, fin, record, line_code);
		ids_PRI(p, outfile, fin, record, line_code);
		if (record.lexeme == ")") {
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		SA_output(outfile, "<Primary Prime> -> <Empty>\n");
	}
}

//r28
//problem with <identifier>:
void primary(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Primary> ->  <Identifier> <Primary Prime> | <Integer> | (<Expression>) | <Real> | true | false\n");
	//automatically called
	if (record.token == "identifier") {
		if (!(lookupSymbolTable(record.lexeme))) {
			cout << "<primary>: lexeme not found" << endl;
			exit(0);
		}
		gen_instr("PUSHM", pop_mem_addr);
		get_token(p, outfile, fin, record, line_code);
		primaryP(p, outfile, fin, record, line_code);
	}
	else if (record.token == "integer") {
		gen_instr("PUSHI", stoi(record.lexeme));
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Primary> -> <integer>\n");
	}
	else if (record.lexeme == "(") {
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Primary> -> (<Expression>)\n");
		expression(p, outfile, fin, record, line_code);
		if (record.lexeme == ")") {
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else if (record.token == "Real") {
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Primary> -> <real>\n");
	}
	else if (record.lexeme == "true") {
		get_token(p, outfile, fin, record, line_code);
		gen_instr("PUSHI", 1);
		SA_output(outfile, "<Primary> -> true\n");
	}
	else if (record.lexeme == "false") {
		get_token(p, outfile, fin, record, line_code);
		gen_instr("PUSHI", 0);
		SA_output(outfile, "<Primary> -> false\n");
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r27
void factor(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Factor> -> -<Primary> | <Primary>\n");
	//automatically called

	if (record.lexeme == "-") {
		gen_instr("SUB", 0);
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Factor> -> - <Primary>\n");
		primary(p, outfile, fin, record, line_code);
	}
	else {
		SA_output(outfile, "<Factor> -> <Primary>\n");
		primary(p, outfile, fin, record, line_code);
	}
}


//r26 prime
void termP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Term Prime> -> *<Factor> <Term Prime> | /<Factor> <Term Prime> |<Empty>\n");
	//automatically called
	if (record.lexeme == "*") {
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Term Prime> -> * <Factor> <Term Prime>\n");
		factor(p, outfile, fin, record, line_code);
		gen_instr("MUL", 0);
		termP(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "/") {
		get_token(p, outfile, fin, record, line_code);
		SA_output(outfile, "<Term Prime> -> / <Factor> <Term Prime>\n");
		factor(p, outfile, fin, record, line_code);
		gen_instr("DIV", 0);
		termP(p, outfile, fin, record, line_code);
	}
	else {
		SA_output(outfile, "<Term Prime> -> <Empty>\n");
	}
}

//r26
void term(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Term> -> <Factor> <Term Prime>\n");
	factor(p, outfile, fin, record, line_code);
	termP(p, outfile, fin, record, line_code);
}


//r25 prime
void expressionP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Expression Prime> -> + <Term> <Expression Prime> | - <Term> <Expression Prime> | Empty\n");
	if ((record.lexeme == "+") || (record.lexeme == "-")) {
		if (record.lexeme == "+") {
			get_token(p, outfile, fin, record, line_code);
			term(p, outfile, fin, record, line_code);
			expressionP(p, outfile, fin, record, line_code);
			gen_instr("ADD", 0);
		}
		else {
			get_token(p, outfile, fin, record, line_code);
			term(p, outfile, fin, record, line_code);
			expressionP(p, outfile, fin, record, line_code);
			gen_instr("SUB", 0);
		}
	}
	else {
		SA_output(outfile, "<Expression Prime> -> <Empty>\n");
	}
}

//r25
void expression(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Expression> -> <Term> <Expression Prime>\n");
	term(p, outfile, fin, record, line_code);
	expressionP(p, outfile, fin, record, line_code);
}

//r24
void relop(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Relop> -> == | ^= | > | < | => | =< \n");

	//save the operator (>, <, etc.) to be used for instruct table later
	temp_op = record.lexeme;

	if ((record.lexeme == "==") || (record.lexeme == "^=")) {
		get_token(p, outfile, fin, record, line_code);
	}
	else if ((record.lexeme == ">") || (record.lexeme == "<")) {
		get_token(p, outfile, fin, record, line_code);
	}
	else if ((record.lexeme == "=>") || (record.lexeme == "=<")) {
		get_token(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r23
void condition(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Condition> -> <Expression> <Relop> <Expression>\n");
	expression(p, outfile, fin, record, line_code);
	relop(p, outfile, fin, record, line_code);

	expression(p, outfile, fin, record, line_code);

	if (temp_op == "==") {
		gen_instr("EQU", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else if (temp_op == "^=") {
		gen_instr("NEQ", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else if (temp_op == ">") {
		gen_instr("GRT", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else if (temp_op == "<") {
		gen_instr("LES", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else if (temp_op == "=>") {
		gen_instr("GEQ", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else if (temp_op == "=<") {
		gen_instr("LEQ", 0);
		jumpstack.push(instr_address);
		gen_instr("JUMPZ", 0);
	}
	else {
		cout << "<Relop> error" << endl;
		exit(0);
	}

}

//r22
void while_r(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<While> -> while (<Condition>) <Statement>\n");


	if (record.lexeme == "while") {
		const int addr = instruct_Table.instr_row_num + 1;
		//assembly instructions for starting the loop
        gen_instr("LABEL", 0);
		get_token(p, outfile, fin, record, line_code);
		if (record.lexeme == "(") {
			get_token(p, outfile, fin, record, line_code);

			condition(p, outfile, fin, record, line_code);
			if (record.lexeme == ")") {
				get_token(p, outfile, fin, record, line_code);
				statement(p, outfile, fin, record, line_code);
				gen_instr("JUMP", addr);
				back_patch(instr_address);
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
}

//r21
void scan(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Scan> -> get (<Ids>);\n");
	if (record.lexeme == "get") {
		get_token(p, outfile, fin, record, line_code);
		if (record.lexeme == "(") {
			get_token(p, outfile, fin, record, line_code);
			ids_SCA(p, outfile, fin, record, line_code);
			if (record.lexeme == ")") {
				get_token(p, outfile, fin, record, line_code);
				if (record.lexeme == ";") {
					get_token(p, outfile, fin, record, line_code);
				}
				else {
					error(p, outfile, fin, record, line_code);
				}
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
}

//r20
void print(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Print> -> put ( <Expression> );\n");
	if (record.lexeme == "put") {
		get_token(p, outfile, fin, record, line_code);
		if (record.lexeme == "(") {
			get_token(p, outfile, fin, record, line_code);
			expression(p, outfile, fin, record, line_code);
			if (record.lexeme == ")") {
				get_token(p, outfile, fin, record, line_code);
				//OUTPUT THE EXPRESSION
				gen_instr("STDOUT", 0);

				if (record.lexeme == ";") {
					get_token(p, outfile, fin, record, line_code);
				}
				else {
					error(p, outfile, fin, record, line_code);
				}
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
}

//r19p
void return_rp(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<ReturnP> -> ;|<Expression>;\n");

	if (record.lexeme == ";") {
		get_token(p, outfile, fin, record, line_code);
	}
	else {
		expression(p, outfile, fin, record, line_code);
		if (record.lexeme == ";") {
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}

}

//r19
void return_r(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Return> -> return <ReturnP>\n");

	if (record.lexeme == "return") {
		get_token(p, outfile, fin, record, line_code);
		return_rp(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}

}

//r18p
void if_rp(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<If Prime> -> endif | else <Statement> endif\n");
	if (record.lexeme == "endif") {
		back_patch(instr_address);
		get_token(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "else") {
		get_token(p, outfile, fin, record, line_code);
		statement(p, outfile, fin, record, line_code);
		if (record.lexeme == "endif") {
			back_patch(instr_address);
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}

	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}


//r18
void if_r(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<If> -> if ( <Condition> ) <Statement> <ifP> \n");

	if (record.lexeme == "if") {
		//for the back_patch line # i.e. when the if condition starts
		get_token(p, outfile, fin, record, line_code);
		if (record.lexeme == "(") {
			get_token(p, outfile, fin, record, line_code);
			condition(p, outfile, fin, record, line_code);
			if (record.lexeme == ")") {
				get_token(p, outfile, fin, record, line_code);
				statement(p, outfile, fin, record, line_code);
				if_rp(p, outfile, fin, record, line_code);
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}

	}
}

//r17
void assign(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Assign> -> <Identifier> = <Expression>;\n");

	if (record.token == "identifier") {
		/*
		want to save the identifier to find the memory address
		which will be used to insert as an operand for the instruction
		table.
		*/
		id_lexeme = record.lexeme;

		if (!(lookupSymbolTable(record.lexeme))) {
			cout << "<Assign>: could not find lexeme" << endl;
			exit(0);
		}

		get_token(p, outfile, fin, record, line_code);
		if (record.lexeme == "=") {
			get_token(p, outfile, fin, record, line_code);
			if ((((((record.token == "identifier") || (record.token == "integer")) || (record.token == "real")) || (record.lexeme == "true")) || (record.lexeme == "false")) || (record.lexeme == "(")) {
				expression(p, outfile, fin, record, line_code);
				/*
				pop the arithmetic operation results to left-side of equal sign
				get_instr("POPM", get_address(id_type));
				*/
				if (lookupSymbolTable(id_lexeme)) {
					gen_instr("POPM", pop_mem_addr);
				}

				if (record.lexeme == ";") {
					get_token(p, outfile, fin, record, line_code);
				}
				else {
					error(p, outfile, fin, record, line_code);
				}
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
}

//r16
void compound(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Compound> -> { <Statement List> }\n");

	if (record.lexeme == "{") {
		get_token(p, outfile, fin, record, line_code);
		statementlist(p, outfile, fin, record, line_code);
		if (record.lexeme == "}") {
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		error(p, outfile, fin, record, line_code);
	}

}

//r15
void statement(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>\n");

	if (record.lexeme == "{") {
		compound(p, outfile, fin, record, line_code);
	}
	else if (record.token == "identifier") {
		assign(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "if") {
		if_r(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "return") {
		return_r(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "put") {
		print(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "get") {
		scan(p, outfile, fin, record, line_code);
	}
	else if (record.lexeme == "while") {
		while_r(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r14p
void statementlistP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Statement ListP> -><Empty>|<Statement List\n");

	if (end_file) {
		
	}
	else {
		while (((record.token == "identifier") || (record.token == "keyword")) || (record.lexeme == "{")) {
			statement(p, outfile, fin, record, line_code);
		}
	}

	SA_output(outfile, "<Statement List Prime> -> <Empty>\n");
}

//r14
void statementlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Statement List> -><Statement><Statement ListP\n");
	statement(p, outfile, fin, record, line_code);
	statementlistP(p, outfile, fin, record, line_code);
}

/*
 Problem:
 IDS is called by three separate productions each of which expect something
 different with regard to the assembly instructions
 1. If called by <Declaration>, we don't expect the identifier to exist in the symbol table
 2. If called by <Scan>, we expect the identifier to exist for STDIN
 3. If called by <Primary Prime>, we expect the identifier to exist; like a while statement
 */

//r13p
void idsP_PRI(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Ids Prime> ->  <Empty> |, <Ids>\n");
	if (record.lexeme == ",") {
		get_token(p, outfile, fin, record, line_code);
		ids_PRI(p, outfile, fin, record, line_code);
	}
	else {
		//empty
		SA_output(outfile, "<Ids Prime> -> <Empty>\n");
	}
}

//r13p
void idsP_SCA(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Ids Prime> ->  <Empty> |, <Ids>\n");
	if (record.lexeme == ",") {
		get_token(p, outfile, fin, record, line_code);
		ids_SCA(p, outfile, fin, record, line_code);
	}
	else {
		//empty
		SA_output(outfile, "<Ids Prime> -> <Empty>\n");
	}
}


//r13p
void idsP_DEC(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Ids Prime> ->  <Empty> |, <Ids>\n");
	if (record.lexeme == ",") {
		get_token(p, outfile, fin, record, line_code);
		ids_DEC(p, outfile, fin, record, line_code);
	}
	else {
		//empty
		SA_output(outfile, "<Ids Prime> -> <Empty>\n");
	}
}

//r13 PRI
//PRIMARY
void ids_PRI(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<IDs> ->  <identifier> <IDs Prime>\n");
	if (record.token == "identifier") {
		if ((lookupSymbolTable(record.lexeme))) {
			get_token(p, outfile, fin, record, line_code);
			idsP_PRI(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r13 SCA
//SCAN
void ids_SCA(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<IDs> ->  <identifier> <IDs Prime>\n");
	if (record.token == "identifier") {
		if ((lookupSymbolTable(record.lexeme))) {
			gen_instr("STDIN", 0);
			gen_instr("POPM", pop_mem_addr);
			get_token(p, outfile, fin, record, line_code);
			idsP_SCA(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r13 DEC
//DECLARATION
void ids_DEC(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<IDs> ->  <identifier> <IDs Prime>\n");
	if (record.token == "identifier") {

		if (!(lookupSymbolTable(record.lexeme))) {
			//insert identifier to symbol table
			addIdToSymTable(record.lexeme);
			get_token(p, outfile, fin, record, line_code);
			idsP_DEC(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r12
void declaration(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Declaration> ->  <Qualifier> <Ids>\n");
	qualifier(p, outfile, fin, record, line_code);
	ids_DEC(p, outfile, fin, record, line_code);
}

//r11p
void declarationlistP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Declaration List Prime> ->  <Empty> | <Declaration List>\n");
	while (((record.token == "integer") || (record.token == "real")) || ((record.lexeme == "true") || (record.lexeme == "false"))) {
		declarationlist(p, outfile, fin, record, line_code);
	}
	SA_output(outfile, "<Declaration List Prime> -> <Empty>\n");
}

//r11
void declarationlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Declaration List> ->  <Declaration> ; <Declaration List Prime>\n");
	declaration(p, outfile, fin, record, line_code);
	if (record.lexeme == ";") {
		get_token(p, outfile, fin, record, line_code);
		declarationlistP(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}


//r10
void optDeclarationlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Opt Declaration List> ->  <Declaration List> | <Empty>\n");
	while (((record.lexeme == "int") || (record.lexeme == "real")) || (record.lexeme == "boolean")) {
		declarationlist(p, outfile, fin, record, line_code);
	}
	SA_output(outfile, "<Opt Declaration List> -> <Empty>\n");
	if (record.lexeme.length() > 0) {}

}

//r9
void body(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Body> ->  { <Statement List> }\n");

	if (record.lexeme == "{") {
		get_token(p, outfile, fin, record, line_code);
		statementlist(p, outfile, fin, record, line_code);
		if (record.lexeme == "}") {
			get_token(p, outfile, fin, record, line_code);
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r8
void qualifier(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Qualifier> -> int | boolean | real\n");


	if (((record.lexeme == "int") || (record.lexeme == "real")) || (record.lexeme == "boolean")) {
		//save type (global variable) as this lexeme and use it for all of the IDs provided.
		id_type = record.lexeme;
		get_token(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r7
void parameter(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Parameter> -> <Ids> : <Qualifier>\n");

	ids_DEC(p, outfile, fin, record, line_code);
	if (record.lexeme == ":") {
		get_token(p, outfile, fin, record, line_code);
		qualifier(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}
}

//r6p
void parameterlistP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Parameter List Prime> -> <Empty> | , <Parameter List>\n");
	while (record.lexeme == ",") {
		get_token(p, outfile, fin, record, line_code);
		parameterlist(p, outfile, fin, record, line_code);
	}
	SA_output(outfile, "<Parameter List Prime> -> <Empty>\n");
}

//r6
void parameterlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Parameter List> -> <Parameter> <Parameter List Prime>\n");
	parameter(p, outfile, fin, record, line_code);
	parameterlistP(p, outfile, fin, record, line_code);

}

//r5
void optParameterlist(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Opt Parameter List> -> <Parameter List> | <Empty>\n");
	if (record.token == "identifier") {
		parameterlist(p, outfile, fin, record, line_code);
	}
	else {
		if (record.lexeme.length() > 0) {}
	}
}

//r4
void function_r(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Function> -> function <Identifier> [ <Opt Parameter List> ] <Opt Declaration List> <Body>\n");

	if (record.lexeme == "function") {
		get_token(p, outfile, fin, record, line_code);
		if (record.token == "identifier") {
			get_token(p, outfile, fin, record, line_code);
			if (record.lexeme == "[") {
				get_token(p, outfile, fin, record, line_code);
				optParameterlist(p, outfile, fin, record, line_code);
				if (record.lexeme == "]") {
					get_token(p, outfile, fin, record, line_code);
					optDeclarationlist(p, outfile, fin, record, line_code);
					body(p, outfile, fin, record, line_code);
				}
				else {
					error(p, outfile, fin, record, line_code);
				}
			}
			else {
				error(p, outfile, fin, record, line_code);
			}
		}
		else {
			error(p, outfile, fin, record, line_code);
		}
	}

	else {
		error(p, outfile, fin, record, line_code);
	}

}

//r3p
void functiondefinitionsP(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Function Definitions Prime> -> <Empty> | <Function Definitions>\n");

	if (record.lexeme == "function") {
		functiondefinitions(p, outfile, fin, record, line_code);
	}
	else {
		//empty
		SA_output(outfile, "<Function Definitions Prime> -> <Empty>\n");
	}
}

//r3
void functiondefinitions(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Function Definitions> -> <Function> <Function Definitions Prime>\n");
	function_r(p, outfile, fin, record, line_code);
	functiondefinitionsP(p, outfile, fin, record, line_code);
}

//r2
void optFunctiondefinitions(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<Opt Function Definitions> -> <Opt Function Definitions> | <Empty> \n");


	if (record.lexeme == "function") {
		functiondefinitions(p, outfile, fin, record, line_code);
	}
	else {
		//empty
		SA_output(outfile, "<Opt Function Definitions> -> <Empty>\n");
	}
}

//r1
void rat18s(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {
	SA_output(outfile, "<rat18s> -><Opt Function Definitions > %% <Opt Declaration List> <Statement List>\n");

	//optFunctiondefinitions(p, outfile, fin, record, line_code);
	if (record.lexeme == "%%") {
		get_token(p, outfile, fin, record, line_code);
		optDeclarationlist(p, outfile, fin, record, line_code);
		statementlist(p, outfile, fin, record, line_code);
	}
	else {
		error(p, outfile, fin, record, line_code);
	}

}

//The start of the syntax analyzation
void SA(char *&p, ofstream & outfile, fstream & fin, LA_output & record, string & line_code) {

	get_token(p, outfile, fin, record, line_code);
	rat18s(p, outfile, fin, record, line_code);
}
//end part 3
/////////////////////////////////////////////////////////////////

int main() {

	//read file
	fstream fin;

	//candidate lexeme
	char * lex_candid;

	//holds each input file line
	string line_code;
	string file_name;

	//structure containing lexer function results
	LA_output record;

	//prompt user to enter file name
	cout << "What is the name of the txt file you want to test:  " << endl;
	cin >> file_name;

	fin.open(file_name);

	//while unable to open file prompt user to re-enter file name
	while (!(fin.is_open())) {
		cout << "Unable to open file; please re-enter file name" << endl;
		cin >> file_name;
		fin.open(file_name);
	}

	//creating and formatting output file
	ofstream outfile("output.txt");
	outfile << file_name << endl;
	outfile << setw(10) << "token" << "\t\t\t" << "lexeme" << endl;
	outfile << "--------------------------------------------------------------\n";

	new_line(lex_candid, outfile, fin, record, line_code);
	SA(lex_candid, outfile, fin, record, line_code);

	gen_instr("LABEL", 0);

	outfile << endl;
	outfile << "Symbol Table: " << endl;
	printSymbolTable(outfile);
	outfile << endl;
	outfile << "Instruction Table: " << endl;
	print_instr_t(outfile);

	fin.close();
	outfile.close();
	return 0;
}

