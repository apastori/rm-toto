/*
 * Chain-of-thought (Step 1 — before code):
 *
 * 1. Single responsibility: public constants, options bag, exit codes, and
 *    the allocation-free path joiner shared by the walk and unit tests.
 * 2. Syscalls: none in this header.
 * 3. Heap: none — join_path writes into a caller-supplied stack buffer.
 * 4. Not throughput-sensitive.
 * 5. C11 — static inline, stdbool, size_t.
 */

#ifndef RM_TOTO_H
#define RM_TOTO_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifdef PATH_MAX
#define RM_TOTO_PATH_MAX PATH_MAX
#else
#define RM_TOTO_PATH_MAX 4096
#endif

#define RM_TOTO_VERSION_STRING "1.0.0"

typedef enum {
    RM_TOTO_EXIT_OK = 0,
    RM_TOTO_EXIT_FAILURE = 1
} rm_toto_exit_t;

typedef enum {
    RM_TOTO_INTERACTIVE_NEVER = 0,
    RM_TOTO_INTERACTIVE_ONCE = 1,
    RM_TOTO_INTERACTIVE_ALWAYS = 2
} rm_toto_interactive_t;

/*
 * Option state produced by CLI parsing. The walk sees only this bag — never
 * argv spellings.
 */
typedef struct {
    bool ignore_missing;           /* -f / --force */
    rm_toto_interactive_t interactive;
    bool recursive;                /* -r / -R / --recursive */
    bool dir;                      /* -d / --dir */
    bool verbose;                  /* -v / --verbose */
    bool preserve_root;            /* default true; cleared by --no-preserve-root */
    bool preserve_root_all;        /* --preserve-root=all */
    bool one_file_system;          /* --one-file-system */
} rm_toto_opts_t;

/*
 * rm_toto_remove_path — remove a single pathname (file or, with opts, tree).
 *
 * Preconditions: path and opts non-NULL; path NUL-terminated.
 * Postconditions: returns RM_TOTO_EXIT_OK on success (including skipped
 *   confirmations and -f-suppressed missing targets); RM_TOTO_EXIT_FAILURE
 *   if any removal under this path failed.
 * Error behaviour: reports via rm_toto_emit_*; never exits.
 */
int rm_toto_remove_path(const char *path, const rm_toto_opts_t *opts);

/*
 * rm_toto_join_path — build parent + "/" + name into dest.
 *
 * Preconditions:
 *   - dest, parent, name non-NULL; parent and name NUL-terminated
 *   - dest_sz > 0
 *   - name is non-empty (empty name is a precondition violation; callers
 *     must not invoke this helper with name == "")
 * Postconditions:
 *   - on success (return 0): dest holds the joined path, NUL-terminated,
 *     length < dest_sz; if parent already ends in '/', no doubled separator
 *   - on would-be truncation (return non-zero): dest contents unspecified
 * Error behaviour: no errno; caller treats non-zero as failure.
 */
static inline int rm_toto_join_path(char *dest, size_t dest_sz,
                                   const char *parent, const char *name)
{
    size_t parent_len;
    size_t name_len;
    size_t need;
    int need_sep;

    parent_len = strlen(parent);
    name_len = strlen(name);
    need_sep = (parent_len > 0 && parent[parent_len - 1u] != '/') ? 1 : 0;
    need = parent_len + (size_t)need_sep + name_len;

    if (need >= dest_sz) {
        return -1;
    }

    memcpy(dest, parent, parent_len);
    if (need_sep != 0) {
        dest[parent_len] = '/';
    }
    memcpy(dest + parent_len + (size_t)need_sep, name, name_len);
    dest[need] = '\0';
    return 0;
}

#endif /* RM_TOTO_H */
