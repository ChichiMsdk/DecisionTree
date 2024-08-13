#ifndef CSV_H
#define CSV_H

#include <stdio.h>

#define HEADER_MAX 120

typedef struct fields
{
	char	header[HEADER_MAX];
	float	*entries;
}fields;

typedef struct data_set
{
	fields	fields[5];
	FILE	*file;
	size_t	lines;
}data_set;

#endif
