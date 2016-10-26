#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#include "vec.h"

iter_t *
vec_to_iter(vec_t * vec) {
    vec->cur = vec->cur + vec->size_of;
    return NULL;
}

vec_t *
arr_to_vec(void * array_start, size_t size, size_t len)
{

    return NULL;
}

vec_t * new_vec(size_t size, size_t max_len,
        void (*init_data)(void *), void * (*free_data)(void *))
{
    vec_t * vec = malloc(sizeof(vec_t));
    vec->init_data_fields = init_data;
    vec->free_data_fields = free_data;
    void * data = malloc(size * max_len);

    *((uintptr_t*)&(vec->data)) = (uintptr_t) data; // Ummm... yeah....

    return vec;
}

/**
 * Pushes a new value at the end of the vector, returns a ptr to that
 * initialized value to be modified.
 */
void * vec_push(vec_t * vec)
{
    if (vec->len >= vec->max_len) { return NULL; }

    // uhh.... yeahhhhhhh. This is fine.
    void * data = (void *) (vec->data + (vec->size_of * vec->len));
    vec->init_data_fields(data);
    vec->len += 1;
    return data;
}

/**
 * Pop function for vec_t
 */
void * vec_pop (vec_t * vec)
{
    if (vec->len == 0) { return NULL; }
    void * data = (void *) (vec->data + (vec->size_of * vec->len));
    vec->len -= 1;
    return data;
}

