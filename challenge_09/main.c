#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// http://cryptopals.com/sets/2/challenges/9/

void print_padded(unsigned char* data, size_t data_len)
{
   int i = 0;
   for (i = 0; i < data_len; ++i) {
      if (isprint(data[i])) {
         printf("%c", data[i]);
      } else {
         printf("\\x%02x", data[i]);
      }
   }
}

unsigned char* pad_pkcs7(unsigned char* data, size_t* data_len, size_t multiple)
{
   unsigned char* padded = NULL;
   size_t padded_len = ((*data_len / multiple) + 1) * multiple;
   int i = 0;
   
   padded = (unsigned char*)malloc(padded_len);
   if (!padded) {
      printf("failed to allocate %d bytes\n", padded_len);
      _exit(-1);
   }
   memcpy(padded, data, *data_len);
   for (i = *data_len; i < padded_len; ++i) {
      padded[i] = padded_len - *data_len;
   }
   *data_len = padded_len;

   print_padded(padded, padded_len);

   return padded;
}

int main(int argc, char *argv[])
{
   unsigned char input[] = "YELLOW SUBMARINE";
   size_t len = 0;
   
   len = strlen(input);
   pad_pkcs7(input, &len, 20);
   printf("\n");

   // TODO free buffers
   return 0;
}
