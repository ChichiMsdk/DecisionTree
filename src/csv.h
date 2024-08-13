#ifndef CSV_H
#define CSV_H

#include "cm_error.h"

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


int			count_lines_with_numbers(FILE *file);
data_set	allocate_struct(data_set ds);
data_set	load_dataset_csv(const char *fname);
void		CM_destroy_dataset(data_set ds);


#endif
