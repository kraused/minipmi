
#ifndef MINIPMI_VERSION2_H_INCLUDED
#define MINIPMI_VERSION2_H_INCLUDED 1

struct minipmi_context;

int minipmi_version2_initialize(struct minipmi_context *ctx);
int minipmi_version2_finalize(struct minipmi_context *ctx);
int minipmi_version2_kvs_put(struct minipmi_context *ctx, const char *key, const char *value);
int minipmi_version2_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen);
int minipmi_version2_kvs_sync(struct minipmi_context *ctx);

#endif

