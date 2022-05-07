#include "AutoFixMatchers.hpp"
#include <iostream>
#include <set>

void emitWarningWithHintReplacement(DiagnosticsEngine &DE, std::string &msg,
                                    std::string &replacementStr, SourceRange SR,
                                    SourceLocation SL) {
  unsigned ID =
      DE.getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Warning, msg);
  FixItHint hint = FixItHint::CreateReplacement(SR, replacementStr);
  DE.Report(SL, ID) << hint;
}

void emitWarningWithHintInsertion(DiagnosticsEngine &DE, std::string &msg,
                                  std::string &str, SourceLocation insertLoc,
                                  SourceLocation diagLoc) {
  unsigned ID =
      DE.getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Warning, msg);
  FixItHint hint = FixItHint::CreateInsertion(insertLoc, str);
  DE.Report(diagLoc, ID) << hint;
}

void A8_5_3::run(const MatchFinder::MatchResult &Result) {
  auto VD = Result.Nodes.getNodeAs<clang::VarDecl>("A8_5_3_Matcher");
  if (VD->hasInit()) {
    warnAutoTypeBracedInit(VD);
  }
}

bool A8_5_3::warnAutoTypeBracedInit(const VarDecl *VD) {
  if (auto dty = llvm::dyn_cast<clang::AutoType>(VD->getType().getTypePtr())) {
    if (!dty->isDecltypeAuto()) {
      if (VD->getInitStyle() == VarDecl::ListInit ||
          VD->getInitStyle() == VarDecl::CInit) {
        auto listInitExpr = getChildOfType<InitListExpr>(VD->getInit());
        if (!listInitExpr) {
          return false;
        }
        std::string exprStr = getExprStr(listInitExpr, ASTCtx);
        exprStr = exprStr.substr(1, exprStr.size() - 2);
        exprStr = " = " + exprStr;

        std::string replacementStr = "auto " + VD->getNameAsString() + exprStr;
        std::string msg =
            "A variable of type auto shall not be initialized using {} "
            "or ={} braced initialization";
        emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), msg,
                                       replacementStr, VD->getSourceRange(),
                                       VD->getLocation());
        return true;
      }
    }
  }
  return false;
}

void A8_5_2::run(const MatchFinder::MatchResult &Result) {
  auto VD = Result.Nodes.getNodeAs<clang::VarDecl>("A8_5_2_Matcher");
  if (VD->hasInit()) {
    warnNonAutoTypeBracedInit(VD);
  }
}

bool A8_5_2::warnNonAutoTypeBracedInit(const VarDecl *VD) {
  if (VD->getInitStyle() == VarDecl::ListInit) {
    return false;
  }
  std::string typeStr = VD->getType().getAsString();
  stripTypeString(typeStr);
  auto initListExpr = getChildOfType<InitListExpr>(VD->getInit());
  auto cxxConstructExpr = getChildOfType<CXXConstructExpr>(VD->getInit());
  std::string exprStr;
  if (initListExpr) {
    exprStr = getExprStr(initListExpr, ASTCtx);
    exprStr = exprStr.substr(1, exprStr.size() - 2);
  } else if (cxxConstructExpr) {
    exprStr = getExprStr(cxxConstructExpr, ASTCtx);
    if (exprStr[0] == '{' && exprStr[exprStr.size() - 1] == '}') {
      exprStr = exprStr.substr(1, exprStr.size() - 2);
    }
  } else {
    exprStr = getExprStr(VD->getInit(), ASTCtx);
  }

  exprStr = "{" + exprStr + "}";
  std::string replacementStr = typeStr + " " + VD->getNameAsString() + exprStr;
  std::string msg = "Braced-initialization {}, without equals sign, shall be "
                    "used for variable initialization";
  emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), msg, replacementStr,
                                 VD->getSourceRange(), VD->getLocation());
  return true;
}

void A7_2_3::run(const MatchFinder::MatchResult &Result) {
  auto ED = Result.Nodes.getNodeAs<clang::EnumDecl>("A7_2_3_Matcher");
  if (!ED->isScopedUsingClassTag()) {
    std::string msg = "Enumerations shall be declared as scoped enum classes.";
    std::string insStr = "class ";
    emitWarningWithHintInsertion(
        ASTCtx.getDiagnostics(), msg, insStr,
        ED->getSourceRange().getBegin().getLocWithOffset(5), ED->getLocation());
  }
}

void A7_1_8::run(const MatchFinder::MatchResult &Result) {
  auto D = Result.Nodes.getNodeAs<clang::Decl>("A7_1_8_Matcher");
  warnWrongPlacedSpecifiers(D);
}

void A7_1_8::checkWrongPlacedSpecifiers(std::string &typeStr,
                                        std::string &declString,
                                        const Decl *D) {
  std::vector<std::string> nonTypeSpecifiers{
      "typedef",      "friend",  "constexpr", "register", "static",  "extern",
      "thread_local", "mutable", "inline",    "virtual",  "explicit"};

  std::vector<std::string> typeIdsVec = getWordsFromString(typeStr);
  std::vector<std::string> declWordsVec = getWordsFromString(declString);
  for (auto nonTypeWord : nonTypeSpecifiers) {
    auto NTSIt =
        std::find(declWordsVec.begin(), declWordsVec.end(), nonTypeWord);
    if (NTSIt != declWordsVec.end()) {
      for (auto typeId : typeIdsVec) {
        auto TSIt = std::find(declWordsVec.begin(), declWordsVec.end(), typeId);
        if (TSIt != declWordsVec.end() && TSIt < NTSIt) {
          std::iter_swap(NTSIt, TSIt);
          // Update iterator for non-type specifier since its place in vector
          // changed.
          NTSIt =
              std::find(declWordsVec.begin(), declWordsVec.end(), nonTypeWord);

          // create replacement string by concatenating words from vector
          std::string replacementStr = declWordsVec[0];
          for (unsigned i = 1; i < declWordsVec.size(); i++) {
            replacementStr += " " + declWordsVec[i];
          }
          std::string warningMsg =
              "A non-type specifier shall be placed before a "
              "type specifier in a declaration";
          emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), warningMsg,
                                         replacementStr, D->getSourceRange(),
                                         D->getLocation());
        }
      }
    }
  }
}

void A7_1_8::warnWrongPlacedSpecifiers(const Decl *D) {
  std::string declString;
  std::string typeStr;
  if (const ValueDecl *VD = llvm::dyn_cast<clang::ValueDecl>(D)) {
    declString = getSourceString(SM, VD->getBeginLoc(), VD->getEndLoc());
    typeStr = VD->getType().getAsString();
    checkWrongPlacedSpecifiers(typeStr, declString, D);
  } else if (const TypedefDecl *TD = llvm::dyn_cast<clang::TypedefDecl>(D)) {
    auto name = TD->getName().str();
    declString =
        getSourceString(SM, TD->getBeginLoc(), TD->getEndLoc(), name.length());
    typeStr = TD->getUnderlyingType().getAsString();
    checkWrongPlacedSpecifiers(typeStr, declString, D);
  }
}

void A7_1_6::run(const MatchFinder::MatchResult &Result) {
  auto TD = Result.Nodes.getNodeAs<clang::TypedefDecl>("A7_1_6_Matcher");

  auto TypeStr = TD->getUnderlyingType().getAsString();
  std::string ReplacementStr = "using " + TD->getName().str() + " = " + TypeStr;

  std::string Msg = "The typedef specifier shall not be used.";
  emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), Msg, ReplacementStr,
                                 TD->getSourceRange(), TD->getLocation());
}