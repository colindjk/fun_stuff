#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#include "vec.h"

vec_t *
arr_to_vec(void * array_start, size_t size_of, size_t len)
{

    return NULL;
}

vec_t * new_vec(size_t size, size_t max_len, void (*free_data)(void *))
{
    vec_t * vec = malloc(sizeof(vec_t));
    vec->free_data = free_data;
    vec->len = 0;
    vec->max_len = max_len;
    void * data = malloc(size * max_len);

    *((size_t*)&(vec->size_of)) = size; // Ummm... yeah....
    *((uintptr_t*)&(vec->data)) = (uintptr_t) data; // Ummm... yeah....

    return vec;
}

/**
 * Pushes a new value at the end of the vector, returns a ptr to that
 * initialized value to be modified.
 * Returns pointer to last elem, ready to be initialized.
 */
void * vec_push(vec_t * vec)
{
    if (vec->len >= vec->max_len) { return NULL; }

    // uhh.... yeahhhhhhh. This is fine.
    void * data = (void *) (vec->data + (vec->size_of * vec->len));
    vec->len += 1;
    return data;
}

/**
 * Pop function for vec_t
 * TODO: memcpy so overwrite is not a problem.
 */
void * vec_pop(vec_t * vec)
{
    if (vec->len == 0) { return NULL; }
    vec->len -= 1;
    void * data = (void *) (vec->data + (vec->size_of * vec->len)); 
    void * ret_data = malloc(vec->size_of);
    return memcpy(ret_data, data, vec->size_of);
}

iter_t *
vec_to_iter(vec_t * vec)
{

    return NULL;
}

void
vec_free(vec_t * vec)
{
    void * v;
    while ((v = vec_pop(vec)) != NULL) {
        vec->free_data(v);
    }
    free(vec);
}

