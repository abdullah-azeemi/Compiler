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

# Loop Interchange Pass (coursework submission)

## Overview

This LLVM pass identifies simple doubly-nested loops (outer loop with exactly one inner subloop)
and performs a best-effort **Loop Interchange** transformation: the inner loop is made the outer
loop and the outer loop becomes the inner loop. The pass uses a clone-and-rewire strategy and
is intentionally simplified for the assignment (we skip dependence checks / legality analysis).

> **Important:** This implementation assumes ideal canonical loops (LoopSimplify-like). It is
> not production ready. Use it only for the assignment demonstration.

---

## Files in this folder

- `LoopInterchange.cpp` — the pass source (New PassManager plugin).
- `CMakeLists.txt` — to build the pass as an LLVM plugin.
- `test.c` — small C test containing a doubly-nested loop (row-major array store).
- `README.md` — (this file) contains build/run, explanation, and screenshot instructions.

---

## Core idea / logic used to swap loop headers

**Assumptions (explicit):**
- Each loop has a preheader and a single latch (LoopSimplify form).
- Loops are perfectly nested: outer loop contains exactly one subloop, the inner loop,
  and the inner loop has no sub-subloops.
- Induction variables are canonical PHI nodes in the loop headers.
- We do **not** run dependence analysis or memory-safety checks; we assume interchange is safe.

**High-level transformation steps the pass uses:**

1. **Locate candidate loops**
   - Use `LoopAnalysis` / `LoopInfo` to iterate top-level loops.
   - Select loops `Outer` where `Outer->getSubLoops().size() == 1` and the subloop `Inner`
     itself has no subloops (ie. it is a pure 2-level nest).

2. **Validate canonical structure**
   - Require both `Outer` and `Inner` to have preheaders and latches.
   - Find canonical PHI induction nodes in both headers (a simple integer PHI whose incoming
     blocks are preheader and latch).

3. **Clone inner loop blocks**
   - Collect all basic blocks that belong to `Inner`.
   - Clone those blocks into the function using `CloneBasicBlock` while building a `ValueToValueMapTy`.
   - Remap instructions in cloned blocks to refer to each other (using `RemapInstruction`).

4. **Rewire control flow**
   - Redirect the `outerPreheader` predecessor that would have gone to `innerPreheader` to the new (cloned) inner preheader.
   - Change the **cloned inner latch**'s loop-back branch that originally pointed to the cloned inner header,
     so that when the cloned inner iteration finishes it branches to the original `outerHeader`.
   - Change the **original outer latch**'s loop-back successor (which previously went to the original `outerHeader`)
     to instead go to the cloned inner header.
   - This results in the cloned inner loop acting as the **new outer loop**, and the original outer loop being executed
     inside it (effectively becoming the inner loop).

5. **Finish**
   - The pass prints a diagnostic message and returns that it modified the function.
   - Analyses like DominatorTree and LoopInfo are conservatively marked as not preserved (PassManager can recompute).

**Why clone+rewire?**
- Directly reconstructing swapped loop CFG and PHIs precisely is tedious and error-prone.
- Cloning the inner loop and rewiring a few edges is a simpler, illustrative way to demonstrate the concept
  under the assignment assumptions.

**What the pass does NOT do (important):**
- Perform any dependence or alias analysis: legality of interchange is not checked.
- Robustly maintain/updating all analyses (like DomTree/LoopInfo/ScalarEvolution) incrementally.

---

## Build instructions

You need a matching LLVM dev environment. `cmake` and a recent LLVM (12+) are required.

Example (adjust `LLVM_DIR` as needed):

```bash
mkdir build && cd build
cmake -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm ..
make
# plugin will be produced: libLoopInterchangePass.so (name may vary)


