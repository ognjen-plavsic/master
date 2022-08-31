#include "AutoFixMatchers.hpp"
#include "AutoFixHelper.hpp"
#include <iostream>
#include <set>

internal::Matcher<Decl> *
matcherFactoryAllFiles(const std::string &MatcherName) {
  if (MatcherName == "A7_1_6") {
    return new internal::Matcher<Decl>{
        typedefDecl(unless(isExpansionInSystemHeader()), unless(isImplicit()))
            .bind("A7_1_6_Matcher")};
  } else if (MatcherName == "A7_1_8") {
    return new internal::Matcher<Decl>{
        decl(unless(isExpansionInSystemHeader()), unless(isImplicit()))
            .bind("A7_1_8_Matcher")};
  } else if (MatcherName == "A7_2_3") {
    return new internal::Matcher<Decl>{
        enumDecl(unless(isImplicit()), unless(isExpansionInSystemHeader()))
            .bind("A7_2_3_Matcher")};
  } else if (MatcherName == "A8_5_2") {
    return new internal::Matcher<Decl>{
        varDecl(unless(isExpansionInSystemHeader()),
                unless(hasParent(declStmt(hasParent(cxxForRangeStmt())))),
                unless(isInstantiated()), unless(isImplicit()))
            .bind("A8_5_2_Matcher")};
  } else if (MatcherName == "A8_5_3") {
    return new internal::Matcher<Decl>{
        varDecl(unless(isExpansionInSystemHeader()),
                unless(hasParent(declStmt(hasParent(cxxForRangeStmt())))),
                unless(isInstantiated()), unless(isImplicit()))
            .bind("A8_5_3_Matcher")};
  } else {
    llvm::errs() << "Unsuported rule " + MatcherName + ".\n";
    std::exit(1);
  }
}

internal::Matcher<Decl> *
matcherFactoryOnlyMainFile(const std::string &MatcherName) {
  if (MatcherName == "A7_1_6") {
    return new internal::Matcher<Decl>{
        typedefDecl(isExpansionInMainFile(), unless(isImplicit()))
            .bind("A7_1_6_Matcher")};
  } else if (MatcherName == "A7_1_8") {
    return new internal::Matcher<Decl>{
        decl(isExpansionInMainFile(), unless(isImplicit()))
            .bind("A7_1_8_Matcher")};
  } else if (MatcherName == "A7_2_3") {
    return new internal::Matcher<Decl>{
        enumDecl(isExpansionInMainFile(), unless(isImplicit()))
            .bind("A7_2_3_Matcher")};
  } else if (MatcherName == "A8_5_2") {
    return new internal::Matcher<Decl>{
        varDecl(
            allOf(isExpansionInMainFile(),
                  unless(hasParent(declStmt(hasParent(cxxForRangeStmt()))))),
            unless(isInstantiated()), unless(isImplicit()))
            .bind("A8_5_2_Matcher")};
  } else if (MatcherName == "A8_5_3") {
    return new internal::Matcher<Decl>{
        varDecl(
            allOf(isExpansionInMainFile(),
                  unless(hasParent(declStmt(hasParent(cxxForRangeStmt()))))),
            unless(isInstantiated()), unless(isImplicit()))
            .bind("A8_5_3_Matcher")};
  } else {
    llvm::errs() << "Unsuported rule " + MatcherName + ".\n";
    std::exit(1);
  }
}

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

void A8_5_3::warnAutoTypeBracedInit(const VarDecl *VD) {
  if (auto dty = llvm::dyn_cast<clang::AutoType>(VD->getType().getTypePtr())) {
    if (!dty->isDecltypeAuto()) {
      if (VD->getInitStyle() == VarDecl::ListInit ||
          VD->getInitStyle() == VarDecl::CInit) {
        auto listInitExpr = getChildOfType<InitListExpr>(VD->getInit());
        if (!listInitExpr) {
          return;
        }
        std::string exprStr = getExprStr(listInitExpr, ASTCtx, PP);
        trim(exprStr);
        trimBraces(exprStr);
        auto DeclarationTypeLoc =
            VD->getCanonicalDecl()->getTypeSourceInfo()->getTypeLoc();

        if(VD->getInitStyle() == VarDecl::ListInit){
          exprStr = " = " + exprStr;
        }

        std::string typeStr =
            getSourceString(SM, DeclarationTypeLoc.getBeginLoc(),
                            DeclarationTypeLoc.getEndLoc());
        std::string msg =
            "A variable of type auto shall not be initialized using {} "
            "or ={} braced initialization";
        emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), msg, exprStr,
                                       listInitExpr->getSourceRange(),
                                       listInitExpr->getLBraceLoc());
        return;
      }
    }
  }
}

void A8_5_2::run(const MatchFinder::MatchResult &Result) {
  auto VD = Result.Nodes.getNodeAs<clang::VarDecl>("A8_5_2_Matcher");
  if (VD->hasInit()) {
    warnNonAutoTypeBracedInit(VD);
  }
}

void A8_5_2::warnNonAutoTypeBracedInit(const VarDecl *VD) {
  if (VD->getInitStyle() == VarDecl::ListInit) {
    return;
  }

  std::string typeStr = VD->getType().getAsString(PP);
  stripTypeString(typeStr);
  std::string exprStr = getExprStr(VD->getInit(), ASTCtx, PP);

  trimBraces(exprStr);
  exprStr = "{" + exprStr + "}";
  
  std::string replacementStr;
  std::string msg = "Braced-initialization {}, without equals sign, shall be "
                    "used for variable initialization";

  if (auto dty = llvm::dyn_cast<clang::AutoType>(VD->getType().getTypePtr())) {
    if (dty->isDecltypeAuto()) {
      return;
    }
    std::string typeStr = VD->getType().getAsString(PP);
    stripTypeString(typeStr);

    replacementStr = typeStr + " " + VD->getNameAsString() + exprStr;
    emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), msg, replacementStr,
                                   VD->getSourceRange(), VD->getLocation());
  } else {

    replacementStr = exprStr;
    unsigned offset = VD->getIdentifier()->getLength();
    SourceLocation beginLoc(VD->getLocation().getLocWithOffset(offset));
    SourceRange SR(beginLoc, VD->getInit()->getSourceRange().getEnd());

    if (VD->getType()->isArrayType()) {
      replacementStr = "[]" + replacementStr;
    }

    emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), msg, replacementStr,
                                   SR, beginLoc);
  }
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

void A7_1_6::run(const MatchFinder::MatchResult &Result) {
  auto TD = Result.Nodes.getNodeAs<clang::TypedefDecl>("A7_1_6_Matcher");

  auto TypeStr = TD->getUnderlyingType().getAsString(PP);
  std::string ReplacementStr = "using " + TD->getName().str() + " = " + TypeStr;

  std::string Msg = "The typedef specifier shall not be used.";
  emitWarningWithHintReplacement(ASTCtx.getDiagnostics(), Msg, ReplacementStr,
                                 TD->getSourceRange(), TD->getLocation());
}