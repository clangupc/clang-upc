//===--- CGExprUPC.cpp - Emit LLVM Code for UPC expressions ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This contains code dealing with code generation of UPC expressions
//
//===----------------------------------------------------------------------===//

#include "CodeGenFunction.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/ADT/SmallVector.h"
#include "clang/Config/config.h" // for UPC_IR_RP_THREAD/ADDR
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

RValue CodeGenFunction::EmitUPCCall(
                   llvm::StringRef Name,
                   QualType ResultTy,
                   const CallArgList& Args) {
  ASTContext &Context = CGM.getContext();
  llvm::SmallVector<QualType, 5> ArgTypes;

  for (CallArgList::const_iterator iter = Args.begin(),
         end = Args.end(); iter != end; ++iter) {
    ArgTypes.push_back(iter->Ty);
  }

  QualType FuncType =
    Context.getFunctionType(ResultTy,
                            ArgTypes,
                            FunctionProtoType::ExtProtoInfo());
    const CGFunctionInfo &Info =
      getTypes().arrangeFreeFunctionCall(Args, FuncType->castAs<FunctionType>(), false);
    llvm::FunctionType * FTy =
      cast<llvm::FunctionType>(ConvertType(FuncType));
    llvm::Constant * Fn = CGM.CreateRuntimeFunction(FTy, Name);

    return EmitCall(Info, CGCallee::forDirect(Fn), ReturnValueSlot(), Args);
}

llvm::Value *CodeGenFunction::EmitUPCCastSharedToLocal(llvm::Value *Value,
                                                       QualType DestTy,
                                                       SourceLocation Loc) {
  const ASTContext& Context = getContext();
  QualType ArgTy = Context.getPointerType(Context.getSharedType(Context.VoidTy));
  QualType ResultTy = Context.VoidPtrTy;

  const char *FnName = "__getaddr";

  CallArgList Args;
  Args.add(RValue::get(Value), ArgTy);
  if (CGM.getCodeGenOpts().UPCDebug) {
    getFileAndLine(*this, Loc, &Args);
    FnName = "__getaddrg";
  }

  RValue Result = EmitUPCCall(FnName, ResultTy, Args);

  return Builder.CreateBitCast(Result.getScalarVal(), ConvertType(DestTy));
}

llvm::Value *CodeGenFunction::EmitUPCBitCastZeroPhase(llvm::Value *Value, QualType DestTy) {
  return EmitUPCPointer(llvm::ConstantInt::get(SizeTy, 0),
                        EmitUPCPointerGetThread(Value),
                        EmitUPCPointerGetAddr(Value));
}

llvm::Value *CodeGenFunction::EmitUPCPointerToBoolConversion(llvm::Value *Pointer) {
  llvm::Value *Phase = EmitUPCPointerGetPhase(Pointer);
  llvm::Value *Thread = EmitUPCPointerGetThread(Pointer);
  llvm::Value *Addr = EmitUPCPointerGetAddr(Pointer);
  return Builder.CreateOr(Builder.CreateOr(Builder.CreateIsNotNull(Phase),
                                           Builder.CreateIsNotNull(Thread)),
                          Builder.CreateIsNotNull(Addr));
}

llvm::Value *CodeGenFunction::EmitUPCPointerToInt(llvm::Value *Pointer, llvm::Type *DestTy) {
  llvm::Type *SrcTy = Pointer->getType();
  llvm::Type *TmpTy = SrcTy;
  if(CGM.getDataLayout().getTypeSizeInBits(SrcTy) < CGM.getDataLayout().getTypeSizeInBits(DestTy))
    TmpTy = DestTy;
  Address Tmp = CreateDefaultAlignTempAlloca(TmpTy);
  InitTempAlloca(Tmp, llvm::Constant::getNullValue(TmpTy));
  Builder.CreateStore(Pointer, Builder.CreateBitCast(Tmp, SrcTy->getPointerTo()));
  return Builder.CreateLoad(Builder.CreateBitCast(Tmp, DestTy->getPointerTo()));
}

llvm::Value *CodeGenFunction::EmitUPCNullPointer(QualType DestTy) {
  return EmitUPCPointer(llvm::ConstantInt::get(SizeTy, 0),
                        llvm::ConstantInt::get(SizeTy, 0),
                        llvm::ConstantInt::get(SizeTy, 0));
}

