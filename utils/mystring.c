#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "corelib.h"
#include "mystring.h"

string new_string(int max_length)
{
    string s = calloc(max_length + 1, sizeof(char));
    return s;
}

string concat_string(string s1, string s2)
{
    int s1len = strlen(s1);
    int s2len = strlen(s2);
    string s = new_string(s1len + s2len);
    int i = 0;
    for (;i < s1len;i++)
        i[s] = i[s1];
    string sp = s + s1len;
    for (i = 0;i < s2len;i++)
        i[sp] = i[s2];
    return s;
}