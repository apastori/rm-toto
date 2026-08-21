#ifndef RM_TOTO_EMIT_H
#define RM_TOTO_EMIT_H

/*
 * rm_toto_emit_error — write "rm-toto: <context>: <strerror(errno)>\n"
 * to stderr via write(2).
 *
 * Preconditions: context non-NULL; errno already set by the failing call.
 * Postconditions: one diagnostic line on stderr (best-effort).
 */
void rm_toto_emit_error(const char *context);

/*
 * rm_toto_emit_verbose — write "removed '<path>'\n" to stdout via write(2).
 *
 * Preconditions: path non-NULL.
 * Postconditions: one verbose line on stdout (best-effort).
 */
void rm_toto_emit_verbose(const char *path);

/*
 * Non-errno stderr messages used by CLI / main / preserve-root.
 */
void rm_toto_emit_msg(const char *message);
void rm_toto_emit_missing_operand(void);
void rm_toto_emit_preserve_root(const char *path);
void rm_toto_emit_bad_option(const char *option);

#endif /* RM_TOTO_EMIT_H */
