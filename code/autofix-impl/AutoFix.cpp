#include "AutoFixMatchers.hpp"
#include "AutofixDiagnosticConsumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;

static std::string SupportedRules = 
  "A7-1-6 - The typedef specifier shall not be used.\n"
  "A7-1-8 - A non-type specifier shall be placed before a type specifier in a declaration.\n"
  "A7-2-3 - Enumerations shall be declared as scoped enum classes.\n"
  "A8-5-2 - Braced-initialization {}, without equals sign, shall be used for variable initialization.\n"
  "A8-5-3 - A variable of type auto shall not be initialized using {} or ={} braced initialization.\n";

static std::vector<std::string> SupportedRulesVec = {"A7_1_6", "A7_1_8", "A7_2_3", "A8_5_2", "A8_5_3"};

static llvm::cl::OptionCategory AutoFixCategory("auto-fix options");

cl::opt<bool> ApplyFix("apply-fix", cl::desc(R"(Apply suggested fixes. )"),
                       cl::init(false), cl::cat(AutoFixCategory));

static cl::opt<std::string> Rules(
    "rules",
    cl::desc(
        R"(Rules for which AutoFix will check complience for and suggest fixit hints.)"),
    cl::init(""), cl::cat(AutoFixCategory));

static cl::opt<bool>
    ListRules("list-rules",
              cl::desc(R"(List all of the AutFix supported rules.)"),
              cl::init(false), cl::cat(AutoFixCategory));

static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

MatchFinder::MatchCallback *printerFactory(const std::string &MatcherName,
                                           ASTContext &Context,
                                           SourceManager &SM, PrintingPolicy &PP) {
  if (MatcherName == "A7_1_6") {
    return new A7_1_6(Context, SM, PP);
  } else if (MatcherName == "A7_1_8") {
    return new A7_1_8(Context, SM, PP);
  } else if (MatcherName == "A7_2_3") {
    return new A7_2_3(Context, SM, PP);
  } else if (MatcherName == "A8_5_2") {
    return new A8_5_2(Context, SM, PP);
  } else if (MatcherName == "A8_5_3") {
    return new A8_5_3(Context, SM, PP);
  } else {
    llvm::errs() << "Unsuported rule " + MatcherName + ".\n";
    std::exit(1);
  }
}



internal::Matcher<Decl> *matcherFactory(const std::string &MatcherName) {
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

class AutoFixConsumer : public clang::ASTConsumer {
public:
  explicit AutoFixConsumer(ASTContext *Context, SourceManager &SM) : SM(SM) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    auto &DE = Context.getDiagnostics();
    auto &DO = DE.getDiagnosticOptions();
    DO.SnippetLineLimit = 10;

    SmallSet<std::string, 20> RulesMap = parseComaSeparatedWords(Rules);

    PrintingPolicy PP(Context.getLangOpts());
    PP.adjustForCPlusPlus();
    PP.SuppressTagKeyword = true;
    PP.SuppressUnwrittenScope = true;
    PP.SuppressInitializers = true;
    PP.ConstantArraySizeAsWritten = true;
    PP.AnonymousTagLocations = false;
    PP.Nullptr = true;
    PP.Restrict = true;
    PP.Alignof = true;
    PP.ConstantsAsWritten = true;
    PP.PrintCanonicalTypes = false;

    if (RulesMap.count("all")) {
      RulesMap.erase("all");
      for (const auto &Rule : SupportedRulesVec) {
        RulesMap.insert(Rule);
      }
    }

    MatchFinder Finder;
    std::vector<internal::Matcher<Decl> *> MatcherVec;
    std::vector<MatchFinder::MatchCallback *> PrinterVec;
    for (const auto &MatcherName : RulesMap) {
      internal::Matcher<Decl> *Matcher = matcherFactory(MatcherName);
      auto b = decl(isExpansionInMainFile()).bind("A7_1_8_Matcher");

      MatchFinder::MatchCallback *Printer =
          printerFactory(MatcherName, Context, SM, PP);

      MatcherVec.push_back(Matcher);
      PrinterVec.push_back(Printer);
      Finder.addMatcher(*Matcher, Printer);
    }

    Finder.matchAST(Context);

    // TODO: Try using uniquer_ptr to avoid memory leaks insted of new/delete.
    for (auto *Matcher : MatcherVec) {
      delete Matcher;
    }
    for (auto *Printer : PrinterVec) {
      delete Printer;
    }
  }
  SourceManager &SM;
};

class AutoFixAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<AutoFixConsumer>(&Compiler.getASTContext(),
                                             Compiler.getSourceManager());
  }
};

class AutoFixActionFactory : public FrontendActionFactory {
public:
  std::unique_ptr<FrontendAction> create() override {
    return std::make_unique<AutoFixAction>();
  }

  bool runInvocation(std::shared_ptr<CompilerInvocation> Invocation,
                     FileManager *Files,
                     std::shared_ptr<PCHContainerOperations> PCHContainerOps,
                     DiagnosticConsumer *DiagConsumer) override {
    return FrontendActionFactory::runInvocation(Invocation, Files,
                                                PCHContainerOps, DiagConsumer);
  }
};

int main(int argc, const char **argv) {
  auto ExpectedParser =
      CommonOptionsParser::create(argc, argv, AutoFixCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  if(ListRules){
    llvm::outs() << SupportedRules;
  }
  // Debug why this scope is needed for tests to work properly.
  // Probably a namespace issue.
  // TODO: Use VerifyDiagnosticConsumer for tests instead of FileCheck
  {
    auto *DO = new DiagnosticOptions();
    DO->ShowColors = true;
    Rewriter Rewrite;
    AutoFixDiagnosticConsumer DiagConsumer(llvm::outs(), &*DO, Rewrite);
    Tool.setDiagnosticConsumer(&DiagConsumer);
  }

  AutoFixActionFactory actionFactory;
  return Tool.run(&actionFactory);
}