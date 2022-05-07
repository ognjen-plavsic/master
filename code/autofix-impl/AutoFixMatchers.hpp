#ifndef AUTOFIX_MATCHERS_H
#define AUTOFIX_MATCHERS_H

#include "AutoFixHelper.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::ast_matchers;

class A8_5_3 : public MatchFinder::MatchCallback {
public:
  A8_5_3(ASTContext &ASTCtx, SourceManager &SM) : ASTCtx(ASTCtx), SM(SM) {}
  virtual void run(const MatchFinder::MatchResult &Result);
  bool warnAutoTypeBracedInit(const VarDecl *VD);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
};

class A8_5_2 : public MatchFinder::MatchCallback {
public:
  A8_5_2(ASTContext &ASTCtx, SourceManager &SM) : ASTCtx(ASTCtx), SM(SM) {}
  virtual void run(const MatchFinder::MatchResult &Result);
  bool warnNonAutoTypeBracedInit(const VarDecl *VD);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
};

class A7_1_6 : public MatchFinder::MatchCallback {
public:
  A7_1_6(ASTContext &ASTCtx, SourceManager &SM) : ASTCtx(ASTCtx), SM(SM) {}
  virtual void run(const MatchFinder::MatchResult &Result);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
};

class A7_1_8 : public MatchFinder::MatchCallback {
public:
  A7_1_8(ASTContext &ASTCtx, SourceManager &SM) : ASTCtx(ASTCtx), SM(SM) {}
  virtual void run(const MatchFinder::MatchResult &Result);
  void warnWrongPlacedSpecifiers(const Decl *D);
  void checkWrongPlacedSpecifiers(std::string &typeStr, std::string &declString,
                                  const Decl *D);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
};

class A7_2_3 : public MatchFinder::MatchCallback {
public:
  A7_2_3(ASTContext &ASTCtx, SourceManager &SM) : ASTCtx(ASTCtx), SM(SM) {}
  virtual void run(const MatchFinder::MatchResult &Result);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
};

#endif // AUTOFIX_MATCHERS_H