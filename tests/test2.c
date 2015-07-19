
#include <stdio.h>

#include "minipmi.h"

int main(int argc, char **argv)
{
	int err, x;
	struct minipmi_context ctx = MINIPMI_CONTEXT_INIT;

#if defined(DISABLE_VERSION)
	minipmi_disable_version(DISABLE_VERSION);
#endif

	err = minipmi_initialize(&ctx);
	fprintf(stdout, "minipmi_initialize(): %d\n", err);
	fflush (stdout);

	err = minipmi_get_rank(&ctx, &x);
	fprintf(stdout, "minipmi_get_rank(): %d %d\n", err, x);
	fflush (stdout);

	err = minipmi_get_size(&ctx, &x);
	fprintf(stdout, "minipmi_get_size(): %d %d\n", err, x);
	fflush (stdout);

	err = minipmi_finalize(&ctx);
	fprintf(stdout, "minipmi_finalize(): %d\n", err);
	fflush (stdout);

	return 0;
}

