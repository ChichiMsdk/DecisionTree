#include "csv.h"

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
