#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// http://cryptopals.com/sets/1/challenges/4/

struct one_line {
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

struct one_line* read_file(const char* filename)
{
   unsigned char* data = NULL;
   ssize_t total = 0;
   size_t filesize = 0;
   size_t lines = 0;
   struct one_line* all_lines = NULL;

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
      close(fd);
   }
   {  // count lines
      char* p = data;
      for (lines = 0; (p = strchr(p, '\n')); ++lines, ++p) {}
   }
   {  // allocate Lines buffer
      all_lines = (struct one_line*)malloc(sizeof(struct one_line) * (lines + 1));
      if (!all_lines) {
         printf("failed to allocate lines\n");
         _exit(-3);
      }
   }
   {  // store lines
      size_t i = 0;
      unsigned char c = 0;
      size_t pos = 0;
      struct one_line* this_line = all_lines;
      this_line->pos = data;

      // walk the buffer
      for (i = 0, pos = 0; i < filesize; ++i) {
         // on newline, end this line, start a new one
         if ('\n' == data[i]) {
            this_line->len = &data[pos] - this_line->pos;
            ++this_line;
            this_line->pos = &data[pos];
            continue;
         }
         // otherwise convert ascii hex to bin hex
         if (i % 2) {
            c += hex_ascii_to_bin(data[i]);
            data[pos] = c;
            ++pos;
            continue;
         }
         c = hex_ascii_to_bin(data[i]) << 4;
      }
      this_line->pos = 0;       // terminator
      this_line->len = 0;       //
   }
   return all_lines;
}

int main(int argc, char *argv[])
{
   int i = 0;
   int j = 0;
   int best_line = -1;
   int best_key = -1;
   int best_score = -1;
   int best_input = -1;
   struct one_line* lines = read_file("4.txt");

   for (j = 0; (lines[j].pos && lines[j].len); ++j) {
      int key = 0;
      for (key = 0; key <= 255; ++key) {
         int score = 0;
         for (i = 0; i < lines[j].len; ++i) {
            char c = lines[j].pos[i] ^ (char)key;
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
            best_line = j;
            best_key = key;
            best_score = score;
         }
      }
   }
// printf("best line %d best key %d\n", best_line, best_key);
   for (i = 0; i < lines[best_line].len; ++i) {
      printf("%c", lines[best_line].pos[i] ^ (char)best_key);
//    printf("%02x", lines[best_line].pos[i]);
   }
   printf("\n");

   return 0;
}
