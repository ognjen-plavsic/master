#ifndef LLVM_CLANG_TOOLS_EXTRA_AUTOFIX_AUTOFIXDIAGNOSTICCONSUMER_H
#define LLVM_CLANG_TOOLS_EXTRA_AUTOFIX_AUTOFIXDIAGNOSTICCONSUMER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Tooling/Core/Diagnostic.h"

using namespace clang;

class AutoFixDiagnosticConsumer : public TextDiagnosticPrinter {
public:
  AutoFixDiagnosticConsumer(raw_ostream &os, DiagnosticOptions *diags,
                            Rewriter Rewrite, bool OwnsOutputStream = false)
      : TextDiagnosticPrinter(os, diags, OwnsOutputStream), Rewrite(Rewrite) {}
  void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                        const Diagnostic &Info) override;
  void finish() override;
  void BeginSourceFile(const LangOptions &LO, const Preprocessor *PP) override;

  tooling::Replacements Replacements;
  Rewriter Rewrite;
};

#endif // LLVM_CLANG_TOOLS_EXTRA_AUTOFIX_AUTOFIXDIAGNOSTICCONSUMER_H
