/*
 * Chain-of-thought (Step 1 — file scope):
 *
 * 1. Single responsibility: post-order unlink/rmdir walk for one pathname.
 * 2. Syscalls: lstat, unlink, rmdir, opendir/readdir/closedir.
 * 3. Heap: none — one stack path buffer per recursion frame.
 * 4. Errors on one entry do not abort siblings; status aggregates upward.
 * 5. C11 / POSIX.1-2008; never follow symlinks (lstat only).
 */

#include "rm_toto_walk.h"

#include "rm_toto_emit.h"
#include "rm_toto_prompt.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * MinGW UCRT64 does not declare lstat(2). Map to stat(2): Windows typically
 * does not expose POSIX symlink semantics the same way; this matches the
 * project's documented MSYS2 emulation path without inventing retry logic.
 */
#if defined(_WIN32) && !defined(lstat)
#define lstat(path, st) stat((path), (st))
#endif

/*
 * Build the parent directory path of `path` into `parent` (size parent_sz).
 * Returns 0 on success, -1 if path has no parent component or overflows.
 */
static int parent_path_of(const char *path, char *parent, size_t parent_sz)
{
    size_t len;
    size_t i;

    len = strlen(path);
    while (len > 1u && path[len - 1u] == '/') {
        len--;
    }
    if (len == 0u) {
        return -1;
    }

    i = len;
    while (i > 0u && path[i - 1u] != '/') {
        i--;
    }

    if (i == 0u) {
        if (parent_sz < 2u) {
            return -1;
        }
        parent[0] = '.';
        parent[1] = '\0';
        return 0;
    }

    if (i == 1u && path[0] == '/') {
        if (parent_sz < 2u) {
            return -1;
        }
        parent[0] = '/';
        parent[1] = '\0';
        return 0;
    }

    if (i >= parent_sz) {
        return -1;
    }
    memcpy(parent, path, i);
    parent[i] = '\0';
    if (i > 1u && parent[i - 1u] == '/') {
        parent[i - 1u] = '\0';
    }
    return 0;
}

/*
 * Best-effort mount-point check: st_dev of path differs from its parent.
 * Resolves the parent once. Returns false on any lookup failure.
 */
static bool is_mount_point(const char *path)
{
    struct stat st;
    struct stat pst;
    char parent[RM_TOTO_PATH_MAX];

    if (lstat(path, &st) != 0) {
        return false;
    }
    if (parent_path_of(path, parent, sizeof(parent)) != 0) {
        return false;
    }
    if (lstat(parent, &pst) != 0) {
        return false;
    }
    return st.st_dev != pst.st_dev;
}

static int unlink_one(const char *path, const rm_toto_opts_t *opts)
{
    int saved_errno;

    if (opts->interactive == RM_TOTO_INTERACTIVE_ALWAYS) {
        if (!rm_toto_confirm(path)) {
            return RM_TOTO_EXIT_OK;
        }
    }

    if (unlink(path) != 0) {
        saved_errno = errno;
        errno = saved_errno;
        rm_toto_emit_error(path);
        return RM_TOTO_EXIT_FAILURE;
    }

    if (opts->verbose) {
        rm_toto_emit_verbose(path);
    }
    return RM_TOTO_EXIT_OK;
}

static int rmdir_one(const char *path, const rm_toto_opts_t *opts)
{
    int saved_errno;

    if (opts->interactive == RM_TOTO_INTERACTIVE_ALWAYS) {
        if (!rm_toto_confirm(path)) {
            return RM_TOTO_EXIT_OK;
        }
    }

    if (rmdir(path) != 0) {
        saved_errno = errno;
        errno = saved_errno;
        rm_toto_emit_error(path);
        return RM_TOTO_EXIT_FAILURE;
    }

    if (opts->verbose) {
        rm_toto_emit_verbose(path);
    }
    return RM_TOTO_EXIT_OK;
}

/*
 * Post-order recursive removal of a directory tree rooted at `path`.
 * `root_dev` is the starting entry's st_dev for --one-file-system.
 */
