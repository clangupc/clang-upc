//===--- CGStmtUPC.cpp - Emit LLVM Code from Statements -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This contains code to emit UPC Stmt nodes as LLVM code.
//
//===----------------------------------------------------------------------===//

#include "CodeGenModule.h"
#include "CodeGenFunction.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/SmallVector.h"
using namespace clang;
using namespace CodeGen;

static void getFileAndLine(CodeGenFunction &CGF, SourceLocation Loc,
                           llvm::SmallVectorImpl<llvm::Value*> *Out) {
  PresumedLoc PLoc = CGF.CGM.getContext().getSourceManager().getPresumedLoc(Loc);
  llvm::Constant *File =
    CGF.CGM.GetAddrOfConstantCString(PLoc.isValid()? PLoc.getFilename() : "(unknown)").getPointer();
  Out->push_back(CGF.Builder.CreateConstInBoundsGEP2_32(nullptr, File, 0, 0));
  Out->push_back(llvm::ConstantInt::get(CGF.IntTy, PLoc.isValid()? PLoc.getLine() : 0));
}

static void getFileAndLine(CodeGenFunction &CGF, SourceLocation Loc,
                           CallArgList *Out)
{
  ASTContext &Ctx = CGF.CGM.getContext();
  llvm::SmallVector<llvm::Value *, 2> Tmp;
  getFileAndLine(CGF, Loc, &Tmp);
  Out->add(RValue::get(Tmp[0]), Ctx.getPointerType(Ctx.getConstType(Ctx.CharTy)));
  Out->add(RValue::get(Tmp[1]), Ctx.IntTy);
}

static void EmitUPCBarrier(CodeGenFunction& CGF, StringRef Name,
                           const Expr *E, SourceLocation Loc) {
  ASTContext &Context = CGF.CGM.getContext();
  llvm::SmallString<16> N(Name);
  CallArgList Args;

  llvm::Value *Id = E ? CGF.EmitScalarExpr(E) :
    llvm::ConstantInt::get(CGF.IntTy, 0x80000000);
  Args.add(RValue::get(Id), Context.IntTy);

  if (CGF.CGM.getCodeGenOpts().UPCDebug) {
    getFileAndLine(CGF, Loc, &Args);
    N += 'g';
  }

  CGF.EmitUPCCall(N, Context.VoidTy, Args);
}

void CodeGenFunction::EmitUPCNotifyStmt(const UPCNotifyStmt &S) {
  EmitUPCBarrier(*this, "__upc_notify", S.getIdValue(), S.getNotifyLoc());
}

void CodeGenFunction::EmitUPCWaitStmt(const UPCWaitStmt &S) {
  EmitUPCBarrier(*this, "__upc_wait", S.getIdValue(), S.getWaitLoc());
}

void CodeGenFunction::EmitUPCBarrierStmt(const UPCBarrierStmt &S) {
  EmitUPCBarrier(*this, "__upc_barrier", S.getIdValue(), S.getBarrierLoc());
}

ConstantAddress CodeGenModule::getUPCFenceVar() {
  CharUnits Align = getContext().getTypeAlignInChars(getContext().IntTy);
  if (!UPCFenceVar) {
    llvm::GlobalVariable * GV =
      new llvm::GlobalVariable(getModule(), IntTy, false,
                               llvm::GlobalValue::LinkOnceODRLinkage,
                               llvm::ConstantInt::get(IntTy, 0),
                               "__upc_fence_var");

    if(isTargetDarwin())
      GV->setSection("__DATA,upc_shared");
    else
      GV->setSection("upc_shared");
    GV->setAlignment(Align.getQuantity());
    UPCFenceVar = GV;
  }
  return ConstantAddress(UPCFenceVar, Align);
}

void CodeGenFunction::EmitUPCFenceStmt(const UPCFenceStmt &S) {
  Address FencePtr = CGM.getUPCFenceVar();
  FencePtr = EmitSharedVarDeclLValue(FencePtr, getContext().IntTy).getAddress();
  llvm::Value *Val = EmitUPCLoad(FencePtr, /*strict*/true, getContext().IntTy, S.getFenceLoc());
  EmitUPCStore(Val, FencePtr, /*strict*/true, getContext().IntTy, S.getFenceLoc());
}

ConstantAddress getUPCForAllDepth(CodeGenModule& CGM) {
  CharUnits Align = CGM.getContext().getTypeAlignInChars(CGM.getContext().IntTy);
  return ConstantAddress(CGM.getModule().getOrInsertGlobal("__upc_forall_depth", CGM.IntTy), Align);
}

namespace {

  struct UPCForAllCleanup : EHScopeStack::Cleanup {
    explicit UPCForAllCleanup(llvm::Value *D) : Depth(D) {  }
    virtual void Emit(CodeGenFunction &CGF, Flags flags) {
      CGF.Builder.CreateStore(Depth, getUPCForAllDepth(CGF.CGM));
    }
    llvm::Value *Depth;
  };

}

