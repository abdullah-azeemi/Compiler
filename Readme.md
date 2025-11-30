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


# LOOP INTECHANGE:

Find nested loops

Look for loops that have exactly one inner loop (2-level nesting).

Check structure

Make sure both loops have a preheader and a latch (simplified loop form).

Find their induction variables (the PHI nodes controlling the loop).

Clone the inner loop

Make a copy of all the inner loop’s blocks.

Update the instructions inside the cloned blocks to use the new cloned variables.

Rewire the control flow

Change the outer loop’s preheader to start the cloned inner loop.

Make the cloned inner loop branch to the original outer loop when it finishes.

Make the outer loop’s latch branch to the cloned inner loop instead of itself.

Result

The inner loop now runs outside the outer loop.

The original outer loop runs inside the cloned inner loop.



