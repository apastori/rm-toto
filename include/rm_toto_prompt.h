#ifndef RM_TOTO_PROMPT_H
#define RM_TOTO_PROMPT_H

#include <stdbool.h>

/*
 * rm_toto_confirm — prompt on stderr; read one line from stdin.
 *
 * Preconditions: path non-NULL.
 * Postconditions: writes "rm-toto: remove '<path>'? " to stderr; returns
 *   true if the reply begins with 'y' or 'Y', false otherwise (including EOF).
 * Error behaviour: I/O failures / EOF treated as negative (false).
 */
bool rm_toto_confirm(const char *path);

#endif /* RM_TOTO_PROMPT_H */
