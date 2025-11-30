// LoopInterchange.cpp
// Loop Interchange pass (New PassManager).
//
// This is a best-effort toy pass intended for coursework/demonstration only.
// It finds top-level loops that have exactly one direct subloop (a 2-level nest)
// and applies a clone-and-rewire transformation so the inner loop becomes the
// outer loop and the original outer becomes the inner.
//
// Assumptions (you must state these in your README/submission):
//  - Input IR is in a canonical/simple form (LoopSimplify-like): each loop has a preheader and a single latch.
//  - Loops are perfectly nested (outer contains exactly one subloop and inner has no further subloops).
//  - Induction variables are the canonical PHI nodes in the headers (integer phi).
//  - We don't perform dependence analysis / legality checks (assignment allows this).
//
// Limitations:
//  - This is not a fully-general or production-quality implementation. It clones inner-loop blocks
//    and rewires a few CFG edges. It does not fully update all analyses (DT, LoopInfo) in an incremental
//    manner — the pass lets the pass manager recompute analyses if needed afterwards.

#include "llvm/ADT/SmallVector.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace {

PHINode *findCanonicalInduction(Loop *L) {
    BasicBlock *Header = L->getHeader();
    if (!Header) return nullptr;
    for (Instruction &I : *Header) {
        PHINode *PN = dyn_cast<PHINode>(&I);
        if (!PN) break;
        if (!PN->getType()->isIntegerTy()) continue;
        if (PN->getNumIncomingValues() != 2) continue;
        BasicBlock *Pre = L->getLoopPreheader();
        BasicBlock *Latch = L->getLoopLatch();
        if (!Pre || !Latch) return nullptr;
        BasicBlock *A = PN->getIncomingBlock(0);
        BasicBlock *B = PN->getIncomingBlock(1);
        if ((A == Pre && B == Latch) || (B == Pre && A == Latch)) {
            return PN;
        }
    }
    return nullptr;
}

bool isSimpleCanonicalLoop(Loop *L) {
    return L->getLoopPreheader() && L->getLoopLatch();
}

bool interchangeLoops(Loop *Outer, Loop *Inner, LoopInfo &LI, DominatorTree &DT) {
    if (!Outer || !Inner) return false;
    if (!isSimpleCanonicalLoop(Outer) || !isSimpleCanonicalLoop(Inner))
        return false;

    auto subs = Outer->getSubLoops();
    if (subs.size() != 1 || subs[0] != Inner) return false;

    BasicBlock *outerHeader = Outer->getHeader();
    BasicBlock *outerPre = Outer->getLoopPreheader();
    BasicBlock *outerLatch = Outer->getLoopLatch();

    BasicBlock *innerHeader = Inner->getHeader();
    BasicBlock *innerPre = Inner->getLoopPreheader();
    BasicBlock *innerLatch = Inner->getLoopLatch();

    if (!outerHeader || !outerPre || !outerLatch || !innerHeader || !innerPre || !innerLatch)
        return false;

    PHINode *phiOuter = findCanonicalInduction(Outer);
    PHINode *phiInner = findCanonicalInduction(Inner);
    if (!phiOuter || !phiInner) {
        errs() << "LoopInterchange: couldn't find canonical phis, skipping\n";
        return false;
    }

    // collect inner blocks
    auto innerBlocks = Inner->getBlocks();

    ValueToValueMapTy VMap;
    Function *F = outerHeader->getParent();
    SmallVector<BasicBlock*,8> cloned;
    cloned.reserve(innerBlocks.size());

    for (BasicBlock *BB : innerBlocks) {
        BasicBlock *NB = CloneBasicBlock(BB, VMap, ".li.clone", F);
        VMap[BB] = NB;
        cloned.push_back(NB);
    }
    // remap instructions inside clones
    for (BasicBlock *NB : cloned)
        for (Instruction &I : *NB)
            RemapInstruction(&I, VMap, RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);

    // find clone of inner preheader/header/latch
    BasicBlock *clonedInnerHeader = cast_or_null<BasicBlock>(VMap[innerHeader]);
    BasicBlock *clonedInnerPre = cast_or_null<BasicBlock>(VMap[innerPre]);
    BasicBlock *clonedInnerLatch = cast_or_null<BasicBlock>(VMap[innerLatch]);

    // rewire: outer preheader -> innerPre  becomes -> clonedInnerPre
    Instruction *TP = outerPre->getTerminator();
    if (TP) {
        for (unsigned i = 0, e = TP->getNumSuccessors(); i != e; ++i) {
            if (TP->getSuccessor(i) == innerPre && clonedInnerPre)
                TP->setSuccessor(i, clonedInnerPre);
        }
    }

    // rewire cloned inner latch: where it used to branch to cloned inner header on loop back,
    // make it branch to the original outer header (so original outer executes as inner).
    if (clonedInnerLatch) {
        Instruction *T = clonedInnerLatch->getTerminator();
        for (unsigned i = 0, e = T->getNumSuccessors(); i != e; ++i) {
            if (T->getSuccessor(i) == VMap[innerHeader]) {
                T->setSuccessor(i, outerHeader);
            }
        }
    } else {
        errs() << "LoopInterchange: could not find cloned inner latch\n";
        return false;
    }

    // outer latch: change its loop-back successor from outerHeader to clonedInnerHeader
    Instruction *OuterLT = outerLatch->getTerminator();
    if (OuterLT && clonedInnerHeader) {
        for (unsigned i = 0, e = OuterLT->getNumSuccessors(); i != e; ++i) {
            if (OuterLT->getSuccessor(i) == outerHeader)
                OuterLT->setSuccessor(i, clonedInnerHeader);
        }
    }

    errs() << "LoopInterchange: applied clone+rewire interchange (best-effort).\n";
    return true;
}

struct LoopInterchangePass : PassInfoMixin<LoopInterchangePass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopAnalysis::Result &LA = AM.getResult<LoopAnalysis>(F);
        LoopInfo &LI = LA; // alias
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

        bool Changed = false;
        for (Loop *L : LI) {
            auto subs = L->getSubLoops();
            if (subs.size() != 1) continue;
            Loop *Inner = subs[0];
            if (!Inner->getSubLoops().empty()) continue;
            errs() << "LoopInterchange: candidate found in function " << F.getName() << "\n";
            if (interchangeLoops(L, Inner, LI, DT)) Changed = true;
        }

        if (Changed) return PreservedAnalyses::none();
        return PreservedAnalyses::all();
    }
};

} // end anonymous

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LoopInterchangePass", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "loop-interchange-pass") {
                        FPM.addPass(LoopInterchangePass());
                        return true;
                    }
                    return false;
                });
        }};
}
