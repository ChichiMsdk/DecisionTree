#include "csv.h"

void
ds_get_min_max_value(int *max, int *min, fields *field)
{
	int i = 0;
	*max = field->entries[0];
	*min = *max;
	while (i < field->nb_entries)
	{
		if (field->entries[i] >= *max)
			*max = field->entries[i];
		else if (field->entries[i] <= *min)
			*min = field->entries[i];
		i++;
	}
}

void
ds_get_min_max_valuef(float *max, float *min, fields *field)
{
	int i = 0;
	*max = field->entries[0];
	*min = *max;
	while (i < field->nb_entries)
	{
		if (field->entries[i] >= *max)
			*max = field->entries[i];
		else if (field->entries[i] <= *min)
			*min = field->entries[i];
		i++;
	}
}

int
count_lines_with_numbers(FILE *file)
{
	int count = 0;
	char buffer[1024];

	while (fgets(buffer, sizeof(buffer), file)) 
	{
		char *ptr = buffer;
		while (*ptr) 
		{
			if ((*ptr >= '0' && *ptr <= '9') || *ptr == '-' || *ptr == '+') 
			{
				count++;
				break;
			}
			ptr++;
		}
	}
	return count;
}

data_set
allocate_struct(data_set ds)
{
	size_t size = count_lines_with_numbers(ds.file);
	ds.lines = size;
	int i = 0;
	while (i < 5)
	{
		ds.fields[i].entries = malloc(sizeof(float) * size);
		ds.fields[i].nb_entries = size;
		i++;
	}
	return ds;
}

data_set
load_dataset_csv(const char *fname)
{
	data_set ds;

	FILE *file = fopen(fname, "r");
	if (!file)
		CMERR();

	ds.file = file;
	ds = allocate_struct(ds);
	rewind(file);
	fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^\n]", ds.fields[0].header, ds.fields[1].header, 
			ds.fields[2].header, ds.fields[3].header, ds.fields[4].header);  
	int i = 0;
	while ((fscanf(file, "%f,%f,%f,%f,%f", &ds.fields[0].entries[i], &ds.fields[1].entries[i],
				&ds.fields[2].entries[i], &ds.fields[3].entries[i], &ds.fields[4].entries[i]) >= 5))
	{

		i++;
	}
	fclose(file);
	return ds;
}

void
destroy_dataset(data_set ds)
{
	int i = 0;
	while (i < 5)
	{
		free(ds.fields[i].entries);
		i++;
	}
}
