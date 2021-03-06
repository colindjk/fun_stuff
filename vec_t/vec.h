#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#define vec_get(STRUCT, INDEX) \
        (void *)((STRUCT)->data + (((STRUCT)->size_of)*(INDEX)))

#define vec_next(STRUCT) \
    if ((STRUCT)->cur >= (STRUCT)->data + ((STRUCT)->size_of * (STRUCT)->len)) { NULL }\
    else {(void *)((STRUCT)->cur = cur + size_of)}
#define vec_prev(STRUCT) \
    if ((STRUCT)->cur < (STRUCT)->data) { NULL }\
        (void *)((STRUCT)->cur = cur - size_of)
//#define vec_to_iter()

#define to_vec(STATIC_ARRAY) \


/* You think the 'void *' is your ally? */

typedef struct {
    const uintptr_t data;
    const size_t size_of;
    void (*free_data)(void *);
    uintptr_t cur;
    int len;
    int max_len; // Temporary? I'd like to be able to change it lulz, but
} vec_t;         // what about memcpy()?

// Defined by it's functions.
// Basically a place to store certain types of code for the purpose of
// encapsulation.
typedef struct {
    void * (*next)(void*);
    void * (*prev)(void*);
    void * val;
    bool (*check)(void*);
} iter_t;

vec_t *
arr_to_vec(void * vec, size_t size, size_t len);
iter_t *
vec_to_iter(vec_t *);
vec_t *
new_vec(size_t size, size_t max_len, void (*)(void *));
void
vec_free(vec_t * vec);

void * vec_push(vec_t *);
void * vec_pop (vec_t *);

bool concat(vec_t *, vec_t *);

