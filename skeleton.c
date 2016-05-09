#include <stdio.h>
#include <stdlib.h>
#include <linux/ext2_fs.h>
#include <unistd.h>
#include <string.h>
#include "header.h"
#include "functions.c"

int main (int argc, char *argv[]){
    if (argc != 2){
        printf( "usage: %s filename\n", argv[0] );
    }
    else{ 
        FILE *fp = fopen( argv[1], "r+" );
		    // SETUP: read the superblock and the inode and group desc
       	//struct ext2_super_block *sb = read_superblock(fp);  // commented out to remove warnings
       	//struct ext2_inode *inode = read_inode(fp, 2);     // commented out to remove warnings
       	struct ext2_group_desc *group = read_groupdesc(fp); 

       	//setup the bitmap
       	fseek(fp, 1024 * group->bg_inode_bitmap, SEEK_SET);
       	__u16 inode_bitmap;
       	fread(&inode_bitmap, 2, 1, fp);
       	printf("current inode bitmap: %s\n\n", byte_to_binary(inode_bitmap));

        //print the inode information
    	  int free_inode = find_free_inode(fp);
        int n = 1;
        while (n != 17){
            printf("inode %d status: \t%s\n", n, (byte_to_binary(inode_bitmap & (1 << (n-1)))));
            n++;
        }
        printf("The first free inode of this image file is %d\n", free_inode);


    }
    return 0;
}
