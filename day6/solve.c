#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t do_homework_incorrectly(FILE *input, size_t line_ct, size_t line_len) {
  uint64_t result = 0;
  uint64_t problem_result = 0;
  uint64_t operand;
  size_t i;
  int c;

  size_t seek_op = 0;

  while (true) {
    fseek(input, line_ct * (line_len + 1) + seek_op, SEEK_SET);
    c = ' ';
    while (c == ' ') {
      c = fgetc(input);
      seek_op++;
    }
    seek_op--;
    if (c == EOF || c == '\n') {
      break;
    }

    assert(c == '+' || c == '*');

    for (i = 0; i < line_ct; i++) {
      fseek(input, i * (line_len + 1) + seek_op, SEEK_SET);
      fscanf(input, "%lu", &operand);
      if (i == 0) {
        problem_result = operand;
      } else {
        problem_result = c == '+'
                            ? problem_result + operand
                            : problem_result * operand;
      }
    }
    result += problem_result;
    seek_op++;
  }

  return result;
}

uint64_t do_homework_correctly(FILE *input, size_t line_ct, size_t line_len) {
  uint64_t result = 0;
  uint64_t problem_result = 0;
  uint64_t operand;
  size_t i;
  int c;
  int number_char;

  size_t seek_op = 0;

  while (true) {
    fseek(input, line_ct * (line_len + 1) + seek_op, SEEK_SET);
    c = ' ';
    while (c == ' ') {
      c = fgetc(input);
      seek_op++;
    }
    seek_op--;
    if (c == EOF || c == '\n') {
      break;
    }

    assert(c == '+' || c == '*');

    problem_result = 0;
    do {
      operand = 0;
      for (i = 0; i < line_ct; i++) {
        fseek(input, i * (line_len + 1) + seek_op, SEEK_SET);
        number_char = fgetc(input);

        if (number_char != ' ') {
          operand *= 10;
          operand += number_char - '0';
        }
      }
      if (operand != 0) {
        if (problem_result == 0) {
          problem_result = operand;
        } else {
          problem_result = c == '+'
                            ? problem_result + operand
                            : problem_result * operand;
        }
      }
      seek_op++;
    } while (operand != 0);
    result += problem_result;
  }

  return result;
}

int main(int argc, char *argv[]) {
  FILE *input;
  uint64_t result;
  int c;
  size_t line_ct = 0;
  size_t line_len = 0;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");
  if (input == NULL) {
    perror("Failed to open input file for reading");
    exit(1);
  }

  while ((c = fgetc(input)) != '+' && c != '*') {
    if (c == '\n') {
      line_ct++;
    } else if (line_ct == 0) {
      line_len++;
    }
  }

  result = do_homework_incorrectly(input, line_ct, line_len);
  printf("Part 1: %lu\n", result);

  result = do_homework_correctly(input, line_ct, line_len);
  printf("Part 2: %lu\n", result);

  return 0;
}
