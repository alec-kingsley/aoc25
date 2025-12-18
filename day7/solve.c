#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPLITTER_CHAR '^'
#define TACHYON_MANIFOLD_ENTRANCE_CHAR 'S'

typedef enum {
  TACHYON_MANIFOLD_ENTRANCE,
  SPLITTER,
  TACHYON_BEAM,
  VOID
} tachyon_manifold_t;

typedef struct TachyonQueueCell TachyonQueueCell;

struct TachyonQueueCell {
  size_t x;
  size_t y;
  TachyonQueueCell *next;
};

typedef struct {
  TachyonQueueCell *next;
} TachyonQueue;

void enqueue_tachyon_queue(TachyonQueue *queue, size_t x, size_t y) {
  TachyonQueueCell *trav = queue->next;
  TachyonQueueCell *new_cell = calloc(1, sizeof(TachyonQueueCell));
  if (new_cell == NULL) {
    perror("Failed to allocate memory for tachyon queue cell");
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

void dequeue_tachyon_queue(TachyonQueue *queue, size_t *x, size_t *y) {
  assert(queue->next != NULL);
  *x = queue->next->x;
  *y = queue->next->y;
  queue->next = queue->next->next;
}

TachyonQueue *new_tachyon_queue() {
  TachyonQueue *queue = calloc(1, sizeof(TachyonQueue));
  if (queue == NULL) {
    perror("Failed to allocate memory for tachyon queue");
    exit(1);
  }

  queue->next = NULL;
  return queue;
}

uint64_t get_input_ct(tachyon_manifold_t *tachyon_manifold, uint64_t *input_cts,
                      size_t width, size_t height, size_t x, size_t y) {
  /* tachyon_manifold must have tachyon beams flowing already */
  uint64_t input_ct = 0;
  size_t i;

  if (input_cts[y * width + x] != 0) {
    return input_cts[y * width + x];
  }

  if (tachyon_manifold[(y - 1) * width + x] == TACHYON_MANIFOLD_ENTRANCE) {
    input_ct = 1;
  } else {
    if (tachyon_manifold[(y - 1) * width + x] == TACHYON_BEAM) {
      input_ct =
          get_input_ct(tachyon_manifold, input_cts, width, height, x, y - 1);
    }

    for (i = x - 1; i <= x + 1; i += 2) {
      if (tachyon_manifold[y * width + i] == SPLITTER) {
        input_ct +=
            get_input_ct(tachyon_manifold, input_cts, width, height, i, y);
      }
    }
  }
  input_cts[y * width + x] = input_ct;
  return input_ct;
}

uint64_t get_timeline_ct(tachyon_manifold_t *tachyon_manifold, size_t width,
                         size_t height) {
  /* tachyon_manifold must have tachyon beams flowing already */
  uint64_t timeline_ct = 1;
  size_t i;
  uint64_t *input_cts = calloc(width * height, sizeof(uint64_t));
  if (input_cts == NULL) {
    perror("Failed to allocate memory for input cts");
    exit(1);
  }

  for (i = 0; i < width; i++) {
    if (tachyon_manifold[(height - 1) * width + i] == TACHYON_BEAM) {
      timeline_ct += get_input_ct(tachyon_manifold, input_cts, width, height, i,
                                  height - 1);
    }
  }

  free(input_cts);

  return timeline_ct;
}

uint32_t get_splitters_hit(tachyon_manifold_t *tachyon_manifold, size_t width,
                           size_t height) {

  TachyonQueue *queue = NULL;
  size_t i, j;
  uint32_t splitters_hit = 0;

  queue = new_tachyon_queue();

  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      if (tachyon_manifold[j * width + i] == TACHYON_MANIFOLD_ENTRANCE) {
        enqueue_tachyon_queue(queue, i, j + 1);
      }
    }
  }

  while (queue->next != NULL) {
    dequeue_tachyon_queue(queue, &i, &j);
    if (tachyon_manifold[j * width + i] == VOID) {
      tachyon_manifold[j * width + i] = TACHYON_BEAM;
      if (j + 1 < height) {
        enqueue_tachyon_queue(queue, i, j + 1);
      }
    } else if (tachyon_manifold[j * width + i] == SPLITTER) {
      splitters_hit++;
      enqueue_tachyon_queue(queue, i - 1, j);
      enqueue_tachyon_queue(queue, i + 1, j);
    }
  }
  free(queue);
  return splitters_hit;
}

tachyon_manifold_t *build_tachyon_manifold(FILE *input, size_t width,
                                           size_t height) {
  int c;
  size_t i, j;
  tachyon_manifold_t *tachyon_manifold =
      calloc(width * height, sizeof(tachyon_manifold_t));
  if (tachyon_manifold == NULL) {
    perror("Failed to allocate memory for tachyon manifold");
    exit(1);
  }

  do {
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        c = fgetc(input);
        tachyon_manifold[i * width + j] = c == TACHYON_MANIFOLD_ENTRANCE_CHAR
                                              ? TACHYON_MANIFOLD_ENTRANCE
                                          : c == SPLITTER_CHAR ? SPLITTER
                                                               : VOID;
      }
      c = fgetc(input);
      assert(c == '\n' || c == EOF);
    }
  } while (c == '\n');
  return tachyon_manifold;
}

int main(int argc, char *argv[]) {
  FILE *input;
  size_t line_length;
  size_t line_ct;
  long file_size;
  tachyon_manifold_t *tachyon_manifold;

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

  tachyon_manifold = build_tachyon_manifold(input, line_length, line_ct);
  printf("# of splitters hit: %d\n",
         get_splitters_hit(tachyon_manifold, line_length, line_ct));
  printf("# of timelines: %lu\n",
         get_timeline_ct(tachyon_manifold, line_length, line_ct));

  free(tachyon_manifold);

  return 0;
}
