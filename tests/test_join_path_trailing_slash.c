/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: parent ending in '/' must not double the slash.
 * 2. No syscalls.
 * 3. No heap.
 * 4. Not throughput-sensitive.
 * 5. C11 / assert().
 */

#include "test_join_path_trailing_slash.h"

#include "rm_toto.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_join_path_trailing_slash(void)
{
    char dest[RM_TOTO_PATH_MAX];
    int rc;

    rc = rm_toto_join_path(dest, sizeof(dest), "dir/", "file.txt");
    assert(rc == 0);
    assert(strcmp(dest, "dir/file.txt") == 0);
    assert(strstr(dest, "//") == NULL);
    printf("PASS: join_path trailing slash does not double separator\n");
}
