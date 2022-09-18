// RUN: auto-fix -rules="A8_5_2" %s 2>&1 -- -Wno-everything | FileCheck -dump-input-filter=all -color %s

#include <cstdint>
#include <initializer_list>
void f1() noexcept {
  std::int32_t x1 = 7.9; // Non-compliant - x1 becomes 7 without compilation error

  // std::int32_t y {7.9}; // Compliant - compilation error, narrowing
  std::int8_t x2{50};
  // Compliant
  std::int8_t x3 = {50}; // Non-compliant - std::int8_t x3 {50} is equivalent
  // and more readable
  std::int8_t x4 = 1.0;              // Non-compliant - implicit conversion from double to std::int8_t

  std::int8_t x5 = 300; // Non-compliant - narrowing occurs implicitly
  std::int8_t x6(x5);
  // Non-compliant
}
class A {
public:
  A(std::int32_t first, std::int32_t second) : x{first}, y{second} {}

private:
  std::int32_t x;
  std::int32_t y;
};
struct B {
  std::int16_t x;
  std::int16_t y;
};
class C {
public:
  C(std::int32_t first, std::int32_t second) : x{first}, y{second} {}
  C(std::initializer_list<std::int32_t> list) : x{0}, y{0} {}

private:
  std::int32_t x;
  std::int32_t y;
};
void f2() noexcept {
  //   A a1{1, 5}; // Compliant - calls constructor of class A
  A a2 = {1, 5}; // Non-compliant - calls a default constructor of class A
                 // and not copy constructor or assignment operator.

  A a3(1, 5); // Non-compliant

  B b1{5, 0}; // Compliant - struct members initialization

  C c1{2, 2};   // Compliant - C(std::initializer_list<std::int32_t>)
                // constructor is  called
  C c2(2, 2);   // Non-compliant 

  C c3{{}};     // Compliant - C(std::initializer_list<std::int32_t>) constructor is
                // called with an empty initializer_list
  C c4({2, 2}); // Compliant by exception -
                // C(std::initializer_list<std::int32_t>)
                // constructor is called
};
template <typename T, typename U>
void f1(T t, U u) noexcept(false) {
  std::int32_t x = 0;
  T v1(x); // Non-compliant
  T v2{x}; // Compliant - v2 is a variable
  // auto y = T(u); // Non-compliant - is it construction or cast?
  // Compilation error
};
void f3() noexcept {
  f1(0, "abcd"); // Compile-time error, cast from const char* to int
}

// CHECK-NOT: {{.+}}
// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}/auto-fix-A8-5-2.cpp:6:18: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization{{$}}
// CHECK-NEXT: {{^}} std::int32_t x1 = 7.9; // Non-compliant - x1 becomes 7 without compilation error{{$}}
// CHECK-NEXT: {{^}}                ^~~~~~{{$}}
// CHECK-NEXT: {{^}}                {7.9000000000000004}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:11:17: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} std::int8_t x3 = {50}
// CHECK-NEXT: {{^}}               ^~~~~~~
// CHECK-NEXT: {{^}}               {50}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:13:17: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} std::int8_t x4 = 1.0;
// CHECK-NEXT: {{^}}               ^~~~~~{{$}}
// CHECK-NEXT: {{^}}               {1.}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:15:17: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} std::int8_t x5 = 300;
// CHECK-NEXT: {{^}}               ^~~~~~{{$}}
// CHECK-NEXT: {{^}}               {300}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:16:17: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} std::int8_t x6(x5);
// CHECK-NEXT: {{^}}               ^~~~{{$}}
// CHECK-NEXT: {{^}}               {x5}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:42:7: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} A a2 = {1, 5};
// CHECK-NEXT: {{^}}     ^~~~~~~~~{{$}}
// CHECK-NEXT: {{^}}     {1, 5}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:45:7: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} A a3(1, 5);
// CHECK-NEXT: {{^}}     ^~~~~~{{$}}
// CHECK-NEXT: {{^}}     {1, 5}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:51:7: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} C c2(2, 2);
// CHECK-NEXT: {{^}}     ^~~~~~{{$}}
// CHECK-NEXT: {{^}}     {2, 2}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:55:7: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} C c4({2, 2});
// CHECK-NEXT: {{^}}   ^~~~~~~~{{$}}
// CHECK-NEXT: {{^}}   {2, 2}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:61:17: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} std::int32_t x = 0;
// CHECK-NEXT: {{^}}               ^~~~{{$}}
// CHECK-NEXT: {{^}}               {0}{{$}}

// CHECK: {{^((/|/[a-zA-Z0-9_-]+)+)}}auto-fix-A8-5-2.cpp:62:7: warning: Braced-initialization {}, without equals sign, shall be used for variable initialization
// CHECK-NEXT: {{^}} T v1(x);
// CHECK-NEXT: {{^}}     ^~~
// CHECK-NEXT: {{^}}     {(x)}
// CHECK-NEXT: {{^}}11 warnings generated.
// CHECK-NOT: {{.+}}
