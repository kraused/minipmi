
#include <stdio.h>

#include "minipmi.h"

int main(int argc, char **argv)
{
	int err, x;
	struct minipmi_context ctx = MINIPMI_CONTEXT_INIT;

#if defined(DISABLE_VERSION)
	minipmi_disable_version(DISABLE_VERSION);
#endif

	err = minipmi_initialized(&ctx, &x);
	fprintf(stdout, "minipmi_initialized(): %d %d\n", err, x);
	fflush (stdout);

	err = minipmi_initialize(&ctx);
	fprintf(stdout, "minipmi_initialize(): %d\n", err);
	fflush (stdout);

	err = minipmi_initialized(&ctx, &x);
	fprintf(stdout, "minipmi_initialized(): %d %d\n", err, x);
	fflush (stdout);

	err = minipmi_finalized(&ctx, &x);
	fprintf(stdout, "minipmi_finalized(): %d %d\n", err, x);
	fflush (stdout);

	err = minipmi_finalize(&ctx);
	fprintf(stdout, "minipmi_finalize(): %d\n", err);
	fflush (stdout);

	err = minipmi_finalized(&ctx, &x);
	fprintf(stdout, "minipmi_finalized(): %d %d\n", err, x);
	fflush (stdout);

	return 0;
}

