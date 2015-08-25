#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// http://cryptopals.com/sets/1/challenges/8/

struct block {
      unsigned char* pos;
      size_t len;
};

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
            printf("end-of-file\n");
            break;
         }
         total += now;
      }
      *len = (size_t)total;
      close(fd);
   }
   return data;
}

struct block* hex_to_bin_blocks(unsigned char* data, size_t len)
{
   size_t lines = 0;
   struct block* blocks = NULL;

   {  // count lines
      unsigned char* p = data;
      for (lines = 0; p < (data + len); ++lines, ++p) {
         p = strchr(p, '\n');
      }
   }
   {  // allocate blocks buffer
      blocks = (struct block*)malloc(sizeof(struct block) * (lines + 1));
      if (!blocks) {
         printf("failed to allocate blocks\n");
         _exit(-3);
      }
   }
   {  // store lines
      size_t i = 0;
      unsigned char c = 0;
      size_t pos = 0;
      struct block* this = blocks;
      this->pos = data;

      // walk the buffer
      lines = 0;
      for (i = 0, pos = 0; i < len; ++i) {
         // on newline, end this block, start a new one
         if ('\n' == data[i]) {
            //printf(" <-- len %d\n", i);
            ++lines;
            this->len = &data[pos] - this->pos;
            ++this;
            this->pos = &data[pos];
            continue;
         }
         //printf("%02x", data[i]);
         // otherwise convert ascii hex to bin hex
         if ((i - lines) % 2) {
            c += hex_ascii_to_bin(data[i]);
            data[pos] = c;
            //printf("%02x", data[pos]);
            ++pos;
            continue;
         }
         c = hex_ascii_to_bin(data[i]) << 4;
      }
      this->pos = 0;       // terminator
      this->len = 0;       //
   }
   return blocks;
}

int find_best_aes_block(struct block* blocks)
{
   int best_block = -1;
   int best_score = -1;
   int i = 0;
   
   for (i = 0; (blocks[i].pos && blocks[i].len); ++i) {
      int score = 0;
      int j = 0;
      for (j = 0; j < blocks[i].len; j += 16) {
         int k = 0;
         for (k = j + 16; k < blocks[i].len; k += 16) {
            if (0 == memcmp(&blocks[i].pos[j], &blocks[i].pos[k], 16)) {
               ++score;
            }
         }
      }
      if (score > best_score) {
         //printf("score %d is better than %d for %d\n", score, best_score, i);
         best_score = score;
         best_block = i;
      }
   }

   return best_block;
}

int main(int argc, char *argv[])
{
   size_t data_len = 0;
   unsigned char* data = NULL;
   struct block* blocks = NULL;
   int best = -1;

   // read file to buffer
   data = read_file("8.txt", &data_len);

   // convert hex to binary data blocks
   blocks = hex_to_bin_blocks(data, data_len);

   // find best block
   best = find_best_aes_block(blocks);
   printf("best block is %d\n", best);

   // print it in blocks of 16 byte
   {
      int i = 0;
      for (i = 0; i < blocks[best].len; ++i) {
         printf("%02x", blocks[best].pos[i]);
         if (15 == i % 16) {
            printf("\n");
         }
      }
   }

   // TODO free buffers
   return 0;
}
