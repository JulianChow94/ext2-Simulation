

/* delcarations for functions of functions.c */
struct ext2_super_block *read_superblock(FILE *fp);
struct ext2_group_desc *read_groupdesc(FILE *fp);
struct ext2_inode *read_inode(FILE *fp, int inode_num);
int countfile(FILE *fp);
int find_free_inode(FILE *fp);
const char *byte_to_binary(int x);
char **split(char *path, int *size);

