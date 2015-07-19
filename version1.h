
#ifndef MINIPMI_VERSION1_H_INCLUDED
#define MINIPMI_VERSION1_H_INCLUDED 1

struct minipmi_context;

int minipmi_version1_initialize(struct minipmi_context *ctx);
int minipmi_version1_finalize(struct minipmi_context *ctx);
int minipmi_version1_kvs_put(struct minipmi_context *ctx, const char *key, const char *value);
int minipmi_version1_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen);
int minipmi_version1_kvs_sync(struct minipmi_context *ctx);

#endif

