#include "ir_generator.hpp" 
#include <stdexcept>
#include <algorithm>


string IRGenerator::newTemp() {
    return "_t" + to_string(tempCounter++);
}

string IRGenerator::newLabel() {
    return "_L" + to_string(labelCounter++);
}

void IRGenerator::emit(const Quad& quad) {
    quads.push_back(quad);
}

void IRGenerator::emitLabel(const string& label) {
    emit(Quad("label", "", "", "", label));
}

string IRGenerator::tokenTypeToOp(TokenType type) {
    switch (type) {
        case T_PLUS_RL: return "+";
        case T_MINUS_RL: return "-";
        case T_MUL_RL: return "*";
        case T_DIV_RL: return "/";
        case T_MOD_RL: return "%";
        case T_LESS_THAN_RL: return "<";
        case T_GREATER_THAN_RL: return ">";
        case T_LESS_EQUAL_RL: return "<=";
        case T_GREATER_EQUAL_RL: return ">=";
        case T_EQUALSOP_RL: return "==";
        case T_NOT_EQUALS_RL: return "!=";
        case T_AND_LOGICAL_RL: return "&&"; 
        case T_OR_LOGICAL_RL: return "||";  
        default: return "op_unhandled";
    }
}

string IRGenerator::generateExpression(Expr* expr) {
    if (!expr) return "";

    switch (expr->nodeType) {
        case NODE_INT_LIT:
            return to_string(static_cast<IntLiteral*>(expr)->value);
        case NODE_FLOAT_LIT:
            return to_string(static_cast<FloatLiteral*>(expr)->value);
        case NODE_STRING_LIT:
            return static_cast<StringLiteral*>(expr)->value;
        case NODE_BOOL_LIT:
            return static_cast<BoolLiteral*>(expr)->value ? "true" : "false";
        case NODE_IDENTIFIER:
            return static_cast<Identifier*>(expr)->name;
        case NODE_BINARY_OP:
            return generateBinaryOp(static_cast<BinaryOp*>(expr));
        case NODE_UNARY_OP:
            return generateUnaryOp(static_cast<UnaryOp*>(expr));
        case NODE_ASSIGNMENT:
            return generateAssignment(static_cast<Assignment*>(expr));
        case NODE_FUNC_CALL: {
            for (auto arg : static_cast<FunctionCall*>(expr)->args) {
                generateExpression(arg); 
            }
            string funcTemp = newTemp();
            emit(Quad("call", static_cast<FunctionCall*>(expr)->name, "", funcTemp));
            return funcTemp;
        } 
        default:
            throw runtime_error("Unhandled expression type in IR generation.");
    }
}

string IRGenerator::generateBinaryOp(BinaryOp* op) {
    string left = generateExpression(op->left);
    string right = generateExpression(op->right);
    string result = newTemp();
    string opStr = tokenTypeToOp(op->op);

    emit(Quad(opStr, left, right, result));
    return result;
}

string IRGenerator::generateUnaryOp(UnaryOp* op) {
    string operand = generateExpression(op->operand);
    string result = newTemp();

    if (op->op == T_MINUS_RL) {
        emit(Quad("neg", operand, "", result));
    } else if (op->op == T_NOT_RL) {
        emit(Quad("not", operand, "", result));
    } else {
        throw runtime_error("Unhandled unary operator in IR generation.");
    }
    return result;
}

string IRGenerator::generateAssignment(Assignment* assign) {
    string value = generateExpression(assign->value);
    
    emit(Quad("copy", value, "", assign->ident)); 
    return assign->ident; 
}