static const char * getUPCTypeID(CodeGenFunction& CGF,
                                 QualType *AccessTy,
                                 llvm::Type *Ty,
                                 uint64_t Size,
                                 uint64_t Align) {
  ASTContext &Context = CGF.getContext();
  unsigned UnitWidth = Context.getCharWidth();
  const char *Result;

  if (!Ty->isSingleValueType())
    return 0;

  if(Ty->isFloatTy()) {
    *AccessTy = Context.FloatTy;
    Result = "sf";
  } else if(Ty->isDoubleTy()) {
    *AccessTy = Context.DoubleTy;
    Result = "df";
  } else if(Ty->isX86_FP80Ty() || Ty->isFP128Ty() || Ty->isPPC_FP128Ty()) {
    *AccessTy = Context.LongDoubleTy;
    Result = "tf";
  } else if (Size % UnitWidth == 0 && Size / UnitWidth <= 16) {
    if (Size == Context.getTypeSize(Context.UnsignedCharTy)) {
      *AccessTy = Context.UnsignedCharTy;
    } else if (Size == Context.getTypeSize(Context.UnsignedShortTy)) {
      *AccessTy = Context.UnsignedShortTy;
    } else if (Size == Context.getTypeSize(Context.UnsignedIntTy)) {
      *AccessTy = Context.UnsignedIntTy;
    } else if (Size == Context.getTypeSize(Context.UnsignedLongTy)) {
      *AccessTy = Context.UnsignedLongTy;
    } else if (Size == Context.getTypeSize(Context.UnsignedLongLongTy)) {
      *AccessTy = Context.UnsignedLongLongTy;
    } else if (Size == Context.getTypeSize(Context.UnsignedInt128Ty)) {
      *AccessTy = Context.UnsignedInt128Ty;
    } else {
      return 0;
    }
    switch (Size / UnitWidth) {
    case 1: Result = "qi"; break;
    case 2: Result = "hi"; break;
    case 4: Result = "si"; break;
    case 8: Result = "di"; break;
    case 16: Result = "ti"; break;
    default: return 0;
    }
  } else {
    return 0;
  }

  if (Align % Context.getTypeAlign(*AccessTy) == 0)
    return Result;
  else
    return 0;
}

llvm::Value *CodeGenFunction::EmitUPCLoad(Address Addr,
                                          bool isStrict, QualType Ty,
                                          SourceLocation Loc) {
  llvm::Type *DestLTy = ConvertTypeForMem(Ty);
  return EmitFromMemory(EmitUPCLoad(Addr, isStrict, DestLTy, Loc), Ty);
}

static const int UPCAddrSpace = UPC_IR_RP_ADDRSPACE;

static llvm::Value *ConvertPTStoLLVMPtr(CodeGenFunction& CGF,
                                        llvm::Value *Ptr, llvm::Type *LTy) {
  llvm::Value *Addr = CGF.EmitUPCPointerGetAddr(Ptr);
  llvm::Value *Thread = CGF.EmitUPCPointerGetThread(Ptr);

  const LangOptions& LangOpts = CGF.getContext().getLangOpts();
  if(!LangOpts.UPCPtsRep) {
    llvm::Value *SectionStart = CGF.CGM.getModule().getOrInsertGlobal("__upc_shared_start", CGF.Int8Ty);
    llvm::Value *StartInt = CGF.Builder.CreatePtrToInt(SectionStart, CGF.PtrDiffTy, "sect.cast");
    Addr = CGF.Builder.CreateSub(Addr, StartInt, "addr.sub");
  }

  llvm::Value *IntVal = CGF.Builder.CreateOr(Thread, CGF.Builder.CreateShl(Addr, UPC_IR_RP_THREAD));
  
  llvm::Type *LLPtsTy = LTy->getPointerTo(UPCAddrSpace);
  return CGF.Builder.CreateIntToPtr(IntVal, LLPtsTy);
}

llvm::Value *CodeGenFunction::EmitUPCLoad(Address A,
                                          bool isStrict,
                                          llvm::Type *LTy,
                                          SourceLocation Loc) {
  const LangOptions& Opts = getContext().getLangOpts();
  llvm::Value * Addr = A.getPointer();
  CharUnits Align = A.getAlignment();
  if (Opts.UPCGenIr) {
    llvm::Value *InternalAddr = ConvertPTStoLLVMPtr(*this, Addr, LTy);
    llvm::LoadInst * Result = Builder.CreateLoad(Address(InternalAddr, Align));
    if(isStrict) {
      Result->setOrdering(llvm::AtomicOrdering::SequentiallyConsistent);
    }
    return Result;
  }
  const ASTContext& Context = getContext();
  const llvm::DataLayout &Target = CGM.getDataLayout();
  uint64_t Size = Target.getTypeSizeInBits(LTy);
  QualType ArgTy = Context.getPointerType(Context.getSharedType(Context.VoidTy));
  QualType ResultTy;
  llvm::SmallString<16> Name("__get");
  if (isStrict) Name += 's';
  if (CGM.getCodeGenOpts().UPCDebug) Name += "g";

  if (const char * ID = getUPCTypeID(*this, &ResultTy, LTy, Size, Context.toBits(Align))) {
    Name += ID;

    CallArgList Args;
    Args.add(RValue::get(Addr), ArgTy);
    if (CGM.getCodeGenOpts().UPCDebug) {
      getFileAndLine(*this, Loc, &Args);
      Name += '3';
    } else {
      Name += '2';
    }
    RValue Result = EmitUPCCall(Name, ResultTy, Args);
    llvm::Value *Value = Result.getScalarVal();
    if (LTy->isPointerTy())
      Value = Builder.CreateIntToPtr(Value, LTy);
    else
      Value = Builder.CreateBitCast(Value, LTy);
    return Value;
  } else {
    Name += "blk";

    Address Mem = CreateDefaultAlignTempAlloca(LTy);
    Address Tmp = Builder.CreateBitCast(Mem, VoidPtrTy);
    llvm::Value *SizeArg = llvm::ConstantInt::get(SizeTy, Size/Context.getCharWidth());
    
    CallArgList Args;
    Args.add(RValue::get(Tmp.getPointer()), Context.VoidPtrTy);
    Args.add(RValue::get(Addr), ArgTy);
    Args.add(RValue::get(SizeArg), Context.getSizeType());
    if (CGM.getCodeGenOpts().UPCDebug) {
      getFileAndLine(*this, Loc, &Args);
      Name += '5';
    } else {
      Name += '3';
    }
    EmitUPCCall(Name, getContext().VoidTy, Args);

    return Builder.CreateLoad(Mem);
  }
}

