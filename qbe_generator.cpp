#include "qbe_generator.hpp"
#include <algorithm>
#include <map>
#include <stdexcept>

string QBEGenerator::typeToQBE(TokenType type) {
    switch (type) {
        case T_INT_RL:
            return "l"; 
        case T_FLOAT_RL:
            return "d"; 
        case T_BOOL_RL:
            return "b";
        default:
            return "l"; 
    }
}

string QBEGenerator::formatName(const string& name) {
    if (name == "true") return "1";
    if (name == "false") return "0";
    try {
        size_t pos;
        stod(name, &pos);
        if (pos == name.length()) {
            return name; 
        }
    } catch (...) {
    }
    
    if (name.rfind("_t", 0) == 0) {
        return "%" + name.substr(1); // e.g., _t0 -> %t0
    }
    return "%" + name;
}

void QBEGenerator::emit(const string& line) {
    qbe_ir << line << endl;
}

void QBEGenerator::generateMainWrapper(const vector<Quad>& quads) {
    emit("export function l $main() {"); 
    emit("@start"); 

    for (size_t i = 0; i < quads.size(); ++i) {
        translateQuad(quads, i);
    }
    
    emit("  ret 0"); 
    emit("}");
}
void QBEGenerator::translateQuad(const vector<Quad>& quads, size_t& index) {
    const Quad& quad = quads[index];
    string resultName = formatName(quad.result);
    string arg1Name = formatName(quad.arg1);
    string arg2Name = formatName(quad.arg2);
    
    if (quad.op == "label") {
        emit(quad.result + ":"); 
        return;
    }

    if (quad.op == "copy") {
        emit("  " + resultName + " =l copy " + arg1Name);
        return;
    }

    if (quad.op == "goto") {
        // e.g., jmp _L0
        emit("  jmp " + quad.result); 
        return;
    }

    if (quad.op == "if_false") {
        
        string falseLabel = quad.result;
        string trueLabel = "$L" + to_string(rand() % 1000000); 
        string condReg = formatName(quad.arg1); 
        emit("  jmpf " + condReg + ", " + falseLabel + ", " + trueLabel);
        emit(trueLabel + ":");
        return;
    }
    
    map<string, string> opMap = {
        {"+", "add"}, {"-", "sub"}, {"*", "mul"}, {"/", "div"},
        {"==", "ceq"}, {"!=", "cne"}, {"<", "clt"}, {">", "cgt"}, {"<=", "cle"}, {">=", "cge"},
        {"neg", "neg"}
    };

    if (opMap.count(quad.op)) {
        string qbeOp = opMap.at(quad.op);
        
        if (quad.op == "neg") { 
            emit("  " + resultName + " =l neg " + arg1Name);
        } else if (qbeOp.rfind("c", 0) == 0) {
            string tempResult = "$b" + quad.result.substr(1); 
            emit("  " + tempResult + " =b " + qbeOp + " " + arg1Name + ", " + arg2Name);
            emit("  " + resultName + " =l extub " + tempResult); 
        } else {
            emit("  " + resultName + " =l " + qbeOp + " " + arg1Name + ", " + arg2Name);
        }
        return;
    }

    if (quad.op == "return") {
        emit("  ret " + arg1Name);
        return;
    }
    emit("  # Unhandled quad: " + quad.toString());
}



string QBEGenerator::generate(const vector<Quad>& quads) {
    qbe_ir.str(""); 
    qbe_ir.clear();
    generateMainWrapper(quads);

    return qbe_ir.str();
}

string QBEGenerator::newTemp() {
    return "_t" + to_string(argCounter++);
}