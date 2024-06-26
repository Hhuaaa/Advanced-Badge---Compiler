Overview
This project consists of a simple compiler that translates a subset of a high-level programming language (e.g., C-like language) into MIPS assembly language. The project is implemented in C++ and includes tokenization, parsing, AST transformation, and MIPS code generation.

Components
Tokenization (tokenize function): Converts a string of code into a vector of tokens.
Parsing (Parser class): Parses the vector of tokens into an Abstract Syntax Tree (AST).
Transformation (Transformer class): Transforms the AST into a MIPS-specific AST.
Code Generation (CodeGenerator class): Generates MIPS assembly code from the MIPS-specific AST.

Classes and Functions
Node: Represents a node in the AST.
MIPSNode: Represents a node in the MIPS-specific AST.
Parser: Parses tokens into an AST.
Transformer: Transforms the AST into a MIPS-specific AST.
CodeGenerator: Generates MIPS assembly code from the MIPS-specific AST.

Token Types
TokenType::Keyword: Represents keywords like int, for, if.
TokenType::Identifier: Represents variable names.
TokenType::Number: Represents numeric literals.
TokenType::Operator: Represents operators like +, -, ++, --, =, ==, !=, +=, -=, <=, >=.
TokenType::Punctuation: Represents punctuation like (, ), {, }, ;.

How It Works
Tokenization: The tokenize function takes a string of code and breaks it into a list of tokens.
Parsing: The Parser class processes the list of tokens and constructs an AST.
Transformation: The Transformer class takes the AST and converts it into a MIPS-specific AST.
Code Generation: The CodeGenerator class takes the MIPS-specific AST and generates MIPS assembly code.

Example
Given the following input code:
int a = 3;
int b = 4;
int i;
for (i = 0; i <= 3; i++) {
    if (a == b) {
        a += i + 2;
    }
    if (a != b) {
        a = a + 1;
    }
}

The output MIPS assembly code would be:
li $t0, 3
li $t2, 4
li $t1, 0
LOOP_START:
bgt $t1, 3, LOOP_END
beq $t0, $t2, IF_TRUE0
j IF_FALSE0
IF_TRUE0:
add $t0, $t0, $t1
addi $t0, $t0, 2
j END_IF0
IF_FALSE0:
END_IF0:
bne $t0, $t2, IF_TRUE1
j IF_FALSE1
IF_TRUE1:
add $t0, $t0, 1
j END_IF1
IF_FALSE1:
END_IF1:
addi $t1, $t1, 1
j LOOP_START
LOOP_END:

Files
badge.cpp: Contains the implementation for tokenization, parsing, and transformation.
badge.h: Contains definitions for Node, MIPSNode, Token, TokenType, Parser, Transformer, and CodeGenerator.
main.cpp: The main entry point for the compiler.
badge.asm: The generated MIPS assembly code formatted in MARS. 

How to Run
Compile the code:
g++ main.cpp badge.cpp -o badge
Run the compiler:
./badge
The generated MIPS assembly code will be printed to the console.
Run the MIPS code:
Open the badge.asm in MARS and click Run-Assemble the output will be printed to the console.
