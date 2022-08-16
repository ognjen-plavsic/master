#include "AutoFixHelper.hpp"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceManager.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <locale>
#include <algorithm>

std::string getExprStr(const Expr *expr, const ASTContext &Context, PrintingPolicy &PP) {
  std::string expr_string;
  llvm::raw_string_ostream stream(expr_string);
  expr->printPretty(stream, nullptr, PP);
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
  unsigned ptrDif = endPtr - beginPtr;
  // Some declarations can be large. If declaration is larger than 100
  // characters, ignore the rest.
  auto len = std::min(ptrDif + 1 + offset, (unsigned)100);
  return std::string(beginPtr, len);
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
void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

void ltrimBraces(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !(ch == '{');
    }));
}

// trim from end (in place)
void rtrimBraces(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !(ch == '}');
    }).base(), s.end());
}

void trimBraces(std::string &s) {
    ltrimBraces(s);
    rtrimBraces(s);
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