void CodeGenFunction::EmitUPCStore(llvm::Value *Value,
                                   Address Addr,
                                   bool isStrict,
                                   QualType Ty,
                                   SourceLocation Loc) {
  return EmitUPCStore(EmitToMemory(Value, Ty), Addr.getPointer(),
                      isStrict, Addr.getAlignment(), Loc);
}

void CodeGenFunction::EmitUPCStore(llvm::Value *Value,
                                   llvm::Value *Addr,
                                   bool isStrict,
                                   QualType Ty,
                                   CharUnits Align,
                                   SourceLocation Loc) {
  return EmitUPCStore(EmitToMemory(Value, Ty), Addr,
                      isStrict, Align, Loc);
}

void CodeGenFunction::EmitUPCStore(llvm::Value *Value,
                                   Address Addr,
                                   bool isStrict,
                                   SourceLocation Loc) {
  return EmitUPCStore(Value, Addr.getPointer(),
                      isStrict, Addr.getAlignment(), Loc);
}

void CodeGenFunction::EmitUPCStore(llvm::Value *Value,
                                   llvm::Value *Addr,
                                   bool isStrict,
                                   CharUnits Align,
                                   SourceLocation Loc) {
  const LangOptions& Opts = getContext().getLangOpts();
  if (Opts.UPCGenIr) {
    llvm::Value *InternalAddr = ConvertPTStoLLVMPtr(*this, Addr, Value->getType());
    llvm::StoreInst * Result = Builder.CreateStore(Value, Address(InternalAddr, Align));
    if(isStrict) {
      Result->setOrdering(llvm::AtomicOrdering::SequentiallyConsistent);
    }
    return;
  }
  const ASTContext& Context = getContext();
  const llvm::DataLayout &Target = CGM.getDataLayout();
  uint64_t Size = Target.getTypeSizeInBits(Value->getType());
  QualType AddrTy = Context.getPointerType(Context.getSharedType(Context.VoidTy));
  QualType ValTy;
  llvm::SmallString<16> Name("__put");
  if (isStrict) Name += 's';
  if (CGM.getCodeGenOpts().UPCDebug) Name += "g";

  if (const char * ID = getUPCTypeID(*this, &ValTy, Value->getType(), Size, Context.toBits(Align))) {
    Name += ID;

    llvm::Type *ValLTy = ConvertTypeForMem(ValTy);

    if (Value->getType()->isPointerTy())
      Value = Builder.CreatePtrToInt(Value, ValLTy);
    else
      Value = Builder.CreateBitCast(Value, ValLTy);

    CallArgList Args;
    Args.add(RValue::get(Addr), AddrTy);
    Args.add(RValue::get(Value), ValTy);

    if (CGM.getCodeGenOpts().UPCDebug) {
      getFileAndLine(*this, Loc, &Args);
      Name += '4';
    } else {
      Name += '2';
    }

    EmitUPCCall(Name, Context.VoidTy, Args);
  } else {
    Name += "blk";

    Address Mem = CreateDefaultAlignTempAlloca(Value->getType());
    Builder.CreateStore(Value, Mem);
    Address Tmp = Builder.CreateBitCast(Mem, VoidPtrTy);
    llvm::Value *SizeArg = llvm::ConstantInt::get(SizeTy, Size/Context.getCharWidth());
    
    CallArgList Args;
    Args.add(RValue::get(Addr), AddrTy);
    Args.add(RValue::get(Tmp.getPointer()), Context.VoidPtrTy);
    Args.add(RValue::get(SizeArg), Context.getSizeType());
    if (CGM.getCodeGenOpts().UPCDebug) {
      getFileAndLine(*this, Loc, &Args);
      Name += '5';
    } else {
      Name += '3';
    }
    EmitUPCCall(Name, getContext().VoidTy, Args);
  }
}

