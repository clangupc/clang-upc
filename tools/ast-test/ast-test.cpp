//===-- tools/ast-test/main.cpp - Clang Ast Walker Example -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <map>
#include <cctype>
#include <vector>
#include <sstream>
#include <memory>
#include <cassert>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "clang/Basic/AttrKinds.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;

namespace {
  unsigned int ErrVerifyDiagNoCorrespondingDecl       = 0;
  unsigned int ErrVerifyDiagMultipleCorrespondingDecl = 0;
  unsigned int ErrVerifyDiagNoCorrespondingStmt       = 0;
  unsigned int ErrVerifyDiagMultipleCorrespondingStmt = 0;
  unsigned int ErrVerifyExpectingAttribute            = 0;
  unsigned int ErrVerifyDeclKind                      = 0;
  unsigned int ErrVerifyDeclName                      = 0;
  unsigned int ErrVerifyFunctionDeclReturnType        = 0;
  unsigned int ErrVerifyCXXMethodDispID               = 0;
  unsigned int ErrVerifyThreadSpecified               = 0;
  unsigned int ErrVerifyTypeName                      = 0;

  // Register the custom diagnostics
  void registerCustomDiagnostics(DiagnosticsEngine &Diag)
  {
    // Errors that can happen while parsing directives
    ErrVerifyDiagNoCorrespondingDecl = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "Directive has no corresponding decl");

