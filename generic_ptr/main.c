#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

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

typedef struct {
    uintptr_t data;
    size_t size_of;
} vec_t;

static void
banana_init(banana_t * banana, char * name, enum ripeness level) {
    banana->name = name;
    banana->ripeness = level;
    banana->expiration_date = 0;
}

static void
print_banana(banana_t * banana) {
    printf("Banana: %s\n", banana->name);
    switch (banana->ripeness) {
        case underripe: printf("Not quite good yet");           break;
        case ripe:      printf("Good to eat!");                 break;
        case overripe:  printf("Oops... left out too long :("); break;
    }
    printf("\n");
}

static void
vec_iter(vec_t * vec) {
    vec->data = vec->data + vec->size_of;
}

int main(void) {
    // Disable stdout buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    banana_t * banana;
    banana = malloc(sizeof(banana_t) * 3);
    banana_init(banana + 0, "First" , underripe);
    banana_init(banana + 1, "Second", ripe);
    banana_init(banana + 2, "Third" , overripe);

    assert(banana == &banana[0]); // Just to remember what's going on here.

    print_banana(&banana[0]);
    print_banana(&banana[1]);
    print_banana(&banana[2]);

    // Now we go bonkers
    vec_t * vec = malloc(sizeof(vec_t));
    vec->data = (uintptr_t) banana;
    vec->size_of = sizeof(banana_t);

    printf("Size of banana: %zd\n", vec->size_of);

    print_banana((banana_t *) vec->data);
    vec_iter(vec);
    print_banana((banana_t *) vec->data);
    vec_iter(vec);
    print_banana((banana_t *) vec->data);

    vec_iter(vec); // no one complains

    return 0;
}

