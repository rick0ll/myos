
#include <stdlib.h>

int itoa(int num, char number[11]) {

  if (num == 0) {
    number[0] = '0';
    number[1] = '\0';
    return 2;
  }

  int i = 0, start;
  if (num < 0) {
    number[0] = '-';
    num = -num;
    i = 1;
  }

  start = i;

  char figure;
  while (i < 11 && num != 0) {
    figure = num % 10;
    num /= 10;

    number[i++] = figure + '0';
  }

  int end = i - 1;
  while (start < end) {
    SWAP(char, number[start], number[end]);
    start++;
    end--;
  }

  number[i] = '\0';
  return i;
}
