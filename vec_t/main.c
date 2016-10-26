#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "vec.h"

/**
 * So I was originally making a vec_t for a class project, and I defined it as
 * such:
 *  { void ** data, size_t len, .. }
 * I realized that the 'void **' was iffy... basically iteration was too
 * expensive if for each value, I had to deref a pointer. Instead whatever was
 * stored should be stored in contiguous memory-space, that way walking up and
 * down the vec_t worked the way it was supposed to.
 *
 * So that... took a little work.
 *
 * TODO: Make a generic iterator!
 */

enum ripeness {
    underripe,
    ripe,
    overripe,
};

typedef struct {
    enum ripeness ripeness;
    char * name;
    long expiration_date;
} banana_t;

static void
banana_field_init(void * banana_ptr) {
    banana_t * banana = banana_ptr;
    // banana->name = malloc(20); // this is really optional.
}

static void
banana_init(banana_t * banana, char * name, enum ripeness level) {
    banana->name = name;
    banana->ripeness = level;
    banana->name = name;
}

static void
banana_field_free(void * banana_ptr) {
    banana_t * banana = banana_ptr;
    free(banana->name);
}

static void
print_banana(void * banana_ptr) {
    banana_t * banana = banana_ptr;
    printf("Banana: %s\n", banana->name);
    switch (banana->ripeness) {
        case underripe: printf("Not quite good yet");           break;
        case ripe:      printf("Good to eat!");                 break;
        case overripe:  printf("Oops... left out too long :("); break;
    }
    printf("\n");
}

int main(void)
{

    /*banana_t * banana;*/

    /*print_banana((banana_t *) vec->data);*/

    return 0;
}

