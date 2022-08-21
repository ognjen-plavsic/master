#ifndef AUTOFIX_MATCHERS_H
#define AUTOFIX_MATCHERS_H

#include "AutoFixHelper.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::ast_matchers;

internal::Matcher<Decl> *matcherFactoryAllFiles(const std::string &MatcherName);
internal::Matcher<Decl> *matcherFactoryOnlyMainFile(const std::string &MatcherName);

class A8_5_3 : public MatchFinder::MatchCallback {
public:
  A8_5_3(ASTContext &ASTCtx, SourceManager &SM, PrintingPolicy &PP) : ASTCtx(ASTCtx), SM(SM), PP{PP} {}
  virtual void run(const MatchFinder::MatchResult &Result);
  void warnAutoTypeBracedInit(const VarDecl *VD);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
  PrintingPolicy &PP;
};

class A8_5_2 : public MatchFinder::MatchCallback {
public:
  A8_5_2(ASTContext &ASTCtx, SourceManager &SM, PrintingPolicy &PP) : ASTCtx(ASTCtx), SM(SM), PP{PP} {}
  virtual void run(const MatchFinder::MatchResult &Result);
  void warnNonAutoTypeBracedInit(const VarDecl *VD);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
  PrintingPolicy &PP;
};

class A7_1_6 : public MatchFinder::MatchCallback {
public:
  A7_1_6(ASTContext &ASTCtx, SourceManager &SM, PrintingPolicy &PP) : ASTCtx(ASTCtx), SM(SM), PP{PP} {}
  virtual void run(const MatchFinder::MatchResult &Result);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
  PrintingPolicy &PP;
};

class A7_2_3 : public MatchFinder::MatchCallback {
public:
  A7_2_3(ASTContext &ASTCtx, SourceManager &SM, PrintingPolicy &PP) : ASTCtx(ASTCtx), SM(SM), PP{PP} {}
  virtual void run(const MatchFinder::MatchResult &Result);

private:
  ASTContext &ASTCtx;
  SourceManager &SM;
  PrintingPolicy &PP;
};

#endif // AUTOFIX_MATCHERS_H