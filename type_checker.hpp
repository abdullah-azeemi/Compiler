#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "Utilities/token_types.hpp"
#include "ast.hpp"
#include "scope_analyzer.hpp"

using namespace std;

enum class TypeCheckError {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    EmptyExpression,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonNumeric,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    AttemptedExponentiationOfNonNumeric,
    ReturnStmtNotFound
};

struct TypeInfo {
    TokenType type;
    bool isValid;
    
    TypeInfo() : type(T_UNKNOWN_RL), isValid(false) {}
    TypeInfo(TokenType t) : type(t), isValid(true) {}
    
    bool isNumeric() const {
        return type == T_INT_RL || type == T_FLOAT_RL || type == T_GINTI_RL;
    }
    
    bool isInteger() const {
        return type == T_INT_RL || type == T_GINTI_RL;
    }
    
    bool isBoolean() const {
        return type == T_BOOL_RL;
    }
    
    bool matches(const TypeInfo& other) const {
        if (!isValid || !other.isValid) return false;
        return type == other.type;
    }
};

struct FunctionSignature {
    TokenType returnType;
    vector<TokenType> paramTypes;
    string name;
    FunctionSignature() : returnType(T_UNKNOWN_RL), name("") {}
    
    FunctionSignature(string n, TokenType rt, vector<TokenType> params)
        : name(n), returnType(rt), paramTypes(params) {}
};

class TypeChecker {
private:
    shared_ptr<Scope> currentScope;
    unordered_map<string, FunctionSignature> functionTable;
    TokenType currentFunctionReturnType;
    int loopDepth; 
    bool hasReturnStmt;  
    
    void reportError(TypeCheckError err, const string& context = "") {
        cerr << "Type Check Error: ";
        switch (err) {
            case TypeCheckError::ErroneousVarDecl:
                cerr << "Erroneous variable declaration";
                break;
            case TypeCheckError::FnCallParamCount:
                cerr << "Function call parameter count mismatch";
                break;
            case TypeCheckError::FnCallParamType:
                cerr << "Function call parameter type mismatch";
                break;
            case TypeCheckError::ErroneousReturnType:
                cerr << "Return type does not match function signature";
                break;
            case TypeCheckError::ExpressionTypeMismatch:
                cerr << "Expression type mismatch";
                break;
            case TypeCheckError::ExpectedBooleanExpression:
                cerr << "Expected boolean expression";
                break;
            case TypeCheckError::ErroneousBreak:
                cerr << "Break/continue statement outside of loop";
                break;
            case TypeCheckError::NonBooleanCondStmt:
                cerr << "Non-boolean condition in control statement";
                break;
            case TypeCheckError::EmptyExpression:
                cerr << "Empty expression encountered";
                break;
            case TypeCheckError::AttemptedBoolOpOnNonBools:
                cerr << "Boolean operation on non-boolean operands";
                break;
            case TypeCheckError::AttemptedBitOpOnNonNumeric:
                cerr << "Bitwise operation on non-numeric operands";
                break;
            case TypeCheckError::AttemptedShiftOnNonInt:
                cerr << "Shift operation on non-integer operands";
                break;
            case TypeCheckError::AttemptedAddOpOnNonNumeric:
                cerr << "Arithmetic operation on non-numeric operands";
                break;
            case TypeCheckError::AttemptedExponentiationOfNonNumeric:
                cerr << "Exponentiation of non-numeric value";
                break;
            case TypeCheckError::ReturnStmtNotFound:
                cerr << "Function missing return statement";
                break;
        }
        if (!context.empty()) {
            cerr << " -> " << context;
        }
        cerr << endl;
    }
    
