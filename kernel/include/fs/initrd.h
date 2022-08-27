#ifndef INITRD_H
#define INITRD_H

/*
 *  NOTE: The initrd should not have any subdirectories.
 *  NOTE: Everything starts with initrd/ thus a hypothetical file called 
 *        file.txt will be initrd/file.txt
 */


void initrd_init(void);
void* initrd_open(const char* path);
void initrd_close(void* fp);                // A wrapper around kfree.


#endif
