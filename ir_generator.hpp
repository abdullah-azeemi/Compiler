#ifndef IR_GENERATOR_HPP
#define IR_GENERATOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include "ast.hpp"
#include "Utilities/token_types.hpp"

using namespace std;

/**
 * @brief Represents a single Three-Address Code instruction (Quadruple).
 */
struct Quad {
    string op;    
    string arg1;  
    string arg2;  
    string result; 

    Quad(string o, string a1, string a2, string r) : op(o), arg1(a1), arg2(a2), result(r) {}
    Quad(string o, string a1, string a2, string r, string target) : op(o), arg1(a1), arg2(a2), result(r) {
        if (o == "goto" || o == "if_false") result = target;
        if (o == "label") result = target;
    }

    string toString() const {
        if (op == "copy") {
            return result + " = " + arg1;
        } else if (op == "goto") {
            return "goto " + result;
        } else if (op == "if_false") {
            return "if_false " + arg1 + " goto " + result;
        } else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "==" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "!=") {
            return result + " = " + arg1 + " " + op + " " + arg2;
        } else if (op == "neg" || op == "not") {
             return result + " = " + op + " " + arg1;
        } else if (op == "return") {
            return "return " + arg1;
        }
        return "/* Unhandled Quad: " + op + " " + arg1 + " " + arg2 + " " + result + " */";
    }
};


class IRGenerator {
private:
    vector<Quad> quads;
    int tempCounter = 0;
    int labelCounter = 0;

    stack<string> breakTargets;  
    stack<string> continueTargets; 

    // Helper functions
    string newTemp();
    string newLabel();
    void emit(const Quad& quad);
    void emitLabel(const string& label);
    string tokenTypeToOp(TokenType type);

    // Expression generation
    string generateExpression(Expr* expr);
    string generateBinaryOp(BinaryOp* op);
    string generateUnaryOp(UnaryOp* op);
    string generateAssignment(Assignment* assign);

    // Statement generation
    void generateStatement(Stmt* stmt);
    void generateVarDecl(VarDecl* decl);
    void generateBlock(Block* block);
    void generateIfStmt(IfStmt* ifStmt);
    void generateWhileStmt(WhileStmt* whileStmt);
    void generateBreakStmt(BreakStmt* breakStmt);
    void generateContinueStmt(ContinueStmt* continueStmt);
    void generateReturnStmt(ReturnStmt* returnStmt);


public:
    IRGenerator() = default;
    vector<Quad> generate(const vector<Stmt*>& program);
    void printIRCode(); 
};

#endif