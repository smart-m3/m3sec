#define FUSE_USE_VERSION 26
     
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
   
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *second_path = "/second";
static const char *second_str = "This is the second file! It works!\n";
static const char *third_path = "/third";
static const char *third_str = "This is the third, and you shouldn't be reading this.\n";
  17. static const char *fourth_path = "/fourth";
  18. #define fourth_size 20
  19. #define ALPHABET 26
  20.
  21. static int hello_getattr(const char *path, struct stat *stbuf) {
  22. int res = 0;
  23. memset(stbuf, 0, sizeof(struct stat));
  24. if (strcmp(path, "/") == 0) {
  25. stbuf->st_mode = S_IFDIR | 0755;
  26. stbuf->st_nlink = 2;
  27. } else if (strcmp(path, hello_path) == 0) {
  28. stbuf->st_mode = S_IFREG | 0444;
  29. stbuf->st_nlink = 1;
  30. stbuf->st_size = strlen(hello_str);
  31. } else if (strcmp(path, second_path) == 0) {
  32. stbuf->st_mode = S_IFREG | 0444;
  33. stbuf->st_nlink = 1;
  34. stbuf->st_size = strlen(second_str);
  35. } else if (strcmp(path, third_path) == 0) {
  36. stbuf->st_mode = S_IFREG | 0000;
  37. stbuf->st_nlink = 1;
  38. stbuf->st_size = strlen(third_str);
  39. } else if (strcmp(path, fourth_path) == 0) {
  40. stbuf->st_mode = S_IFREG | 0444;
  41. stbuf->st_nlink = 1;
  42. stbuf->st_size = fourth_size;
  43. } else
  44. res = -ENOENT;
  45.
  46. return res;
  47. }
  48.
  49. static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
  50. off_t offset, struct fuse_file_info *fi) {
  51. (void) offset;
  52. (void) fi;
  53.
  54. if (strcmp(path, "/") != 0)
  55. return -ENOENT;
  56. filler(buf, ".", NULL, 0);
  57. filler(buf, "..", NULL, 0);
  58. filler(buf, hello_path + 1, NULL, 0);
  59.
  60. filler(buf, second_path + 1, NULL, 0);
  61.
  62. filler(buf, third_path + 1, NULL, 0);
  63.
  64. filler(buf, fourth_path + 1, NULL, 0);
  65. return 0;
  66. }
  67.
  68. static int hello_open(const char *path, struct fuse_file_info *fi) {
  69. if ((strcmp(path, hello_path) != 0) && (strcmp(path, second_path) != 0)
  70. && (strcmp(path, third_path) != 0) && (strcmp(path, fourth_path)
  71. != 0)){
  72. return -ENOENT;
  73. }
  74.
  75. if ((strcmp(path, hello_path) == 0) || (strcmp(path, second_path) == 0)
  76. || (strcmp(path, fourth_path) == 0)) {
  77. if ((fi->flags & 3) != O_RDONLY)
  78. return -EACCES;
  79. } else if (strcmp(path, third_path) == 0) {
  80. return -EACCES;
  81. }
  82.
  83. return 0;
  84. }
  85.
  86. static int hello_read(const char *path, char *buf, size_t size, off_t offset,
  87. struct fuse_file_info *fi) {
  88. size_t len;
  89. (void) fi;
  90.
  91. if ((strcmp(path, hello_path) != 0) && (strcmp(path, second_path) != 0)
  92. && (strcmp(path, third_path) != 0) && (strcmp(path, fourth_path) != 0))
  93. return -ENOENT;
  94.
  95. if (strcmp(path, hello_path) == 0) {
  96. len = strlen(hello_str);
  97. if (offset < len) {
  98. if (offset + size > len)
  99. size = len - offset;
 100. memcpy(buf, hello_str + offset, size);
 101. } else
 102. size = 0;
 103. } else if (strcmp(path, second_path) == 0) {
 104. len = strlen(second_str);
 105. if (offset < len) {
 106. if (offset + size > len)
 107. size = (len) - offset;
 108. memcpy(buf, second_str + offset, len);
 109. } else
 110. size = 0;
 111. } else if (strcmp(path, third_path) == 0) {
 112. len = strlen(third_str);
 113. if (offset < len) {
 114. if (offset + size > len)
 115. size = (len) - offset;
 116. memcpy(buf, third_str + offset, len);
 117. } else
 118. size = 0;
 119. } else if (strcmp(path, fourth_path) == 0) {
 120. int i;
 121. int fourthLen = rand() % fourth_size;
 122. char *randString;
 123. randString = malloc(fourthLen * sizeof(char));
 124. int randChar;
 125. len = fourthLen;
 126. for (i = 0; i < len; i++) {
 127. randChar = rand() % ALPHABET;
 128. randString[i] = (char) (randChar + 'A');
 129. }
 130. randString[len-1] = '\n';
 131. if (offset < len) {
 132. if (offset + size > len)
 133. size = (len) - offset;
 134. memcpy(buf, randString + offset, len);
 135.
 136. free(randString);
 137. } else
 138. size = 0;
 139. }
 140.
 141. return size;
 142. }
 143.
 144. static struct fuse_operations hello_oper = { .getattr = hello_getattr,
 145. .readdir = hello_readdir, .open = hello_open, .read = hello_read, };
 146.
 147. int main(int argc, char *argv[]) {
 148. return fuse_main(argc, argv, &hello_oper, NULL);
 149. }
