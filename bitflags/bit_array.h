#include "stdbool.h"

#ifndef VM_BIT_ARRAY_H
#define VM_BIT_ARRAY_H

// Define macros / declare bit functions.
#define ARRAY_SIZE(x) (x/8+(!!(x%8)))

bool init_flags (char **);

char get_bit    (char * array, int index);
void toggle_bit (char * array, int index);
bool flag_bit   (char * array, int index);
bool unflag_bit (char * array, int index);

int  get_free   (char * array, int size);


#endif
