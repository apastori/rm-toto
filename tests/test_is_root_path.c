/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: assert rm_toto_is_root_path pure string checks.
 * 2. No syscalls.
 * 3. No heap.
 * 4. Not throughput-sensitive.
 * 5. C11 / assert().
 */

#include "test_is_root_path.h"

#include "rm_toto_walk.h"

#include <assert.h>
#include <stdio.h>

void test_is_root_path(void)
{
    assert(rm_toto_is_root_path("/") == true);
    assert(rm_toto_is_root_path("/tmp") == false);
    assert(rm_toto_is_root_path("") == false);
    assert(rm_toto_is_root_path("relative") == false);
    assert(rm_toto_is_root_path("./") == false);
    printf("PASS: is_root_path detects only exact \"/\"\n");
}
