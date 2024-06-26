// Copyright: Logan Liu, May 16 2024

#ifndef BADGE_H
#define BADGE_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <cctype>

enum class TokenType {
    Keyword, Identifier, Number, Operator, Punctuation, EndOfLine
};

struct Token {
    TokenType type;
    std::string value;
};

struct Node {
    std::string type;
    std::vector<std::shared_ptr<Node>> children;
    std::pair<std::string, std::string> leaf;

    Node(std::string t);
};

struct MIPSNode {
    std::string instruction;
    std::string details;

    MIPSNode(std::string instr, std::string det);
};

std::vector<Token> tokenize(const std::string& code);

class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<Node> parse();
private:
    std::shared_ptr<Node> parseDeclaration();
    std::shared_ptr<Node> parseForLoop();
    std::shared_ptr<Node> parseStatement();
    std::shared_ptr<Node> parseInitialization();
    std::shared_ptr<Node> parseCondition();
    std::shared_ptr<Node> parseIfStatement();
    std::shared_ptr<Node> parseIncrement();
    std::shared_ptr<Node> parseExpression();
    Token consume(std::initializer_list<TokenType> acceptableTypes);
    Token consume(TokenType type, const std::string& value = "");
};

class Transformer {
    std::shared_ptr<Node> ast;
    std::map<std::string, std::string> variableToRegisterMap;
    std::set<std::string> initializedRegisters;

public:
    Transformer(std::shared_ptr<Node> ast);
    std::shared_ptr<MIPSNode> transform();
    void transformIncrement(std::shared_ptr<Node> incr, std::string reg, std::shared_ptr<MIPSNode> mipsNode);
private:
    void transformDeclaration(std::shared_ptr<Node> decl, std::shared_ptr<MIPSNode> mipsNode);
    void transformForLoop(std::shared_ptr<Node> forLoop, std::shared_ptr<MIPSNode> mipsNode);
    void transformIfStatement(std::shared_ptr<Node> ifNode, std::shared_ptr<MIPSNode> mipsNode);
    void transformExpression(std::shared_ptr<Node> expr, std::shared_ptr<MIPSNode> mipsNode);
};

class CodeGenerator {
    std::shared_ptr<MIPSNode> mipsNode;

public:
    CodeGenerator(std::shared_ptr<MIPSNode> mipsNode);
    std::string generateCode();
};

#endif