void CodeGenFunction::EmitUPCAggregateCopy(llvm::Value *Dest, llvm::Value *Src,
                                           QualType DestTy, QualType SrcTy,
                                           SourceLocation Loc) {
  const ASTContext& Context = getContext();
  QualType ArgTy = Context.getPointerType(Context.getSharedType(Context.VoidTy));
  QualType SizeType = Context.getSizeType();
  assert(DestTy->getCanonicalTypeUnqualified() == SrcTy->getCanonicalTypeUnqualified());
  llvm::Constant *Len =
    llvm::ConstantInt::get(ConvertType(SizeType),
                           Context.getTypeSizeInChars(DestTy).getQuantity());
  llvm::SmallString<16> Name;
  const char *OpName;
  QualType DestArgTy, SrcArgTy;
  if (DestTy.getQualifiers().hasShared() && SrcTy.getQualifiers().hasShared()) {
    // both shared
    OpName = "copy";
    DestArgTy = SrcArgTy = ArgTy;
  } else if (DestTy.getQualifiers().hasShared()) {
    OpName = "put";
    DestArgTy = ArgTy;
    SrcArgTy = Context.VoidPtrTy;
  } else if (SrcTy.getQualifiers().hasShared()) {
    OpName = "get";
    DestArgTy = Context.VoidPtrTy;
    SrcArgTy = ArgTy;
  } else {
    llvm_unreachable("expected at least one shared argument");
  }

  Name += "__";
  Name += OpName;
  if (DestTy.getQualifiers().hasStrict() || SrcTy.getQualifiers().hasStrict())
    Name += 's';
  if (CGM.getCodeGenOpts().UPCDebug) Name += "g";
  Name += "blk";
  CallArgList Args;
  Args.add(RValue::get(Dest), DestArgTy);
  Args.add(RValue::get(Src), SrcArgTy);
  Args.add(RValue::get(Len), SizeType);
  if (CGM.getCodeGenOpts().UPCDebug) {
    getFileAndLine(*this, Loc, &Args);
    Name += '5';
  } else {
    Name += '3';
  }
  EmitUPCCall(Name, Context.VoidTy, Args);
}

llvm::Value *CodeGenFunction::EmitUPCAtomicCmpXchg(llvm::Value *Addr,
                                                   llvm::PHINode *AtomicPhi,
                                                   llvm::Value *Value,
                                                   SourceLocation Loc) {
  CGM.Error(Loc, "cannot compile this atomic expression yet");
  return Value;
}

llvm::Value *CodeGenFunction::EmitUPCPointerGetPhase(llvm::Value *Pointer) {
  const LangOptions& LangOpts = getContext().getLangOpts();
  unsigned PhaseBits = LangOpts.UPCPhaseBits;
  unsigned ThreadBits = LangOpts.UPCThreadBits;
  unsigned AddrBits = LangOpts.UPCAddrBits;
  llvm::Value *Result;
  if (LangOpts.UPCPtsRep) {
    llvm::Value *Val = Builder.CreateExtractValue(Pointer, 0);
    if (LangOpts.UPCVaddrFirst) {
      Result = Builder.CreateAnd(Val, llvm::APInt::getLowBitsSet(64, PhaseBits));
    } else {
      Result = Builder.CreateLShr(Val, ThreadBits + AddrBits);
    }
  } else {
    if (LangOpts.UPCVaddrFirst) {
      Result = Builder.CreateExtractValue(Pointer, 2);
    } else {
      Result = Builder.CreateExtractValue(Pointer, 0);
    }
  }
  return Builder.CreateZExtOrTrunc(Result, SizeTy);
}

llvm::Value *CodeGenFunction::EmitUPCPointerGetThread(llvm::Value *Pointer) {
  const LangOptions& LangOpts = getContext().getLangOpts();
  unsigned PhaseBits = LangOpts.UPCPhaseBits;
  unsigned ThreadBits = LangOpts.UPCThreadBits;
  unsigned AddrBits = LangOpts.UPCAddrBits;
  llvm::Value *Result;
  if (LangOpts.UPCPtsRep) {
    llvm::Value *Val = Builder.CreateExtractValue(Pointer, 0);
    if (LangOpts.UPCVaddrFirst) {
      Val = Builder.CreateLShr(Val, PhaseBits);
    } else {
      Val = Builder.CreateLShr(Val, AddrBits);
    }
    Result = Builder.CreateAnd(Val, llvm::APInt::getLowBitsSet(64, ThreadBits));
  } else {
    Result = Builder.CreateExtractValue(Pointer, 1);
  }
  return Builder.CreateZExtOrTrunc(Result, SizeTy);
}

