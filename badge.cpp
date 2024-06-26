// Copyright: Logan Liu, May 16 2024

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

#include "badge.h"

Node::Node(std::string t) : type(t) {}

MIPSNode::MIPSNode(std::string instr, std::string det) : instruction(instr), details(det) {}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    size_t i = 0, len = code.length();
    while (i < len) {
        if (isspace(code[i])) {
            ++i;
            continue;
        }
        if (isdigit(code[i])) {
            std::string number;
            while (i < len && isdigit(code[i])) number += code[i++];
            tokens.push_back({TokenType::Number, number});
            continue;
        }
        if (isalpha(code[i])) {
            std::string identifier;
            while (i < len && isalnum(code[i])) identifier += code[i++];
            if (identifier == "for" || identifier == "int" || identifier == "if") {
                tokens.push_back({TokenType::Keyword, identifier});
            } else {
                tokens.push_back({TokenType::Identifier, identifier});
            }
            continue;
        }
        static const std::string operators = "+-*/<=>!";
        if (operators.find(code[i]) != std::string::npos) {
            std::string op(1, code[i]);
            if (i + 1 < len && (operators.find(code[i + 1]) != std::string::npos || code[i + 1] == '=')) {
                op += code[i + 1];
                i++;
            }
            tokens.push_back({TokenType::Operator, op});
            i++;
            continue;
        }
        static const std::string punctuation = "();{}";
        if (punctuation.find(code[i]) != std::string::npos) {
            tokens.push_back({TokenType::Punctuation, std::string(1, code[i])});
            i++;
            continue;
        }
        // Skip unexpected characters
        std::cerr << "Warning: Unexpected character in input: " << code[i] << std::endl;
        ++i;
    }
    return tokens;
}

std::shared_ptr<Node> Parser::parse() {
    auto rootNode = std::make_shared<Node>("Root");
    while (current < tokens.size()) {
        if (tokens[current].value == "int") {
            rootNode->children.push_back(parseDeclaration());
        } else if (tokens[current].value == "for") {
            rootNode->children.push_back(parseForLoop());
        } else {
            ++current; // Skip unknown tokens or add error handling
        }
    }
    return rootNode;
}

std::shared_ptr<Node> Parser::parseDeclaration() {
    auto decl = std::make_shared<Node>("Declaration");
    consume(TokenType::Keyword, "int"); // 'int'
    auto varName = consume(TokenType::Identifier).value; // variable name
    std::string value = "0"; // Default initialization value if none provided

    if (current < tokens.size() && tokens[current].type == TokenType::Operator && tokens[current].value == "=") {
        consume(TokenType::Operator, "="); // '='
        value = consume({TokenType::Identifier, TokenType::Number}).value; // initial value
    }
    consume(TokenType::Punctuation, ";"); // ';'
    decl->leaf = {varName, value};
    return decl;
}

std::shared_ptr<Node> Parser::parseForLoop() {
    auto forLoop = std::make_shared<Node>("ForLoop");
    consume(TokenType::Keyword, "for");
    consume(TokenType::Punctuation, "(");
    
    forLoop->children.push_back(parseInitialization());
    consume(TokenType::Punctuation, ";");
    
    forLoop->children.push_back(parseCondition());
    consume(TokenType::Punctuation, ";");
    
    forLoop->children.push_back(parseIncrement());
    consume(TokenType::Punctuation, ")");
    
    consume(TokenType::Punctuation, "{");
    while (current < tokens.size() && tokens[current].type != TokenType::Punctuation && tokens[current].value != "}") {
        if (tokens[current].value == "if") {
            forLoop->children.push_back(parseIfStatement());
        } else {
            forLoop->children.push_back(parseExpression());
        }
    }
    consume(TokenType::Punctuation, "}");
    return forLoop;
}

std::shared_ptr<Node> Parser::parseIfStatement() {
    auto ifNode = std::make_shared<Node>("IfStatement");
    consume(TokenType::Keyword, "if");
    consume(TokenType::Punctuation, "(");
    ifNode->children.push_back(parseCondition());
    consume(TokenType::Punctuation, ")");
    consume(TokenType::Punctuation, "{");
    while (current < tokens.size() && tokens[current].type != TokenType::Punctuation && tokens[current].value != "}") {
        ifNode->children.push_back(parseExpression());
    }
    consume(TokenType::Punctuation, "}");
    return ifNode;
}

