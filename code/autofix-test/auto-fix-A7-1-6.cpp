// RUN: auto-fix -rules="A7_1_6" %s 2>&1 -- | FileCheck -dump-input-filter=all -color %s

#include <cstdint>

typedef unsigned long ulong;

typedef std::int32_t (*fPointer1)(std::int32_t);

typedef int int_t, *intp_t;

// CHECK: auto-fix-A7-1-6.cpp:5:23: warning: The typedef specifier shall not be used.
// CHECK-NEXT: typedef unsigned long ulong;
// CHECK-NEXT: ~~~~~~~~~~~~~~~~~~~~~~^~~~~
// CHECK-NEXT: using ulong = unsigned long

// CHECK: auto-fix-A7-1-6.cpp:7:24: warning: The typedef specifier shall not be used.
// CHECK-NEXT: typedef std::int32_t (*fPointer1)(std::int32_t);
// CHECK-NEXT: ~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~
// CHECK-NEXT: using fPointer1 = std::int32_t (*)(std::int32_t)

// CHECK: auto-fix-A7-1-6.cpp:9:13: warning: The typedef specifier shall not be used.
// CHECK-NEXT: typedef int int_t, *intp_t;
// CHECK-NEXT: ~~~~~~~~~~~~^~~~~
// CHECK-NEXT: using int_t = int

// CHECK: auto-fix-A7-1-6.cpp:9:21: warning: The typedef specifier shall not be used.
// CHECK-NEXT: typedef int int_t, *intp_t;
// CHECK-NEXT: ~~~~~~~~~~~~~~~~~~~~^~~~~~
// CHECK-NEXT: using intp_t = int *