llvm::Value *CodeGenFunction::EmitUPCPointerGetAddr(llvm::Value *Pointer) {
  const LangOptions& LangOpts = getContext().getLangOpts();
  unsigned PhaseBits = LangOpts.UPCPhaseBits;
  unsigned ThreadBits = LangOpts.UPCThreadBits;
  unsigned AddrBits = LangOpts.UPCAddrBits;
  llvm::Value *Result;
  if (LangOpts.UPCPtsRep) {
    llvm::Value *Val = Builder.CreateExtractValue(Pointer, 0);
    if (LangOpts.UPCVaddrFirst) {
      Result = Builder.CreateLShr(Val, ThreadBits + PhaseBits);
    } else {
      Result = Builder.CreateAnd(Val, llvm::APInt::getLowBitsSet(64, AddrBits));
    }
  } else {
    if (LangOpts.UPCVaddrFirst) {
      Result = Builder.CreateExtractValue(Pointer, 0);
    } else {
      Result = Builder.CreateExtractValue(Pointer, 2);
    }
  }
  return Builder.CreateZExtOrTrunc(Result, SizeTy);
}

llvm::Value *CodeGenFunction::EmitUPCPointer(llvm::Value *Phase, llvm::Value *Thread, llvm::Value *Addr) {
  const LangOptions& LangOpts = getContext().getLangOpts();
  unsigned PhaseBits = LangOpts.UPCPhaseBits;
  unsigned ThreadBits = LangOpts.UPCThreadBits;
  unsigned AddrBits = LangOpts.UPCAddrBits;
  llvm::Value *Result = llvm::UndefValue::get(GenericPtsTy);
  if (LangOpts.UPCPtsRep) {
    // The arguments are size_t.  Convert them
    // to the correct size.
    Phase = Builder.CreateZExtOrTrunc(Phase, Int64Ty);
    Thread = Builder.CreateZExtOrTrunc(Thread, Int64Ty);
    Addr = Builder.CreateZExtOrTrunc(Addr, Int64Ty);
    llvm::Value *Val;
    if (LangOpts.UPCVaddrFirst) {
      Val = Builder.CreateOr(Builder.CreateShl(Addr, ThreadBits + PhaseBits),
                             Builder.CreateOr(Builder.CreateShl(Thread, PhaseBits),
                                              Phase));
    } else {
      Val = Builder.CreateOr(Builder.CreateShl(Phase, ThreadBits + AddrBits),
                             Builder.CreateOr(Builder.CreateShl(Thread, AddrBits),
                                              Addr));
    }
    Result = Builder.CreateInsertValue(Result, Val, 0);
  } else {
    if (getContext().getTargetInfo().getPointerWidth(0) == 64) {
      Phase = Builder.CreateZExtOrTrunc(Phase, Int32Ty);
      Thread = Builder.CreateZExtOrTrunc(Thread, Int32Ty);
    } else {
      Phase = Builder.CreateZExtOrTrunc(Phase, Int16Ty);
      Thread = Builder.CreateZExtOrTrunc(Thread, Int16Ty);
    }
    if (LangOpts.UPCVaddrFirst) {
      Result = Builder.CreateInsertValue(Result, Addr, 0);
      Result = Builder.CreateInsertValue(Result, Thread, 1);
      Result = Builder.CreateInsertValue(Result, Phase, 2);
    } else {
      Result = Builder.CreateInsertValue(Result, Phase, 0);
      Result = Builder.CreateInsertValue(Result, Thread, 1);
      Result = Builder.CreateInsertValue(Result, Addr, 2);
    }
  }
  return Result;
}

ConstantAddress CodeGenModule::getUPCThreads() {
  CharUnits Align = getContext().getTypeAlignInChars(getContext().IntTy);
  if (!UPCThreads) {
    if (getModule().getNamedGlobal("THREADS"))
      UPCThreads = getModule().getOrInsertGlobal("THREADS", IntTy);
    else
      UPCThreads = new llvm::GlobalVariable(getModule(), IntTy, true,
                                            llvm::GlobalValue::ExternalLinkage, 0,
                                            "THREADS");
  }
  return ConstantAddress(UPCThreads, Align);
}

ConstantAddress CodeGenModule::getUPCMyThread() {
  CharUnits Align = getContext().getTypeAlignInChars(getContext().IntTy);
  if (!UPCMyThread) {
    if (getModule().getNamedGlobal("MYTHREAD"))
      UPCMyThread = getModule().getOrInsertGlobal("MYTHREAD", IntTy);
    else
      UPCMyThread = new llvm::GlobalVariable(getModule(), IntTy, true,
                                             llvm::GlobalValue::ExternalLinkage, 0,
                                             "MYTHREAD");
  }
  return ConstantAddress(UPCMyThread, Align);
}

