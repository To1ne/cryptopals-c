#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <polarssl/aes.h>

// http://cryptopals.com/sets/2/challenges/10/

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

void encrypt_aes_cbc(unsigned char* data, size_t data_len, unsigned char* key, size_t key_len, unsigned char* iv)
{
   int ret = 0;
   aes_context ctx;
   unsigned char enc[16];
   int i = 0;

   ret = aes_setkey_dec(&ctx, key, key_len*8); // length in bits
   if (ret) {
      printf("invalid AES key length %d\n", key_len);
      _exit(-20);
   }
   for (i = 0; i < data_len; i += 16) {
      int j = 0;
      ret = aes_crypt_ecb(&ctx, AES_DECRYPT, &data[i], enc);
      if (ret) {
         printf("failed to encrypt\n");
         _exit(-21);
      }
      for (j = 0; j < 16; ++j) {
         enc[j] ^= iv[j];
         printf("%c", enc[j]);
      }
      memcpy(iv, &data[i], 16);
   }
}

int main(int argc, char *argv[])
{
   size_t data_len = 0;
   unsigned char* data = NULL;
   unsigned char key[] = "YELLOW SUBMARINE";
   unsigned char iv[16] = { 0 };

   // read file to buffer
   data = read_file("10.txt", &data_len);

   // convert base64 to binary data
   base64_to_bin(data, &data_len);

   // encrypt CBC
   encrypt_aes_cbc(data, data_len, key, sizeof(key)-1, iv);

   // TODO free buffers
   return 0;
}