void CodeGenFunction::EmitUPCForAllStmt(const UPCForAllStmt &S) {
  JumpDest LoopExit = getJumpDestInCurrentScope("upc_forall.end");

  RunCleanupsScope ForScope(*this);

  llvm::Value *Depth = 0;
  if (S.getAfnty()) {
    Address DepthAddr = getUPCForAllDepth(CGM);
    Depth = Builder.CreateLoad(DepthAddr);
    Builder.CreateStore(Builder.CreateNUWAdd(Depth,
                                             llvm::ConstantInt::get(IntTy, 1),
                                             "upc_forall.inc_depth"),
                        DepthAddr);
    EHStack.pushCleanup<UPCForAllCleanup>(NormalAndEHCleanup, Depth);
  }

  CGDebugInfo *DI = getDebugInfo();
  if (DI)
    DI->EmitLexicalBlockStart(Builder, S.getSourceRange().getBegin());

  // Evaluate the first part before the loop.
  if (S.getInit())
    EmitStmt(S.getInit());

  // Start the loop with a block that tests the condition.
  // If there's an increment, the continue scope will be overwritten
  // later.
  JumpDest Continue = getJumpDestInCurrentScope("upc_forall.cond");
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);

  // Create a cleanup scope for the condition variable cleanups.
  RunCleanupsScope ConditionScope(*this);
  
  llvm::Value *BoolCondVal = 0;
  if (S.getCond()) {
    // If the for statement has a condition scope, emit the local variable
    // declaration.
    llvm::BasicBlock *ExitBlock = LoopExit.getBlock();
    if (S.getConditionVariable()) {
      EmitAutoVarDecl(*S.getConditionVariable());
    }

    // If there are any cleanups between here and the loop-exit scope,
    // create a block to stage a loop exit along.
    if (ForScope.requiresCleanups())
      ExitBlock = createBasicBlock("upcforall.cond.cleanup");
    
    // As long as the condition is true, iterate the loop.
    llvm::BasicBlock *ForBody = createBasicBlock("upc_forall.filter");

    // C99 6.8.5p2/p4: The first substatement is executed if the expression
    // compares unequal to 0.  The condition must be a scalar type.
    BoolCondVal = EvaluateExprAsBool(S.getCond());
    Builder.CreateCondBr(BoolCondVal, ForBody, ExitBlock);

    if (ExitBlock != LoopExit.getBlock()) {
      EmitBlock(ExitBlock);
      EmitBranchThroughCleanup(LoopExit);
    }

    EmitBlock(ForBody);
  } else {
    // Treat it as a non-zero constant.  Don't even create a new block for the
    // body, just fall into it.
  }

  // If the for loop doesn't have an increment we can just use the
  // condition as the continue block.  Otherwise we'll need to create
  // a block for it (in the current scope, i.e. in the scope of the
  // condition), and that we will become our continue block.
  if (S.getInc())
    Continue = getJumpDestInCurrentScope("upc_forall.inc");

  // Store the blocks to use for break and continue.
  BreakContinueStack.push_back(BreakContinue(LoopExit, Continue));

  if (const Expr *Afnty = S.getAfnty()) {
    llvm::Value *Affinity = EmitScalarExpr(Afnty);
    if (Afnty->getType()->hasPointerToSharedRepresentation()) {
      // get threadof
      Affinity = EmitUPCPointerGetThread(Affinity);
    } else {
      assert(Affinity->getType()->isIntegerTy());
      llvm::Value *Threads = EmitUPCThreads();
      if (cast<llvm::IntegerType>(Threads->getType())->getBitWidth() <
          cast<llvm::IntegerType>(Affinity->getType())->getBitWidth()) {
        Threads = Builder.CreateIntCast(Threads, Affinity->getType(), false);
      } else {
        Affinity = Builder.CreateIntCast(Affinity, Threads->getType(),
                                         Afnty->getType()->hasSignedIntegerRepresentation());
      }
      if (Afnty->getType()->hasSignedIntegerRepresentation()) {
        Affinity = Builder.CreateSRem(Affinity, Threads);
        llvm::Value *Zero = llvm::ConstantInt::get(Affinity->getType(), 0);
        Affinity = Builder.CreateSelect(Builder.CreateICmpSLT(Affinity, Zero),
                                        Builder.CreateAdd(Affinity, Threads),
                                        Affinity);
      } else {
        Affinity = Builder.CreateURem(Affinity, Threads);
      }
    }
    Affinity = Builder.CreateIntCast(Affinity, IntTy, false);

    llvm::Value *MyThread = EmitUPCMyThread();

    llvm::Value *Test =
      Builder.CreateOr(Builder.CreateICmpUGT(Depth, llvm::ConstantInt::get(IntTy, 0)),
                        Builder.CreateICmpEQ(Affinity, MyThread));

    llvm::BasicBlock *RealBody = createBasicBlock("upc_forall.body");
    Builder.CreateCondBr(Test, RealBody, Continue.getBlock());
    EmitBlock(RealBody);
  }

  {
    // Create a separate cleanup scope for the body, in case it is not
    // a compound statement.
    RunCleanupsScope BodyScope(*this);
    EmitStmt(S.getBody());
  }

  // If there is an increment, emit it next.
  if (S.getInc()) {
    EmitBlock(Continue.getBlock());
    EmitStmt(S.getInc());
  }

  BreakContinueStack.pop_back();

  ConditionScope.ForceCleanup();
  EmitBranch(CondBlock);

  ForScope.ForceCleanup();

  if (DI)
    DI->EmitLexicalBlockEnd(Builder, S.getSourceRange().getEnd());

  // Emit the fall-through block.
  EmitBlock(LoopExit.getBlock(), true);
}
