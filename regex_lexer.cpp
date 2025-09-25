#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

regex keyword("(fn|return|if|else|while|for|int|float|string|bool)");
// regex identifier("[a-zA-Z_][a-zA-Z0-9_]*");
regex identifier("([_[:alpha:]][_[:alnum:]_]*)");
regex floats("[0-9]+\\.[0-9]+");
regex integers("[0-9]+");
regex strings("\"(\\\\.|[^\"])*\"");
regex parenL("\\("), parenR("\\)");
regex braceL("\\{"), braceR("\\}");
regex bracketL("\\["), bracketR("\\]");
regex assign("=");
regex comparison("(==|>=|<=|!=|>|<)");
regex logical("(\\|\\||&&|!)");
regex operations("[+\\-*/%]");
regex semicolon(";"), comma(","), dot("\\."), quotes("\"");
regex spaces("\\s+");
regex comment("\\\\");
regex singleLineComment("//.*");
regex multiLineComment("/\\*([^*]|(\\*+[^*/]))*\\*+/");
regex bitwise("[&|^]");

enum TokenType
{
    T_FUNCTION_RL,
    T_RETURN_RL,
    T_IF_RL,
    T_ELSE_RL,
    T_WHILE_RL,
    T_FOR_RL,
    T_INT_RL,
    T_FLOAT_RL,
    T_STRING_RL,
    T_BOOL_RL,
    T_IDENTIFIER_RL,
    T_INT_RLLIT,
    T_FLOAT_RLLIT,
    T_STRING_RLLIT,
    T_ASSIGNOP_RL,
    T_EQUALSOP_RL,
    T_COMPARISON_RL,
    T_LOGICAL_RL,
    T_PARENL_RL,
    T_PARENR_RL,
    T_BRACEL_RL,
    T_BRACER_RL,
    T_BRACKETL_RL,
    T_BRACKETR_RL,
    T_PLUS_RL,
    T_MINUS_RL,
    T_MUL_RL,
    T_DIV_RL,
    T_MOD_RL,
    T_SEMICOLON_RL_RL,
    T_COMMA_RL,
    T_DOT_RL,
    T_QUOTES_RL,
    T_UNKNOWN_RL,
    T_SINGLE_COMMENT_RL,
    T_MULTI_COMMENT_RL,
    T_AND_RL,
    T_OR_RL,
    T_XOR_RL,

};

class Regex_Lexer
{
public:
    string tokenToString(TokenType type, const string &val = "")
    {
        switch (type)
        {
        case T_FUNCTION_RL:
            return "T_FUNCTION_RL";
        case T_RETURN_RL:
            return "T_RETURN_RL";
        case T_IF_RL:
            return "T_IF_RL";
        case T_ELSE_RL:
            return "T_ELSE_RL";
        case T_WHILE_RL:
            return "T_WHILE_RL";
        case T_FOR_RL:
            return "T_FOR_RL";
        case T_INT_RL:
            return "T_INT_RL";
        case T_FLOAT_RL:
            return "T_FLOAT_RL";
        case T_STRING_RL:
            return "T_STRING_RL";
        case T_BOOL_RL:
            return "T_BOOL_RL";
        case T_IDENTIFIER_RL:
            return "T_IDENTIFIER_RL(\"" + val + "\")";
        case T_INT_RLLIT:
            return "T_INT_RLLIT(" + val + ")";
        case T_FLOAT_RLLIT:
            return "T_FLOAT_RLLIT(" + val + ")";
        case T_STRING_RLLIT:
            return "T_STRING_RLLIT(" + val + ")";
        case T_ASSIGNOP_RL:
            return "T_ASSIGNOP_RL";
        case T_EQUALSOP_RL:
            return "T_EQUALSOP_RL";
        case T_COMPARISON_RL:
            return "T_COMPARISON_RL";
        case T_LOGICAL_RL:
            return "T_LOGICAL_RL";
        case T_PARENL_RL:
            return "T_PARENL_RL";
        case T_PARENR_RL:
            return "T_PARENR_RL";
        case T_BRACEL_RL:
            return "T_BRACEL_RL";
        case T_BRACER_RL:
            return "T_BRACER_RL";
        case T_BRACKETL_RL:
            return "T_BRACKETL_RL";
        case T_BRACKETR_RL:
            return "T_BRACKETR_RL";
        case T_PLUS_RL:
            return "T_PLUS_RL";
        case T_MINUS_RL:
            return "T_MINUS_RL";
        case T_MUL_RL:
            return "T_MUL_RL";
        case T_DIV_RL:
            return "T_DIV_RL";
        case T_MOD_RL:
            return "T_MOD_RL";
        case T_SEMICOLON_RL_RL:
            return "T_SEMICOLON_RL_RL";
        case T_COMMA_RL:
            return "T_COMMA_RL";
        case T_DOT_RL:
            return "T_DOT_RL";
        case T_QUOTES_RL:
            return "T_QUOTES_RL";
        case T_SINGLE_COMMENT_RL:
            return "T_SINGLE_COMMENT_RL(\"" + val + "\")";
        case T_MULTI_COMMENT_RL:
            return "T_MULTI_COMMENT_RL(\"" + val + "\")";
        default:
            return "T_UNKNOWN_RL(" + val + ")";
        }
    }

