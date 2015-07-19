
#include <stdio.h>

#include "version1.h"
#include "minipmi.h"
#include "common.h"

int minipmi_version1_initialize(struct minipmi_context *ctx)
{
	int err;
	const char *v;

	err = minipmi_get_fd(&ctx->fd);
	if (unlikely(err))
		return err;

	ctx->version    = 1;
	ctx->subversion = 1;

	err = minipmi_version1_handshake(ctx);
	if (unlikely(err))
		return err;

	err = env_variable_as_integer("PMI_RANK", &ctx->rank);
	if (unlikely(err))
		return err;

	err = env_variable_as_integer("PMI_SIZE", &ctx->size);
	if (unlikely(err))
		return err;

	err = minipmi_version1_send(ctx, "cmd=get_my_kvsname\n");
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "my_kvsname", 1);
	if (unlikely(err))
		return err;

	v = minipmi_find_value_by_key(ctx, "kvsname");
	if (unlikely(!v))
		return -1;

	snprintf(ctx->kvsname, MINIPMI_BUFLEN, "%s", v);

	ctx->initialized = 1;

	return 0;
}

int minipmi_version1_finalize(struct minipmi_context *ctx)
{
	int err, x;

	err = minipmi_version1_send(ctx, "cmd=finalize\n");
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "finalize_ack", 0);
	if (unlikely(err))
		return err;

	ctx->finalized = 1;

	return 0;
}

int minipmi_version1_kvs_put(struct minipmi_context *ctx, const char *key, const char *value)
{
	int err, x;

	err = minipmi_version1_send(ctx, "cmd=put kvsname=%s key=%s value=%s\n",
	                            ctx->kvsname, key, value);
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "put_result", 1);
	if (unlikely(err))
		return err;

	return 0;
}

int minipmi_version1_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen)
{
	int err, x;
	const char *v;

	*value    = 0;
	*valuelen = 0;

	err = minipmi_version1_send(ctx, "cmd=get kvsname=%s key=%s\n",
	                            ctx->kvsname, key);
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "get_result", 1);
	if (unlikely(err))
		return err;

	v = minipmi_find_value_by_key(ctx, "value");
	if (unlikely(!v))
		return -1;

	x = snprintf(value, maxvaluelen, "%s", v);
	*valuelen = (x < maxvaluelen) ? x : maxvaluelen;

	return 0;
}

int minipmi_version1_kvs_sync(struct minipmi_context *ctx)
{
	int err, x;
	const char *v;

	err = minipmi_version1_send(ctx, "cmd=barrier_in\n");
	if (unlikely(err))
		return err;

	err = minipmi_version1_recv_parse_match(ctx, "barrier_out", 0);
	if (unlikely(err))
		return err;

	return 0;
}

