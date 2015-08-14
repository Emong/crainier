#include <stdio.h>
#include "calfunc.h"
t_mesh *get_mesh()
{
	static t_mesh only_mesh;
	return(&only_mesh);
}

void debug(const char * format,...)
{
#ifdef DEBUG1
	va_list args;
 
 	va_start(args,format);
 	vprintf(format,args);
 	va_end(args);
#endif
}