    struct Token
    {
        TokenType type;
        string value;
    };

    vector<Token> tokenize(const string &code)
    {
        vector<Token> tokens;
        auto begin = code.begin();

        while (begin != code.end())
        {
            smatch match;

            if (regex_search(begin, code.end(), match, spaces) && match.position() == 0)
            {
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, keyword) && match.position() == 0)
            {
                string val = match.str();
                if (val == "fn")
                    tokens.push_back({T_FUNCTION_RL, val});
                else if (val == "return")
                    tokens.push_back({T_RETURN_RL, val});
                else if (val == "if")
                    tokens.push_back({T_IF_RL, val});
                else if (val == "else")
                    tokens.push_back({T_ELSE_RL, val});
                else if (val == "while")
                    tokens.push_back({T_WHILE_RL, val});
                else if (val == "for")
                    tokens.push_back({T_FOR_RL, val});
                else if (val == "int")
                    tokens.push_back({T_INT_RL, val});
                else if (val == "float")
                    tokens.push_back({T_FLOAT_RL, val});
                else if (val == "string")
                    tokens.push_back({T_STRING_RL, val});
                else if (val == "bool")
                    tokens.push_back({T_BOOL_RL, val});
                else if (val == "&")
                    tokens.push_back({T_AND_RL, val});
                else if (val == "|")
                    tokens.push_back({T_OR_RL, val});
                else if (val == "^")
                    tokens.push_back({T_XOR_RL, val});

                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, identifier) && match.position() == 0)
            {
                tokens.push_back({T_IDENTIFIER_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, floats) && match.position() == 0)
            {
                tokens.push_back({T_FLOAT_RLLIT, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, integers) && match.position() == 0)
            {
                tokens.push_back({T_INT_RLLIT, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, strings) && match.position() == 0)
            {
                tokens.push_back({T_STRING_RLLIT, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, assign) && match.position() == 0)
            {
                tokens.push_back({T_ASSIGNOP_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, comparison) && match.position() == 0)
            {
                tokens.push_back({T_COMPARISON_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, logical) && match.position() == 0)
            {
                tokens.push_back({T_LOGICAL_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, operations) && match.position() == 0)
            {
                string val = match.str();
                if (val == "+")
                    tokens.push_back({T_PLUS_RL, val});
                else if (val == "-")
                    tokens.push_back({T_MINUS_RL, val});
                else if (val == "*")
                    tokens.push_back({T_MUL_RL, val});
                else if (val == "/")
                    tokens.push_back({T_DIV_RL, val});
                else if (val == "%")
                    tokens.push_back({T_MOD_RL, val});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, parenL) && match.position() == 0)
            {
                tokens.push_back({T_PARENL_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, parenR) && match.position() == 0)
            {
                tokens.push_back({T_PARENR_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, braceL) && match.position() == 0)
            {
                tokens.push_back({T_BRACEL_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, braceR) && match.position() == 0)
            {
                tokens.push_back({T_BRACER_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, bracketL) && match.position() == 0)
            {
                tokens.push_back({T_BRACKETL_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, bracketR) && match.position() == 0)
            {
                tokens.push_back({T_BRACKETR_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, semicolon) && match.position() == 0)
            {
                tokens.push_back({T_SEMICOLON_RL_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, comma) && match.position() == 0)
            {
                tokens.push_back({T_COMMA_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, dot) && match.position() == 0)
            {
                tokens.push_back({T_DOT_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, quotes) && match.position() == 0)
            {
                tokens.push_back({T_QUOTES_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, multiLineComment) && match.position() == 0)
            {
                tokens.push_back({T_MULTI_COMMENT_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, singleLineComment) && match.position() == 0)
            {
                tokens.push_back({T_SINGLE_COMMENT_RL, match.str()});
                begin += match.length();
                continue;
            }
            if (regex_search(begin, code.end(), match, bitwise) && match.position() == 0)
            {
                string val = match.str();
                if (val == "&")
                    tokens.push_back({T_UNKNOWN_RL, "&"});
                else if (val == "|")
                    tokens.push_back({T_UNKNOWN_RL, "|"});
                else if (val == "^")
                    tokens.push_back({T_UNKNOWN_RL, "^"});
                begin += match.length();
                continue;
            }

            tokens.push_back({T_UNKNOWN_RL, string(1, *begin)});
            ++begin;
        }

        return tokens;
    }

    void RegexLexer(string input)
    {
        vector<Token> tokens = tokenize(input);
        for (auto &t : tokens)
        {
            cout << tokenToString(t.type, t.value) << endl;
        }
    }
};