    ErrVerifyDiagMultipleCorrespondingDecl = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "Directive has multiple corresponding decls");

    ErrVerifyDiagNoCorrespondingStmt = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "Directive has no corresponding stmt");

    ErrVerifyDiagMultipleCorrespondingStmt = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "Directive has multiple corresponding stmts");

    // Errors emitted by the attribute verifier
    ErrVerifyExpectingAttribute = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting attribute '%1'.");

    // Errors emitted by the declaration kind verifier
    ErrVerifyDeclKind = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting declaration kind '%1'. Got '%2'.");

    // Errors emitted by the declaration kind verifier
    ErrVerifyDeclName = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting declaration named '%1'.");

    ErrVerifyFunctionDeclReturnType = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting function return type '%1'.");

    ErrVerifyCXXMethodDispID = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting dispid '%1'.");

    ErrVerifyThreadSpecified = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting thread-specified VarDecl.");

    ErrVerifyTypeName = Diag.getCustomDiagID(
      DiagnosticsEngine::Error,
      "%0xpecting type '%1'. Got '%2'");
  }

  char const *getAttrKindAsString(attr::Kind Kind)
  {
    switch(Kind)
    {
      default: return "<<Unknown Attribute>>";
// Kind - This is a list of all the recognized kinds of attributes.
#define ATTR(X) case attr::X: return #X;
#include "clang/Basic/AttrList.inc"
    }
  }

  class Directive {
  protected:
    SourceLocation Location;
    std::size_t Offset;
    unsigned int ErrorNbr;
    bool Negate;
    std::vector<std::string> Args;
  public:
    Directive(SourceLocation Pos, std::size_t Off, unsigned int ErrNbr, bool Neg)
      : Location(Pos), Offset(Off), ErrorNbr(ErrNbr), Negate(Neg), Args() {
    }
    virtual ~Directive() {
    }
    SourceLocation getLocation() const {
      return Location;
    }
    bool isNegate() const { return Negate; }
  };

  struct TypeDumper
  {
  private:
    const void *Instance;
    void(*Caller)(const void*);

  public:
    template<typename T>
    TypeDumper(const T * Instance)
      : Instance(Instance),
        Caller(TypeDumper::CallDump<T>)
    {}

    TypeDumper()
      : Instance(0),
        Caller(0)
    {}

    void doDump()
    { if(Instance && Caller) Caller(Instance); }

  private:
    template<typename T>
    static
    void
    CallDump(const void * V)
    { static_cast<const T*>(V)->dump(); }
  };

  class MaybeDiagnostic
  {
  private:
    DiagnosticsEngine               *Diag;
    std::string                     Got;
    SourceLocation                  Loc;
    unsigned                        DiagID;
    bool                            Negate;
    std::vector<std::string> const  Args;
    TypeDumper                      Dumper;

  public:
    MaybeDiagnostic():
      Diag(0),
      Loc(),
      DiagID(0),
      Negate(false),
      Args()
    {}

    MaybeDiagnostic(DiagnosticsEngine &              Diag,
                    std::string const &              Got,
                    SourceLocation                   Loc,
                    unsigned                         DiagID,
                    bool                             Negate,
                    std::vector<std::string> const & Args,
                    TypeDumper                       Dumper)
      : Diag(&Diag),
        Got(Got),
        Loc(Loc),
        DiagID(DiagID),
        Negate(Negate),
        Args(Args),
        Dumper(Dumper)
    {}

    bool isDiagnostic() const {
      return Diag != 0;
    }

    void maybeReport() {
      if(!isDiagnostic()) return;

      DiagnosticBuilder DiagBuilder = Diag->Report(Loc,DiagID);
      if (Negate)
        DiagBuilder << "Not e";
      else
        DiagBuilder << "E";

      for(std::size_t I = 0; I < Args.size(); ++I) {
          DiagBuilder << Args[I];
      }
      if(Got.size())
        DiagBuilder << Got;
      else
        DiagBuilder << "<not-implemented>";
      Dumper.doDump();
      DiagBuilder.Emit();
    }

  };

  template<typename Type>
  class DirectiveMatchBase : public Directive,
                             public llvm::RefCountedBase<DirectiveMatchBase<Type> > {
    virtual bool MatchImpl(Type const * T, std::string & OutGot) const = 0;

    static void Dumper(void const * V) {
      static_cast<const Type*>(V)->dump();
    }
  public:

    explicit DirectiveMatchBase(SourceLocation Pos, std::size_t Off, unsigned int ErrNbr, bool Neg)
      : Directive(Pos,Off,ErrNbr,Neg) {}

    MaybeDiagnostic Match(Type const *T, DiagnosticsEngine &Diag) const {
      std::string Got;
      if (MatchImpl(T,Got) == Negate) {
        return MaybeDiagnostic(Diag,
                               Got,
                               Location.getLocWithOffset(Offset),
                               ErrorNbr,
                               Negate,
                               Args,
                               TypeDumper(T));
      } else {
        return MaybeDiagnostic();
      }
    }

  };

  // Abstract base class representing a directive to check
  // against a Decl. Override virtual MatchImpl member fun in
  // derived classes.
  typedef DirectiveMatchBase<Decl> DeclDirective;

  // Abstract base class representing a directive to check
  // against a Stmt. Override virtual MatchImpl member fun in
  // derived classes.
  typedef DirectiveMatchBase<Stmt> StmtDirective;

  template<typename ExprType, typename BaseType>
  class DirectiveHelper : public DirectiveMatchBase<BaseType> {
  protected:
    DirectiveHelper(SourceLocation Pos, std::size_t Off, unsigned ErrNmbr, bool Negate)
      : DirectiveMatchBase<BaseType>(Pos,Off,ErrNmbr,Negate)
    {}

    virtual bool TypedMatchImpl(ExprType const * E, std::string & Got) const = 0;

    virtual bool MatchImpl(BaseType const * S, std::string & Got) const {
      ExprType const * E = dyn_cast<ExprType>(S);
      if(!E)
        return false;
      else
        return TypedMatchImpl(E,Got);
    }
  };

  // Check whether a Decl has a certain attribute
  class AttributeDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      if (D->hasAttrs()) {
        const AttrVec &Attrs = D->getAttrs();
        for (AttrVec::const_iterator AttrsBeg = Attrs.begin(), AttrsEnd = Attrs.end();
          AttrsBeg != AttrsEnd; ++AttrsBeg) {
          Attr &A = **AttrsBeg;
          if (Args[0] == getAttrKindAsString(A.getKind())) {
            return true;
          }
        }
      }
      return false;
    }
  public:
    AttributeDirective(std::string const &AttrName, SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyExpectingAttribute, Negate) {
      Args.push_back(AttrName);
    }
  };

  // Check whether a Decl has a certain Kind. The potential kinds are
  // found in clang/AST/DeclNodes.inc
  class DeclKindDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      return (D->getDeclKindName() == Args[0]);
    }
  public:
    DeclKindDirective(std::string const &DeclKind, SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyDeclKind, Negate) {
      Args.push_back(DeclKind);
    }
  };

  // Check whether a Decl has the given name
  class DeclNameDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      NamedDecl const *ND = dyn_cast<NamedDecl>(D);
      return (0 != ND && ND->getNameAsString() == Args[0]);
    }
  public:
    DeclNameDirective(std::string const &DeclName, SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyDeclName, Negate) {
      Args.push_back(DeclName);
    }
  };

  // Check whether a Decl (a) is a function, and (b) has a certain return type.
  class FunctionDeclReturnTypeDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      FunctionDecl const *FD = dyn_cast<FunctionDecl>(D);
      return (0 != FD && FD->getCallResultType().getAsString() == Args[0]);
    }
  public:
    FunctionDeclReturnTypeDirective(std::string const &TypeName, SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyFunctionDeclReturnType, Negate) {
      Args.push_back(TypeName);
    }
  };

  // Check whether a Decl (a) is a function, and (b) does not return.
  class FunctionDeclNoReturnDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      if (FunctionDecl const *FD = dyn_cast<FunctionDecl>(D)) {
        if (FunctionType const *FT = dyn_cast<FunctionType>(FD->getType())) {
          return FT->getNoReturnAttr();
        }
      }
      return false;
    }
  public:
    FunctionDeclNoReturnDirective(SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyFunctionDeclReturnType, Negate) {
    }
  };

  // Check whether a Decl (a) is a variable, and (b) is thread-specific.
  class ThreadSpecifiedDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const *D, std::string &) const {
      VarDecl const *VD = dyn_cast<VarDecl>(D);
      return (0 != VD && VD->getTLSKind() != VarDecl::TLS_None);
    }
  public:
    ThreadSpecifiedDirective(SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos, Off, ErrVerifyThreadSpecified, Negate) {
    }
  };

  // Check if a given statement has the specified class name
  class StmtClassNameDirective : public StmtDirective {
    virtual bool MatchImpl(Stmt const *S, std::string &Got) const {
      Got = S->getStmtClassName();
      return (S->getStmtClassName() == Args[0]);
    }
  public:
    StmtClassNameDirective(std::string const &StmtClassName, SourceLocation Pos, std::size_t Off, bool Negate)
      : StmtDirective(Pos, Off, ErrVerifyDeclKind, Negate) {
      Args.push_back(StmtClassName);
    }
  };

  class DeclTypeDirective : public DeclDirective {
    virtual bool MatchImpl(Decl const * D, std::string & Got) const {
      ValueDecl const *VD = dyn_cast<ValueDecl>(D);
      if(!VD)
        return false;
      Got = VD->getType().getAsString();
      return Got == Args[0];
    }
  public:
    DeclTypeDirective(std::string const & TypeName, SourceLocation Pos, std::size_t Off, bool Negate)
      : DeclDirective(Pos,Off,ErrVerifyDeclKind,Negate)
    {
      Args.push_back(TypeName);
    }
  };

  class ExprTypeDirective : public DirectiveHelper<Expr,Stmt> {
    virtual bool TypedMatchImpl(Expr const * E, std::string & Got) const {
      Got = E->getType().getAsString();
      return (Got == Args[0]);
    }
  public:
    ExprTypeDirective(std::string const & TypeName, SourceLocation Pos, std::size_t Off, bool Negate)
      : DirectiveHelper<Expr,Stmt>(Pos,Off,ErrVerifyTypeName,Negate)
    {
      Args.push_back(TypeName);
    }
  };

  class VirtualFunctionDirective : public DirectiveHelper<CXXMethodDecl,Decl> {
    virtual bool TypedMatchImpl(CXXMethodDecl const * D, std::string & Got) const {
      Got = D->isVirtual() ? "virtual" : "not virtual";
      return D->isVirtual();
    }

  public:
    VirtualFunctionDirective(SourceLocation Pos, std::size_t Off, bool Negate)
      : DirectiveHelper<CXXMethodDecl,Decl>(Pos,Off,ErrVerifyExpectingAttribute,Negate)
    {}
  };

  typedef llvm::IntrusiveRefCntPtr<DeclDirective> DeclDirectivePtr;
  typedef llvm::IntrusiveRefCntPtr<StmtDirective> StmtDirectivePtr;
  typedef std::vector<DeclDirectivePtr> DeclDirectiveList;
  typedef std::vector<StmtDirectivePtr> StmtDirectiveList;

  /// ParseDirective - Go through the comment and see if it indicates expected
  /// diagnostics. If so, then create the appropriate directive checker.
  ///
  /// Directives are of the form:
  ///   (un)?expected-(\w+) {{ parameters }}
  /// where X is one of the known directive types.
  void ParseDirective(std::string const &Comment,
                      DeclDirectiveList &DL,
                      StmtDirectiveList &SL,
                      Preprocessor &PP,
                      SourceLocation Loc) {
    std::size_t EndPos = Comment.size();
    for (std::size_t Pos = Comment.find("expected-"); std::string::npos != Pos; Pos = Comment.find("expected-", Pos)) {
      bool const Negate = (Pos >= 2 && 0 == Comment.compare(Pos-2, 2, "un"));
      std::size_t const DirStart = Pos - (Negate ? 2 : 0);
      Pos += std::strlen("expected-");
      std::string KindStr;
      for (; Pos != EndPos && '{' != Comment[Pos] && !std::isspace(Comment[Pos]); ++Pos) {
        KindStr += Comment[Pos];
      }
      for (; Pos != EndPos && std::isspace(Comment[Pos]); ++Pos) {}
      if (Pos == EndPos || Comment[Pos] != '{' || ++Pos == EndPos || Comment[Pos] != '{' ) {
        PP.Diag(Loc.getLocWithOffset(Pos), diag::err_verify_missing_start) << KindStr;
        continue;
      }
      for (++Pos; Pos != EndPos && std::isspace(Comment[Pos]); ++Pos) {}
      std::size_t Pos2 = Comment.find("}}", Pos);
      if (std::string::npos == Pos2) {
        PP.Diag(Loc.getLocWithOffset(Pos), diag::err_verify_missing_end) << KindStr;
        continue;
      }
      for (; Pos != Pos2 && std::isspace(Comment[Pos2-1]); --Pos2) {}
      std::string Param = Comment.substr(Pos, Pos2-Pos);
      // known directives around found here:
      if (KindStr == "attribute") {
        DL.push_back(DeclDirectivePtr(new AttributeDirective(Param, Loc, DirStart, Negate)));
      }
      else if (KindStr == "decl-kind") {
        DL.push_back(DeclDirectivePtr(new DeclKindDirective(Param, Loc, DirStart, Negate)));
      }
      else if (KindStr == "decl-name") {
        DL.push_back(DeclDirectivePtr(new DeclNameDirective(Param, Loc, DirStart, Negate)));
      }
      else if (KindStr == "return-type") {
        DL.push_back(DeclDirectivePtr(new FunctionDeclReturnTypeDirective(Param, Loc, DirStart, Negate)));
      }
      else if (KindStr == "no-return") {
        DL.push_back(DeclDirectivePtr(new FunctionDeclNoReturnDirective(Loc, DirStart, Negate)));
      }
      else if (KindStr == "thread-specified") {
        DL.push_back(DeclDirectivePtr(new ThreadSpecifiedDirective(Loc, DirStart, Negate)));
      }
      else if (KindStr == "stmt-class-name") {
        SL.push_back(StmtDirectivePtr(new StmtClassNameDirective(Param, Loc, DirStart, Negate)));
      }
      else if (KindStr == "decl-type") {
        DL.push_back(DeclDirectivePtr(new DeclTypeDirective(Param,Loc,DirStart,Negate)));
      }
      else if (KindStr == "expr-type") {
        SL.push_back(StmtDirectivePtr(new ExprTypeDirective(Param,Loc,DirStart,Negate)));
      }
      else if (KindStr == "virtual-function") {
        DL.push_back(DeclDirectivePtr(new VirtualFunctionDirective(Loc,DirStart,Negate)));
      }
      else {
        continue;
      }
    }
  }

  /// FindExpectedDiags - Lex the main source file to find all of the
  //   expected errors and warnings.
  void FindExpectedDiags(Preprocessor &PP, DeclDirectiveList &DL, StmtDirectiveList &SL) {
    // Create a raw lexer to pull all the comments out of the main file.  We don't
    // want to look in #include'd headers for expected-error strings.
    SourceManager &SM = PP.getSourceManager();
    FileID FID = SM.getMainFileID();
    if (SM.getMainFileID().isInvalid())
      return;

    // Create a lexer to lex all the tokens of the main file in raw mode.
    const llvm::MemoryBuffer *FromFile = SM.getBuffer(FID);
    Lexer RawLex(FID, FromFile, SM, PP.getLangOpts());

    // Return comments as tokens, this is how we find expected diagnostics.
    RawLex.SetCommentRetentionState(true);

    Token Tok;
    Tok.setKind(tok::comment);
    while (Tok.isNot(tok::eof)) {
      RawLex.LexFromRawLexer(Tok);
      if (Tok.is(tok::comment)) {
        const char* TokStart = SM.getCharacterData(Tok.getLocation());
        std::string Comment = std::string(TokStart, TokStart+Tok.getLength());
        if (!Comment.empty()) {
          // Find all expected errors/warnings/notes.
          ParseDirective(Comment, DL, SL, PP, Tok.getLocation());
        }
      }
    }
  }

  typedef std::multimap<unsigned int, std::pair<SourceLocation, Decl *> > DeclsMap;
  typedef std::multimap<unsigned int, std::pair<SourceLocation, Stmt *> > StmtsMap;

  class ASTWalkerConsumer : public ASTConsumer,
                            public RecursiveASTVisitor<ASTWalkerConsumer> {
    SourceManager &SourceMgr;
    DeclsMap &Decls;
    StmtsMap &Stmts;
    DiagnosticsEngine &Diags;

    void addToDeclsMap(Decl *D)
    {
      SourceLocation Pos = D->getLocation();
      unsigned int Line = SourceMgr.getExpansionLineNumber(Pos);
      std::pair<DeclsMap::iterator, DeclsMap::iterator> rng = Decls.equal_range(Line);
      for( ; rng.first != rng.second; ++rng.first ) {
        if (D == rng.first->second.second)
          return; // already there
      }
      Decls.insert(rng.second, DeclsMap::value_type(Line, std::make_pair(Pos, D)));
    }

    void addToStmtsMap(Stmt *S)
    {
      SourceLocation Pos = S->getLocStart();
      unsigned int Line = SourceMgr.getExpansionLineNumber(Pos);
      std::pair<StmtsMap::iterator, StmtsMap::iterator> rng = Stmts.equal_range(Line);
      for( ; rng.first != rng.second; ++rng.first ) {
        if (S == rng.first->second.second)
          return; // already there
      }
      Stmts.insert(rng.second, StmtsMap::value_type(Line, std::make_pair(Pos, S)));
    }

  public:
    ASTWalkerConsumer(SourceManager &Mgr, DeclsMap &Decls_, StmtsMap &Stmts_, DiagnosticsEngine &Diags_)
      : SourceMgr(Mgr), Decls(Decls_), Stmts(Stmts_), Diags(Diags_) {
    }

    /// HandleTranslationUnit - This method is called when the ASTs for entire
    /// translation unit have been parsed.
    virtual void HandleTranslationUnit(ASTContext &Ctx) {
      TraverseDecl(Ctx.getTranslationUnitDecl());
    }

#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) \
  bool Visit##CLASS##Decl(CLASS##Decl *D) { \
      addToDeclsMap(D); \
      return true; \
    }
