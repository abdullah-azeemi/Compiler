#ifndef SCOPE_ANALYZER_HPP
#define SCOPE_ANALYZER_HPP

#include <iostream>
#include <unordered_map>
#include <memory>
#include <stack>
#include <string>
#include "Utilities/token_types.hpp"
#include "ast.hpp"

using namespace std;

// The error types for scope analysis
enum class ScopeError {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition
};

// symbol representation
struct Symbol {
    string name;
    TokenType type;
    bool isFunction;
    bool isDefined;

    Symbol() : name(""), type(T_IDENTIFIER_RL), isFunction(false), isDefined(false) {}

    Symbol(string n, TokenType t, bool func = false, bool defined = true)
        : name(n), type(t), isFunction(func), isDefined(defined) {}
};

// Scope (spaghetti scope representation)
class Scope {
public:
    unordered_map<string, Symbol> symbols;
    shared_ptr<Scope> parent;

    Scope(shared_ptr<Scope> p = nullptr) : parent(p) {}

    bool addSymbol(const Symbol &sym) {
        if (symbols.find(sym.name) != symbols.end()) {
            return false; 
        }
        symbols[sym.name] = sym;
        return true;
    }

    Symbol *lookup(const string &name) {
        if (symbols.find(name) != symbols.end())
            return &symbols[name];
        if (parent)
            return parent->lookup(name);
        return nullptr;
    }
};

class ScopeAnalyzer {
private:
    shared_ptr<Scope> currentScope;
    
    void pushScope() {
        currentScope = make_shared<Scope>(currentScope);
    }

    void popScope() {
        if (currentScope) currentScope = currentScope->parent;
    }

    void reportError(ScopeError err, const string &name) {
        switch (err) {
        case ScopeError::UndeclaredVariableAccessed:
            cerr << "Scope Error: Undeclared variable accessed -> " << name << endl;
            break;
        case ScopeError::UndefinedFunctionCalled:
            cerr << "Scope Error: Undefined function called -> " << name << endl;
            break;
        case ScopeError::VariableRedefinition:
            cerr << "Scope Error: Variable redefinition -> " << name << endl;
            break;
        case ScopeError::FunctionPrototypeRedefinition:
            cerr << "Scope Error: Function redefinition -> " << name << endl;
            break;
        }
    }

public:
    ScopeAnalyzer() {
        currentScope = make_shared<Scope>(); // the global scope
    }

    void analyze(ASTNode *node) {
        if (!node) return;

        switch (node->nodeType) {
        // Expressions ----------
        case NODE_IDENTIFIER: {
            auto *id = static_cast<Identifier *>(node);
            if (!currentScope->lookup(id->name)) {
                reportError(ScopeError::UndeclaredVariableAccessed, id->name);
            }
            break;
        }
        case NODE_ASSIGNMENT: {
            auto *as = static_cast<Assignment *>(node);
            if (!currentScope->lookup(as->ident)) {
                reportError(ScopeError::UndeclaredVariableAccessed, as->ident);
            }
            analyze(as->value);
            break;
        }
        case NODE_FUNC_CALL: {
            auto *fc = static_cast<FunctionCall *>(node);
            Symbol *fn = currentScope->lookup(fc->name);
            if (!fn || !fn->isFunction) {
                reportError(ScopeError::UndefinedFunctionCalled, fc->name);
            }
            for (auto arg : fc->args)
                analyze(arg);
            break;
        }
        case NODE_BINARY_OP: {
            auto *bin = static_cast<BinaryOp *>(node);
            analyze(bin->left);
            analyze(bin->right);
            break;
        }
        case NODE_UNARY_OP: {
            auto *un = static_cast<UnaryOp *>(node);
            analyze(un->operand);
            break;
        }

        // Statements ----------
        case NODE_VAR_DECL: {
            auto *vd = static_cast<VarDecl *>(node);
            Symbol sym(vd->ident, vd->type, false);
            if (!currentScope->addSymbol(sym)) {
                reportError(ScopeError::VariableRedefinition, vd->ident);
            }
            if (vd->expr) analyze(vd->expr);
            break;
        }

        case NODE_BLOCK: {
            auto *block = static_cast<Block *>(node);
            pushScope();
            for (auto stmt : block->stmts)
                analyze(stmt);
            popScope();
            break;
        }

        case NODE_IF: {
            auto *ifstmt = static_cast<IfStmt *>(node);
            analyze(ifstmt->condition);
            analyze(ifstmt->thenBranch);
            if (ifstmt->elseBranch)
                analyze(ifstmt->elseBranch);
            break;
        }

        case NODE_WHILE: {
            auto *wh = static_cast<WhileStmt *>(node);
            analyze(wh->condition);
            analyze(wh->body);
            break;
        }

        case NODE_FOR: {
            auto *f = static_cast<ForStmt *>(node);
            pushScope();
            if (f->init) analyze(f->init);
            if (f->condition) analyze(f->condition);
            if (f->update) analyze(f->update);
            analyze(f->body);
            popScope();
            break;
        }
        case NODE_BREAK:
        {
            break;
        }
        case NODE_CONTINUE:
        {
            break;
        }
        case NODE_RETURN: {
            auto *ret = static_cast<ReturnStmt *>(node);
            if (ret->expr) analyze(ret->expr);
            break;
        }

        case NODE_FUNC_DECL: {
            auto *fd = static_cast<FunctionDecl *>(node);
            Symbol sym(fd->name, fd->returnType, true);
            if (!currentScope->addSymbol(sym)) {
                reportError(ScopeError::FunctionPrototypeRedefinition, fd->name);
            }

            pushScope();
            for (auto &p : fd->params) {
                Symbol paramSym(p.name, p.type);
                if (!currentScope->addSymbol(paramSym)) {
                    reportError(ScopeError::VariableRedefinition, p.name);
                }
            }
            analyze(fd->body);
            popScope();
            break;
        }

        case NODE_EXPR_STMT: {
            auto *es = static_cast<ExprStmt *>(node);
            analyze(es->expr);
            break;
        }

        default:
            break;
        }
    }

    shared_ptr<Scope> getGlobalScope() {
        auto s = currentScope;
        while (s->parent) {
            s = s->parent;
        }
        return s;
    }
};

#endif