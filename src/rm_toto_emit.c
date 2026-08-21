/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: format diagnostics and verbose lines via write(2).
 * 2. Syscalls: write(2) on STDERR_FILENO / STDOUT_FILENO.
 * 3. Heap: none (stack buffers for message assembly).
 * 4. Cold path — not throughput-sensitive.
 * 5. C11 via the project Makefile (-std=c11).
 */

#include "rm_toto_emit.h"

#include "rm_toto.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void rm_toto_emit_error(const char *context)
{
    char buf[512];
    int n;

    n = snprintf(buf, sizeof(buf), "rm-toto: %s: %s\n", context,
                 strerror(errno));
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }
}

void rm_toto_emit_verbose(const char *path)
{
    char buf[RM_TOTO_PATH_MAX + 32];
    int n;

    n = snprintf(buf, sizeof(buf), "removed '%s'\n", path);
    if (n > 0) {
        (void)write(STDOUT_FILENO, buf, (size_t)n);
    }
}

void rm_toto_emit_msg(const char *message)
{
    char buf[512];
    int n;

    n = snprintf(buf, sizeof(buf), "rm-toto: %s\n", message);
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }
}

void rm_toto_emit_missing_operand(void)
{
    rm_toto_emit_msg("missing operand");
}

void rm_toto_emit_preserve_root(const char *path)
{
    char buf[RM_TOTO_PATH_MAX + 128];
    int n;

    n = snprintf(buf, sizeof(buf),
                 "rm-toto: it is dangerous to operate recursively on '%s'\n"
                 "rm-toto: use --no-preserve-root to override this failsafe\n",
                 path);
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }
}

void rm_toto_emit_bad_option(const char *option)
{
    char buf[256];
    int n;

    n = snprintf(buf, sizeof(buf), "rm-toto: invalid option -- '%s'\n",
                 option);
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }
}
