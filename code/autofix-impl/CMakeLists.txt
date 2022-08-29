set(LLVM_LINK_COMPONENTS support)

add_clang_tool(auto-fix
  AutoFix.cpp
  AutoFixMatchers.cpp
  AutoFixHelper.cpp
  AutofixDiagnosticConsumer.cpp
  )
target_link_libraries(auto-fix
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )
