// RUN: auto-fix -rules="A8_5_3" %s 2>&1 -- -Wno-everything | FileCheck %s

#include <cstdint>
#include <initializer_list>
void fn() noexcept {
  auto x1(10);
  // Compliant - the auto-declared variable is of type int, but
  // not compliant with A8-5-2.
  auto x2{10};
  // Non-compliant - according to C++14 standard the
  // auto-declared variable is of type std::initializer_list.
  // However, it can behave differently on different compilers.
  auto x3 = 10; // Compliant - the auto-declared variable is of type int, but
  // non-compliant with A8-5-2.
  auto x4 = {10}; // Non-compliant - the auto-declared variable is of type
  // std::initializer_list, non-compliant with A8-5-2.
  std::int8_t x5{10}; // Compliant
}

// CHECK-NOT: {{.+}}
// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-3.cpp:9:8: warning: A variable of type auto shall not be initialized using {} or ={} braced initialization
// CHECK-NEXT: {{^}} auto x2{10};
// CHECK-NEXT: {{^}} ~~~~~^~~~~~
// CHECK-NEXT: {{^}} auto x2 = 10
// CHECK-NOT: {{.+}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-3.cpp:15:8: warning: A variable of type auto shall not be initialized using {} or ={} braced initialization
// CHECK-NEXT: {{^}} auto x4 = {10};
// CHECK-NEXT: {{^}} ~~~~~^~~~~~~~~
// CHECK-NEXT: {{^}} auto x4 = 10
// CHECK-NEXT: {{^}}2 warnings generated.{{$}}
// CHECK-NOT: {{.+}}

