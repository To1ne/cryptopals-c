#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// http://cryptopals.com/sets/1/challenges/6/

unsigned char hex_ascii_to_bin(unsigned char c) {
   switch (c) {
      case '0' ... '9' : return c - '0';
      case 'a' ... 'f' : return c - 'a' + 10;
      case 'A' ... 'F' : return c - 'A' + 10;
   }
   return '\0';
}

unsigned char* read_file(const char* filename, size_t* len)
{
   unsigned char* data = NULL;
   ssize_t total = 0;
   size_t filesize = 0;

   {  // get file size
      struct stat info;
      if (stat(filename, &info)) {
         printf("failed to get file info [%s]: %m\n", filename);
         _exit(-1);
      }
      filesize = info.st_size;
   }
   {  // allocate buffer
      data = (char*)malloc(filesize);
      if (!data) {
         printf("failed to allocate buffer\n");
         _exit(-2);
      }
   }
   {  // read file to buffer
      int fd = open(filename, O_RDONLY);
      if (0 > fd) {
         printf("failed to open file [%s]: %m\n", filename);
         _exit(-3);
      }
      while (total < (ssize_t)filesize) {
         ssize_t now = read(fd, &data[total], filesize - total);
         if (0 > now) {
            printf("error while reading from file [%s]: %m\n", filename);
            _exit(-3);
         }
         if (0 == now) {
            printf("end-of-file [%s]\n");
            break;
         }
         total += now;
      }
      *len = (size_t)total;
      close(fd);
   }
   return data;
}

char char_to_base64(int val) {
   switch (val) {
      case 'A' ... 'Z': return (val +  0) - 'A';
      case 'a' ... 'z': return (val + 26) - 'a';
      case '0' ... '9': return (val + 52) - '0';
      case '+':         return 62;
      case '/':         return 63;
   }
   return 0;
}

void base64_to_bin(unsigned char* data, size_t *len)
{
   size_t i = 0;
   size_t j = 0;
   unsigned int c = 0;
   size_t shift = 3;
   
   // walk the buffer
   for (i = 0; i < *len; ++i) {
      // skip newlines
      if ('\n' == data[i]) {
         continue;
      }
      // convert base64 to bin hex
      c += char_to_base64(data[i]) << (6 * shift);
      if (0 == shift) {
         data[j++] = c >> (2 * 8);
         data[j++] = c >> (1 * 8);
         data[j++] = c >> (0 * 8);
         shift = 3;
         c = 0;
      } else {
         --shift;
      }
   }
   *len = j;
}

size_t hamming_distance(const unsigned char* a, const unsigned char* b, size_t len)
{
   size_t dist = 0;
   size_t i = 0;
   for (i = 0; i < len; ++i) {
      unsigned char c = a[i] ^ b[i];
      while (c) {
         if (c & 0x01) { ++dist; }
         c = (c & 0xff) >> 1;
      }
   }
   return dist;
}

size_t find_best_key_size(unsigned char* data, size_t data_len)
{
   size_t best_size = 0;
   size_t best_score = 1000000;
   int key = 0;

   for (key = 2; key <= 40; ++key) {
      int i = 0;
      size_t score = 0;
      for (i = 0; i < 10; ++i) {  // do the test twice
         score += hamming_distance(&data[i*key], &data[(i+1)*key], key);
      }
      score = score * 10000 / key; // normalize
      //printf("score for key %d is %d\n", key, score);
      if (score < best_score) {
         best_score = score;
         best_size = key;
      }
   }
   //printf("found best key length %d with score %d\n", best_size, best_score);
   return best_size;
}

int score_for_char(char c)
{
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
         break;
//      case 0x80 ... 0xff: points = -1;
   }
   return points;
}

void calculate_key(const unsigned char* data, size_t data_len, unsigned char* key, size_t key_len)
{
   int i = 0;

   for (i = 0; i < key_len; ++i) {
      // calculate single byte at 'i'
      unsigned char best_byte = 0;
      int best_score = 0;
      int byte = 0;
      for (byte = 0; byte <= 255; ++byte) {
         int j = 0;
         int score = 0;
         for (j = i; j < data_len; j += key_len) {
            score += score_for_char(data[j] ^ (unsigned char)byte);
         }
         if (score > best_score) {
            best_score = score;
            best_byte = byte;
         }
      }
      key[i] = best_byte;
      //printf("best byte for %d is 0x%02x with score %d\n", i, best_byte, best_score);
   }
}

void decrypt_repeated_key(const unsigned char* data, size_t data_len, unsigned char* key, size_t key_size)
{
   size_t i = 0;
   size_t k = 0;
   for (i = 0; i < data_len; ++i) {
      printf("%c", data[i] ^ key[k]);
      ++k;
      k %= key_size;
   }
}

int main(int argc, char *argv[])
{
   size_t data_len = 0;
   unsigned char* data = NULL;
   size_t key_size = 0;
   unsigned char* key = NULL;

   {  // test Hamming distance
      const char a[] = "this is a test";
      const char b[] = "wokka wokka!!!";
      size_t dist = hamming_distance(a, b, strlen(a));
      if (37 != dist) {
         printf("Hamming #define istance %d is not 37\n", dist);
         _exit(-1);
      }
   }

   // read file to buffer
   data = read_file("6.txt", &data_len);

   // convert base64 to binary data
   base64_to_bin(data, &data_len);

   // find best key size
   key_size = find_best_key_size(data, data_len);

   // calculate key
   key = (unsigned char*)malloc(key_size);
   calculate_key(data, data_len, key, key_size);

   // decrypt
   decrypt_repeated_key(data, data_len, key, key_size);

   printf("\n");

   // TODO free buffers
   return 0;
}
