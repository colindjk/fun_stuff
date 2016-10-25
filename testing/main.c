#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

struct string {
	char *value;
	struct list_elem elem;
};

struct string *
string_init(char * str)
{
	struct string * s = malloc(sizeof(struct string));
	struct list_elem e;
	e.prev = NULL;
	e.next = NULL;
	s->value = str;
	s->elem = e;
	return s;
}

static bool
find_cow(struct list_elem * e)
{
	struct string * s = list_entry(e, struct string, elem);
	if (strcmp(s->value, "cow") == 0) {
		printf("found cow\n");
		return true;
	}
	printf("no cow\n");
	return false;
}

void *
find_donkey_struct(struct list_elem * e)
{
	struct string * s = list_entry(e, struct string, elem);
	if (strcmp(s->value, "donkey") == 0) {
		printf("found donkey\n");
		return s;
	}
	printf("no donkey\n");
	return NULL;
}

int
main() {
	char * a = strdup("apple");
	char * b = strdup("banana");
	char * c = strdup("cow");
	char * d = strdup("donkey");
	char * e = strdup("epsilon");

	struct list * str_list = malloc(sizeof(struct list));
	list_init(str_list);

	list_push_back(str_list, &string_init(a)->elem);
	list_push_back(str_list, &string_init(b)->elem);
	list_push_back(str_list, &string_init(c)->elem);
	list_push_back(str_list, &string_init(d)->elem);
	list_push_back(str_list, &string_init(e)->elem);

	struct list_elem * cow_elem = list_find(str_list, find_cow, 0);
	struct string * s = list_entry(cow_elem, struct string, elem);
	printf("Found the %s\n", s->value);

	struct string *s2j = list_find_struct(str_list, find_donkey_struct);
	printf("Found the %s\n", s2j->value);

}