    void pushScope() {
        currentScope = make_shared<Scope>(currentScope);
    }
    void popScope() {
        if (currentScope && currentScope->parent) {
            currentScope = currentScope->parent;
        }
    }
    TypeInfo checkExpression(Expr* expr) {
        if (!expr) {
            reportError(TypeCheckError::EmptyExpression);
            return TypeInfo();
        }
        switch (expr->nodeType) {
            case NODE_INT_LIT:
                return TypeInfo(T_INT_RL);
            case NODE_FLOAT_LIT:
                return TypeInfo(T_FLOAT_RL);
            case NODE_STRING_LIT:
                return TypeInfo(T_STRING_RL); 
            case NODE_BOOL_LIT:
                return TypeInfo(T_BOOL_RL);
            case NODE_IDENTIFIER: {
                auto* id = static_cast<Identifier*>(expr);
                Symbol* sym = currentScope->lookup(id->name);
                if (!sym) {
                    return TypeInfo();
                }
                return TypeInfo(sym->type);
            }
            case NODE_BINARY_OP: {
                auto* binOp = static_cast<BinaryOp*>(expr);
                return checkBinaryOp(binOp);
            }
            case NODE_UNARY_OP: {
                auto* unOp = static_cast<UnaryOp*>(expr);
                return checkUnaryOp(unOp);
            }
            case NODE_ASSIGNMENT: {
                auto* assign = static_cast<Assignment*>(expr);
                return checkAssignment(assign);
            }
            case NODE_FUNC_CALL: {
                auto* funcCall = static_cast<FunctionCall*>(expr);
                return checkFunctionCall(funcCall);
            }
            default:
                return TypeInfo();
        }
    }
    TypeInfo checkBinaryOp(BinaryOp* op) {
        TypeInfo leftType = checkExpression(op->left);
        TypeInfo rightType = checkExpression(op->right);
        
        if (!leftType.isValid || !rightType.isValid) {
            return TypeInfo();
        }
        // Arithmetic operators: +, -, *, /, %
        if (op->op == T_PLUS_RL || op->op == T_MINUS_RL || 
            op->op == T_MUL_RL || op->op == T_DIV_RL || op->op == T_MOD_RL) {
            
            if (!leftType.isNumeric() || !rightType.isNumeric()) {
                reportError(TypeCheckError::AttemptedAddOpOnNonNumeric);
                return TypeInfo();
            }
            if (leftType.type == T_FLOAT_RL || rightType.type == T_FLOAT_RL) {
                return TypeInfo(T_FLOAT_RL);
            }
            return TypeInfo(T_INT_RL);
        }
        // Logical operators: &&, ||
        if (op->op == T_AND_LOGICAL_RL || op->op == T_OR_LOGICAL_RL) {
            if (!leftType.isBoolean() || !rightType.isBoolean()) {
                reportError(TypeCheckError::AttemptedBoolOpOnNonBools);
                return TypeInfo();
            }
            return TypeInfo(T_BOOL_RL);
        }
        // Bitwise operators: &, |, ^
        if (op->op == T_AND_BIT_RL || op->op == T_OR_BIT_RL || op->op == T_XOR_BIT_RL) {
            if (!leftType.isInteger() || !rightType.isInteger()) {
                reportError(TypeCheckError::AttemptedBitOpOnNonNumeric);
                return TypeInfo();
            }
            return TypeInfo(T_INT_RL);
        }
        // Comparison operators: <, >, <=, >=, ==, !=
        if (op->op == T_LESS_THAN_RL || op->op == T_GREATER_THAN_RL ||
            op->op == T_LESS_EQUAL_RL || op->op == T_GREATER_EQUAL_RL ||
            op->op == T_EQUALSOP_RL || op->op == T_NOT_EQUALS_RL) {
            if (!leftType.matches(rightType)) {
                reportError(TypeCheckError::ExpressionTypeMismatch);
                return TypeInfo();
            }
            return TypeInfo(T_BOOL_RL);
        }
        return TypeInfo();
    }
    TypeInfo checkUnaryOp(UnaryOp* op) {
        TypeInfo operandType = checkExpression(op->operand);
        if (!operandType.isValid) {
            return TypeInfo();
        }
        // Negation: -
        if (op->op == T_MINUS_RL) {
            if (!operandType.isNumeric()) {
                reportError(TypeCheckError::AttemptedAddOpOnNonNumeric);
                return TypeInfo();
            }
            return operandType;
        }
        // Logical NOT: !
        if (op->op == T_NOT_RL) {
            if (!operandType.isBoolean()) {
                reportError(TypeCheckError::AttemptedBoolOpOnNonBools);
                return TypeInfo();
            }
            return TypeInfo(T_BOOL_RL);
        }
        
        return TypeInfo();
    }
    TypeInfo checkAssignment(Assignment* assign) {
        Symbol* sym = currentScope->lookup(assign->ident);
        if (!sym) {
            return TypeInfo();
        }
        TypeInfo varType(sym->type);
        TypeInfo valueType = checkExpression(assign->value);
        
        if (!varType.matches(valueType)) {
            reportError(TypeCheckError::ExpressionTypeMismatch, assign->ident);
            return TypeInfo();
        }
        return varType;
    }
    