void IRGenerator::generateStatement(Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->nodeType) {
        case NODE_VAR_DECL:
            generateVarDecl(static_cast<VarDecl*>(stmt));
            break;
        case NODE_EXPR_STMT:
            generateExpression(static_cast<ExprStmt*>(stmt)->expr);
            break;
        case NODE_BLOCK:
            generateBlock(static_cast<Block*>(stmt));
            break;
        case NODE_IF:
            generateIfStmt(static_cast<IfStmt*>(stmt));
            break;
        case NODE_WHILE:
            generateWhileStmt(static_cast<WhileStmt*>(stmt));
            break;
        case NODE_BREAK:
            generateBreakStmt(static_cast<BreakStmt*>(stmt));
            break;
        case NODE_CONTINUE:
            generateContinueStmt(static_cast<ContinueStmt*>(stmt));
            break;
        case NODE_RETURN:
            generateReturnStmt(static_cast<ReturnStmt*>(stmt));
            break;
        default:
            throw runtime_error("Unhandled statement type in IR generation.");
    }
}

void IRGenerator::generateVarDecl(VarDecl* decl) {
    if (decl->expr) {
        string value = generateExpression(decl->expr);
        emit(Quad("copy", value, "", decl->ident));
    }
}

void IRGenerator::generateBlock(Block* block) {
    for (auto stmt : block->stmts) {
        generateStatement(stmt);
    }
}

void IRGenerator::generateIfStmt(IfStmt* ifStmt) {
    string condResult = generateExpression(ifStmt->condition);
    string endLabel = newLabel();
    string elseLabel = newLabel();
    emit(Quad("if_false", condResult, "", "", ifStmt->elseBranch ? elseLabel : endLabel));
    generateStatement(ifStmt->thenBranch);
    
    if (ifStmt->elseBranch) {
        emit(Quad("goto", "", "", "", endLabel));
        emitLabel(elseLabel);
        generateStatement(ifStmt->elseBranch);
    }
    emitLabel(endLabel);
}

/**
 * @brief Generates TAC for a WhileStmt, managing loop context.
 */
void IRGenerator::generateWhileStmt(WhileStmt* whileStmt) {
    string loopStartLabel = newLabel(); 
    string loopEndLabel = newLabel();  

    breakTargets.push(loopEndLabel);
    continueTargets.push(loopStartLabel);
    emitLabel(loopStartLabel);
    string condResult = generateExpression(whileStmt->condition);
    emit(Quad("if_false", condResult, "", "", loopEndLabel));
    generateStatement(whileStmt->body); 
    emit(Quad("goto", "", "", "", loopStartLabel));
    emitLabel(loopEndLabel);
    continueTargets.pop();
    breakTargets.pop();
}

/**
 * @brief Generates TAC for a BreakStmt. Jumps to the current loop's exit label.
 */
void IRGenerator::generateBreakStmt(BreakStmt* breakStmt) {
    if (breakTargets.empty()) return; 
    emit(Quad("goto", "", "", "", breakTargets.top()));
}

/**
 * @brief Generates TAC for a ContinueStmt. Jumps to the current loop's start label.
 */
void IRGenerator::generateContinueStmt(ContinueStmt* continueStmt) {
    if (continueTargets.empty()) return;
    emit(Quad("goto", "", "", "", continueTargets.top()));
}

void IRGenerator::generateReturnStmt(ReturnStmt* returnStmt) {
    if (returnStmt->expr) {
        string result = generateExpression(returnStmt->expr);
        emit(Quad("return", result, "", ""));
    } else {
        emit(Quad("return", "", "", ""));
    }
}

vector<Quad> IRGenerator::generate(const vector<Stmt*>& program) {
    quads.clear();
    tempCounter = 0;
    labelCounter = 0;
    while (!breakTargets.empty()) breakTargets.pop();
    while (!continueTargets.empty()) continueTargets.pop();

    for (auto stmt : program) {
        try {
            generateStatement(stmt);
        } catch (const runtime_error& e) {
            cerr << "IR Generation Error: " << e.what() << endl;
            break;
        }
    }

    return quads;
}

void IRGenerator::printIRCode() { 
    cout << "# Intermediate Representation (TAC)" << endl;
    cout << "--- Generated Three-Address Code (TAC) ---" << endl;
    for (const auto& quad : quads) {
        if (quad.op == "label") {
            cout << quad.result << ":" << endl;
        } else {
            cout << quad.toString() << endl;
        }
    }
    cout << endl;
}