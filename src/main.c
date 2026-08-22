/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: wire meta flags, option parsing, and per-target
 *    removal; aggregate exit status. No removal logic here.
 * 2. Syscalls: none directly (delegated to walk / emit / prompt).
 * 3. Heap: none.
 * 4. Cold path for setup; removal cost is in the walk.
 * 5. C11; default signal disposition (no custom SIGINT handler).
 */

#include "rm_toto.h"

#include "rm_toto_cli.h"
#include "rm_toto_emit.h"
#include "rm_toto_prompt.h"

int main(int argc, char **argv)
{
    int want_help = 0;
    int want_version = 0;
    rm_toto_opts_t opts;
    int first;
    int n_targets;
    int i;
    int status = RM_TOTO_EXIT_OK;

    scan_meta_flags(argc, argv, &want_help, &want_version);

    if (want_help) {
        print_help();
        return RM_TOTO_EXIT_OK;
    }

    if (want_version) {
        print_version();
        return RM_TOTO_EXIT_OK;
    }

    if (parse_options(argc, argv, &opts, &first) != 0) {
        return RM_TOTO_EXIT_FAILURE;
    }

    /* number of targets to remove */
    n_targets = argc - first;
    /* If no targets to remove, emit a message and exit */
    if (n_targets <= 0) {
        if (opts.ignore_missing) {
            return RM_TOTO_EXIT_OK;
        }
        rm_toto_emit_missing_operand();
        return RM_TOTO_EXIT_FAILURE;
    }

    if (opts.interactive == RM_TOTO_INTERACTIVE_ONCE &&
        (opts.recursive || n_targets >= 3)) {
        if (!rm_toto_confirm("these files")) {
            return RM_TOTO_EXIT_OK;
        }
        /* After the once-prompt, do not re-prompt per entry. */
        opts.interactive = RM_TOTO_INTERACTIVE_NEVER;
    }

    for (i = first; i < argc; i++) {
        if (rm_toto_remove_path(argv[i], &opts) != RM_TOTO_EXIT_OK) {
            /* If one of the targets removal fails, set the status to failure */
            status = RM_TOTO_EXIT_FAILURE;
        }
    }

    return status;
}
