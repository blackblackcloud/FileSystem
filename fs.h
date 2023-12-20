#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "disk.h"
#include "config.h"
//我要实现类似ext2的文件系统，帮我搞一段示意图
// 1 block -> 1024B
// | super block | inode bitmap | block bitmap | inode table  | data block |
// | 1 block     | 1 block      | 1 block      | 56 block     | 965 block  |

typedef struct SuperBlock {
    unsigned short inode_bitmap_block;    // 2B   inode bitmap block number
    unsigned short block_bitmap_block;    // 2B   block bitmap block number
    unsigned short inode_table_block;     // 2B   inode table block number
    unsigned short data_block;            // 2B   data block number
    unsigned short inode_num;             // 2B   inode number
    unsigned short block_num;             // 2B   block number
    unsigned short inode_size;            // 2B   inode size
    unsigned short block_size;            // 2B   block size
} SuperBlock;

typedef int* InodeBitmap;
typedef int* BlockBitmap;

typedef struct FileSystem {
    Disk* disk;
    SuperBlock* super_block;
    InodeBitmap* inode_bitmap;
    BlockBitmap* block_bitmap;
    Dentry* root;
    Dentry* current;
} FileSystem;

typedef struct INode {
    unsigned short inode_number;          // 2B   inode的编号
    unsigned short size;                  // 2B   文件大小
    unsigned short direct_block[10];      // 16B  直接块
    unsigned short first_inedxed_block;   // 2B   一级索引块
    unsigned short second_indexed_block;  // 2B   二级索引块 
    unsigned short type;                  // 2B   文件类型 dir/file/link rwxrwxrwx
    unsigned short link_count;            // 2B   链接数
    time_t created_time;                  // 8B   创建时间
    time_t modified_time;                 // 8B   修改时间
    time_t access_time;                   // 8B   访问时间
} INode;                                  // 52B

typedef struct Dentry {
    unsigned short inode;                 // 2B   inode的编号
    unsigned short father_inode;          // 2B   父目录inode编号
    unsigned short name_length;           // 2B   文件名长度
    char* name;                           // 8B   文件名
    unsigned short sub_dir_count;         // 2B   子目录数
    unsigned short* sub_dir_length;       // 8B   子目录名长度
    unsigned short* sub_dir_inode;        // 8B   子目录inode编号
    char** sub_dir;                       // 8B   子目录名
} Dentry;                                 // 48B

typedef struct UserOpenItem {
    INode inode;                          // 修改之后的文件inode, 用于保存变更, 也可用于判断是否打开
    int offset;                           // 文件内读写偏移量
    bool modify;                          // inode是否被修改
    bool open;                            // 文件是否被打开
} UserOpenItem;

typedef struct UserOpenTable {
    UserOpenItem* items;                  // 用户打开文件表
    int size;                             // 用户打开文件数
    int capacity;                         // 用户打开文件表容量
} UserOpenTable;

void tbl_push_back(UserOpenTable* tb, UserOpenItem item);

void tbl_pop_back(UserOpenTable* tb);

void tbl_init(UserOpenTable* tb);

void tbl_destroy(UserOpenTable* tb);

void tbl_resize(UserOpenTable* tb, int new_size);

void tbl_clear(UserOpenTable* tb);

void tbl_remove(UserOpenTable* tb, int index);

// file system
void saveFs(void *ptr, size_t size, FILE *stream);

void loadFs(void *ptr, size_t size, FILE *stream);

// api

// my_format
void format();

// my_mkdir
void mkdir(Dentry* d, char* path);

// my_rm + my_rmdir
// recursive = 0: rm file or empty dir; recursive = 1: rm dir and all its subdirs
void rm(Dentry* d, char* path, int recursive); 

// my_ls
void ls(Dentry* d, char* path);

// my_cd
void cd(Dentry* d, char* path);

// my_create
void touch(Dentry* d, char* path);

// my_open
void open(UserOpenTable* tb, unsigned short inode_num);

// my_close
void close(UserOpenTable* tb, unsigned short inode_num);

// my_read
void read(UserOpenTable* tb, unsigned short tbl_index, int length, char* content);

// my_write
void write(UserOpenTable* tb, unsigned short tbl_index, int length, char* content);

// my_ln
// soft = 0: hard link; soft = 1: soft link
void ln (Dentry* d, char* path, char* link, int soft);

// exit fs
void exitfs();

#endif