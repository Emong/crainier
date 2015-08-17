#include "nrand.h"
#include <stdlib.h>
inline double nrand()
{
	return (rand()%10000) / 20000.0;
}