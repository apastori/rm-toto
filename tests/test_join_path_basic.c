/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: assert short parent + short name join correctly.
 * 2. No syscalls.
 * 3. No heap.
 * 4. Not throughput-sensitive.
 * 5. C11 / assert().
 */

#include "test_join_path_basic.h"

#include "rm_toto.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_join_path_basic(void)
{
    char dest[RM_TOTO_PATH_MAX];
    int rc;

    rc = rm_toto_join_path(dest, sizeof(dest), "dir", "file.txt");
    assert(rc == 0);
    assert(strcmp(dest, "dir/file.txt") == 0);
    printf("PASS: join_path basic dir + file.txt\n");
}
