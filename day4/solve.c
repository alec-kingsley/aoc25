#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PAPER_ROLL '@'

#define MAX_ADJACENT_PAPER_ROLLS 3

typedef struct CafeteriaQueueCell CafeteriaQueueCell;

struct CafeteriaQueueCell {
  size_t x;
  size_t y;
  CafeteriaQueueCell *next;
};

typedef struct {
  CafeteriaQueueCell *next;
} CafeteriaQueue;

void enqueue_cafeteria_queue(CafeteriaQueue *queue, size_t x, size_t y) {
  CafeteriaQueueCell *trav = queue->next;
  CafeteriaQueueCell *new_cell = calloc(1, sizeof(CafeteriaQueueCell));
  if (new_cell == NULL) {
    perror("Failed to allocate memory for cafeteria queue cell");
    exit(1);
  }

  new_cell->x = x;
  new_cell->y = y;
  new_cell->next = NULL;

  if (trav == NULL) {
    queue->next = new_cell;
  } else {
    while (trav->next != NULL) {
      trav = trav->next;
    }
    trav->next = new_cell;
  }
}

void dequeue_cafeteria_queue(CafeteriaQueue *queue, size_t *x, size_t *y) {
  assert(queue->next != NULL);
  *x = queue->next->x;
  *y = queue->next->y;
  queue->next = queue->next->next;
}

CafeteriaQueue *new_cafeteria_queue() {
  CafeteriaQueue *queue = calloc(1, sizeof(CafeteriaQueue));
  if (queue == NULL) {
    perror("Failed to allocate memory for cafeteria queue");
    exit(1);
  }

  queue->next = NULL;
  return queue;
}

bool is_forklift_accessible(bool *cafeteria, size_t width, size_t height, size_t x, size_t y) {
  int8_t di, dj;
  uint8_t adjacent_paper_roll_ct = 0;

  for (di = -1; di <= 1; di++) {
    if ((int32_t)x + di < 0 || x + di >= width) {
      continue;
    }
    for (dj = -1; dj <= 1; dj++) {
      if ((int32_t)y + dj < 0 || y + dj >= height || (di == dj && di == 0)) {
        continue;
      }
      adjacent_paper_roll_ct += cafeteria[(y + dj) * height + x + di] ? 1 : 0;
    }
  }
  return adjacent_paper_roll_ct <= MAX_ADJACENT_PAPER_ROLLS;
}

void add_neighbors_to_queue(bool *cafeteria, CafeteriaQueue *queue, size_t width, size_t height,
                            size_t x, size_t y) {
  int8_t di, dj;

  for (di = -1; di <= 1; di++) {
    if ((int32_t)x + di < 0 || x + di >= width) {
      continue;
    }
    for (dj = -1; dj <= 1; dj++) {
      if ((int32_t)y + dj < 0 || y + dj >= height || (di == dj && di == 0)) {
        continue;
      }
      if (cafeteria[(y + dj) * height + x + di]) {
        enqueue_cafeteria_queue(queue, x + di, y + dj);
      }
    }
  }
}

uint32_t get_forklift_accessible_ct(bool *cafeteria, size_t width, size_t height,
                                    bool explore_deeper) {

  CafeteriaQueue *queue = NULL;
  uint32_t forklift_accessible_ct = 0;
  size_t i, j;

  if (explore_deeper) {
    queue = new_cafeteria_queue();
  }

  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      if (cafeteria[j * width + i]) {
        if (is_forklift_accessible(cafeteria, width, height, i, j)) {
          forklift_accessible_ct++;
          if (explore_deeper) {
            add_neighbors_to_queue(cafeteria, queue, width, height, i, j);
            cafeteria[j * width + i] =  false;
          }
        }
      }
    }
  }

  if (explore_deeper) {
    while (queue->next != NULL) {
      dequeue_cafeteria_queue(queue, &i, &j); 
      if (cafeteria[j * width + i]) {
        if (is_forklift_accessible(cafeteria, width, height, i, j)) {
          forklift_accessible_ct++;
          add_neighbors_to_queue(cafeteria, queue, width, height, i, j);
          cafeteria[j * width + i] = false;
        }
      }
    }
    free(queue);
  }
  return forklift_accessible_ct;
}

bool *build_cafeteria(FILE *input, size_t width, size_t height) {
  int c;
  size_t i, j;
  bool *cafeteria = calloc(width * height, sizeof(bool));
  if (cafeteria == NULL) {
    perror("Failed to allocate memory for cafeteria");
    exit(1);
  }

  do {
    for (i = 0; i <  height; i++) {
      for (j = 0; j < width; j++) {
        cafeteria[i * width+ j]
          = fgetc(input) == PAPER_ROLL;
      }
      c = fgetc(input);
      assert(c == '\n' || c == EOF);
    }
  } while(c == '\n');
  return cafeteria;
}

int main(int argc, char *argv[]) {
  FILE *input;
  size_t line_length;
  size_t line_ct;
  long file_size;
  bool *cafeteria;

  if (argc != 2) {
    printf("Usage: %s <input file>\n", argv[0]);
    exit(1);
  }

  input = fopen(argv[1], "r");
  line_length = 0;
  while (fgetc(input) != '\n') {
    line_length++;
  }
  fseek(input, 0, SEEK_END);
  file_size = ftell(input);
  if (file_size == -1L) {
    perror("Failed to get input file size");
  }
  fseek(input, 0, SEEK_SET);

  line_ct = (file_size + 1) / (line_length + 1);

  cafeteria = build_cafeteria(input, line_length, line_ct);
  printf("Part 1 accessible paper rolls: %d\n",
         get_forklift_accessible_ct(cafeteria, line_length, line_ct, false));
  printf("Part 2 accessible paper rolls: %d\n",
         get_forklift_accessible_ct(cafeteria, line_length, line_ct, true));

  free(cafeteria);

  return 0;
}