#include "clang/AST/DeclNodes.inc"

#define ABSTRACT_STMT(STMT)
#define STMT(CLASS, PARENT) \
  bool Visit##CLASS(CLASS *S) {\
      addToStmtsMap(S);\
      return true;\
    }
#include "clang/AST/StmtNodes.inc"
  };
}

class ASTWalkerAction : public SyntaxOnlyAction {
  DeclsMap Decls;
  StmtsMap Stmts;
  DeclDirectiveList DL;
  StmtDirectiveList SL;
  DiagnosticsEngine &Diags;
public:
  explicit ASTWalkerAction(DiagnosticsEngine &Diags_)
    : Diags(Diags_) {
  }

protected:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                         llvm::StringRef InFile) {
    return std::unique_ptr<ASTConsumer>(new ASTWalkerConsumer(CI.getSourceManager(), Decls, Stmts, Diags));
  }

  // TODO: Use traits to grab Underlying from one of the other two types.
  template<typename Underlying, typename DirectiveList, typename DirectiveMap>
  void
  DiagnoseFailures(DirectiveList & DL, DirectiveMap & DM) {
    for(typename DirectiveList::const_iterator I = DL.begin(), E = DL.end(); I != E; ++I) {
      Underlying & Dir = **I;
      unsigned int Line = getCompilerInstance().getSourceManager().getExpansionLineNumber(Dir.getLocation());
      std::pair<typename DirectiveMap::iterator, typename DirectiveMap::iterator> rng = DM.equal_range(Line);
      switch (std::distance(rng.first, rng.second))
      {
        case 0:
          Diags.Report(Dir.getLocation(), ErrVerifyDiagNoCorrespondingDecl);
          break;
        default:
          // Delay evaluation of diagnostics until we are certain that
          // none of the statements on this line match
          llvm::SmallVector<MaybeDiagnostic, 4> diags;
          bool isGood = false;
          // TODO: If something is unexpected, then it should not appear in any of the subexpressions.
          for(typename DirectiveMap::iterator it = rng.first, end = rng.second; it != end; ++it) {
            MaybeDiagnostic MD = Dir.Match(it->second.second,Diags);
            // unexpected-<foo> must never fail
            if(Dir.isNegate()) {
              if(MD.isDiagnostic()) {
                diags.push_back(MD);
                isGood = false;
                break;
              }
            } else {
              if(MD.isDiagnostic())
                diags.push_back(MD);
              else
                isGood = true;
            }
          }
          if(!isGood) {
            for(std::size_t ii = 0; ii < diags.size(); ++ii)
              diags[ii].maybeReport();
          }
          break;
      }
    }
  }

  virtual void EndSourceFileAction() {
    FindExpectedDiags(getCompilerInstance().getPreprocessor(), DL, SL);
    DiagnoseFailures<DeclDirective>(DL,Decls);
    DiagnoseFailures<StmtDirective>(SL,Stmts);
  }
};