llvm::Value *CodeGenFunction::EmitUPCThreads() {
  if (uint32_t Threads = getContext().getLangOpts().UPCThreads) {
    return llvm::ConstantInt::get(IntTy, Threads);
  } else {
    return Builder.CreateLoad(CGM.getUPCThreads());
  }
}

llvm::Value *CodeGenFunction::EmitUPCMyThread() {
  return Builder.CreateLoad(CGM.getUPCMyThread());
}


llvm::Value *CodeGenFunction::EmitUPCPointerArithmetic(
    llvm::Value *Pointer, llvm::Value *Index, QualType PtrTy, const Expr *E, bool IsSubtraction) {
  
  const BinaryOperator *expr = cast<BinaryOperator>(E);
  Expr *pointerOperand = expr->getLHS();
  Expr *indexOperand = expr->getRHS();

  if (!IsSubtraction && !Index->getType()->isIntegerTy()) {
    std::swap(Pointer, Index);
    std::swap(pointerOperand, indexOperand);
  }
  return EmitUPCPointerArithmetic(Pointer, Index, PtrTy, indexOperand->getType(), IsSubtraction);
}

static std::pair<QualType, llvm::Value *> unwrapArray(CodeGenFunction& CGF, QualType Ty) {
  llvm::Value *Dim = 0;
  
  while (const ArrayType *AT = CGF.getContext().getAsArrayType(Ty)) {
    Ty = AT->getElementType();
    llvm::Value *ArrayDim;
    if (const ConstantArrayType *CAT = dyn_cast<ConstantArrayType>(AT)) {
      ArrayDim = llvm::ConstantInt::get(CGF.PtrDiffTy, CAT->getSize());
    } else if (const UPCThreadArrayType *TAT = dyn_cast<UPCThreadArrayType>(AT)) {
      ArrayDim = llvm::ConstantInt::get(CGF.PtrDiffTy, TAT->getSize());
      if (TAT->getThread())
        ArrayDim =
          CGF.Builder.CreateNUWMul(ArrayDim,
                                   CGF.Builder.CreateZExt(CGF.EmitUPCThreads(),
                                                          CGF.PtrDiffTy));
    } else {
      llvm_unreachable("illegal array type for pointer-to-shared arithmetic");
    }
    
    if (Dim)
      Dim = CGF.Builder.CreateNUWMul(Dim, ArrayDim, "mul.dim");
    else
      Dim = ArrayDim;
  }

  return std::make_pair(Ty, Dim);
}

llvm::Value *CodeGenFunction::EmitUPCPointerArithmetic(
    llvm::Value *Pointer, llvm::Value *Index, QualType PtrTy, QualType IndexTy, bool IsSubtraction) {

  llvm::Value *Phase = EmitUPCPointerGetPhase(Pointer);
  llvm::Value *Thread = EmitUPCPointerGetThread(Pointer);
  llvm::Value *Addr = EmitUPCPointerGetAddr(Pointer);

  bool isSigned = IndexTy->isSignedIntegerOrEnumerationType();

  unsigned width = cast<llvm::IntegerType>(Index->getType())->getBitWidth();
  if (width != PointerWidthInBits) {
    // Zero-extend or sign-extend the pointer value according to
    // whether the index is signed or not.
    Index = Builder.CreateIntCast(Index, PtrDiffTy, isSigned,
                                      "idx.ext");
  }

  QualType PointeeTy = PtrTy->getAs<PointerType>()->getPointeeType();
  QualType ElemTy;
  llvm::Value *Dim;
  std::tie(ElemTy, Dim) = unwrapArray(*this, PointeeTy);
  if (Dim) {
    Index = Builder.CreateMul(Index, Dim, "idx.dim", !isSigned, isSigned);
  }
  Qualifiers Quals = ElemTy.getQualifiers();

  if (IsSubtraction)
    Index = Builder.CreateNeg(Index);

  if (Quals.getLayoutQualifier() == 0) {
    // UPC 1.2 6.4.2p2
    // If the shared array is declared with indefinite block size,
    // the result of the pointer-to-shared arithmetic is identical
    // to that described for normal C pointers in [IOS/IEC00 Sec 6.5.2]
    // except that the thread of the new pointer shall be the
    // same as that of the original pointer and the phase
    // component is defined to always be zero.

    uint64_t ElemSize = getContext().getTypeSizeInChars(ElemTy).getQuantity();
    llvm::Value *ByteIndex = Builder.CreateMul(Index, llvm::ConstantInt::get(SizeTy, ElemSize));
    Addr = Builder.CreateAdd(Addr, ByteIndex, "add.addr");
  } else {
    llvm::Value *OldPhase = Phase;
    llvm::Constant *B = llvm::ConstantInt::get(SizeTy, Quals.getLayoutQualifier());
    llvm::Value *Threads = Builder.CreateZExt(EmitUPCThreads(), SizeTy);
    llvm::Value *GlobalBlockSize = Builder.CreateNUWMul(Threads, B);
    // Combine the Phase and Thread into a single unit
    llvm::Value *TmpPhaseThread =
      Builder.CreateNUWAdd(Builder.CreateNUWMul(Thread, B),
                           Phase);

    TmpPhaseThread = Builder.CreateAdd(TmpPhaseThread, Index);

    // Div is the number of (B * THREADS) blocks that we need to jump
    // Rem is Thread * B + Phase
    llvm::Value *Div = Builder.CreateSDiv(TmpPhaseThread, GlobalBlockSize);
    llvm::Value *Rem = Builder.CreateSRem(TmpPhaseThread, GlobalBlockSize);
    // Fix the result of the division/modulus
    llvm::Value *Test = Builder.CreateICmpSLT(Rem, llvm::ConstantInt::get(SizeTy, 0));
    Rem = Builder.CreateSelect(Test, Builder.CreateAdd(Rem, GlobalBlockSize), Rem);
    llvm::Value *DecDiv = Builder.CreateSub(Div, llvm::ConstantInt::get(SizeTy, 1));
    Div = Builder.CreateSelect(Test, DecDiv, Div);

    // Split out the Phase and Thread components
    Thread = Builder.CreateUDiv(Rem, B);
    Phase = Builder.CreateURem(Rem, B);

    uint64_t ElemSize = getContext().getTypeSizeInChars(ElemTy).getQuantity();
    // Compute the final Addr.
    llvm::Value *AddrInc =
      Builder.CreateMul(Builder.CreateAdd(Builder.CreateSub(Phase, OldPhase),
                                          Builder.CreateMul(Div, B)),
                        llvm::ConstantInt::get(SizeTy, ElemSize));
    Addr = Builder.CreateAdd(Addr, AddrInc);
  }

  return EmitUPCPointer(Phase, Thread, Addr);
}

