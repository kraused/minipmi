
#include <string.h>
#include <stdio.h>

#include "minipmi.h"

int main(int argc, char **argv)
{
	int err, x, i;
	int rank, size;
	struct minipmi_context ctx = MINIPMI_CONTEXT_INIT;
	char value[64], key[64], buf[64];

#if defined(DISABLE_VERSION)
	minipmi_disable_version(DISABLE_VERSION);
#endif

	err = minipmi_initialize(&ctx);
	fprintf(stdout, "minipmi_initialize(): %d\n", err);
	fflush (stdout);

	err = minipmi_get_rank(&ctx, &rank);
	fprintf(stdout, "minipmi_get_rank(): %d %d\n", err, rank);
	fflush (stdout);

	err = minipmi_get_size(&ctx, &size);
	fprintf(stdout, "minipmi_get_size(): %d %d\n", err, size);
	fflush (stdout);

	snprintf(key, sizeof(key), "A%d", rank);
	snprintf(value, sizeof(value), "B%d", rank);

	err = minipmi_kvs_put(&ctx, key, value);
	fprintf(stdout, "minipmi_kvs_put(): %d\n", err);
	fflush (stdout);

	snprintf(key, sizeof(key), "U%d", rank);
	snprintf(value, sizeof(value), "V%d", rank);

	err = minipmi_kvs_put(&ctx, key, value);
	fprintf(stdout, "minipmi_kvs_put(): %d\n", err);
	fflush (stdout);

	err = minipmi_kvs_sync(&ctx);
	fprintf(stdout, "minipmi_kvs_sync(): %d\n", err);
	fflush (stdout);

	for (i = 0; i < size; ++i) {
		snprintf(key, sizeof(key), "A%d", i);
		snprintf(buf, sizeof(buf), "B%d", i);

		err = minipmi_kvs_get(&ctx, key, value, sizeof(value), &x);
		fprintf(stdout, "minipmi_kvs_get(): %d %d '%s'\n", err, x, value);
		fflush (stdout);

		if (strcmp(value, buf)) {
			fprintf(stdout, "Consistency check failed.\n");
			fflush (stdout);
		}
	}

	for (i = (size - 1); i >= 0; --i) {
		snprintf(key, sizeof(key), "U%d", i);
		snprintf(buf, sizeof(buf), "V%d", i);

		err = minipmi_kvs_get(&ctx, key, value, sizeof(value), &x);
		fprintf(stdout, "minipmi_kvs_get(): %d %d '%s'\n", err, x, value);
		fflush (stdout);

		if (strcmp(value, buf)) {
			fprintf(stdout, "Consistency check failed.\n");
			fflush (stdout);
		}
	}
		
	snprintf(key, sizeof(key), "X");
	err = minipmi_kvs_get(&ctx, key, value, sizeof(value), &x);
	fprintf(stdout, "minipmi_kvs_get(): %d %d '%s'\n", err, x, value);
	fflush (stdout);

	err = minipmi_finalize(&ctx);
	fprintf(stdout, "minipmi_finalize(): %d\n", err);
	fflush (stdout);

	return 0;
}