std::shared_ptr<Node> Parser::parseInitialization() {
    auto init = std::make_shared<Node>("Initialization");
    auto varName = consume(TokenType::Identifier).value;
    consume(TokenType::Operator, "=");
    Token valueToken = consume({TokenType::Identifier, TokenType::Number});
    std::string value = valueToken.value;
    init->leaf = {varName, value};
    return init;
}

std::shared_ptr<Node> Parser::parseCondition() {
    auto condition = std::make_shared<Node>("Condition");
    auto varName = consume(TokenType::Identifier).value;
    auto op = consume(TokenType::Operator).value;
    Token valueToken = consume({TokenType::Identifier, TokenType::Number});
    std::string value = valueToken.value;
    condition->leaf = {varName + " " + op, value};
    return condition;
}

std::shared_ptr<Node> Parser::parseIncrement() {
    auto increment = std::make_shared<Node>("Increment");
    auto varName = consume(TokenType::Identifier).value;
    auto op = consume(TokenType::Operator).value;
    increment->leaf = {varName, op};
    return increment;
}

std::shared_ptr<Node> Parser::parseExpression() {
    auto expr = std::make_shared<Node>("Expression");
    std::string fullExpr;
    while (current < tokens.size() && tokens[current].type != TokenType::Punctuation) {
        fullExpr += tokens[current].value + " ";
        current++;
    }
    consume(TokenType::Punctuation, ";"); // Consume the semicolon ending the expression

    expr->leaf = {fullExpr, ""}; // Store the entire expression
    return expr;
}

Token Parser::consume(std::initializer_list<TokenType> acceptableTypes) {
    if (current < tokens.size()) {
        for (auto type : acceptableTypes) {
            if (tokens[current].type == type) {
                return tokens[current++];
            }
        }
    }
    std::ostringstream msg;
    msg << "Unexpected token at position " << current << ": Expected types ";
    for (auto type : acceptableTypes) msg << static_cast<int>(type) << " ";
    msg << ", but got type " << static_cast<int>(tokens[current].type);
    msg << " with value '" << tokens[current].value << "'";
    throw std::runtime_error(msg.str());
}

Token Parser::consume(TokenType type, const std::string& value) {
    if (current < tokens.size() && tokens[current].type == type &&
        (value.empty() || tokens[current].value == value)) {
        return tokens[current++];
    }
    std::ostringstream msg;
    msg << "Unexpected token at position " << current << ": Expected type "
        << static_cast<int>(type);
    if (!value.empty()) {
        msg << " with value '" << value << "'";
    }
    msg << ", but got type " << static_cast<int>(tokens[current].type);
    if (!tokens[current].value.empty()) {
        msg << " with value '" << tokens[current].value << "'";
    }
    throw std::runtime_error(msg.str());
}

Transformer::Transformer(std::shared_ptr<Node> ast) : ast(ast) {
    variableToRegisterMap["a"] = "$t0";
    variableToRegisterMap["i"] = "$t1";
    variableToRegisterMap["b"] = "$t2";
}

std::shared_ptr<MIPSNode> Transformer::transform() {
    auto mipsNode = std::make_shared<MIPSNode>("", "");
    for (auto& child : ast->children) {
        if (child->type == "Declaration") {
            transformDeclaration(child, mipsNode);
        } else if (child->type == "ForLoop") {
            transformForLoop(child, mipsNode);
        }
    }
    return mipsNode;
}

void Transformer::transformDeclaration(std::shared_ptr<Node> decl, std::shared_ptr<MIPSNode> mipsNode) {
    auto varName = decl->leaf.first;
    auto value = decl->leaf.second;
    mipsNode->details += "li " + variableToRegisterMap[varName] + ", " + value + "\n";
}

