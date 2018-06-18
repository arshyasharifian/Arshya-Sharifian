I wrote a compiler for a language called <rat18s>.  

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


Here are the rules:
R1. <Rat18S>  ::=   <Opt Function Definitions>   %%  <Opt Declaration List>  <Statement List>  
R2. <Opt Function Definitions> ::= <Function Definitions>     |  <Empty>
R3. <Function Definitions>  ::= <Function> | <Function> <Function Definitions>   
R4. <Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>
R5. <Opt Parameter List> ::=  <Parameter List>    |     <Empty>
R6. <Parameter List>  ::=  <Parameter>    |     <Parameter> , <Parameter List>
R7. <Parameter> ::=  <IDs > : <Qualifier> 
R8. <Qualifier> ::= int     |    boolean    |  real 
R9. <Body>  ::=  { < Statement List> }
R10. <Opt Declaration List> ::= <Declaration List>   |    <Empty>
R11. <Declaration List>  := <Declaration> ;     |      <Declaration> ; <Declaration List>
R12. <Declaration> ::=   <Qualifier > <IDs>                   
R13. <IDs> ::=     <Identifier>    | <Identifier>, <IDs>
R14. <Statement List> ::=   <Statement>   | <Statement> <Statement List>
R15. <Statement> ::=   <Compound>  |  <Assign>  |   <If>  |  <Return>   | <Print>   |   <Scan>   |  <While> 
R16. <Compound> ::=   {  <Statement List>  } 
R17. <Assign> ::=     <Identifier> = <Expression> ;
R18. <If> ::=     if  ( <Condition>  ) <Statement>   endif    |   
                          if  ( <Condition>  ) <Statement>   else  <Statement>  endif   
R19. <Return> ::=  return ; |  return <Expression> ;
R20. <Print> ::=    put ( <Expression>);
R21. <Scan> ::=    get ( <IDs> );
R22. <While> ::=  while ( <Condition>  )  <Statement> 
R23. <Condition> ::=     <Expression>  <Relop>   <Expression>
R24. <Relop> ::=        ==   |   ^=    |   >     |   <    |   =>    |   =<          
R25. <Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>
R26. <Term>    ::=      <Term>  *  <Factor>     |   <Term>  /  <Factor>     |     <Factor>
R27. <Factor> 	::=      -  <Primary>    |    <Primary>
R28. <Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  
                                     <Real>  |   true   |  false                        
R29. <Empty>   ::= 