    TypeInfo checkFunctionCall(FunctionCall* call) {
        auto it = functionTable.find(call->name);
        if (it == functionTable.end()) {
            return TypeInfo();
        }
        FunctionSignature& sig = it->second;
        if (call->args.size() != sig.paramTypes.size()) {
            reportError(TypeCheckError::FnCallParamCount, call->name);
            return TypeInfo();
        }
        for (size_t i = 0; i < call->args.size(); i++) {
            TypeInfo argType = checkExpression(call->args[i]);
            if (argType.type != sig.paramTypes[i]) {
                reportError(TypeCheckError::FnCallParamType, 
                    call->name + " at parameter " + to_string(i + 1));
                return TypeInfo();
            }
        }
        return TypeInfo(sig.returnType);
    }
    void checkStatement(Stmt* stmt) {
        if (!stmt) return;
        
        switch (stmt->nodeType) {
            case NODE_VAR_DECL: {
                auto* varDecl = static_cast<VarDecl*>(stmt);
                checkVarDecl(varDecl);
                break;
            }
            case NODE_EXPR_STMT: {
                auto* exprStmt = static_cast<ExprStmt*>(stmt);
                checkExpression(exprStmt->expr);
                break;
            }
            case NODE_RETURN: {
                auto* retStmt = static_cast<ReturnStmt*>(stmt);
                checkReturnStmt(retStmt);
                break;
            }
            case NODE_BREAK:
            case NODE_CONTINUE: {
                if (loopDepth == 0) {
                    reportError(TypeCheckError::ErroneousBreak);
                }
                break;
            }
            case NODE_BLOCK: {
                auto* block = static_cast<Block*>(stmt);
                pushScope();
                for (auto* s : block->stmts) {
                    checkStatement(s);
                }
                popScope();
                break;
            }
            case NODE_IF: {
                auto* ifStmt = static_cast<IfStmt*>(stmt);
                checkIfStmt(ifStmt);
                break;
            }
            case NODE_WHILE: {
                auto* whileStmt = static_cast<WhileStmt*>(stmt);
                checkWhileStmt(whileStmt);
                break;
            }
            case NODE_FOR: {
                auto* forStmt = static_cast<ForStmt*>(stmt);
                checkForStmt(forStmt);
                break;
            }
            default:
                break;
        }
    }
    
    void checkVarDecl(VarDecl* decl) {
        if (decl->expr) {
            TypeInfo initType = checkExpression(decl->expr);
            if (initType.isValid && initType.type != decl->type) {
                reportError(TypeCheckError::ErroneousVarDecl, decl->ident);
            }
        }
    }
    
    void checkReturnStmt(ReturnStmt* stmt) {
        hasReturnStmt = true;
        
        TypeInfo returnType;
        if (stmt->expr) {
            returnType = checkExpression(stmt->expr);
        } else {
            returnType = TypeInfo(T_INT_RL);  
        }
        
        if (returnType.isValid && returnType.type != currentFunctionReturnType) {
            reportError(TypeCheckError::ErroneousReturnType);
        }
    }
    
    void checkIfStmt(IfStmt* stmt) {
        TypeInfo condType = checkExpression(stmt->condition);
        
        if (condType.isValid && !condType.isBoolean()) {
            reportError(TypeCheckError::NonBooleanCondStmt);
        }
        
        checkStatement(stmt->thenBranch);
        if (stmt->elseBranch) {
            checkStatement(stmt->elseBranch);
        }
    }
    
    void checkWhileStmt(WhileStmt* stmt) {
        TypeInfo condType = checkExpression(stmt->condition);
        
        if (condType.isValid && !condType.isBoolean()) {
            reportError(TypeCheckError::NonBooleanCondStmt);
        }
        
        loopDepth++;
        checkStatement(stmt->body);
        loopDepth--;
    }
    
    void checkForStmt(ForStmt* stmt) {
        pushScope();
        
        if (stmt->init) {
            checkStatement(stmt->init);
        }
        
        if (stmt->condition) {
            TypeInfo condType = checkExpression(stmt->condition);
            if (condType.isValid && !condType.isBoolean()) {
                reportError(TypeCheckError::NonBooleanCondStmt);
            }
        }
        
        if (stmt->update) {
            checkExpression(stmt->update);
        }
        
        loopDepth++;
        checkStatement(stmt->body);
        loopDepth--;
        
        popScope();
    }
    
public:
    TypeChecker() : loopDepth(0), hasReturnStmt(false) {
        currentScope = make_shared<Scope>();
        currentFunctionReturnType = T_INT_RL;
    }
    
    void check(vector<Stmt*>& program, shared_ptr<Scope> globalScope) {
        currentScope = globalScope;
        for (auto* stmt : program) {
            if (stmt->nodeType == NODE_FUNC_DECL) {
                auto* funcDecl = static_cast<FunctionDecl*>(stmt);
                vector<TokenType> paramTypes;
                for (const auto& param : funcDecl->params) {
                    paramTypes.push_back(param.type);
                }
                functionTable.insert({funcDecl->name, 
                    FunctionSignature(funcDecl->name, funcDecl->returnType, paramTypes)});
            }
        }
        for (auto* stmt : program) {
            if (stmt->nodeType == NODE_FUNC_DECL) {
                auto* funcDecl = static_cast<FunctionDecl*>(stmt);
                checkFunctionDecl(funcDecl);
            } else {
                checkStatement(stmt);
            }
        }
    }
    
    void checkFunctionDecl(FunctionDecl* decl) {
        currentFunctionReturnType = decl->returnType;
        hasReturnStmt = false;
        pushScope();
        for (const auto& param : decl->params) {
            Symbol paramSym(param.name, param.type);
            currentScope->addSymbol(paramSym);
        }
        checkStatement(decl->body);
        if (!hasReturnStmt) {
            reportError(TypeCheckError::ReturnStmtNotFound, decl->name);
        }
        popScope();
    }
};
#endif