#  Simple Lexer & Parser

A **basic compiler front-end** that tokenizes source code and parses it into an **Abstract Syntax Tree (AST)**.

---

### Lexical Analysis

* Takes code like:

  ```cpp
  fn add(int a) { return a + 1; }
  ```
* Splits it into tokens:

  ```
  fn, add, (, int, a, ), {, return, a, +, 1, ;, }
  ```
* Classifies each token (e.g., keyword, identifier, operator, symbol, number).

### Parsing

* Converts the token stream into a structured AST.
* Supports variable declarations, assignments, and expressions like:

  ```cpp
  int a = b + c ----d .
  ```
* Validates syntax and builds nested tree nodes such as:

  ```
  VarDecl(a = BinaryOp(+, b, UnaryOp(-, UnaryOp(-, UnaryOp(-, UnaryOp(-, d))))))
  ```

---

## ⚙️ How to Use

1. **Compile**

   ```bash
   g++ -std=c++17 -o lexer lexer1.cpp
   ```
2. **Run**

   ```bash
   ./lexer
   ```

---
