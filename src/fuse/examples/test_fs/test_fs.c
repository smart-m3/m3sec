/*
  FUSE - test example FS
	- create files, dirs, links 

  gcc -Wall `pkg-config fuse --cflags --libs` test_fs.c -o test_fs
*/

#define FUSE_USE_VERSION 26

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>

#include <stdlib.h>

static const char *file1 = "/file1";
static const char *file1_content = "Hello World!\n";
static const char *file2 = "/file2";
static const char *file2_content = "Hello Hell!\n";
static const char *file3 = "/file3";

static const char *slink1 = "/link1";

static const char *dir1 = "/dir1";

#define file3_size 20
#define ALPHABET 26

// is called to get the attributes of a specific file.
// returns metadata concerning a file specified by path in a special stat structure.
static int test_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;	// dir | rigths
		stbuf->st_nlink = 2;	// hardlinks, 2 - not have another dirs inside ???
	} else if ((strcmp(path, dir1) == 0)) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
	} else if ((strcmp(path, file1) == 0)) {
		stbuf->st_mode = S_IFREG | 0775;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(file1_content);
	} else if ((strcmp(path, file2) == 0)) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(file2_content);
	} else if ((strcmp(path, file3) == 0)) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = file3_size;
	} else if ((strcmp(path, slink1) == 0)) {
		stbuf->st_mode = S_IFLNK | 0755;
		stbuf->st_nlink = 1;
	}
	else
		res = -ENOENT;

	return res;
}

// is called to obtain metainformation about files in a directory
// is used to read directory contents.
static int test_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, file1 + 1, NULL, 0);
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, file2 + 1, NULL, 0);

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, file3 + 1, NULL, 0);
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, slink1 + 1, NULL, 0);
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, dir1 + 1, NULL, 0);

	return 0;
}

// is called to check if user is permitted to open the file using given flags. 
// checks whatever user is permitted to open the /hello file with flags given in the fuse_file_info structure. 
static int test_open(const char *path, struct fuse_file_info *fi)
{
	if ((strcmp(path, file1) != 0) && 
	    (strcmp(path, file2) != 0) &&
	    (strcmp(path, file3) != 0))
		return -ENOENT;

	if ((strcmp(path, file1) == 0) || 
	    (strcmp(path, file2) == 0) ||
	    (strcmp(path, file3) == 0))	
	{
		if ((fi->flags & 3) != O_RDONLY)
			return -EACCES;
	}

	return 0;
}

// is called to read data from already opened file. 
// is used to feed the user with data from the file.
// read content from file
static int test_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;

	if ((strcmp(path, file1) != 0) && 
	    (strcmp(path, file2) != 0) &&
	    (strcmp(path, file3) != 0))
		return -ENOENT;

	if (strcmp(path, file1) == 0) {
		len = strlen(file1_content);
		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, file1_content + offset, size);
		} else 
			size = 0;
	} else if (strcmp(path, file2) == 0) {
		len = strlen(file2_content);
		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, file2_content + offset, size);
		} else 
			size = 0;
	} else if (strcmp(path, file3) == 0) {
		int i;
		int file3Len = rand() % file3_size;
		char *randString;
		
		randString = malloc(file3Len * sizeof(char));
		int randChar;
		len = file3Len;
		for (i = 0; i < len; i++) {
			randChar = rand() % ALPHABET;
			randString[i] = (char) (randChar + 'A');
		}
		randString[len-1] = '\n';
		if (offset < len) {
			if (offset + size > len)
				size = (len) - offset;
			memcpy(buf, randString + offset, len);

			free(randString);
		} else
			size = 0;
	} 

	return size;
}
//*/

/*
// Read the target of a symbolic link 
static int test_readlink(const char *path, char *buf, size_t size)
{
	int res;

	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}
//

*/
// create symlink
static int test_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}


// create direcroty
static int test_mkdir(const char *path, mode_t mode)
{
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}


struct test_dirp {
	DIR *dp;
	struct dirent *entry;
	off_t offset;
};

// 
/*
static int test_opendir(const char *path, struct fuse_file_info *fi)
{
	int res;
	struct test_dirp *d = malloc(sizeof(struct test_dirp));
	if (d == NULL)
		return -ENOMEM;

	d->dp = opendir(path);
	if (d->dp == NULL) {
		res = -errno;
		free(d);
		return res;
	}
	d->offset = 0;
	d->entry = NULL;

	fi->fh = (unsigned long) d;
	return 0;
}
//*/
static struct fuse_operations test_oper = {
	.getattr	= test_getattr,
	.readdir	= test_readdir,
	.open		= test_open,
	.read		= test_read,
	//.readlink	= test_readlink,
	.symlink	= test_symlink,
	.mkdir		= test_mkdir,
	//.readdir	= test_readdir,
	//.opendir	= test_opendir,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &test_oper, NULL);
}
