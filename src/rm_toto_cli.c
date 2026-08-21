/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: meta-flag scan, option parsing, help/version text.
 * 2. Syscalls: write(2) for help/version on stdout.
 * 3. Heap: none.
 * 4. Cold path.
 * 5. C11 — walk never sees argv spellings, only rm_toto_opts_t.
 */

#include "rm_toto_cli.h"

#include "rm_toto_emit.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void write_stdout_literal(const char *text)
{
    size_t len = strlen(text);
    if (len > 0u) {
        (void)write(STDOUT_FILENO, text, len);
    }
}

static int argv_has_exact(int argc, char **argv, const char *needle)
{
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], needle) == 0) {
            return 1;
        }
    }
    return 0;
}

void scan_meta_flags(int argc, char **argv, int *help, int *version)
{
    *help = argv_has_exact(argc, argv, "--help") ||
            argv_has_exact(argc, argv, "--h");
    *version = 0;
    if (*help) {
        return;
    }
    *version = argv_has_exact(argc, argv, "--version") ||
               argv_has_exact(argc, argv, "--v");
}

static void opts_init_defaults(rm_toto_opts_t *opts)
{
    opts->ignore_missing = false;
    opts->interactive = RM_TOTO_INTERACTIVE_NEVER;
    opts->recursive = false;
    opts->dir = false;
    opts->verbose = false;
    opts->preserve_root = true;
    opts->preserve_root_all = false;
    opts->one_file_system = false;
}

static void set_force(rm_toto_opts_t *opts)
{
    opts->interactive = RM_TOTO_INTERACTIVE_NEVER;
    opts->ignore_missing = true;
}

static void set_interactive_always(rm_toto_opts_t *opts)
{
    opts->interactive = RM_TOTO_INTERACTIVE_ALWAYS;
    opts->ignore_missing = false;
}

static void set_interactive_once(rm_toto_opts_t *opts)
{
    opts->interactive = RM_TOTO_INTERACTIVE_ONCE;
    opts->ignore_missing = false;
}

static int apply_short_flag(char flag, rm_toto_opts_t *opts)
{
    switch (flag) {
        case 'f':
            set_force(opts);
            return 0;
        case 'i':
            set_interactive_always(opts);
            return 0;
        case 'I':
            set_interactive_once(opts);
            return 0;
        case 'r':
        case 'R':
            opts->recursive = true;
            return 0;
        case 'd':
            opts->dir = true;
            return 0;
        case 'v':
            opts->verbose = true;
            return 0;
        default: {
            char flag_str[2];

            flag_str[0] = flag;
            flag_str[1] = '\0';
            rm_toto_emit_bad_option(flag_str);
            return -1;
        }
    }
}

static int apply_long_flag(const char *arg, rm_toto_opts_t *opts)
{
    const char *name = arg + 2; /* skip "--" */

    if (strcmp(name, "force") == 0) {
        set_force(opts);
        return 0;
    }
    if (strcmp(name, "interactive") == 0 ||
        strcmp(name, "interactive=always") == 0) {
        set_interactive_always(opts);
        return 0;
    }
    if (strcmp(name, "interactive=once") == 0) {
        set_interactive_once(opts);
        return 0;
    }
    if (strcmp(name, "interactive=never") == 0) {
        opts->interactive = RM_TOTO_INTERACTIVE_NEVER;
        /* Do not set ignore_missing — unlike -f. */
        return 0;
    }
    if (strcmp(name, "recursive") == 0) {
        opts->recursive = true;
        return 0;
    }
    if (strcmp(name, "dir") == 0) {
        opts->dir = true;
        return 0;
    }
    if (strcmp(name, "verbose") == 0) {
        opts->verbose = true;
        return 0;
    }
    if (strcmp(name, "preserve-root") == 0) {
        opts->preserve_root = true;
        opts->preserve_root_all = false;
        return 0;
    }
    if (strcmp(name, "preserve-root=all") == 0) {
        opts->preserve_root = true;
        opts->preserve_root_all = true;
        return 0;
    }
    if (strcmp(name, "no-preserve-root") == 0) {
        opts->preserve_root = false;
        opts->preserve_root_all = false;
        return 0;
    }
    if (strcmp(name, "one-file-system") == 0) {
        opts->one_file_system = true;
        return 0;
    }

    rm_toto_emit_bad_option(arg);
    return -1;
}

int parse_options(int argc, char **argv, rm_toto_opts_t *opts,
                  int *first_operand_index)
{
    int i;

    opts_init_defaults(opts);

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        /* Standard input Option, used to separate options from operands */
        if (strcmp(arg, "--") == 0) {
            *first_operand_index = i + 1;
            return 0;
        }

        /* Apply flags */
        if (arg[0] == '-' && arg[1] != '\0') {
            /* Long flag */
            if (arg[1] == '-') {
                if (apply_long_flag(arg, opts) != 0) {
                    return -1;
                }
                continue;
            }

            /* Short flags */
            {
                int j;
                /* Apply short flags */
                for (j = 1; arg[j] != '\0'; j++) {
                    if (apply_short_flag(arg[j], opts) != 0) {
                        return -1;
                    }
                }
            }
            continue;
        }

        *first_operand_index = i;
        return 0;
    }

    *first_operand_index = argc;
    return 0;
}

void print_help(void)
{
    write_stdout_literal(
        "Usage: rm-toto [OPTION]... [FILE]...\n"
        "Remove (unlink) the FILE(s).\n"
        "\n"
        "  -f, --force           ignore nonexistent files, never prompt\n"
        "  -i                    prompt before every removal\n"
        "  -I                    prompt once before removing more than three\n"
        "                          files, or when removing recursively\n"
        "      --interactive[=WHEN]  prompt according to WHEN: never, once\n"
        "                          (-I), or always (-i); without WHEN, always\n"
        "  -r, -R, --recursive   remove directories and their contents\n"
        "  -d, --dir             remove empty directories\n"
        "  -v, --verbose         explain what is being done\n"
        "      --preserve-root[=all]  do not remove '/' (default);\n"
        "                          with =all, refuse mount points too\n"
        "      --no-preserve-root    do not treat '/' specially\n"
        "      --one-file-system     stay on one filesystem when recursing\n"
        "      --help, --h       display this help and exit\n"
        "      --version, --v    output version information and exit\n"
        "\n"
        "To remove a file whose name begins with a '-', use './-' or '--'.\n");
}

void print_version(void)
{
    char buf[64];
    int n;

    n = snprintf(buf, sizeof(buf), "rm-toto %s\n", RM_TOTO_VERSION_STRING);
    if (n > 0) {
        (void)write(STDOUT_FILENO, buf, (size_t)n);
    }
}
