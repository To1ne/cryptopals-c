#include <stdio.h>
#include <string.h>

// http://cryptopals.com/sets/1/challenges/1/

static const char input[] = "\x49\x27\x6d\x20\x6b\x69\x6c\x6c\x69\x6e\x67\x20\x79\x6f\x75\x72\x20\x62\x72\x61\x69\x6e\x20\x6c\x69\x6b\x65\x20\x61\x20\x70\x6f\x69\x73\x6f\x6e\x6f\x75\x73\x20\x6d\x75\x73\x68\x72\x6f\x6f\x6d";

char base64_char(int val) {
   switch (val) {
      case  0 ... 25: return (val -  0) + 'A';
      case 26 ... 51: return (val - 26) + 'a';
      case 52 ... 61: return (val - 52) + '0';
      case 62:        return '+';
      case 63:        return '/';
   }
   return '?';
}

int main(int argc, char *argv[])
{
   int i = 0;

   for (i = 0; i*6 < strlen(input)*8; ++i) {
      const char* p = input + (i*6)/8;
      char val = 0;

      switch(i % 4) {
         case 0:
            val += (*p & 0xFC) >> 2;
            break;
         case 1:
            val += (*p & 0x03) << 4;
            val += *(p+1) >> 4;
            break;
         case 2:
            val += (*p & 0x0F) << 2;
            val += *(p+1) >> 6;
            break;
         case 3:
            val += (*p & 0x3f);
            val += *(p+1) >> 8;
            break;
      }
      printf("%c", base64_char(val));
   }
   printf("\n");

   return 0;
}
