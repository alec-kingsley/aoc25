#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DYN_LIST_DEFAULT_CAP 32

typedef uint64_t dyn_list_t;

typedef struct {
  size_t len;
  size_t cap;
  dyn_list_t *values;
} DynList;

DynList *dyn_list_create() {
  DynList *new = calloc(1, sizeof(DynList));
  if (new == NULL) {
    goto dyn_list_create_fail;
  }
  new->len = 0;
  new->cap = DYN_LIST_DEFAULT_CAP;
  new->values = calloc(new->cap, sizeof(dyn_list_t));

  return new;
dyn_list_create_fail:
  if (new != NULL) {
    free(new);
  }
  perror("Failed to create DynList");
  exit(1);
}

void dyn_list_destroy(DynList *list) {
  if (list != NULL) {
    free(list->values);
  }
  free(list);
}

void dyn_list_grow(DynList *list) {
  list->cap <<= 1;
  list->values = realloc(list->values, list->cap * sizeof(dyn_list_t));
  if (list->values == NULL) {
    perror("Failed to grow DynList");
    exit(1);
  }
}

void dyn_list_insert(DynList *list, size_t index, dyn_list_t value) {
  if (list->len == list->cap) {
    dyn_list_grow(list);
  }
  if (index < list->len) {
    memmove(&list->values[index + 1],
            &list->values[index],
            (list->len - index) * sizeof(dyn_list_t));
  }
  list->values[index] = value;
  list->len++;
}

void dyn_list_remove(DynList *list, size_t index) {
  memmove(&list->values[index],
          &list->values[index + 1],
          (list->len - index) * sizeof(dyn_list_t));
  list->len--;
}

void update_ranges(DynList *range_mins, DynList *range_maxs,
                   dyn_list_t min, dyn_list_t max) {
  int64_t i_too_low = -1;
  int64_t i_high_enough = range_mins->len;
  size_t i_guess;
  size_t i;

  while (i_high_enough - i_too_low > 1) {
    i_guess = (i_too_low + i_high_enough) / 2;
    if (range_mins->values[i_guess] < min) {
      i_too_low = i_guess;
    } else {
      i_high_enough = i_guess;
    }
  }
  i = i_high_enough;

  dyn_list_insert(range_mins, i, min);
  dyn_list_insert(range_maxs, i, max);

  while (range_maxs->len > i + 1 && range_maxs->values[i + 1] <= max) {
    dyn_list_remove(range_mins, i + 1);
    dyn_list_remove(range_maxs, i + 1);
  }

  while (range_maxs->len > i + 1 && max >= range_mins->values[i + 1]) {
    dyn_list_remove(range_mins, i + 1);
    dyn_list_remove(range_maxs, i);
  }

  if (i > 0 && min <= range_maxs->values[i - 1]) {
    dyn_list_remove(range_mins, i);
    if (range_maxs->values[i - 1] < max) {
      dyn_list_remove(range_maxs, i - 1);
    } else {
      dyn_list_remove(range_maxs, i);
    }
  }
}

void read_ranges(FILE *input, DynList *range_mins, DynList *range_maxs) {
  dyn_list_t min;
  dyn_list_t max;
  int c;
  while (isdigit(c = fgetc(input))) {
    ungetc(c, input);

    fscanf(input, "%lu", &min);
    assert(fgetc(input) == '-');
    fscanf(input, "%lu", &max);

    update_ranges(range_mins, range_maxs, min, max);

    assert(fgetc(input) == '\n');
  }
}

bool is_fresh(dyn_list_t ingredient_id, DynList* range_mins, DynList *range_maxs) {
  size_t i;
  for (i = 0; i < range_mins->len; i++) {
    if (ingredient_id >= range_mins->values[i]
        && ingredient_id <= range_maxs->values[i]) {
      return true;
    }
  }
  return false;  
}

uint32_t get_fresh_ct(FILE *input, DynList *range_mins, DynList *range_maxs) {
  uint32_t fresh_ct = 0;
  dyn_list_t value;
  int c;

  while (isdigit(c = fgetc(input))) {
    ungetc(c, input);
    fscanf(input, "%lu", &value);

    fresh_ct += is_fresh(value, range_mins, range_maxs) ? 1 : 0;
    assert((c = fgetc(input)) == '\n' || c == EOF);
  }
  
  return fresh_ct;
}

dyn_list_t get_total_fresh_ids(DynList *range_mins, DynList *range_maxs) {
  dyn_list_t total_fresh_ids = 0;
  size_t i;

  assert(range_mins->len == range_maxs->len);

  for (i = 0; i < range_mins->len; i++) {
    total_fresh_ids += range_maxs->values[i] - range_mins->values[i] + 1;
  }

  return total_fresh_ids;
}

int main(int argc, char *argv[]) {
  FILE *input;
  DynList *range_mins = dyn_list_create();
  DynList *range_maxs = dyn_list_create();
  uint32_t fresh_ct;
  dyn_list_t total_fresh_ids;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");
  if (input == NULL) {
    perror("Failed to open input file for reading");
    exit(1);
  }

  read_ranges(input, range_mins, range_maxs);
  fresh_ct = get_fresh_ct(input, range_mins, range_maxs);
  total_fresh_ids = get_total_fresh_ids(range_mins, range_maxs);

  printf("Fresh count: %d\n", fresh_ct);
  printf("Total fresh ids: %lu\n", total_fresh_ids);

  dyn_list_destroy(range_mins);
  dyn_list_destroy(range_maxs);

  return 0;
}
