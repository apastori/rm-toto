/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: assert overlong joins fail without a usable dest.
 * 2. No syscalls.
 * 3. No heap.
 * 4. Not throughput-sensitive.
 * 5. C11 / assert().
 */

#include "test_join_path_truncation.h"

#include "rm_toto.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_join_path_truncation(void)
{
    char dest[RM_TOTO_PATH_MAX];
    char parent[RM_TOTO_PATH_MAX];
    char name[32];
    size_t i;
    int rc;

    for (i = 0; i < RM_TOTO_PATH_MAX - 8u; i++) {
        parent[i] = 'p';
    }
    parent[RM_TOTO_PATH_MAX - 8u] = '\0';
    strcpy(name, "longish_name_xx");

    rc = rm_toto_join_path(dest, sizeof(dest), parent, name);
    assert(rc != 0);
    printf("PASS: join_path truncation returns failure\n");
}
