#ifndef RM_TOTO_CLI_H
#define RM_TOTO_CLI_H

#include "rm_toto.h"

/*
 * scan_meta_flags — detect --help/--h and --version/--v anywhere in argv.
 * Help wins if both appear. Does not consume removal options.
 */
void scan_meta_flags(int argc, char **argv, int *help, int *version);

/*
 * parse_options — fill opts from argv; stop at "--" or first non-option.
 *
 * Preconditions: opts non-NULL; first_operand_index non-NULL.
 * Postconditions: *first_operand_index is the first removal target index
 *   (or argc if none). Returns 0 on success, -1 on invalid option (message
 *   already emitted).
 */
int parse_options(int argc, char **argv, rm_toto_opts_t *opts,
                  int *first_operand_index);

void print_help(void);
void print_version(void);

#endif /* RM_TOTO_CLI_H */