void Transformer::transformForLoop(std::shared_ptr<Node> forLoop, std::shared_ptr<MIPSNode> mipsNode) {
    // Initialization
    auto init = forLoop->children[0];
    std::string initReg = variableToRegisterMap[init->leaf.first];
    std::string initializationLine = "li " + initReg + ", " + init->leaf.second;

    // Only add initialization if it hasn't been added before
    if (mipsNode->details.find(initializationLine) == std::string::npos) {
        mipsNode->details += initializationLine + "\n";
    }

    std::string loopStartLabel = "LOOP_START";
    std::string endLoopLabel = "LOOP_END";
    mipsNode->details += loopStartLabel + ":\n";

    // Condition
    auto cond = forLoop->children[1];
    std::string condReg = variableToRegisterMap[cond->leaf.first.substr(0, cond->leaf.first.find(' '))];
    std::string condValue = cond->leaf.second;

    mipsNode->details += "bgt " + condReg + ", " + condValue + ", " + endLoopLabel + "\n";

    // Loop Body - handle any number of expressions or if statements
    for (size_t i = 2; i < forLoop->children.size(); ++i) {
        if (forLoop->children[i]->type == "IfStatement") {
            transformIfStatement(forLoop->children[i], mipsNode);
        } else if (forLoop->children[i]->type == "Expression") {
            transformExpression(forLoop->children[i], mipsNode);
        }
    }

    // Increment - this should happen regardless of the 'if' condition
    auto incr = forLoop->children[2];
    transformIncrement(incr, initReg, mipsNode);

    // Loop end and jump back to the start
    mipsNode->details += "j " + loopStartLabel + "\n";
    mipsNode->details += endLoopLabel + ":\n";
}

void Transformer::transformIfStatement(std::shared_ptr<Node> ifNode, std::shared_ptr<MIPSNode> mipsNode) {
    auto condition = ifNode->children[0];  // First child is the condition
    auto varName = condition->leaf.first.substr(0, condition->leaf.first.find(' '));
    auto conditionOperator = condition->leaf.first.substr(condition->leaf.first.find(' ') + 1);
    auto value = condition->leaf.second;

    std::string trueLabel = "IF_TRUE";
    std::string falseLabel = "IF_FALSE";
    std::string endIfLabel = "END_IF";

    // Ensure unique labels for nested if statements
    static int ifCount = 0;
    trueLabel += std::to_string(ifCount);
    falseLabel += std::to_string(ifCount);
    endIfLabel += std::to_string(ifCount);
    ++ifCount;

    std::string reg = variableToRegisterMap[varName];
    std::string valueReg = variableToRegisterMap["b"];  // Assuming the comparison is always with 'b'

    // Determine the appropriate comparison instruction
    std::string compareInstruction;
    if (conditionOperator == "==") {
        compareInstruction = "beq";
    } else if (conditionOperator == "!=") {
        compareInstruction = "bne";
    } else {
        throw std::runtime_error("Unsupported condition operator: " + conditionOperator);
    }

    // Generate the MIPS code for the comparison
    mipsNode->details += compareInstruction + " " + reg + ", " + valueReg + ", " + trueLabel + "\n";
    mipsNode->details += "j " + falseLabel + "\n";
    mipsNode->details += trueLabel + ":\n";

    // Execute expression inside if statement
    transformExpression(ifNode->children[1], mipsNode);  // Transform the expression inside the if block

    mipsNode->details += "j " + endIfLabel + "\n";  // Jump to end of if after true block
    mipsNode->details += falseLabel + ":\n";
    mipsNode->details += endIfLabel + ":\n";
}

void Transformer::transformIncrement(std::shared_ptr<Node> incr, std::string reg, std::shared_ptr<MIPSNode> mipsNode) {
    // Assuming the increment logic is simple: handle +=, -=, ++, and --
    std::string op = incr->leaf.second;  // This should be the operation like '++', '--', '+=2', etc.
    if (op == "++") {
        mipsNode->details += "addi " + reg + ", " + reg + ", 1\n";
    } else if (op == "--") {
        mipsNode->details += "addi " + reg + ", " + reg + ", -1\n";
    } else if (op.find("+=") != std::string::npos) {
        // Extract the number from '+=' operation
        std::string number = op.substr(2);
        mipsNode->details += "addi " + reg + ", " + reg + ", " + number + "\n";
    } else if (op.find("-=") != std::string::npos) {
        std::string number = op.substr(2);
        mipsNode->details += "addi " + reg + ", " + reg + ", -" + number + "\n";
    }
}

