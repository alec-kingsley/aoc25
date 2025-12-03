#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef int8_t digit_t;

#define PART_1_N_DIGITS 2
#define PART_2_N_DIGITS 12

uint64_t get_battery_joltage(digit_t *battery, size_t n_digits) {
  uint64_t joltage = 0;
  size_t i;
  for (i = 0; i < n_digits; i++) {
    joltage *= 10;
    joltage += battery[i];
  }
  return joltage;
}

uint64_t get_bank_joltage(char *line_buf, size_t line_length, digit_t *battery, size_t n_digits) {
  size_t i, j;
  digit_t contender;
  size_t weakest_link;

  for (i = 0; i < n_digits; i++) {
    battery[n_digits - i - 1] = line_buf[line_length - i - 1] - '0';
  }

  for (i = n_digits; i < line_length; i++) {
    contender = line_buf[line_length - i - 1] - '0';
    if (contender >= battery[0]) {
      weakest_link = 0;
      while (weakest_link < n_digits - 1
             && battery[weakest_link + 1] <= battery[weakest_link]) {
        weakest_link++;
      }
      for (j = weakest_link; j > 0; j--) {
        battery[j] = battery[j - 1];
      }
      battery[0] = contender;
    }
  }

  return get_battery_joltage(battery, n_digits);
}

int main(int argc, char *argv[]) {
  FILE *input;
  uint64_t part_1_total_joltage;
  uint64_t part_2_total_joltage;
  size_t line_length;
  char *line_buf;
  digit_t *battery; 
  size_t i;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");
  line_length = 0;
  while (fgetc(input) != '\n') {
    line_length++;
  }
  fseek(input, 0, SEEK_SET);

  line_buf = calloc(line_length, sizeof(char));
  if (line_buf == NULL) {
    perror("Failed to allocate memory for line_buf");
    exit(1);
  }

  battery = calloc(PART_2_N_DIGITS, sizeof(digit_t));
  if (battery == NULL) {
    perror("Failed to allocate memory for battery");
    exit(1);
  }

  part_1_total_joltage = 0;
  part_2_total_joltage = 0;
  do {
    for (i = 0; i < line_length; i++) {
      line_buf[i] = fgetc(input);
    }
     part_1_total_joltage +=
       get_bank_joltage(line_buf, line_length, battery, PART_1_N_DIGITS);
     part_2_total_joltage +=
       get_bank_joltage(line_buf, line_length, battery, PART_2_N_DIGITS); 
  } while(fgetc(input) == '\n');

  free(line_buf);
  free(battery);

  printf("Part 1 total joltage: %lu\n", part_1_total_joltage);
  printf("Part 2 total joltage: %lu\n", part_2_total_joltage);

  return 0;
}
