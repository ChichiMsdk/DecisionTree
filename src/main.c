#define _CRT_SECURE_NO_WARNINGS

#include "cm_error.h"
#include "csv.h"

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
			ds.fields[2].header, ds.fields[3].header,  ds.fields[4].header);  
	int i = 0;
	while (i < 5)
	{
		printf("header[%d]: %s\n", i, ds.fields[i].header);
		i++;
	}
	i = 0;
	printf("second scanf\n");
	while ((fscanf(file, "%f,%f,%f,%f,%f", &ds.fields[0].entries[i], &ds.fields[1].entries[i],
				&ds.fields[2].entries[i], &ds.fields[3].entries[i], &ds.fields[4].entries[i]) >= 5))
	{

		printf("[%d]\t %f, %f, %f, %f, %f\n", i, ds.fields[0].entries[i], ds.fields[1].entries[i],
				ds.fields[2].entries[i], ds.fields[3].entries[i], ds.fields[4].entries[i]);
		i++;
	}
	printf("size:%llu\n", ds.lines);
	fclose(file);
	return ds;
}

void
CM_destroy_dataset(data_set ds)
{
	int i = 0;
	while (i < 5)
	{
		free(ds.fields[i].entries);
		i++;
	}
}

int
main(int ac, char *av[])
{
	if (ac != 2)
	{
		fprintf(stderr, "Usage: \"./tree <file.csv>\"\n");
		return 1;
	}
	data_set ds = load_dataset_csv(av[1]);
	CM_destroy_dataset(ds);
	return 0;
}