static int remove_directory_tree(const char *path, const rm_toto_opts_t *opts,
                                 dev_t root_dev)
{
    DIR *dir;
    struct dirent *ent;
    int status = RM_TOTO_EXIT_OK;
    int saved_errno;

    dir = opendir(path);
    if (dir == NULL) {
        saved_errno = errno;
        errno = saved_errno;
        rm_toto_emit_error(path);
        return RM_TOTO_EXIT_FAILURE;
    }

    while ((errno = 0, ent = readdir(dir)) != NULL) {
        char child[RM_TOTO_PATH_MAX];
        struct stat st;
        const char *name = ent->d_name;

        // Skip . (Current Directory)
        if (name[0] == '.' && name[1] == '\0') {
            continue;
        }
        
        // Skip .. (Parent Directory)
        if (name[0] == '.' && name[1] == '.' && name[2] == '\0') {
            continue;
        }

        if (rm_toto_join_path(child, sizeof(child), path, name) != 0) {
            errno = ENAMETOOLONG;
            rm_toto_emit_error("path too long");
            status = RM_TOTO_EXIT_FAILURE;
            continue;
        }

        if (lstat(child, &st) != 0) {
            saved_errno = errno;
            errno = saved_errno;
            rm_toto_emit_error(child);
            status = RM_TOTO_EXIT_FAILURE;
            continue;
        }

        // If the path is a directory, we need to remove the directory
        if (S_ISDIR(st.st_mode)) {
            if (opts->one_file_system && st.st_dev != root_dev) {
                continue;
            }
            if (remove_directory_tree(child, opts, root_dev) !=
                RM_TOTO_EXIT_OK) {
                status = RM_TOTO_EXIT_FAILURE;
            }
            continue;
        }

        // If the path is not a directory, we need to remove the file
        if (unlink_one(child, opts) != RM_TOTO_EXIT_OK) {
            status = RM_TOTO_EXIT_FAILURE;
        }
    }

    if (errno != 0) {
        saved_errno = errno;
        errno = saved_errno;
        rm_toto_emit_error(path);
        status = RM_TOTO_EXIT_FAILURE;
    }

    if (closedir(dir) != 0) {
        saved_errno = errno;
        errno = saved_errno;
        rm_toto_emit_error(path);
        status = RM_TOTO_EXIT_FAILURE;
    }

    // Once the directory is empty, we need to remove the directory
    if (rmdir_one(path, opts) != RM_TOTO_EXIT_OK) {
        status = RM_TOTO_EXIT_FAILURE;
    }

    return status;
}

int rm_toto_remove_path(const char *path, const rm_toto_opts_t *opts)
{
    struct stat st;
    int saved_errno;

    // Get the stat of the path
    if (lstat(path, &st) != 0) {
        saved_errno = errno;
        /* ENOENT: File does not exist */
        /* ENOTDIR: File is not a directory (does not exist as a directory)*/
        if (opts->ignore_missing &&
            (saved_errno == ENOENT || saved_errno == ENOTDIR)) {
            return RM_TOTO_EXIT_OK;
        }
        errno = saved_errno;
        rm_toto_emit_error(path);
        return RM_TOTO_EXIT_FAILURE;
    }

    // If the path is a directory, we need to remove the directory
    if (S_ISDIR(st.st_mode)) {
        if (opts->recursive) {
            if (opts->preserve_root) {
                if (rm_toto_is_root_path(path)) {
                    rm_toto_emit_preserve_root(path);
                    return RM_TOTO_EXIT_FAILURE;
                }
                if (opts->preserve_root_all && is_mount_point(path)) {
                    rm_toto_emit_preserve_root(path);
                    return RM_TOTO_EXIT_FAILURE;
                }
            }
            return remove_directory_tree(path, opts, st.st_dev);
        }

        // if te path is an empty directory, we need to remove the directory
        if (opts->dir) {
            return rmdir_one(path, opts);
        }

        errno = EISDIR;
        rm_toto_emit_error(path);
        return RM_TOTO_EXIT_FAILURE;
    }

    // If the path is not a directory, we need to remove the file
    return unlink_one(path, opts);
}
