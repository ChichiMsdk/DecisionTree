#ifndef CSV_H
#define CSV_H

#include "cm_error.h"

#define HEADER_MAX 120

typedef struct fields
{
	char	header[HEADER_MAX];
	float	*entries;
	size_t	nb_entries;

	float	max_value;
	float	min_value;
}fields;

typedef struct data_set
{
	fields	fields[5];
	FILE	*file;
	size_t	lines;
}data_set;

void		ds_get_min_max_value(int *max, int *min, fields *field);
void		ds_get_min_max_valuef(float *max, float *min, fields *field);
int			count_lines_with_numbers(FILE *file);
data_set	allocate_struct(data_set ds);
data_set	load_dataset_csv(const char *fname);
void		destroy_dataset(data_set ds);

#endif
