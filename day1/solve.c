#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define START_DIAL_POSITION 50
#define MODULUS 100

int mod(int dividend, int modulus) {
  int mod = dividend % modulus;
  if (mod < 0) {
    mod += modulus;
  }
  return mod;
}

int main(int argc, char *argv[]) {
  FILE *input;
  bool right;
  int direction;
  bool was_at_zero;
  int diff;
  int16_t position;
  uint32_t password_1;
  uint32_t password_2;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");

  position = START_DIAL_POSITION;

  password_1 = 0;
  password_2 = 0;
  
  direction = fgetc(input);
  while (direction != EOF) {
    assert(direction == 'R' || direction == 'L');
    right = direction == 'R';

    fscanf(input, "%d", &diff);

    was_at_zero = position == 0;

    position += (right ? 1 : -1) * diff;

    password_2 += abs(position) / MODULUS + (position <= 0 && !was_at_zero);
    position = mod(position, MODULUS);
    password_1 += position == 0;

    fgetc(input); /* new line */

    direction = fgetc(input);
  }

  fclose(input);

  printf("Part 1 password: %i\n", password_1);
  printf("Part 2 password: %i\n", password_2);

  return 0;
}
