
#include <stdio.h>

#include "version2.h"
#include "minipmi.h"
#include "common.h"

int minipmi_version2_initialize(struct minipmi_context *ctx)
{
	int err;

	err = minipmi_get_fd(&ctx->fd);
	if (unlikely(err))
		return err;

	ctx->version    = 2;
	ctx->subversion = 0;

	err = minipmi_version1_handshake(ctx);
	if (unlikely(err))
		return err;

	err = minipmi_version2_send(ctx, "cmd=fullinit;threaded=false;pmijobid=-1;pmirank=-1;");
	if (unlikely(err))
		return err;

	err = minipmi_version2_recv_parse_match(ctx, "fullinit-response", 1);
	if (unlikely(err))
		return err;

	err = minipmi_find_integer_value_by_key(ctx, "rank", &ctx->rank);
	if (unlikely(err))
		return err;

	err = minipmi_find_integer_value_by_key(ctx, "size", &ctx->size);
	if (unlikely(err))
		return err;

	ctx->initialized = 1;

	return 0;
}

int minipmi_version2_finalize(struct minipmi_context *ctx)
{
	int err, x;

	err = minipmi_version2_send(ctx, "cmd=finalize;thrid=0;");
	if (unlikely(err))
		return err;

	err = minipmi_version2_recv_parse_match(ctx, "finalize-response", 1);
	if (unlikely(err))
		return err;

	ctx->finalized = 1;

	return 0;
}

int minipmi_version2_kvs_put(struct minipmi_context *ctx, const char *key, const char *value)
{
	int err, x;

	err = minipmi_version2_send(ctx, "cmd=kvs-put;thrid=0;key=%s;value=%s;", key, value);
	if (unlikely(err))
		return err;

	err = minipmi_version2_recv_parse_match(ctx, "kvs-put-response", 1);
	if (unlikely(err))
		return err;

	return 0;
}

int minipmi_version2_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen)
{
	int err, x;
	const char *v;

	*value    = 0;
	*valuelen = 0;

	err = minipmi_version2_send(ctx, "cmd=kvs-get;thrid=0;key=%s;", key);
	if (unlikely(err))
		return err;

	err = minipmi_version2_recv_parse_match(ctx, "kvs-get-response", 1);
	if (unlikely(err))
		return err;
	
	v = minipmi_find_value_by_key(ctx, "found");
	if (unlikely(!v))
		return -1;
	if (unlikely(strcmp("TRUE", v)))
		return -1;

	v = minipmi_find_value_by_key(ctx, "value");
	if (unlikely(!v))
		return -1;

	x = snprintf(value, maxvaluelen, "%s", v);
	*valuelen = (x < maxvaluelen) ? x : maxvaluelen;

	return 0;
}

int minipmi_version2_kvs_sync(struct minipmi_context *ctx)
{
	int err, x;

	err = minipmi_version2_send(ctx, "cmd=kvs-fence;thrid=0;");
	if (unlikely(err))
		return err;

	err = minipmi_version2_recv_parse_match(ctx, "kvs-fence-response", 1);
	if (unlikely(err))
		return err;

	return 0;
}

