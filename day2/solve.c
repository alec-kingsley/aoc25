#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint64_t min;
  uint64_t max;
} range_t;

range_t read_range(FILE *input) {
  range_t range;
  fscanf(input, "%lu", &(range.min));
  assert(fgetc(input) == '-');
  fscanf(input, "%lu", &(range.max));
  return range;
}

uint8_t get_n_digits(uint64_t val) {
  return val < 10 ? 1 : 1 + get_n_digits(val / 10);
}

bool repeats_with_mod(uint64_t val, uint64_t modulus) {
  uint64_t larger_digits = val / modulus;
  return larger_digits % modulus == val % modulus
        && (larger_digits < modulus
            || repeats_with_mod(larger_digits, modulus));
}

bool is_invalid(uint64_t id, int8_t part) {
  uint8_t n_digits = get_n_digits(id);
  uint64_t ten_pow;
  uint8_t i;

  if (n_digits == 1) return false;

  ten_pow = 10;
  for (i = 1; i * 2 <= n_digits; i++) {
    if ((part == 1 && i * 2 == n_digits)
        || (part == 2 && n_digits % i == 0)) {
      if (repeats_with_mod(id, ten_pow)) return true;
    }
    ten_pow *= 10;  
  }
  return false;
}

uint64_t sum_invalid_ids_in_range(range_t range, uint8_t part) {
  uint64_t invalid_sum = 0;
  uint64_t id;
  for (id = range.min; id <= range.max; id++) {
    if (is_invalid(id, part)) {
      invalid_sum += id;
    }
  }
  return invalid_sum;
}

int main(int argc, char *argv[]) {
  FILE *input;
  range_t range;
  uint64_t invalid_1_sum;
  uint64_t invalid_2_sum;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");

  invalid_1_sum = 0;
  invalid_2_sum = 0;
  do {
    range = read_range(input);

    invalid_1_sum += sum_invalid_ids_in_range(range, 1);
    invalid_2_sum += sum_invalid_ids_in_range(range, 2);
    
  } while (fgetc(input) == ',');

  fclose(input);

  printf("Part 1: %lu\n", invalid_1_sum);
  printf("Part 2: %lu\n", invalid_2_sum);

  return 0;
}
