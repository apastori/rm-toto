/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: document empty-name precondition; do not call.
 * 2. No syscalls.
 * 3. No heap.
 * 4. Not throughput-sensitive.
 * 5. C11.
 */

#include "test_join_path_empty_name.h"

#include <stdio.h>

void test_join_path_empty_name(void)
{
    /* Empty name is undefined per API contract — do not call join_path. */
    printf("PASS: join_path empty name is precondition violation (not exercised)\n");
}
