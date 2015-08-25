#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// http://cryptopals.com/sets/1/challenges/5/

const char const key[] = "ICE";

void read_and_encrypt_file(const char* filename)
{
   unsigned char* data = NULL;
   int fd = -1;
   size_t key_index = 0;
   ssize_t ret = 0;

   // allocate buffer
   data = (char*)malloc(512);
   if (!data) {
      printf("failed to allocate buffer\n");
      _exit(-2);
   }
   // read file to buffer
   fd = open(filename, O_RDONLY);
   if (0 > fd) {
      printf("failed to open file [%s]: %m\n", filename);
      _exit(-3);
   }
   while (ret = read(fd, data, 512)) {
      int i = 0;
      if (0 > ret) {
         printf("error while reading from file [%s]: %m\n", filename);
         _exit(-3);
      }
      for (i = 0; i < ret; ++i) {
         printf("%02x", data[i] ^ key[key_index]);
         ++key_index;
         key_index %= strlen(key);
      }
   }
   close(fd);
   printf("\n");
}

int main(int argc, char *argv[])
{
   const char* file = NULL;
   if (2 <= argc) {
      file = argv[1];
   }
   else {
      file = "5.txt";
   }
   read_and_encrypt_file(file);

   return 0;
}
