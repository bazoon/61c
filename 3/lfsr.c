#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


int getBit(int number, int position) {
  int n = 0x0001;
  int i = 0;
  
  n = n << position;
  
  return (n & number) ? 1 : 0;
}

void lfsr_calculate(uint16_t *reg) {
  int b11;
  int b13; 
  int b14;
  int b16;
  int f;
  int b = 0x0001;

  b11 = getBit(*reg, 10);
  b13 = getBit(*reg, 12);
  b14 = getBit(*reg, 13);
  b16 = getBit(*reg, 15);

  f = ((b14 ^ b16) ^ b13) ^ b11;

  printf("here: %d, %d\n", *reg, f);

  *reg = *reg | f;

  unsigned bit;
  bit = (*reg >> 0) ^ (*reg >> 2) ^ (*reg >> 3) ^ (*reg >> 5) & 1;
  *reg = (*reg >> 1) | (bit << 15); 
}

int main() {
  printf("%d", getBit(8, 2));
  int8_t *numbers = (int8_t*) malloc(sizeof(int8_t) * 65535);
  if (numbers == NULL) {
    printf("Memory allocation failed!");
    exit(1);
  }

  memset(numbers, 0, sizeof(int8_t) * 65535);
  uint16_t reg = 0x1;
  uint32_t count = 0;
  int i;

  do {
    count++;
    numbers[reg] = 1;
    if (count < 24) {
      printf("My number is: %u\n", reg);
    } else if (count == 24) {
      printf(" ... etc etc ... \n");
    }
    for (i = 0; i < 32; i++)
      lfsr_calculate(&reg);
  } while (numbers[reg] != 1);

  printf("Got %u numbers before cycling!\n", count);

  if (count == 65535) {
    printf("Congratulations! It works!\n");
  } else {
    printf("Did I miss something?\n");
  }

  free(numbers);

  return 0;
}