class ASTTextDiagnosticPrinter : public TextDiagnosticPrinter {
  // FIXME: The only purpose of this class is to override EndSourceFile() to
  // prevent it from setting TextDiagnosticPrinter::LangOpts to 0. We need to
  // make this cloneable, but to do that we need to duplicate some of the data
  // members from TextDiagnosticPrinter.
  raw_ostream &OS;
  DiagnosticOptions *DiagOpts;
  bool OwnsOutputStream;
public:
  ASTTextDiagnosticPrinter(llvm::raw_ostream &os, DiagnosticOptions *diags,
                           bool OwnsOutputStream_ = false)
    : TextDiagnosticPrinter(os, diags, OwnsOutputStream_),
      OS(os), DiagOpts(diags), OwnsOutputStream(OwnsOutputStream_) {
  }

  void EndSourceFile() {}

  DiagnosticConsumer* clone(DiagnosticsEngine &Diags) const {
    return new ASTTextDiagnosticPrinter(OS, DiagOpts, OwnsOutputStream);
  }
};

// The call to CompilerInvocation::GetResourcesPath needs the address
// of some non-static function in this translation unit.
void foo() {}

//===----------------------------------------------------------------------===//
// Main driver
//===----------------------------------------------------------------------===//

int main(int argc, const char **argv) {
  // Only do front-end stuff
  llvm::SmallVector<const char *, 16> Args(argv + 1, argv + argc);
  Args.push_back("-fsyntax-only");

  std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());
  llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

  DiagnosticOptions *DiagOpts = new DiagnosticOptions;
  // Buffer diagnostics from argument parsing so that we can output them using a
  // well formed diagnostic object.
  TextDiagnosticBuffer *DiagsBuffer = new TextDiagnosticBuffer;
  DiagnosticsEngine Diags(DiagID, DiagOpts, DiagsBuffer);
  CompilerInvocation::CreateFromArgs(Clang->getInvocation(), Args.begin(), Args.end(),
                                     Diags);

  // Infer the builtin include path if unspecified.
  if (Clang->getHeaderSearchOpts().UseBuiltinIncludes &&
      Clang->getHeaderSearchOpts().ResourceDir.empty())
    Clang->getHeaderSearchOpts().ResourceDir =
      CompilerInvocation::GetResourcesPath(argv[0], (void*) (intptr_t) foo);

  // Create the actual diagnostics engine.
  ASTTextDiagnosticPrinter *DiagPrinter =
    new ASTTextDiagnosticPrinter(llvm::errs(), &Clang->getDiagnosticOpts());
  Clang->createDiagnostics(DiagPrinter);
  if (!Clang->hasDiagnostics()) {
    return EXIT_FAILURE;
  }

  DiagsBuffer->FlushDiagnostics(Clang->getDiagnostics());

  // If there were errors in processing arguments, don't do anything else.
  if (Clang->getDiagnostics().hasErrorOccurred()) {
    return EXIT_FAILURE;
  }

  // We can emit custom diagnostic messages that must be registered here.
  registerCustomDiagnostics(Clang->getDiagnostics());

  std::unique_ptr<FrontendAction> WalkAct(new ASTWalkerAction(Clang->getDiagnostics()));
  if(!Clang->ExecuteAction(*WalkAct)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
