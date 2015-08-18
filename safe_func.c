#include "safe_func.h"
#include <stdio.h>
void *safe_malloc(size_t __size)
{
	void *p;
	p = malloc(__size);
	if(NULL != p)
	{
		return p;
	}
	perror("malloc:");
	exit(1);
}
void *safe_free(void *__ptr)
{

	if(NULL != __ptr)
		free(__ptr);
	return NULL;
}