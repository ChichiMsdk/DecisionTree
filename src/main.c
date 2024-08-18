#include "csv.h"
#include "linked_list.h"

#include <string.h>

void
add_target(Node **node, data_set *ds, const char *field_name)
{
	int i = 0;
	while (i < 5)
	{
		if (strcmp(field_name, ds->fields[i].header) == 0)
		{
			printf("Found \"%s\" in the data_set\n", field_name);
			(*node)->content = ds->fields[i].entries;
			(*node)->content_size = ds->fields[i].nb_entries;
			return;
		}
		i++;
	}
	fprintf(stderr, "Could not find the target: \"%s\"\n", field_name);
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
	Node *node;
	create_list(&node);
	int index = 3;
	get_min_max_valuef(&ds.fields[index].max_value, &ds.fields[index].min_value, ds.fields + index);
	printf("fields: %s\nmax: %f\tmin: %f\n", ds.fields[index].header, ds.fields[index].max_value, ds.fields[index].min_value);

	// head node's content will point to target
	add_target(&node, &ds, "target");

	// needs to free nodes
	destroy_dataset(ds);
	return 0;
}
