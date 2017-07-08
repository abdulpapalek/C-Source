#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include "md5.h"


int main()
{
    FILE *fp;
    char fileExt[20],str[10]; 
    int ctr,ctr2=0,di,countdot=0,ch;
    md5_state_t state;
    md5_byte_t digest[16];
    unsigned char buffer[1024];
    char szHash[16*2 + 1];
    size_t len=0,dwTotal = 0,size;
    unsigned long dwFileSize = 0;
    ssize_t read;
    char *filename="/home/alex/virus.exe";

    fp = fopen(filename, "rb");
    if (fp == NULL){
        return 0;
        //exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    dwFileSize = (unsigned long)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (dwFileSize > 100428800)                   
        return 0;  

    size = 1024;
   md5_init(&state);
    while((read = fread (buffer, 1, size, fp)) != 0) {
        //ReadFile(fp, buffer, 1024, &dwRead, NULL);
        //fread(&dwRead, (1024 * 1024), 1, fp);
        //fread(buffer, 1, sizeof(buffer), fp);
        //read = getline(&buffer, &len, fp);
        //fread(buffer, size, 1, fp);
        md5_append(&state, (const md5_byte_t *)buffer, read);

        //dwTotal += strlen(buffer)*sizeof(char);
    }
    //
    md5_finish(&state, digest);
    for (di = 0; di < 16; ++di)
    sprintf(szHash + di * 2, "%02x", digest[di]);

    printf("%s",szHash);
    printf (" %s\n", filename);
    fclose(fp);
}
