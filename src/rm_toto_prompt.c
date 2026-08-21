/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: y/n confirmation prompt for -i / -I paths.
 * 2. Syscalls: write(2) on stderr; read via fgetc on stdin.
 * 3. Heap: none.
 * 4. Cold path.
 * 5. C11.
 */

#include "rm_toto_prompt.h"

#include "rm_toto.h"

#include <stdio.h>
#include <unistd.h>

bool rm_toto_confirm(const char *path)
{
    char buf[RM_TOTO_PATH_MAX + 64];
    int n;
    int c;
    int first;
    bool yes;

    n = snprintf(buf, sizeof(buf), "rm-toto: remove '%s'? ", path);
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }

    first = fgetc(stdin);
    if (first == EOF) {
        return false;
    }

    yes = (first == 'y' || first == 'Y');

    /* Drain the rest of the line. */
    c = first;
    while (c != '\n' && c != EOF) {
        c = fgetc(stdin);
    }

    return yes;
}
