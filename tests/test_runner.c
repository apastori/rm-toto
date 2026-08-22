/*
 * Chain-of-thought (Step 1 — before code):
 *
 * 1. Single responsibility: invoke each pure-helper unit test in order.
 * 2. Syscalls: none.
 * 3. Heap: none in runner.
 * 4. N/A.
 * 5. C11 — matches the main program; assert() lives in individual tests.
 */

#include "test_is_root_path.h"
#include "test_join_path_basic.h"
#include "test_join_path_empty_name.h"
#include "test_join_path_trailing_slash.h"
#include "test_join_path_truncation.h"

int main(void)
{
    test_join_path_basic();
    test_join_path_truncation();
    test_join_path_trailing_slash();
    test_join_path_empty_name();
    test_is_root_path();
    return 0;
}
