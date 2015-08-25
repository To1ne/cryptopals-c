#include <stdio.h>
#include <string.h>

// http://cryptopals.com/sets/1/challenges/2/

static const char feed[] = "\x1c\x01\x11\x00\x1f\x01\x01\x00\x06\x1a\x02\x4b\x53\x53\x50\x09\x18\x1c";
static const char xor[]  = "\x68\x69\x74\x20\x74\x68\x65\x20\x62\x75\x6c\x6c\x27\x73\x20\x65\x79\x65";

int main(int argc, char *argv[])
{
   const char* f = feed;
   const char* x = xor;

   for (; *x; ++f, ++x) {
      printf("%02x", *f ^ *x);
   }
   printf("\n");

   return 0;
}
