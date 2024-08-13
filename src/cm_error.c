#include "cm_error.h"

void
error_msg(const char *file, const int line)
{
	char msg[1000];
	sprintf(msg, "%s:%d", file, line);
	perror(msg);
}
