#ifndef CM_ERROR_H
#define CM_ERROR_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

void	error_msg(const char *file, const int line);

#define CMERR() error_msg(__FILE__, __LINE__)

#endif
