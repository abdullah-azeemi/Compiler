#include<iostream>
#include<conio.h>
#include<sstream>
#include<regex>

using namespace std;

regex keyword("(fn|return|if|else|while|for|int|float|string|bool)");

regex identifier("[a-zA-Z_][a-zA-Z0-9_]*");

regex integers("[0-9]+");
regex floats("[0-9]+\\.[0-9]+");
regex strings("\"(\\\\.|[^\"])*\"");
regex paren("[()]");       
regex brace("[{}]");       
regex brackets("[\\[\\]]"); 
regex assign("=");
regex comparison("(==|>=|<=|!=|>|<)");
regex operations("[+\\-*/%]");
regex spaces("\\s+");
regex logical("(\\|\\||&&|!)");
regex semicolon(";");
regex comma(",");
regex dot("\\.");
regex quotes("\"");





int main()
{
    string check="int a=b + c";

    auto starting=sregex_iterator(check.begin(),check.end(),identifier);
    auto ending=sregex_iterator();

    for(auto i=starting;i!= ending;i++)
    {
        smatch match=* i;
        cout<<match.str()<<"\n";
    }
    return 0;
}


