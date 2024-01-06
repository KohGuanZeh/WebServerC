#include "utils.h"
#include <stdio.h>
#include <string.h>

void teststrsplit(void);

int main(void) {
  teststrsplit();
  return 0;
}

void teststrsplit(void) {
  printf("Testing strsplit");
  char* str;
  strcpy(str, "This - is - way - 2 - dank");
  while (str != NULL) {
    char* res = strsplit(&str, " - ");
    printf("%s\n", res);
  }
}