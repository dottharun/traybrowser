#include <assert.h>
#include <string.h>

void util_append_char_to_str(char* str, char ch) {
    assert(strlen(str) < 100 && "must be a small string");

    // printf("appending %c to %s\n", ch, str);

    int len      = strlen(str);
    str[len]     = ch;
    str[len + 1] = '\0';
}
