#include "AutofixDiagnosticConsumer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

extern llvm::cl::opt<bool> ApplyFix;

void AutoFixDiagnosticConsumer::HandleDiagnostic(
    DiagnosticsEngine::Level DiagLevel, const Diagnostic &Info) {
  TextDiagnosticPrinter::HandleDiagnostic(DiagLevel, Info);

  if (ApplyFix) {
    for (const auto &FixItHint : Info.getFixItHints()) {
      CharSourceRange Range = FixItHint.RemoveRange;
      tooling::Replacement replacement(Info.getSourceManager(), Range,
                                       FixItHint.CodeToInsert);
      llvm::Error Err = Replacements.add(replacement);
    }
  }
}

void AutoFixDiagnosticConsumer::BeginSourceFile(const LangOptions &LO,
                                                const Preprocessor *PP) {
  TextDiagnosticPrinter::BeginSourceFile(LO, PP);
  Rewrite.setSourceMgr(PP->getSourceManager(), LO);
}

void AutoFixDiagnosticConsumer::finish() {
  TextDiagnosticPrinter::finish();
  tooling::applyAllReplacements(Replacements, Rewrite);
  Rewrite.overwriteChangedFiles();
}
