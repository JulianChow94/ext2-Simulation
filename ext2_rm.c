#include <stdio.h>
#include <stdlib.h>
#include <linux/ext2_fs.h>
#include <unistd.h>
#include <string.h>
#include "header.h"
#include "functions.c"

int main (int argc, char *argv[]){
    if (argc != 3){
        /* We print argv[0] assuming it is the program name */
        printf( "usage: %s filename path\n", argv[0] );
    }
    else{
        FILE *fp = fopen( argv[1], "r+" );
        int step = 0;
        //int final_inode_index;
        /* fopen returns 0, the NULL pointer, on failure */
        if (fp == 0){
            printf( "Could not open file\n" );
        }
        else {
            printf("\n Successfully Opened %s!\n\n", argv[1]);
            //int free_inode = find_free_inode(fp);
            printf("Path Entered: %s\n\n", argv[2]);
            int size;
            char **splitted;
            splitted = split(argv[2], &size);

            // SETUP: read the superblock and the inode and group desc
            struct ext2_super_block *sb = read_superblock(fp);
            struct ext2_inode *inode = read_inode(fp, 2);
            struct ext2_group_desc *group = read_groupdesc(fp);
            struct datablock *data = malloc(sizeof(struct datablock));
            step++;

            //setup the bitmap
            fseek(fp, 1024 * group->bg_inode_bitmap, SEEK_SET);
            __u16 inode_bitmap;
            fread(&inode_bitmap, 2, 1, fp);
            printf("current inode bitmap: %s\n\n", byte_to_binary(inode_bitmap));

            /* Special Case */
            if (sb->s_free_inodes_count == 0){
                printf("The disk is already empty!\n");
                return 0;
            }

            /* Traversal Algorithm */
            while((inode->i_mode & 0xF000) != 0x8000){
                fseek(fp, 1024 * inode->i_block[0], SEEK_SET);
                printf("now at block %d\n", inode->i_block[0]);
                fseek(fp, 24, SEEK_CUR); // skip the . and .. (24 bytes)
                fread(data, sizeof(struct datablock), 1, fp);
                while (data->name_length != strlen(splitted[step])){
                    if (data->name_length == 0){
                        printf("No more relevant data in this block\n");
                        printf("ERROR: Could not find %s in %s\n", splitted[step], argv[1]);
                        return 0;
                    }

                    // this is probably not it, read the next one
                    LOOP:while(data->name_length % 4 != 0){
                        data->name_length++;
                    }
                    fseek(fp, data->name_length, SEEK_CUR);
                    printf("This is not %s, moving to next data section\n", splitted[step]);
                    fread(data, sizeof(struct datablock), 1, fp);
                    //printf("strlen(splitted[step]: %d\n", strlen(splitted[step]));
                    //printf("data->name_length: %d\n", data->name_length);

                }
                printf("Found matching lengths!\n");
                char name[data->name_length - 1];
                fread(name, data->name_length, 1, fp);
                name[data->name_length] = '\0';
                if (strcmp(name, splitted[step]) == 0){
                    printf("Found matching name!!\n");
                    step++;
                    // get the data block's inode
                    printf("datablock has inode %d\n", data->inode);
                    printf("we now read that inode into our inode\n");
                    inode = read_inode(fp, data->inode);
                    printf("updated inode\n\n");
                    sleep(1);
                }
                else{
                    fseek(fp, data->name_length, SEEK_CUR);
                    printf("This is not %s, moving to next data section\n", splitted[step]);
                    fread(data, sizeof(struct datablock), 1, fp);
                    goto LOOP;
                }
            }
            // if we got here that means we found a file inode
            printf("\n\n\n******Found %s******\n", splitted[step-1]);

            /* We need to check if the file has already been deleted*/
            if (inode->i_block[0] == 0){
                printf("File has already been deleted!!!\n");
                return 0;
            }

            /*Perform cleanup here, updating the structure*/
            // Update the inode structure and write
            printf("Modifying the inode structure \n");
            fseek(fp, -128, SEEK_CUR);
            inode->i_size = 0;
            inode->i_links_count = 0;
            int n;
            for (n = 0; n < 12; n++){
                if (inode->i_block[n] != 0){
                    printf("Clearing direct pointer %d\n", n+1);
                    inode->i_block[n] = 0;
                }
            }
            fwrite(inode, 128, 1, fp);
            printf("\t* Inode write successful!\n\n");

            // Update the bitmap and write
            inode_bitmap = inode_bitmap >> 1;
            printf("Updating inode bitmap to %s\n", byte_to_binary(inode_bitmap));
            fseek(fp, 1024 * group->bg_inode_bitmap, SEEK_SET);
            fwrite(&inode_bitmap, 2, 1, fp);
            printf("\t* Bitmap update successful!\n\n");

            // update the superblock
            printf("Updating superblock\n");
            sb->s_free_inodes_count++;
            fseek(fp, 1 * 1024, SEEK_SET);
            fwrite(sb, 1024, 1, fp);
            printf("\t* Superblock write successful\n\n");


            fclose(fp);
            return 0;

        }
        return 0;
    }
    return 0;
}
