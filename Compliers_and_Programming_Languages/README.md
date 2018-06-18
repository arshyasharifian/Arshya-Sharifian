I wrote a compiler for a language called rat18s.  

There are three major components to this project:
1. Lexical Analyzer: To create the lexical analyzer I constructed a RE (regular expression) for each
token type.  Next, I converted these RE's to a NSFM using Thompson's (a method of connecting pieces of 
accepting states into one larger machine). Finally, we simplified the NFSM using subsets to construct a FSM.  
This parses the source code and returns tokens and the lexeme in a structure.
2. Syntax Analyzer: I used a recursive descent parser to check grammer. Here, we are using a recursive descent parser to parse 
the source code and determine whether the syntax is correct.  Using a top-down parse we removed our grammar's 
left recursion and back-tracking. We then constructed a function for each rule and checked the tokens to 
ensure they are consistent with the grammar.
3. Generating Assembly Instructions: Here, we are outputting the assembly instructions for a target machine.  
We utilize a structure for both the assembly instructions table as well as the symbol table.  We
use a jumpstack to track the beginning of our loops to write an assembly instruction and
determine the location of the label.  These instructions are generated during the recursive descent parser.
