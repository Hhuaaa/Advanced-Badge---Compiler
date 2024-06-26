// Copyright: Logan Liu, May 16 2024

#include "badge.h"

int main() {
    std::string code = "int a = 3; int b = 4; int i; for (i = 0; i <= 3; i++) { if (a == b) { a += i + 2; } if (a != b ) { a = a + 1; } }";
    auto tokens = tokenize(code);
    Parser parser(tokens);
    auto ast = parser.parse();
    Transformer transformer(ast);
    auto mipsAst = transformer.transform();
    CodeGenerator codeGen(mipsAst);
    std::string mipsCode = codeGen.generateCode();
    std::cout << mipsCode;
    return 0;
}
