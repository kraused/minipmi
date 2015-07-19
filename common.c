
#include "common.h"
#include "minipmi.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

static int minipmi_write(int fd, const void *buf, long long count);
static int minipmi_read(int fd, void *buf, long long count);
static int minipmi_parse(struct minipmi_context *ctx, char sep);

int libpmi_supported_pmi_versions[] = {1, 2, 0};

int minipmi_disable_version(int version)
{
	int i, j;

	for (i = 0; libpmi_supported_pmi_versions[i] > 0; ++i) {
		if (version != libpmi_supported_pmi_versions[i])
			continue;

		for (j = i; libpmi_supported_pmi_versions[j] > 0; ++j)
			libpmi_supported_pmi_versions[j] =
				libpmi_supported_pmi_versions[j + 1];
	}

	return -1;
}

int minipmi_pmi_version_is_supported(int version)
{
	int i;

	for (i = 0; libpmi_supported_pmi_versions[i] > 0; ++i) {
		if (version == libpmi_supported_pmi_versions[i])
			return 1;
	}

	return 0;
}

int env_variable_as_integer(const char *var, int *x)
{
	const char *env;

	if (unlikely(!x))
		return -EINVAL;

	env = getenv(var);
	if (env) {
		*x = (int )strtol(env, NULL, 10);

		return 0;
	} else {
#if 1 == DEBUG
		fprintf(stderr, "Variable '%s' not found in the environment.\n", var);
		fflush (stderr);
#endif
	}

	return -1;
}


int minipmi_get_fd(int *fd)
{
	return env_variable_as_integer("PMI_FD", fd);
}

int minipmi_version1_send(struct minipmi_context *ctx, const char *fmt, ...)
{
	va_list vl;
	int err, n;

	va_start(vl, fmt);
	n = vsnprintf(ctx->buf, MINIPMI_BUFLEN, fmt, vl);
	va_end(vl);

	if (unlikely((n < 0) || (n >= MINIPMI_BUFLEN)))
		return -1;

	n = strlen(ctx->buf);

#if 1 == DEBUG
	{
		/* Get rid of the newline for printing.
		 */
		char buf[MINIPMI_BUFLEN];
		memcpy(buf, ctx->buf, MINIPMI_BUFLEN);
		buf[n - 1] = 0;

		fprintf(stderr, "Sending message '%s'\n", buf);
	}
#endif

	err = minipmi_write(ctx->fd, ctx->buf, n);
	if (unlikely(err))
		return err;

	return 0;
}

int minipmi_version1_recv(struct minipmi_context *ctx)
{
	int i, j, n;
	int q;

	i = 0;
	q = 0;
	while (!q) {
		n = read(ctx->fd, ctx->buf + i, MINIPMI_BUFLEN - i);

		for (j = 0; j < n; ++j) {
			if ('\n' == ctx->buf[i + j]) {
				ctx->buf[i + j] = 0;
				q = 1;
				break;
			}
		}

		i += n;
	}

	return 0;
}

int minipmi_version1_parse(struct minipmi_context *ctx)
{
	return minipmi_parse(ctx, ' ');
}

int minipmi_version1_recv_parse_match(struct minipmi_context *ctx, const char *cmd, _Bool checkrc)
{
	int err, x;
	const char *v;

	err = minipmi_version1_recv(ctx);
	if (unlikely(err))
		return err;

#if 1 == DEBUG
	fprintf(stderr, "Received message '%s'\n", ctx->buf);
	fflush (stderr);
#endif

	err = minipmi_version1_parse(ctx);
	if (unlikely(err))
		return err;
	
	v = minipmi_find_value_by_key(ctx, "cmd");
	if (unlikely(!v))
		return -1;
	if (strcmp(v, cmd))
		return -2;

	if (checkrc) {
		err = minipmi_find_integer_value_by_key(ctx, "rc", &x);
		if (unlikely(err))
			return err;
		if (unlikely(0 != x))
			return -1;
	}

	return 0;
}

int minipmi_version2_send(struct minipmi_context *ctx, const char *fmt, ...)
{
	va_list vl;
	int err, n;
	char num[7];

	va_start(vl, fmt);
	n = vsnprintf(ctx->buf, MINIPMI_BUFLEN, fmt, vl);
	va_end(vl);

	if (unlikely((n < 0) || (n >= MINIPMI_BUFLEN)))
		return -1;

	n = strlen(ctx->buf);
	snprintf(num, sizeof(num), "%6d", n);

#if 1 == DEBUG
	fprintf(stderr, "Sending message '%s'\n", ctx->buf);
#endif

	err = minipmi_write(ctx->fd, num, 6);
	if (unlikely(err))
		return err;
	err = minipmi_write(ctx->fd, ctx->buf, n);
	if (unlikely(err))
		return err;

	return 0;
}

