#include "AutoFixHelper.hpp"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceManager.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <locale>

std::string getExprStr(const Expr *expr, const ASTContext &Context) {
  static PrintingPolicy print_policy(Context.getLangOpts());
  print_policy.FullyQualifiedName = 1;
  print_policy.SuppressScope = 0;
  print_policy.PrintCanonicalTypes = 1;

  std::string expr_string;
  llvm::raw_string_ostream stream(expr_string);
  expr->printPretty(stream, nullptr, print_policy);
  stream.flush();
  return expr_string;
}

void stripTypeString(std::string &typeStr) {
  auto pos = typeStr.find("class");
  if (pos != std::string::npos) {
    typeStr.erase(pos, 5);
  }
  pos = typeStr.find("struct");
  if (pos != std::string::npos) {
    typeStr.erase(pos, 6);
  }
}

std::string getSourceString(SourceManager &SM, SourceLocation beginLoc,
                            SourceLocation endLoc, int offset) {
  auto beginPtr = SM.getCharacterData(beginLoc);
  auto endPtr = SM.getCharacterData(endLoc);
  return std::string(beginPtr, (endPtr - beginPtr) + 1 + offset);
}

std::vector<std::string> getWordsFromString(std::string &str) {
  std::istringstream strStream(str);
  std::string word;
  std::vector<std::string> wordVec;
  while (getline(strStream, word, ' ')) {
    wordVec.push_back(word);
  }
  return wordVec;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

llvm::SmallSet<std::string, 20> parseComaSeparatedWords(std::string Str){
  llvm::SmallSet<std::string, 20> Words;
  std::istringstream strStream(Str);
  std::string Word;
  while (getline(strStream, Word, ',')) {
    trim(Word);
    Words.insert(Word);
  }
  return Words;
}