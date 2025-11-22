#ifndef QBE_GENERATOR_HPP
#define QBE_GENERATOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "ir_generator.hpp" 
#include "Utilities/token_types.hpp" 

using namespace std;

class QBEGenerator {
private:
    stringstream qbe_ir;
    int argCounter = 0;
    string newTemp();
    void translateQuad(const vector<Quad>& quads, size_t& index); 
    string formatName(const string& name);
    string typeToQBE(TokenType type); 
    void emit(const string& line);
    void generateMainWrapper(const vector<Quad>& quads);
    
public:
    QBEGenerator() = default;
    string generate(const vector<Quad>& quads);
};

#endif