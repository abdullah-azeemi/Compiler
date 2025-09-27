#ifndef TOKEN_QUEUE_HPP
#define TOKEN_QUEUE_HPP

#define MAX_TOKENS 100
#define MAX_LEN 50

#include <iostream>
using namespace std;

struct Token
{
  char type[MAX_LEN];
  char value[MAX_LEN];
};

using namespace std;

class stringQueue
{
public:
  stringQueue()
  {
    front = 0;
    rear = -1;
    count = 0;
  }

  void push(const char *type, const char *value = "")
  {
    if (count == MAX_TOKENS)
      return;

    rear = (rear + 1) % MAX_TOKENS;

    copyString(tokens[rear].type, type);
    copyString(tokens[rear].value, value);
    count += 1;
  }

  Token pop()
  {
    Token t = tokens[front];
    front = (front + 1) % MAX_TOKENS;
    count -= 1;
    return t;
  }

  Token peek()
  {
    return tokens[front];
  }

  bool empty()
  {
    return count == 0;
  }

  int size()
  {
    return count;
  }

  void printAll()
  {
    int ci = front;
    for (int i = 0; i < count; i++)
    {
      printToken(tokens[ci]);
      ci = (ci + 1) % MAX_TOKENS;
    }
  }

private:
  Token tokens[MAX_TOKENS];
  int front, rear, count;

  void copyString(char *dest, const char *src)
  {
    int i = 0;
    while (src[i] != '\0' && i < MAX_LEN - 1)
    {
      dest[i] = src[i];
      i += 1;
    }
    dest[i] = '\0';
  }

  void printToken(Token &t)
  {
    cout << "-> " << t.type;
    if (t.value[0] != '\0')
    {
      cout << ", " << t.value;
    }
    cout << endl;
  }
};

#endif