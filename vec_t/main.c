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
banana_init(banana_t * banana, char * name, enum ripeness level) {
    banana->name = name;
    banana->ripeness = level;
    banana->name = name;
}

static void
banana_free(void * banana_ptr) {
    /*banana_t * banana = banana_ptr;*/
    /*free(banana->name);*/
}

static void
print_banana(void * banana_ptr) {
    banana_t * banana = banana_ptr;
    printf("Banana: %s; ", banana->name);
    switch (banana->ripeness) {
        case underripe: printf("Not quite good yet");           break;
        case ripe:      printf("Good to eat!");                 break;
        case overripe:  printf("Oops... left out too long :("); break;
    }
    printf(".\n");
}

int main(void)
{
    vec_t * vec = new_vec(sizeof(banana_t), 3, banana_free);

    banana_init(vec_push(vec), "First", ripe);
    banana_init(vec_push(vec), "Second", underripe);
    banana_init(vec_push(vec), "Third", overripe);

    print_banana(vec_get(vec, 0));
    print_banana(vec_get(vec, 1));
    print_banana(vec_get(vec, 2));

    print_banana(vec_pop(vec));
    banana_init(vec_push(vec), "Fourth", underripe);
    print_banana(vec_pop(vec));

    print_banana(vec_pop(vec));
    print_banana(vec_pop(vec));
    print_banana(vec_pop(vec));

    return 0;
}

