#include "list.h"
#include <assert.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/* Our doubly linked lists have two header elements: the "head"
   just before the first element and the "tail" just after the
   last element.  The `prev' link of the front header is null, as
   is the `next' link of the back header.  Their other two links
   point toward each other via the interior elements of the list.

   An empty list looks like this:

                      +------+     +------+
                  <---| head |<--->| tail |--->
                      +------+     +------+

   A list with two elements in it looks like this:

        +------+     +-------+     +-------+     +------+
    <---| head |<--->|   1   |<--->|   2   |<--->| tail |<--->
        +------+     +-------+     +-------+     +------+

   The symmetry of this arrangement eliminates lots of special
   cases in list processing.  For example, take a look at
   list_remove(): it takes only two pointer assignments and no
   conditionals.  That's a lot simpler than the code would be
   without header elements.

   (Because only one of the pointers in each header element is used,
   we could in fact combine them into a single header element
   without sacrificing this simplicity.  But using two separate
   elements allows us to do a little bit of checking on some
   operations, which can be valuable.) */

static bool is_sorted (struct list_elem *a, struct list_elem *b,
                       list_less_func *less, void *aux);

/* Returns true if ELEM is a head, false otherwise. */
inline bool
is_head (struct list_elem *elem)
{
  return elem != NULL && elem->prev == NULL && elem->next != NULL;
}

/* Returns true if ELEM is an interior element,
   false otherwise. */
