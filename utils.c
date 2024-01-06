#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
  Function finds the first token in string *stringp that is delimited by all
  bytes in the string delim. This token is terminated by overwriting the
  delimiter with a null bytes ('\0'), and *stringp is updated to point past the
  token. In case no delimiter was found, the token is taken to be the entire
  string *stringp, and *stringp is made NULL. If *stringp is NULL, the
  strsplit() function returns NULL and does nothing. Note that *stringp CANNOT
  be a string literal.
*/
char* strsplit(char** stringp, const char* delim) {
  // May want to strdup to ensure it is dynamically allocated.
  if (*stringp == NULL) {
    return NULL;
  }

  char* start = *stringp;
  char* delimP = strstr(*stringp, delim);
  if (delimP == NULL) {
    // Cannot find delimiter, return entire string.
    // Set *stringp to NULL
    *stringp = NULL;
    return start;
  }

  for (size_t i = 0; i < strlen(delim); i++) {
    *delimP = '\0';
    delimP += 1;
  }
  // Address of *stringp is modified.
  *stringp = delimP;
  return start;
}