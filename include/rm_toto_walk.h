#ifndef RM_TOTO_WALK_H
#define RM_TOTO_WALK_H

#include "rm_toto.h"

#include <string.h>

/*
 * rm_toto_is_root_path — pure string check for the filesystem root "/".
 *
 * Preconditions: path non-NULL, NUL-terminated.
 * Postconditions: returns true iff path is exactly "/"; false for "/tmp",
 *   "", relative paths, and other strings.
 * Error behaviour: none (no syscalls).
 *
 * Mount-point detection for --preserve-root=all lives in the walk .c file.
 */
static inline bool rm_toto_is_root_path(const char *path)
{
    return path[0] == '/' && path[1] == '\0';
}

#endif /* RM_TOTO_WALK_H */
