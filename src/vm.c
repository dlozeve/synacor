#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE (2 << 15)
#define STACK_SIZE 10000

#define val(r) ((r >> 15) ? reg[r & 7] : r)

size_t read_program(uint16_t mem[static 1], char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  int c = 0;
  size_t n = -1;
  int i = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (i == 0) {
      n++;
      mem[n] = c;
    } else {
      mem[n] = (c << 8) | mem[n];
    }
    i = (i + 1) % 2;
  }
  fclose(fp);

  return n + 1;
}

void run(uint16_t mem[static 1]) {
  uint16_t reg[8] = {0};            // registers
  uint16_t stack[STACK_SIZE] = {0}; // stack
  size_t stack_top = 0;

  size_t pc = 0;
  for (;;) {
    uint16_t opcode = mem[pc];
    uint16_t a = mem[pc + 1];
    uint16_t b = mem[pc + 2];
    uint16_t c = mem[pc + 3];

    switch (opcode) {
    case 0:
      return;
    case 1:
      reg[a & 7] = val(b);
      pc += 3;
      break;
    case 2:
      stack[stack_top] = val(a);
      stack_top++;
      pc += 2;
      break;
    case 3:
      stack_top--;
      reg[a & 7] = stack[stack_top];
      pc += 2;
      break;
    case 4:
      reg[a & 7] = val(b) == val(c);
      pc += 4;
      break;
    case 5:
      reg[a & 7] = val(b) > val(c);
      pc += 4;
      break;
    case 6:
      pc = val(a);
      break;
    case 7:
      if (val(a)) {
        pc = val(b);
      } else {
        pc += 3;
      }
      break;
    case 8:
      if (!val(a)) {
        pc = val(b);
      } else {
        pc += 3;
      }
      break;
    case 9:
      reg[a & 7] = (val(b) + val(c)) % 32768;
      pc += 4;
      break;
    case 10:
      reg[a & 7] = (val(b) * val(c)) % 32768;
      pc += 4;
      break;
    case 11:
      reg[a & 7] = val(b) % val(c);
      pc += 4;
      break;
    case 12:
      reg[a & 7] = val(b) & val(c);
      pc += 4;
      break;
    case 13:
      reg[a & 7] = val(b) | val(c);
      pc += 4;
      break;
    case 14:
      reg[a & 7] = ~val(b) & 0x7FFF;
      pc += 3;
      break;
    case 15:
      reg[a & 7] = mem[val(b)];
      pc += 3;
      break;
    case 16:
      mem[val(a)] = val(b);
      pc += 3;
      break;
    case 17:
      stack[stack_top] = pc + 2;
      stack_top++;
      pc = val(a);
      break;
    case 18:
      if (!stack_top) {
        return;
      }
      stack_top--;
      pc = stack[stack_top];
      break;
    case 19:
      putchar(val(a));
      pc += 2;
      break;
    case 20:
      reg[a & 7] = getchar();
      pc += 2;
      break;
    case 21:
      pc++;
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <program>\n", argv[0]);
    return EXIT_SUCCESS;
  }

  uint16_t mem[MEM_SIZE] = {0};
  read_program(mem, argv[1]);
  run(mem);

  return EXIT_SUCCESS;
}
