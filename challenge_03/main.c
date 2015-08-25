#include <stdio.h>
#include <string.h>
#include <ctype.h>

// http://cryptopals.com/sets/1/challenges/3/

static const char enc[] = "\x1b\x37\x37\x33\x31\x36\x3f\x78\x15\x1b\x7f\x2b\x78\x34\x31\x33\x3d\x78\x39\x78\x28\x37\x2d\x36\x3c\x78\x37\x3e\x78\x3a\x39\x3b\x37\x36";

int main(int argc, char *argv[])
{
   int i = 0;
   int key = 0;
   int best_key = -1;
   int best_score = -1;
   
   for (key = 0; key <= 255; ++key) {
      int score = 0;
      for (i = 0; i < strlen(enc); ++i) {
         char c = enc[i] ^ (char)key;
         int points = 0;
         switch (tolower(c)) {
            case 'e': ++points;
            case 't': ++points;
            case 'a': ++points;
            case 'o': ++points;
            case 'i': ++points;
            case ' ': ++points;
            case 's': ++points;
            case 'h': ++points;
            case 'r': ++points;
            case 'd': ++points;
            case 'l': ++points;
            case 'u': ++points;
               score += points;
               break;
         }
      }
      if (score > best_score) {
         best_key = key;
         best_score = score;
      }
   }
   for (i = 0; i < strlen(enc); ++i) {
      printf("%c", enc[i] ^ (char)best_key);
   }
   printf("\n");
   return 0;
}
