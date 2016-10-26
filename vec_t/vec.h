#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

/* You think the void * is your ally? */

typedef struct {
    const uintptr_t data;
    const size_t size_of;
    void   (*init_data_fields)(void *); // obviously, don't malloc() in the init_data.
    void * (*free_data_fields)(void *);
    uintptr_t cur;
    int len;
    int max_len; // Temporary? I'd like to be able to change it lulz, but what about memcpy()?
} vec_t;

// Defined by it's functions.
// Basically a place to store certain types of code for the purpose of
// encapsulation.
typedef struct {
    void * (*next)(void*);
    void * (*prev)(void*);
    bool (*check)(void*);
} iter_t;

vec_t *
arr_to_vec(void * vec, size_t size, size_t len);
iter_t *
vec_to_iter(vec_t *);
vec_t *
new_vec(size_t size, size_t max_len, void (*)(void *), void * (*)(void *));

void * vec_push(vec_t *);
void * vec_pop (vec_t *);

bool concat(vec_t *, vec_t *);