static inline bool
is_interior (struct list_elem *elem)
{
  return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

/* Returns true if ELEM is a tail, false otherwise. */
inline bool
is_tail (struct list_elem *elem)
{
  return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

/* Initializes LIST as an empty list. */
void
list_init (struct list *list)
{
  assert (list != NULL);
  list->head.prev = NULL;
  list->head.next = &list->tail;
  list->tail.prev = &list->head;
  list->tail.next = NULL;
}

/* Returns the beginning of LIST.  */
struct list_elem *
list_begin (struct list *list)
{
  assert (list != NULL);
  return list->head.next;
}

/* Returns the element after ELEM in its list.  If ELEM is the
   last element in its list, returns the list tail.  Results are
   undefined if ELEM is itself a list tail. */
struct list_elem *
list_next (struct list_elem *elem)
{
  assert (is_head (elem) || is_interior (elem));
  return elem->next;
}

/* Returns LIST's tail.

   list_end() is often used in iterating through a list from
   front to back.  See the big comment at the top of list.h for
   an example. */
struct list_elem *
list_end (struct list *list)
{
  assert (list != NULL);
  return &list->tail;
}

/* Returns the LIST's reverse beginning, for iterating through
   LIST in reverse order, from back to front. */
struct list_elem *
list_rbegin (struct list *list) 
{
  assert (list != NULL);
  return list->tail.prev;
}

/* Returns the element before ELEM in its list.  If ELEM is the
   first element in its list, returns the list head.  Results are
   undefined if ELEM is itself a list head. */
struct list_elem *
list_prev (struct list_elem *elem)
{
  assert (is_interior (elem) || is_tail (elem));
  return elem->prev;
}

/* Returns LIST's head.

   list_rend() is often used in iterating through a list in
   reverse order, from back to front.  Here's typical usage,
   following the example from the top of list.h:

      for (e = list_rbegin (&foo_list); e != list_rend (&foo_list);
           e = list_prev (e))
        {
          struct foo *f = list_entry (e, struct foo, elem);
          ...do something with f...
        }
*/
struct list_elem *
list_rend (struct list *list) 
{
  assert (list != NULL);
  return &list->head;
}

/* Return's LIST's head.

   list_head() can be used for an alternate style of iterating
   through a list, e.g.:

      e = list_head (&list);
      while ((e = list_next (e)) != list_end (&list)) 
        {
          ...
        }
*/
struct list_elem *
list_head (struct list *list) 
{
  assert (list != NULL);
  return &list->head;
}

/* Return's LIST's tail. */
struct list_elem *
list_tail (struct list *list) 
{
  assert (list != NULL);
  return &list->tail;
}

/* Inserts ELEM just before BEFORE, which may be either an
   interior element or a tail.  The latter case is equivalent to
   list_push_back(). */
void
list_insert (struct list_elem *before, struct list_elem *elem)
{
  assert (is_interior (before) || is_tail (before));
  assert (elem != NULL);
  elem->prev = before->prev;
  elem->next = before;
  before->prev->next = elem;
  before->prev = elem;
}

/* Removes elements FIRST though LAST (exclusive) from their
   current list, then inserts them just before BEFORE, which may
   be either an interior element or a tail. */
void
list_splice (struct list_elem *before,
             struct list_elem *first, struct list_elem *last)
{
  assert (is_interior (before) || is_tail (before));
  if (first == last)
    return;
  last = list_prev (last);

  assert (is_interior (first));
  assert (is_interior (last));

  /* Cleanly remove FIRST...LAST from its current list. */
  first->prev->next = last->next;
  last->next->prev = first->prev;

  /* Splice FIRST...LAST into new list. */
  first->prev = before->prev;
  last->next = before;
  before->prev->next = first;
  before->prev = last;
}

/* Inserts ELEM at the beginning of LIST, so that it becomes the
   front in LIST. */
void
list_push_front (struct list *list, struct list_elem *elem)
{
  list_insert (list_begin (list), elem);
}

/* Inserts ELEM at the end of LIST, so that it becomes the
   back in LIST. */
void
list_push_back (struct list *list, struct list_elem *elem)
{
  list_insert (list_end (list), elem);
}

/* Removes ELEM from its list and returns the element that
   followed it.  Undefined behavior if ELEM is not in a list.

   It's not safe to treat ELEM as an element in a list after
   removing it.  In particular, using list_next() or list_prev()
   on ELEM after removal yields undefined behavior.  This means
   that a naive loop to remove the elements in a list will fail:

   ** DON'T DO THIS **
   for (e = list_begin (&list); e != list_end (&list); e = list_next (e))
     {
       ...do something with e...
       list_remove (e);
     }
   ** DON'T DO THIS **

   Here is one correct way to iterate and remove elements from a
   list:

   for (e = list_begin (&list); e != list_end (&list); e = list_remove (e))
     {
       ...do something with e...
     }

   If you need to free() elements of the list then you need to be
   more conservative.  Here's an alternate strategy that works
   even in that case:

   while (!list_empty (&list))
     {
       struct list_elem *e = list_pop_front (&list);
       ...do something with e...
     }
*/
struct list_elem *
list_remove (struct list_elem *elem)
{
  assert (is_interior (elem));
  elem->prev->next = elem->next;
  elem->next->prev = elem->prev;
  return elem->next;
}

/* Removes the front element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct list_elem *
list_pop_front (struct list *list)
{
  struct list_elem *front = list_front (list);
  list_remove (front);
  return front;
}

/* Removes the back element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct list_elem *
list_pop_back (struct list *list)
{
  struct list_elem *back = list_back (list);
  list_remove (back);
  return back;
}

/* MUST BE TESTED */
void
list_prioritize(struct list * l, struct list_elem * e)
{
    list_remove(e);
    list_push_front(l, e);
}

/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */
struct list_elem *
list_front (struct list *list)
{
  assert (!list_empty (list));
  return list->head.next;
}

/* Returns the back element in LIST.
   Undefined behavior if LIST is empty. */
struct list_elem *
list_back (struct list *list)
{
  assert (!list_empty (list));
  return list->tail.prev;
}

/* Returns the number of elements in LIST.
   Runs in O(n) in the number of elements. */
size_t
list_size (struct list *list)
{
  struct list_elem *e;
  size_t cnt = 0;

  for (e = list_begin (list); e != list_end (list); e = list_next (e))
    cnt++;
  return cnt;
}

/* Returns true if LIST is empty, false otherwise. */
bool
list_empty (struct list *list)
{
  return list_begin (list) == list_end (list);
}

/* Swaps the `struct list_elem *'s that A and B point to. */
static void
swap (struct list_elem **a, struct list_elem **b) 
{
  struct list_elem *t = *a;
  *a = *b;
  *b = t;
}

int
get_index(struct list_elem * e)
{
  int cnt = -1;

  while (!is_head(e)) {
      e = e->prev;
      cnt++;
  }
  return cnt;
}

/**
 * Iterates through a list, executing the function pointer on each element.
 * PARAM l: List to be searched.
 * PARAM f: function which returns a NULL value unless a particular internal
 * PARAM eq: Int value to be passed to f.
 * NOTE: Int implicitly converts to pid_t and size_t.
 */
void *
list_find(struct list * l, void * (*f)(struct list_elem *, int), int eq)
{
    struct list_elem * e = list_head(l);

    void * v = NULL;
    while ((e = list_next(e)) != list_end(l)) {
        v = (*f)(e, eq);
        if (v != NULL) return v;
    }
    return NULL;
}

/**
 * Iterates through a list, executing the function pointer on each element.
 * PARAM l: List to be searched.
 * PARAM f: function which returns a NULL value unless a particular internal
 * NOTE: Int implicitly converts to pid_t and size_t.
 */
struct list *
list_filter(struct list * l, bool (*f)(struct list_elem *))
{
    struct list * true_list = malloc(sizeof(struct list));
    list_init(true_list);

    struct list_elem * e = list_begin(l);
    struct list_elem * removed;

    // while ((e = list_next(e)) != list_end(l)) {
    while (e != list_end(l)) {
        if ((*f)(e)) {
            removed = e;
            e = list_remove(removed);
            //e = e->prev;
            list_push_back(true_list, removed);
        } else { e = list_next(e); }
    }
    return true_list;
}

/**
 * Basic for_each, function will be called on each element in the list.
 * Returns number of times 'f' is executed, aka # of elements in the list
 * unless returning early. (return -1);
 */
int
for_each(struct list * l, int (*f)(struct list_elem *, const int index))
{
    struct list_elem * e = list_head(l);
    struct list_elem * next = e->next;
    int i = 0;

    while ((e = next) != list_end(l)) {
        next = e->next;
        if ((*f)(e, (const int) i) == -1) {
            return i;
        }
        i++;
    }
    return i;
}

/// Meow what do we have here? Opposite of sincat?
/// Something here could explode at any moment, be careful.
/// Returns the length of the newly formed list.
int
concat(struct list * dest, struct list * src)
{
    while (!list_empty(src)) {
       struct list_elem *e = list_pop_front (src);
       list_push_back(dest, e);
    }
    return get_index(&dest->tail);
}


/**
 * Simple function, returns value discussed.
 */
struct list_elem *
list_find_elem(struct list * l, bool (*f)(struct list_elem *))
{
    struct list_elem * e = list_head(l);
    while ((e = list_next(e)) != list_end(l)) {
        if ((*f)(e)) return e;
    }
    return NULL;
}

/* Reverses the order of LIST. */
void
list_reverse (struct list *list)
{
  if (!list_empty (list)) 
    {
      struct list_elem *e;

      for (e = list_begin (list); e != list_end (list); e = e->prev)
        swap (&e->prev, &e->next);
      swap (&list->head.next, &list->tail.prev);
      swap (&list->head.next->prev, &list->tail.prev->next);
    }
}

/* Returns true only if the list elements A through B (exclusive)
   are in order according to LESS given auxiliary data AUX. */
static bool
is_sorted (struct list_elem *a, struct list_elem *b,
           list_less_func *less, void *aux)
{
  if (a != b)
    while ((a = list_next (a)) != b) 
      if (less (a, list_prev (a), aux))
        return false;
  return true;
}

/* Finds a run, starting at A and ending not after B, of list
   elements that are in nondecreasing order according to LESS
   given auxiliary data AUX.  Returns the (exclusive) end of the
   run.
   A through B (exclusive) must form a non-empty range. */
static struct list_elem *
find_end_of_run (struct list_elem *a, struct list_elem *b,
                 list_less_func *less, void *aux)
{
  assert (a != NULL);
  assert (b != NULL);
  assert (less != NULL);
  assert (a != b);
  
  do 
    {
      a = list_next (a);
    }
  while (a != b && !less (a, list_prev (a), aux));
  return a;
}

/* Merges A0 through A1B0 (exclusive) with A1B0 through B1
   (exclusive) to form a combined range also ending at B1
   (exclusive).  Both input ranges must be nonempty and sorted in
   nondecreasing order according to LESS given auxiliary data
   AUX.  The output range will be sorted the same way. */
static void
inplace_merge (struct list_elem *a0, struct list_elem *a1b0,
               struct list_elem *b1,
               list_less_func *less, void *aux)
{
  assert (a0 != NULL);
  assert (a1b0 != NULL);
  assert (b1 != NULL);
  assert (less != NULL);
  assert (is_sorted (a0, a1b0, less, aux));
  assert (is_sorted (a1b0, b1, less, aux));

  while (a0 != a1b0 && a1b0 != b1)
    if (!less (a1b0, a0, aux)) 
      a0 = list_next (a0);
    else 
      {
        a1b0 = list_next (a1b0);
        list_splice (a0, list_prev (a1b0), a1b0);
      }
}

/* Sorts LIST according to LESS given auxiliary data AUX, using a
   natural iterative merge sort that runs in O(n lg n) time and
   O(1) space in the number of elements in LIST. */
void
list_sort (struct list *list, list_less_func *less, void *aux)
{
  size_t output_run_cnt;        /* Number of runs output in current pass. */

  assert (list != NULL);
  assert (less != NULL);

  /* Pass over the list repeatedly, merging adjacent runs of
     nondecreasing elements, until only one run is left. */
  do
    {
      struct list_elem *a0;     /* Start of first run. */
      struct list_elem *a1b0;   /* End of first run, start of second. */
      struct list_elem *b1;     /* End of second run. */

      output_run_cnt = 0;
      for (a0 = list_begin (list); a0 != list_end (list); a0 = b1)
        {
          /* Each iteration produces one output run. */
          output_run_cnt++;

          /* Locate two adjacent runs of nondecreasing elements
             A0...A1B0 and A1B0...B1. */
          a1b0 = find_end_of_run (a0, list_end (list), less, aux);
          if (a1b0 == list_end (list))
            break;
          b1 = find_end_of_run (a1b0, list_end (list), less, aux);

          /* Merge the runs. */
          inplace_merge (a0, a1b0, b1, less, aux);
        }
    }
  while (output_run_cnt > 1);

  assert (is_sorted (list_begin (list), list_end (list), less, aux));
}

/* Inserts ELEM in the proper position in LIST, which must be
   sorted according to LESS given auxiliary data AUX.
   Runs in O(n) average case in the number of elements in LIST. */
void
list_insert_ordered (struct list *list, struct list_elem *elem,
                     list_less_func *less, void *aux)
{
  struct list_elem *e;

  assert (list != NULL);
  assert (elem != NULL);
  assert (less != NULL);

  for (e = list_begin (list); e != list_end (list); e = list_next (e))
    if (less (elem, e, aux))
      break;
  return list_insert (e, elem);
}

/* Iterates through LIST and removes all but the first in each
   set of adjacent elements that are equal according to LESS
   given auxiliary data AUX.  If DUPLICATES is non-null, then the
   elements from LIST are appended to DUPLICATES. */
void
list_unique (struct list *list, struct list *duplicates,
             list_less_func *less, void *aux)
{
  struct list_elem *elem, *next;

  assert (list != NULL);
  assert (less != NULL);
  if (list_empty (list))
    return;

  elem = list_begin (list);
  while ((next = list_next (elem)) != list_end (list))
    if (!less (elem, next, aux) && !less (next, elem, aux)) 
      {
        list_remove (next);
        if (duplicates != NULL)
          list_push_back (duplicates, next);
      }
    else
      elem = next;
}

/* Returns the element in LIST with the largest value according
   to LESS given auxiliary data AUX.  If there is more than one
   maximum, returns the one that appears earlier in the list.  If
   the list is empty, returns its tail. */
struct list_elem *
list_max (struct list *list, list_less_func *less, void *aux)
{
  struct list_elem *max = list_begin (list);
  if (max != list_end (list)) 
    {
      struct list_elem *e;
      
      for (e = list_next (max); e != list_end (list); e = list_next (e))
        if (less (max, e, aux))
          max = e; 
    }
  return max;
}

/* Returns the element in LIST with the smallest value according
   to LESS given auxiliary data AUX.  If there is more than one
   minimum, returns the one that appears earlier in the list.  If
   the list is empty, returns its tail. */
struct list_elem *
list_min (struct list *list, list_less_func *less, void *aux)
{
  struct list_elem *min = list_begin (list);
  if (min != list_end (list)) 
    {
      struct list_elem *e;
      
      for (e = list_next (min); e != list_end (list); e = list_next (e))
        if (less (e, min, aux))
          min = e; 
    }
  return min;
}

// IGNORE NOT NECESSARY FOR NOW
bool
list_plugin_find(struct list * l, bool (*f)(void *, void *), void * v)
{
    struct list_elem * e = list_head(l);

    while ((e = list_next(e)) != list_end(l)) {
        if ((*f)(e, v)) {
            return true;
        }
    }
    return false;
}

// ------ NEW SHIZZ ------

// vec_t
// vec_each -> get each list and do something with it
//  -> Looking at tail.prev and returning
// find_list_beg -> Looking at head.next and returning

/**
 * Constructor for vec_list_t.
 * Takes ONLY a size_t equal to the max length.
 */
vec_t *
new_vec(size_t max_len)
{
    vec_t * vec = malloc(sizeof(vec_t));

    vec->data = malloc(sizeof(void *) * max_len);
    vec->len = 0;
    vec->max_len = max_len;
    return vec;
}

void
free_vec(vec_t * vec)
{
    while (vec->len != 0) {
        void * v = vec_pop(vec);
        vec->free_data(v);
    }
    free(vec); // finally, free the actual 'vec' struct.
}

/* Allocates space for the new data and returns a pointer to the new data.
 * This way, alloc is handled by push and pop, we won't have to worry about it.
 */
void *
vec_push(vec_t * vec)
{
    assert(vec->len < vec->max_len);

    size_t index = vec->len;
    if (index == vec->max_len) return NULL;
    vec->data[index] = vec->new_data();
    vec->len++;

    return vec->data[index];
}

// Pops value off end of the vector, actually fairly useless.
void *
vec_pop(vec_t * vec)
{
    assert(vec->len < vec->max_len);

    int index = ((int) vec->len) - 1;
    if (index == -1) return NULL;
    vec->len--;
    void * data = vec->data[index];
    return data;
}

/**
 * For each of the elements in an array of lists, exec function fn.
 * Remove is not possible for the vec of lists.
 * RETURN -1 TO BREAK
 */
bool vec_each(vec_t * vec, int (*func)(void * v, ...))
{
    assert(vec->len < vec->max_len);
    size_t i;
    for (i = 0; i < vec->len; i++) {
        if (func(vec->data[i], i) == BREAK_LOOP) {
            return i;
        }
    }
    return i;
}

/**
 * Checks the last element of each element the vec holds to see if ya boi
 * needs removal
 * Return: found list_elem VALUE or NULL.
 * RETURN -1 TO BREAK
 */
void *
vec_find(vec_t * vec, void * (*func)(void * data, ...))
{
    size_t i; void * v;
    for (i = 0; i < vec->len; i++) {
        v = func(vec->data[i], i);
        if (v != NULL) {
            return v;
        }
    }
    return NULL; // or 'v', but either way it's NULL.
}

bool
vec_full(vec_t * vec)
{
    return vec->len == vec->max_len;
}

// ignore comments below
    // Notice sizeof params, it's an array of 'struct list', not 'struct list *'
