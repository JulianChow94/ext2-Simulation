#include <stdio.h>
#include <stdlib.h>
#include <linux/ext2_fs.h>
#include <unistd.h>
#include <string.h>
#include "header.h"

/*
* Structure used to store the first 8 bytes of datablock
* Since the name string itself is variable length 
* we will deal with that seperately
*/
struct datablock{
	__u32	inode;
	__u16	size;
	__u8	name_length;
	__u8	type; 		// 1: file, 2: directory
};

/*
* Function to read the superblock
* Takes a file as a argument 
* since a image file is just a bunch of bytes
*/
struct ext2_super_block *read_superblock(FILE *fp){
	
	// create a new struct to return
	struct ext2_super_block *block = (struct ext2_super_block*)malloc(1024);
	// find the superblock in the file (1024 bytes in)
	fseek(fp, 1024, SEEK_SET);
	// read it into the struct
	fread(block, 1024, 1, fp);
	// return the block
	return (struct ext2_super_block*)block;
}

/*
* Function to read the block group descriptor
* Takes a file as a argument 
* since a image file is just a bunch of bytes
*/
struct ext2_group_desc *read_groupdesc(FILE *fp){

	// create a new struct to return
	struct ext2_group_desc *group = (struct ext2_group_desc*)malloc(1024);
	// find the superblock in the file (1024 bytes in)
	fseek(fp, 2*1024, SEEK_SET);
	// read it into the struct
	fread(group, 1024, 1, fp);
	// return the block
	return (struct ext2_group_desc*)group;
}

/*
* Function to read a certain inode 
* Takes a img file with a offset
* Inodes start at index 1
* Given a index, this function will 
* read it into a inode struct
*/
struct ext2_inode *read_inode(FILE *fp, int inode_num){

	if (inode_num <= 0){
		exit(0);
	}
	if (inode_num > 15){
		exit(0);
	}

	struct ext2_inode *inode = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
	// find the starting address of the inode table (block 5)
	fseek(fp, 1024 * 5, SEEK_SET);	// we are now at the inode table
	// find the inode with the inode_num
	fseek(fp, 128 * (inode_num - 1), SEEK_CUR);
	// read it into the struct
	fread(inode, 128, 1, fp);
	// return the inode
	return (struct ext2_inode*)inode;
}

/* 
* Function to read the contents of a file
* store filename and contents
*/
int countfile(FILE *fp){

	int count = 0;
	while(fgetc(fp) != EOF){
		 count++;
	 }
	 rewind(fp);
	 return count;
}

/*
* Function to convert byte to binary
* Modified From:
* http://stackoverflow.com/questions/111928
  /is-there-a-printf-converter-to-print-in-binary-format
*/
const char *byte_to_binary(int x){
    static char b[17];
    b[0] = '\0';
    int z;
    for (z = 32768; z > 0; z >>= 1){
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

/* 
* Returns the first unallocated inode #
* in the img file 
* Borrowed from:
* http://stackoverflow.com/questions
* /27261558/splitting-up-a-path-string-in-c/27261819#27261819
*/
int find_free_inode(FILE *fp){

    fseek(fp, 1024 * 4, SEEK_SET);  // we are now at the inode bitmap
    //struct bitmap *map = malloc(sizeof(struct bitmap));
    __u16 map;
    fread(&map, 2, 1, fp);
    int n = 1;
    while (n != 15){
        if(atoi(byte_to_binary(map & (1 << (n-1)))) == 0){
            return n;
        }
        else{
            n++;
        }
    }
    return -1;
}

/*
* Function to split  input path
* into a string array
*/
char **split(char *path, int *size){
    char *tmp;
    char **splitted = NULL;
    int i, length;

    if (!path){
        goto Exit;
    }

    tmp = strdup(path);
    length = strlen(tmp);

    *size = 1;
    for (i = 0; i < length; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            (*size)++;
        }
    }

    splitted = (char **)malloc(*size * sizeof(*splitted));
    if (!splitted) {
        free(tmp);
        goto Exit;
    }

    for (i = 0; i < *size; i++) {
        splitted[i] = strdup(tmp);
        tmp += strlen(splitted[i]) + 1;
    }
    return splitted;

Exit:
    *size = 0;
    return NULL;
}