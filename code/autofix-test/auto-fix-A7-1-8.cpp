// RUN: auto-fix -rules="A7_1_8" %s 2>&1 -- | FileCheck -dump-input-filter=all -color %s

#include <cstdint>

typedef std::int32_t int1;
std::int32_t typedef int2;

class C
{
public:
virtual inline void f1();
inline virtual void f2();
void virtual inline f3();
private:
std::int32_t mutable x;
mutable std::int32_t y;
};

// CHECK-NOT: {{.+}}
// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A7-1-8.cpp:6:22: warning: A non-type specifier shall be placed before a type specifier in a declaration{{$}}
// CHECK-NEXT: {{^}}std::int32_t typedef int2;{{$}}
// CHECK-NEXT: {{^}}~~~~~~~~~~~~~~~~~~~~~^~~~{{$}}
// CHECK-NEXT: {{^}}typedef std::int32_t int2;{{$}}
// CHECK-NOT: {{.+}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A7-1-8.cpp:13:21: warning: A non-type specifier shall be placed before a type specifier in a declaration{{$}}
// CHECK-NEXT: {{^}}void virtual inline f3();{{$}}
// CHECK-NEXT: {{^}}~~~~~~~~~~~~~~~~~~~~^~~~{{$}}
// CHECK-NEXT: {{^}}inline virtual void f3(){{$}}
// CHECK-NOT: {{.+}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A7-1-8.cpp:15:22: warning: A non-type specifier shall be placed before a type specifier in a declaration{{$}}
// CHECK-NEXT: {{^}}std::int32_t mutable x;{{$}}
// CHECK-NEXT: {{^}}~~~~~~~~~~~~~~~~~~~~~^{{$}}
// CHECK-NEXT: {{^}}mutable std::int32_t x{{$}}
// CHECK-NEXT: {{^}}3 warnings generated.{{$}}
// CHECK-NOT: {{.+}}