void Transformer::transformExpression(std::shared_ptr<Node> expr, std::shared_ptr<MIPSNode> mipsNode) {
    // Expression like "a += i - 2" or "a = a + 1"
    std::string expression = expr->leaf.first;

    // Remove spaces for easier parsing
    expression.erase(std::remove_if(expression.begin(), expression.end(), ::isspace), expression.end());

    // Detect the operation and split the expression accordingly
    size_t opPos = expression.find_first_of("=+-*/");
    if (opPos == std::string::npos) {
        throw std::runtime_error("Unsupported operation in expression: " + expression);
    }

    std::string destVariable = expression.substr(0, opPos);
    std::string operation = expression.substr(opPos, 1);
    if (expression[opPos+1] == '=') {
        operation += "=";
        ++opPos;
    }

    std::string remainingExpression = expression.substr(opPos + 1);

    // Identify if there's a secondary operation within the remaining expression
    size_t secOpPos = remainingExpression.find_first_of("+-*/");
    std::string srcVariable;
    std::string secOperation;
    int immediateValue = 0;

    if (secOpPos != std::string::npos) {
        srcVariable = remainingExpression.substr(0, secOpPos);
        secOperation = remainingExpression[secOpPos];
        immediateValue = std::stoi(remainingExpression.substr(secOpPos + 1));
    } else {
        srcVariable = remainingExpression;
    }

    std::string destReg = variableToRegisterMap[destVariable];
    std::string srcReg = variableToRegisterMap[srcVariable];

    // Generate the MIPS code for the operation
    if (operation == "+=") {
        if (secOperation == "+") {
            mipsNode->details += "add " + destReg + ", " + destReg + ", " + srcReg + "\n";
            mipsNode->details += "addi " + destReg + ", " + destReg + ", " + std::to_string(immediateValue) + "\n";
        } else if (secOperation == "-") {
            mipsNode->details += "add " + destReg + ", " + destReg + ", " + srcReg + "\n";
            mipsNode->details += "addi " + destReg + ", " + destReg + ", -" + std::to_string(immediateValue) + "\n";
        }
    } else if (operation == "-=") {
        if (secOperation == "+") {
            mipsNode->details += "sub " + destReg + ", " + destReg + ", " + srcReg + "\n";
            mipsNode->details += "addi " + destReg + ", " + destReg + ", " + std::to_string(immediateValue) + "\n";
        } else if (secOperation == "-") {
            mipsNode->details += "sub " + destReg + ", " + destReg + ", " + srcReg + "\n";
            mipsNode->details += "addi " + destReg + ", " + destReg + ", -" + std::to_string(immediateValue) + "\n";
        }
    } else if (operation == "=") {
        // Handle assignment with arithmetic
        if (!secOperation.empty()) {
            if (secOperation == "+") {
                mipsNode->details += "add " + destReg + ", " + srcReg + ", " + std::to_string(immediateValue) + "\n";
            } else if (secOperation == "-") {
                mipsNode->details += "sub " + destReg + ", " + srcReg + ", " + std::to_string(immediateValue) + "\n";
            }
        } else if (isdigit(remainingExpression[0])) {
            mipsNode->details += "li " + destReg + ", " + remainingExpression + "\n";
        } else {
            mipsNode->details += "move " + destReg + ", " + srcReg + "\n";
        }
    } else {
        throw std::runtime_error("Unsupported operation in expression: " + expression);
    }
}

CodeGenerator::CodeGenerator(std::shared_ptr<MIPSNode> mipsNode) : mipsNode(mipsNode) {}

std::string CodeGenerator::generateCode() {
    return mipsNode->details;
}

// Help Function
std::vector<std::string> splitExpression(const std::string& expression) {
    std::istringstream iss(expression);
    std::vector<std::string> parts(
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    );
    return parts;
}
