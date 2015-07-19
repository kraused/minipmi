
#ifndef MINIPMI_COMMON_H_INCLUDED
#define MINIPMI_COMMON_H_INCLUDED 1

#include <errno.h>
/* For memset() */
#include <string.h>

#define likely(X)       __builtin_expect(!!(X), 1)
#define unlikely(X)     __builtin_expect(!!(X), 0)

struct minipmi_context;

/* Zero-terminated list of supported PMI versions.
 */
extern int libpmi_supported_pmi_versions[];

/* Disable a particular PMI version.
 */
int minipmi_disable_version(int version);

/* Query if a particular PMI version is supported.
 */
int minipmi_pmi_version_is_supported(int version);

/* Retrieve an integer variable from the environment.
 */
int env_variable_as_integer(const char *var, int *x);

/* Get the PMI file descriptor. In most cases the PMI_FD environment
 * variable is read.
 */
int minipmi_get_fd(int *fd);

int minipmi_version1_send(struct minipmi_context *ctx, const char *fmt, ...);
int minipmi_version1_recv(struct minipmi_context *ctx);
int minipmi_version1_parse(struct minipmi_context *ctx);
int minipmi_version1_recv_parse_match(struct minipmi_context *ctx, const char *cmd, _Bool checkrc);

int minipmi_version2_send(struct minipmi_context *ctx, const char *fmt, ...);
int minipmi_version2_recv(struct minipmi_context *ctx);
int minipmi_version2_parse(struct minipmi_context *ctx);
int minipmi_version2_recv_parse_match(struct minipmi_context *ctx, const char *cmd, _Bool checkrc);

const char *minipmi_find_value_by_key(struct minipmi_context *ctx, const char *key);
int minipmi_find_integer_value_by_key(struct minipmi_context *ctx, const char *key, int *value);

int minipmi_version1_handshake(struct minipmi_context *ctx);

#endif