int minipmi_version2_recv(struct minipmi_context *ctx)
{
	char num[7];
	int err, n;

	minipmi_read(ctx->fd, num, 6);
	num[6] = 0;

	n = strtol(num, NULL, 10);
	if (unlikely(0 == n))
		return -1;
	if (unlikely(n > MINIPMI_BUFLEN))
		return -2;

	err = minipmi_read(ctx->fd, ctx->buf, n);
	if (unlikely(err))
		return err;

	ctx->buf[n] = 0;

	return 0;
}

int minipmi_version2_parse(struct minipmi_context *ctx)
{
	return minipmi_parse(ctx, ';');
}

int minipmi_version2_recv_parse_match(struct minipmi_context *ctx, const char *cmd, _Bool checkrc)
{
	int err, x;
	const char *v;

	err = minipmi_version2_recv(ctx);
	if (unlikely(err))
		return err;

#if 1 == DEBUG
	fprintf(stderr, "Received message '%s'\n", ctx->buf);
	fflush (stderr);
#endif

	err = minipmi_version2_parse(ctx);
	if (unlikely(err))
		return err;
	
	v = minipmi_find_value_by_key(ctx, "cmd");
	if (unlikely(!v))
		return -1;
	if (strcmp(v, cmd))
		return -2;

	if (checkrc) {
		err = minipmi_find_integer_value_by_key(ctx, "rc", &x);
		if (unlikely(err))
			return err;
		if (unlikely(0 != x))
			return -1;
	}

	return 0;
}

const char *minipmi_find_value_by_key(struct minipmi_context *ctx, const char *key)
{
	int i;

	for (i = 0; i < ctx->nkvps; ++i)
		if (0 == strcmp(ctx->keys[i], key))
			return ctx->vals[i];

	return NULL;
}

int minipmi_find_integer_value_by_key(struct minipmi_context *ctx, const char *key, int *value)
{
	const char *x = minipmi_find_value_by_key(ctx, key);
	if (unlikely(!x))
		return -1;

	*value = strtol(x, NULL, 10);
	return 0;
}

int minipmi_version1_handshake(struct minipmi_context *ctx)
{
	int err, x;

	err = minipmi_version1_send(ctx, "cmd=init pmi_version=%d pmi_subversion=%d\n",
	                            ctx->version, ctx->subversion);
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "response_to_init", 1);
	if (unlikely(err))
		return err;

	err = minipmi_find_integer_value_by_key(ctx, "pmi_version", &x);
	if (unlikely(err))
		return err;
	if (unlikely(ctx->version != x))
		return -1;

	err = minipmi_find_integer_value_by_key(ctx, "pmi_subversion", &x);
	if (unlikely(err))
		return err;
	if (unlikely(ctx->subversion != x))
		return -1;

	return 0;
}

static int minipmi_write(int fd, const void *buf, long long count)
{
	long long i;

	do {
		i = write(fd, buf, count);
	} while ((-1 == i) && (EINTR == errno));

	if (count == i)
		return 0;
	else
		return -1;
}

static int minipmi_read(int fd, void *buf, long long count)
{
	long long i;

	while (count > 0) {
		i = read(fd, buf, count);

		buf   = (void *)(((char *)buf) + count);
		count = count - i;
	}

	return 0;
}

static int minipmi_parse(struct minipmi_context *ctx, char sep)
{
	int i, j, k;
	char *buf = ctx->buf;

	for (k = 0; k < MINIPMI_MAX_KVPS;) {
		for (i = 0;; ++i) {
			if((0 == buf[i]) || (sep == buf[i])) {
				buf[i] = 0;

				if (0 == i)
					break;

				for (j = 0; buf[j]; ++j) {
					if ('=' == buf[j]) {
						buf[j] = 0;
						break;
					}
				}

				if (j > 0) {
					ctx->keys[k] = &buf[0];
					ctx->vals[k] = &buf[j+1];
					++k;
				}

				buf = &buf[i+1];
				break;
			}
		}

		if (i == 0)
			break;
	}

	ctx->nkvps = k;

	return 0;
}