llvm::Value *CodeGenFunction::EmitUPCPointerDiff(
    llvm::Value *Pointer1, llvm::Value *Pointer2, const Expr *E) {

  const BinaryOperator *expr = cast<BinaryOperator>(E);
  Expr *LHSOperand = expr->getLHS();
  QualType PtrTy = LHSOperand->getType();

  llvm::Value *Phase1 = EmitUPCPointerGetPhase(Pointer1);
  llvm::Value *Thread1 = EmitUPCPointerGetThread(Pointer1);
  llvm::Value *Addr1 = EmitUPCPointerGetAddr(Pointer1);

  llvm::Value *Phase2 = EmitUPCPointerGetPhase(Pointer2);
  llvm::Value *Thread2 = EmitUPCPointerGetThread(Pointer2);
  llvm::Value *Addr2 = EmitUPCPointerGetAddr(Pointer2);

  QualType PointeeTy = PtrTy->getAs<PointerType>()->getPointeeType();
  QualType ElemTy;
  llvm::Value *Dim;
  std::tie(ElemTy, Dim) = unwrapArray(*this, PointeeTy);
  Qualifiers Quals = ElemTy.getQualifiers();

  llvm::Constant *ElemSize = 
    llvm::ConstantInt::get(SizeTy, getContext().getTypeSizeInChars(ElemTy).getQuantity());
  llvm::Value *AddrByteDiff = Builder.CreateSub(Addr1, Addr2, "addr.diff");
  llvm::Value *AddrDiff = Builder.CreateExactSDiv(AddrByteDiff, ElemSize);

  llvm::Value *Result;

  if (Quals.getLayoutQualifier() == 0) {
    Result = AddrDiff;
  } else {
    llvm::Constant *B = llvm::ConstantInt::get(SizeTy, Quals.getLayoutQualifier());
    llvm::Value *Threads = Builder.CreateZExt(EmitUPCThreads(), SizeTy);

    llvm::Value *ThreadDiff = Builder.CreateMul(Builder.CreateSub(Thread1, Thread2, "thread.diff"), B);
    llvm::Value *PhaseDiff = Builder.CreateSub(Phase1, Phase2, "phase.diff");
    llvm::Value *BlockDiff =
      Builder.CreateMul(Builder.CreateSub(AddrDiff, PhaseDiff), Threads, "block.diff");

    Result = Builder.CreateAdd(BlockDiff, Builder.CreateAdd(ThreadDiff, PhaseDiff), "ptr.diff");
  }

  if (Dim) {
    Result = Builder.CreateExactSDiv(Result, Dim, "diff.dim");
  }

  // FIXME: Divide by the array dimension
  return Result;
}

