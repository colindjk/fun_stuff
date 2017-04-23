#include <stdbool.h>
#include "vm/bit_array.h"

// When a bit is 'flagged', it becomes equal to 1.
// For our purposes, this means that the space is now occupied.

// SWAP_SIZE defined in header
// ARRAY_SIZE also

void toggle_bit (char *array, int index)
{
  array[index / 8] ^= 1 << (index % 8);
}

char get_bit (char *array, int index)
{
  return 1 & (array[index / 8] >> (index % 8));
}

// Returns
// true, succesfully flagged, bit was 0, is now 1
// false, ALREADY flagged, bit was 1, stays 1
bool flag_bit (char * array, int index)
{
  if (get_bit (array, index))
    return false;

  // Otherwise we toggle the bit to make sure it's flagged
  toggle_bit (array, index);
  return true;
}

// Returns
// true, succesfully unflagged, bit was 1, is now 0
// false, ALREADY flagged, bit was 0, stays 0
bool unflag_bit (char * array, int index)
{
  if (!get_bit (array, index))
    return false;

  // Otherwise we toggle the bit to make sure it's flagged
  toggle_bit (array, index);
  return true;
}

// Returns an int value which relates to the index of an UNFLAGGED bit.
// RETURNS
//  - GREATER THAN ZERO -> Succesfully found a bit
//  - NEGATIVE / BELOW ZERO -> No available bits!!!
int get_free (char * array, int size)
{
  int i;
  for (i = 0; i < size; i++)
    if (!get_bit (array, i)) // if the bit @ array[i] == 0
      return i;
  return -1;
}

//  Example:
//
//  int main(void) {
//      /* initialize empty array with the right size */
//      char x[ARRAY_SIZE(SWAP_SIZE)] = { 0 };
//      int i;
//
//      for (i = 0; i < SWAP_SIZE; i += 2)
//          toggle_bit(x, i);
//      toggle_bit(x, 56);
//      for (i = 0; i < SWAP_SIZE; i++)
//          printf("%d: %d\n", i, get_bit(x, i));
//
//      return 0;
//  }
