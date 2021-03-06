// RUN: %clang_scudo %s -o %t
// RUN: not %run %t malloc   2>&1 | FileCheck %s
// RUN: not %run %t new      2>&1 | FileCheck %s
// RUN: not %run %t newarray 2>&1 | FileCheck %s
// RUN: not %run %t memalign 2>&1 | FileCheck %s

// Tests double-free error on pointers allocated with different allocation
// functions.

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
  assert(argc == 2);
  if (!strcmp(argv[1], "malloc")) {
    void *p = malloc(sizeof(int));
    if (!p)
      return 1;
    free(p);
    free(p);
  }
  if (!strcmp(argv[1], "new")) {
    int *p = new int;
    if (!p)
      return 1;
    delete p;
    delete p;
  }
  if (!strcmp(argv[1], "newarray")) {
    int *p = new int[8];
    if (!p)
      return 1;
    delete[] p;
    delete[] p;
  }
  if (!strcmp(argv[1], "memalign")) {
    void *p = nullptr;
    posix_memalign(&p, 0x100, sizeof(int));
    if (!p)
      return 1;
    free(p);
    free(p);
  }
  return 0;
}

// CHECK: ERROR: invalid chunk state
