#ifndef REGEX_LEXER_HPP
#define REGEX_LEXER_HPP

#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include "Utilities/token_types.hpp"

using namespace std;

class Lexer
{
private:
    regex singleLineComment{"//.*"};
    regex multiLineComment{"/\\*([^*]|(\\*+[^*/]))*\\*+/"};
    regex keyword{"\\b(fn|return|if|else|while|for|break|continue|int|float|string|bool|ginti|wapsi|agar|warna|duhrao|jab|toro|rakho)\\b"};
    std::regex identifier{"[a-zA-Z_][a-zA-Z0-9_]*"};
    regex floatLit{"[0-9]+\\.[0-9]+"};
    regex intLit{"[0-9]+"};
    regex stringLit{"\"(\\\\.|[^\\\"])*\""};
    regex boolLit{"\\b(true|false|sahi|galat)\\b"};
    regex spaces{"\\s+"};

    map<std::string, TokenType> keywordMap = {
        {"fn", T_FUNCTION_RL}, {"return", T_RETURN_RL}, {"if", T_IF_RL}, {"else", T_ELSE_RL}, {"while", T_WHILE_RL}, {"for", T_FOR_RL}, {"break", T_BREAK_RL}, {"continue", T_CONTINUE_RL}, {"int", T_INT_RL}, {"float", T_FLOAT_RL}, {"string", T_STRING_RL}, {"bool", T_BOOL_RL},

        // for asm, urdru keywords
        {"ginti", T_INT_RL},
        {"wapsi", T_RETURN_RL},
        {"agar", T_IF_RL},
        {"warna", T_ELSE_RL},
        {"duhrao", T_FOR_RL},
        {"jab", T_WHILE_RL},
        {"toro", T_BREAK_RL},
        {"rakho", T_CONTINUE_RL}};

    int currentLine = 1;
    int currentColumn = 1;

    void updatePosition(const std::string &text)
    {
        for (char c : text)
        {
            if (c == '\n')
            {
                currentLine++;
                currentColumn = 1;
            }
            else
            {
                currentColumn++;
            }
        }
    }

public:
    vector<Token> tokenize(const std::string &code)
    {
        vector<Token> tokens;
        auto begin = code.cbegin();
        auto end = code.cend();

        currentLine = 1;
        currentColumn = 1;

        while (begin != end)
        {
            smatch match;
            int startLine = currentLine;
            int startCol = currentColumn;

            // Skipping whitespace
            if (regex_search(begin, end, match, spaces) && match.position() == 0)
            {
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            // Skip comments
            if (regex_search(begin, end, match, multiLineComment) && match.position() == 0)
            {
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, singleLineComment) && match.position() == 0)
            {
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, stringLit) && match.position() == 0)
            {
                std::string val = match.str();
                val = val.substr(1, val.length() - 2);
                tokens.push_back(Token(T_STRING_RLLIT, val, startLine, startCol));
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, keyword) && match.position() == 0)
            {
                std::string val = match.str();
                auto it = keywordMap.find(val);
                if (it != keywordMap.end())
                {
                    tokens.push_back(Token(it->second, val, startLine, startCol));
                }
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, boolLit) && match.position() == 0)
            {
                std::string val = match.str();
                std::string boolVal = (val == "true" || val == "sahi") ? "true" : "false";
                tokens.push_back(Token(T_BOOL_RLLIT, boolVal, startLine, startCol));
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            // Identifiers
            if (regex_search(begin, end, match, identifier) && match.position() == 0)
            {
                tokens.push_back(Token(T_IDENTIFIER_RL, match.str(), startLine, startCol));
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, floatLit) && match.position() == 0)
            {
                tokens.push_back(Token(T_FLOAT_RLLIT, match.str(), startLine, startCol));
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (regex_search(begin, end, match, intLit) && match.position() == 0)
            {
                tokens.push_back(Token(T_INT_RLLIT, match.str(), startLine, startCol));
                updatePosition(match.str());
                begin = match[0].second;
                continue;
            }

            if (begin + 1 != end)
            {
                std::string twoChar(begin, begin + 2);
                TokenType twoCharToken = T_UNKNOWN_RL;

                if (twoChar == "==")
                    twoCharToken = T_EQUALSOP_RL;
                else if (twoChar == "!=")
                    twoCharToken = T_NOT_EQUALS_RL;
                else if (twoChar == "<=")
                    twoCharToken = T_LESS_EQUAL_RL;
                else if (twoChar == ">=")
                    twoCharToken = T_GREATER_EQUAL_RL;
                else if (twoChar == "&&")
                    twoCharToken = T_AND_LOGICAL_RL;
                else if (twoChar == "||")
                    twoCharToken = T_OR_LOGICAL_RL;

                if (twoCharToken != T_UNKNOWN_RL)
                {
                    tokens.push_back(Token(twoCharToken, twoChar, startLine, startCol));
                    updatePosition(twoChar);
                    begin += 2;
                    continue;
                }
            }

            // for single character tokens
            char c = *begin;
            TokenType tokenType = T_UNKNOWN_RL;
            std::string tokenValue(1, c);

            switch (c)
            {
            case '(':
                tokenType = T_PARENL_RL;
                break;
            case ')':
                tokenType = T_PARENR_RL;
                break;
            case '{':
                tokenType = T_BRACEL_RL;
                break;
            case '}':
                tokenType = T_BRACER_RL;
                break;
            case '[':
                tokenType = T_BRACKETL_RL;
                break;
            case ']':
                tokenType = T_BRACKETR_RL;
                break;
            case ';':
                tokenType = T_SEMICOLON_RL;
                break;
            case ',':
                tokenType = T_COMMA_RL;
                break;
            case '.':
                tokenType = T_DOT_RL;
                break;
            case '+':
                tokenType = T_PLUS_RL;
                break;
            case '-':
                tokenType = T_MINUS_RL;
                break;
            case '*':
                tokenType = T_MUL_RL;
                break;
            case '/':
                tokenType = T_DIV_RL;
                break;
            case '%':
                tokenType = T_MOD_RL;
                break;
            case '=':
                tokenType = T_ASSIGNOP_RL;
                break;
            case '<':
                tokenType = T_LESS_THAN_RL;
                break;
            case '>':
                tokenType = T_GREATER_THAN_RL;
                break;
            case '!':
                tokenType = T_NOT_RL;
                break;
            case '&':
                tokenType = T_AND_BIT_RL;
                break;
            case '|':
                tokenType = T_OR_BIT_RL;
                break;
            case '^':
                tokenType = T_XOR_BIT_RL;
                break;
            }

            if (tokenType == T_UNKNOWN_RL)
            {
                cerr << "Warning: Unknown character '" << c
                     << "' at line " << startLine
                     << ", column " << startCol << endl;
            }

            tokens.push_back(Token(tokenType, tokenValue, startLine, startCol));
            updatePosition(tokenValue);
            begin++;
        }

        tokens.push_back(Token(T_EOF_RL, "", currentLine, currentColumn));
        return tokens;
    }
};

#endif
