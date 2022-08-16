// RUN: auto-fix -rules="A7_2_3" %s 2>&1 -- -Wno-everything | FileCheck %s

#include <cstdint>
enum E1 : std::int32_t // Non-compliant
{
  E10,
  E11,
  E12
};

enum class E2 : std::int32_t // Compliant
{
  E20,
  E21,
  E22
};

// CHECK-NOT: {{.+}}
// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A7-2-3.cpp:4:6: warning: Enumerations shall be declared as scoped enum classes.
// CHECK-NEXT: {{^}}enum E1 : std::int32_t // Non-compliant{{$}}
// CHECK-NEXT: {{^}}     ^{{$}}
// CHECK-NEXT: {{^}}     class {{$}}
// CHECK-NEXT: {{^}}1 warning generated.{{$}}
// CHECK-NOT: {{.+}}