llvm::Value *CodeGenFunction::EmitUPCPointerCompare(
    llvm::Value *Pointer1, llvm::Value *Pointer2, const BinaryOperator *E) {

  QualType PtrTy = E->getLHS()->getType();

  QualType PointeeTy = PtrTy->getAs<PointerType>()->getPointeeType();
  QualType ElemTy = PointeeTy;
  while (const ArrayType *AT = getContext().getAsArrayType(ElemTy))
    ElemTy = AT->getElementType();
  Qualifiers Quals = ElemTy.getQualifiers();

  // Use the standard transformations so we only
  // have to implement < and ==.
  bool Flip = false;
  switch (E->getOpcode()) {
  case BO_EQ: break;
  case BO_NE: Flip = true; break;
  case BO_LT: break;
  case BO_GT: std::swap(Pointer1, Pointer2); break;
  case BO_LE: std::swap(Pointer1, Pointer2); Flip = true; break;
  case BO_GE: Flip = true; break;
  default: llvm_unreachable("expected a comparison operator");
  }

  llvm::Value *Phase1 = EmitUPCPointerGetPhase(Pointer1);
  llvm::Value *Thread1 = EmitUPCPointerGetThread(Pointer1);
  llvm::Value *Addr1 = EmitUPCPointerGetAddr(Pointer1);

  llvm::Value *Phase2 = EmitUPCPointerGetPhase(Pointer2);
  llvm::Value *Thread2 = EmitUPCPointerGetThread(Pointer2);
  llvm::Value *Addr2 = EmitUPCPointerGetAddr(Pointer2);

  llvm::Value *Result;
  // Equality has to work correctly even if the pointers
  // are not in the same array.
  if (E->getOpcode() == BO_EQ || E->getOpcode() == BO_NE) {
    Result = Builder.CreateAnd(Builder.CreateICmpEQ(Addr1, Addr2),
                               Builder.CreateICmpEQ(Thread1, Thread2));
  } else if (Quals.getLayoutQualifier() == 0) {
    Result = Builder.CreateICmpULT(Addr1, Addr2);
  } else {
    llvm::IntegerType *BoolTy = llvm::Type::getInt1Ty(CGM.getLLVMContext());
    llvm::Constant *LTResult = llvm::ConstantInt::get(BoolTy, 1);
    llvm::Constant *GTResult = llvm::ConstantInt::get(BoolTy, 0);

    llvm::Constant *ElemSize = 
      llvm::ConstantInt::get(SizeTy, getContext().getTypeSizeInChars(ElemTy).getQuantity());
    llvm::Value *AddrByteDiff = Builder.CreateSub(Addr1, Addr2, "addr.diff");
    llvm::Value *PhaseDiff = Builder.CreateSub(Phase1, Phase2, "phase.diff");
    llvm::Value *PhaseByteDiff = Builder.CreateMul(PhaseDiff, ElemSize);
    llvm::Value *TestBlockLT = Builder.CreateICmpSLT(AddrByteDiff, PhaseByteDiff);
    llvm::Value *TestBlockEQ = Builder.CreateICmpEQ(AddrByteDiff, PhaseByteDiff);
    llvm::Value *TestThreadLT = Builder.CreateICmpULT(Thread1, Thread2);
    llvm::Value *TestThreadEQ = Builder.CreateICmpEQ(Thread1, Thread2);
    
    // Compare the block first, then the thread, then the phase
    Result = Builder.CreateSelect(TestBlockLT,
      LTResult,
      Builder.CreateSelect(TestBlockEQ,
        Builder.CreateSelect(TestThreadLT,
          LTResult,
          Builder.CreateSelect(TestThreadEQ,
            Builder.CreateICmpULT(Phase1, Phase2),
            GTResult)),
        GTResult));
  }

  if (Flip)
    Result = Builder.CreateNot(Result);
  return Result;
}

llvm::Value *CodeGenFunction::EmitUPCFieldOffset(llvm::Value *Addr,
                                                 llvm::Type * StructTy,
                                                 int Idx) {
  const llvm::StructLayout * Layout =
    CGM.getDataLayout().getStructLayout(cast<llvm::StructType>(StructTy));
  llvm::Value * Offset =
    llvm::ConstantInt::get(SizeTy, Layout->getElementOffset(Idx));
  return EmitUPCPointer(
    llvm::ConstantInt::get(SizeTy, 0),
    EmitUPCPointerGetThread(Addr),
    Builder.CreateAdd(EmitUPCPointerGetAddr(Addr), Offset));
}

llvm::Value *CodeGenFunction::EmitUPCPointerAdd(llvm::Value *Addr,
                                                int Idx) {
  llvm::Value * Offset =
    llvm::ConstantInt::get(SizeTy, Idx);
  return EmitUPCPointer(
    llvm::ConstantInt::get(SizeTy, 0),
    EmitUPCPointerGetThread(Addr),
    Builder.CreateAdd(EmitUPCPointerGetAddr(Addr), Offset));
}
