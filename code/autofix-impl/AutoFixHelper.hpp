#ifndef AUTOFIX_HELPER_H
#define AUTOFIX_HELPER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallSet.h"
#include <string>

using namespace llvm;
using namespace clang;

std::string getExprStr(const Expr *expr, const ASTContext &Context,
                       PrintingPolicy &PP);

void stripTypeString(std::string &typeStr);

std::string getSourceString(SourceManager &SM, SourceLocation beginLoc,
                            SourceLocation endLoc, int offset = 0);

std::vector<std::string> getWordsFromString(std::string &str);

template <typename T> const T *getChildOfType(const Stmt *S) {
  if (const T *initListExpr = llvm::dyn_cast<const T>(S)) {
    return initListExpr;
  }
  for (const Stmt *child : S->children()) {
    if (auto res = getChildOfType<T>(child)) {
      return res;
    }
  }
  return nullptr;
}

void trim(std::string &s);
void trimBraces(std::string &s);

llvm::SmallSet<std::string, 20> parseComaSeparatedWords(std::string Str);

#endif // AUTOFIX_HELPER_H