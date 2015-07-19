
#ifndef MINIPMI_MINIPMI_H_INCLUDED
#define MINIPMI_MINIPMI_H_INCLUDED 1

/* libminipmi: Implementation of a small subset of PMI1/PMI2
 *
 * libpmi implements a minimal subset of the PMI1 and PMI2 process
 * management interface standards. 
 */

#define MINIPMI_MAJOR		0
#define MINIPMI_MINOR		1

#define MINIPMI_BUFLEN  	1024	/* == PMIU_MAXLINE */
/* Maximal number of key-value pairs in a command.
 */
#define MINIPMI_MAX_KVPS	16

struct minipmi_context
{
        int     	initialized;
        int     	finalized;

        int     	version;
        int     	subversion;

        int     	rank;
        int     	size;

        int     	fd;

        char    	buf[MINIPMI_BUFLEN];

	int		nkvps;
	const char	*keys[MINIPMI_MAX_KVPS];
	const char	*vals[MINIPMI_MAX_KVPS];

	char		kvsname[MINIPMI_BUFLEN];
};

#define MINIPMI_CONTEXT_INIT { .initialized = 0, .finalized = 0 }

/* Disable a particular PMI version.
 */
int minipmi_disable_version(int version);

/* Initialize the minipmi context and connect to the resource management
 * daemon. 
 */
int minipmi_initialize(struct minipmi_context *ctx);

/* Finalize minipmi.
 */
int minipmi_finalize(struct minipmi_context *ctx);

/* Check if minipmi has been initialized.
 */
int minipmi_initialized(struct minipmi_context *ctx, int *initialized);

/* Check if minipmi has been finalized.
 */
int minipmi_finalized(struct minipmi_context *ctx, int *finalized);

/* Query the rank of the process.
 */
int minipmi_get_rank(struct minipmi_context *ctx, int *rank);

/* Query the size of the world.
 */
int minipmi_get_size(struct minipmi_context *ctx, int *size);

/* Put a value in the key value store. Locally set values are not visible globally until
 * the KVS is synchronized.
 */
int minipmi_kvs_put(struct minipmi_context *ctx, const char *key, const char *value);

/* Retrieve a value from the KVS. The length of the value is returned in the last argument.
 * value should have length >= maxvaluelen.
 */
int minipmi_kvs_get(struct minipmi_context *ctx, const char *key, char *value, int maxvaluelen, int *valuelen);

/* Synchronize the KVS.
 */
int minipmi_kvs_sync(struct minipmi_context *ctx);

#endif

