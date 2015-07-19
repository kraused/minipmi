
#include "minipmi.h"
#include "common.h"
#include "version1.h"
#include "version2.h"

static struct {
	int (*finalize)(struct minipmi_context *);
	int (*kvs_put)(struct minipmi_context *, const char *, const char *);
	int (*kvs_get)(struct minipmi_context *, const char *, char *, int, int *);
	int (*kvs_sync)(struct minipmi_context *);
} ops[] = {
	[1] = {
		.finalize = minipmi_version1_finalize,
		.kvs_put  = minipmi_version1_kvs_put,
		.kvs_get  = minipmi_version1_kvs_get,
		.kvs_sync = minipmi_version1_kvs_sync
	},
	[2] = {
		.finalize = minipmi_version2_finalize,
		.kvs_put  = minipmi_version2_kvs_put,
		.kvs_get  = minipmi_version2_kvs_get,
		.kvs_sync = minipmi_version2_kvs_sync
	}
};

int minipmi_initialize(struct minipmi_context *ctx)
{
	int i, err;
	struct {
		int version;
		int (*f)(struct minipmi_context *);
	} inits[] = {
		{2, minipmi_version2_initialize},
		{1, minipmi_version1_initialize},
		{0, NULL}
	};

	if (unlikely(!ctx))
		return -EINVAL;

	for (i = 0; inits[i].f; ++i) {
		if (!minipmi_pmi_version_is_supported(inits[i].version)) {
			err = -1;
			continue;
		}

		memset(ctx, 0, sizeof(struct minipmi_context));
		err = inits[i].f(ctx);
		if (0 == err)
			return 0;
	}

	/* Return the error of the last initialization
	 * attempt.
	 */
	return err;
}

int minipmi_finalize(struct minipmi_context *ctx)
{
	if (unlikely(!ctx))
		return -EINVAL;

	if (unlikely(!minipmi_pmi_version_is_supported(ctx->version)))
		return -1;

	return ops[ctx->version].finalize(ctx);
}

int minipmi_initialized(struct minipmi_context *ctx, int *initialized)
{
	if (unlikely((!ctx) || (!initialized)))
		return -EINVAL;

	*initialized = (1 == ctx->initialized);

	return 0;
}

int minipmi_finalized(struct minipmi_context *ctx, int *finalized)
{
	if (unlikely((!ctx) || (!finalized)))
		return -EINVAL;

	*finalized = (1 == ctx->finalized);

	return 0;
}

int minipmi_get_rank(struct minipmi_context *ctx, int *rank)
{
	if (unlikely((!ctx) || (!rank)))
		return -EINVAL;
	
	if (unlikely(!ctx->initialized || ctx->finalized))
		return -1;

	*rank = ctx->rank;
	
	return 0;
}

int minipmi_get_size(struct minipmi_context *ctx, int *size)
{
	if (unlikely((!ctx) || (!size)))
		return -EINVAL;
	
	if (unlikely(!ctx->initialized || ctx->finalized))
		return -1;

	*size = ctx->size;
	
	return 0;
}

int minipmi_kvs_put(struct minipmi_context *ctx, const char *key, const char *value)
{
	if (unlikely((!ctx) || (!key) || (!value)))
		return -EINVAL;

	if (unlikely(!minipmi_pmi_version_is_supported(ctx->version)))
		return -1;

	return ops[ctx->version].kvs_put(ctx, key, value);
}

int minipmi_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen)
{
	if (unlikely((!ctx) || (!key) || (!value) || (!valuelen)))
		return -EINVAL;

	if (unlikely(!minipmi_pmi_version_is_supported(ctx->version)))
		return -1;

	return ops[ctx->version].kvs_get(ctx, key, value, maxvaluelen, valuelen);
}

int minipmi_kvs_sync(struct minipmi_context *ctx)
{
	if (unlikely(!ctx))
		return -EINVAL;

	if (unlikely(!minipmi_pmi_version_is_supported(ctx->version)))
		return -1;

	return ops[ctx->version].kvs_sync(ctx);
}

