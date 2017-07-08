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

//void processScan();

typedef struct hash_database{
	char * _hash;
	struct hash_database *next;
}hash_database_t;

typedef struct exten_database{
	char *_ext;
	struct exten_database *next;
}exten_database_t;

typedef struct dir_files{
	char *_filepath;
	struct dir_files *next;
}dir_files_t;

int _getallfiles(char *dirname, dir_files_t **filelist);
int _initialize_hash_database(hash_database_t **hash);
int _initialize_ext(exten_database_t **ext);
int _fileScan(char  *filepath,hash_database_t *hash,exten_database_t *ext);
void _free_memory(hash_database_t *hash,exten_database_t *ext,dir_files_t *filelist);

int main(){
	hash_database_t *hash = NULL;
	exten_database_t *ext = NULL;
	dir_files_t *filelist = NULL,*trav_filelist=NULL;
	char dir[50],opt[50];
	int status = 0;

    memset(dir, 0x00, strlen(dir)*sizeof(char));
    memset(opt, 0x00, strlen(opt)*sizeof(char));
    Scanner:
        printf("1 - Scan or 2 - Exit: ");
        scanf("%s",opt);
    if(strcmp(opt,"2")==0){
        printf("exiting");
        sleep(2);
        printf(".\n");
        exit(1);
    }
    else if(strcmp(opt,"1")==0){
        printf("Input a Directory to scan:");
        scanf("%s",dir);      
    }
    else{
        printf("invalid input!!\n");
        goto Scanner;     
    }

	//memset(dir, 0x00, sizeof(char)*50);
	status = _initialize_hash_database(&hash);
    if(status == 0){
        printf("Error updating hash...\n");
        goto Scanner;
    }
	status = _initialize_ext(&ext);
    if(status == 0){
        printf("Error updating extension files...\n");
        goto Scanner;
    }
	status = _getallfiles(dir,&filelist);
    if(status == 0){
        printf("Error in files...\n");
        goto Scanner;
    }
	status = 0;
    trav_filelist = filelist; 

	do{
		status = _fileScan(trav_filelist->_filepath,hash,ext);
		trav_filelist = trav_filelist->next;
	}while(trav_filelist != NULL);

	
    remove("file.txt");
     _free_memory(hash,ext,filelist);
    goto Scanner;

    return 1;

}

void _free_memory(hash_database_t *hash,exten_database_t *ext,dir_files_t *filelist){
	hash_database_t *temp_hash;
	exten_database_t *temp_ext;
	dir_files_t	*temp_dir;

	while(hash!=NULL){
		temp_hash = hash;
        free(temp_hash->_hash);
		hash = hash->next;
		free(temp_hash);
	}

	while(ext!=NULL){
		temp_ext = ext;
        free(temp_ext->_ext);
		ext = ext->next;
		free(temp_ext);
	}

	while(filelist!=NULL){
		temp_dir = filelist;
        free(temp_dir->_filepath);
		filelist = filelist->next;
		free(temp_dir);
	}

    printf("Memory freed\n");
    //exit(1);

}

int _getallfiles(char *dirname, dir_files_t **filelist){
    char *command,cat[50],find[5];
    FILE *fp;
    char *line = NULL;
    int flag = 0,count = 0;
    size_t len = 0;
    ssize_t read;

    memset(cat, 0x00, strlen(cat)*sizeof(char));
    memset(find, 0x00, strlen(find)*sizeof(char));
    strcpy(cat, " -type f -print0 | xargs -0 ls -t > file.txt");
    strcpy(find, "find ");
    command = (char*)malloc(((strlen(dirname)+strlen(cat)+strlen(find))*sizeof(char)) + 1);
    memset(command, 0x00, strlen(command)*sizeof(char));

    strcpy(command,find);
    strcat(command,dirname);
    strcat(command,cat);

    //strcpy( command, "find -type f -print0 | xargs -0 ls -t > file.txt" );
    system(command);
    //printf("%s\n",command);

    fp = fopen("file.txt", "r");
    if (fp == NULL){
        return 0;
        //exit(EXIT_FAILURE);
    }
    else{
        *filelist = (dir_files_t*) calloc(1, sizeof(*filelist));
        (*filelist)->_filepath = NULL;
        (*filelist)->next = NULL;
        flag = 1;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        if(flag ==1){
            (*filelist)->_filepath = (char*)malloc(len*sizeof(char));
            memset(((*filelist)->_filepath), 0x00, len*sizeof(char));
            strncpy((*filelist)->_filepath,line,len);
            (*filelist)->_filepath[ strlen((*filelist)->_filepath) - 1] = '\0';
            (*filelist)->next = NULL;
            filelist = &(*filelist)->next; 
            flag = 0;
        }
        else{
            *filelist = (dir_files_t*) calloc(1, sizeof(*filelist));
            (*filelist)->_filepath = (char*)malloc(len*sizeof(char));
            memset(((*filelist)->_filepath), 0x00, len*sizeof(char));
            strncpy((*filelist)->_filepath,line,len);
            (*filelist)->_filepath[ strlen((*filelist)->_filepath) - 1] = '\0';
            (*filelist)->next = NULL;
            filelist = &(*filelist)->next;                                          
            //(*hash)->_hash = NULL;
        }
        count++;

    }

    fclose(fp);
    if (line)
        free(line);
    if(count > 0){
        //(*hash)->next = *hash;
        return 1;
    }
    return 0;
    //exit(EXIT_SUCCESS);
   //return(0);
}

int _initialize_hash_database(hash_database_t **hash){
    FILE *fp;
    char *line = NULL;
    int flag = 0,count = 0;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/home/alex/hash.txt", "rb");
    if (fp == NULL){
        return 0;
        //exit(EXIT_FAILURE);
    }
    else{
    	*hash = (hash_database_t*) calloc(1, sizeof(*hash));
    	(*hash)->_hash = NULL;
    	(*hash)->next = NULL;
    	flag = 1;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        if(flag ==1){
	    	(*hash)->_hash = (char*)malloc(len*sizeof(char));
			memset(((*hash)->_hash), 0x00, len*sizeof(char));
	    	strncpy((*hash)->_hash,line,len);
            (*hash)->_hash[ strlen((*hash)->_hash) - 1 ] = '\0';
	    	(*hash)->next = NULL;
	    	hash = &(*hash)->next; 
	    	flag = 0;
        }
        else{
	    	*hash = (hash_database_t*) calloc(1, sizeof(*hash));
	    	(*hash)->_hash = (char*)malloc(len*sizeof(char));
			memset(((*hash)->_hash), 0x00, len*sizeof(char));
	    	strncpy((*hash)->_hash,line,len);
            (*hash)->_hash[ strlen((*hash)->_hash) - 1 ] = '\0';
	    	(*hash)->next = NULL;
	    	hash = &(*hash)->next;    					    		    	
	    	//(*hash)->_hash = NULL;
        }
        count++;

    }

    fclose(fp);
    if (line)
        free(line);
    if(count > 0){
    	//(*hash)->next = *hash;
    	return 1;
    }
    return 0;
    //exit(EXIT_SUCCESS);
}

int _initialize_ext(exten_database_t **ext){
    FILE *fp;
    char *line = NULL;
    int flag = 0,count = 0;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/home/alex/ext.txt", "rb");
    if (fp == NULL){
        return 0;
        //exit(EXIT_FAILURE);
    }
    else{
    	*ext = (exten_database_t*) calloc(1, sizeof(*ext));
    	(*ext)->_ext = NULL;
    	(*ext)->next = NULL;
    	flag = 1;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        if(flag == 1){
	    	(*ext)->_ext = (char*)malloc(len*sizeof(char));
			memset(((*ext)->_ext), 0x00, len*sizeof(char));
	    	strncpy((*ext)->_ext,line,len);
            (*ext)->_ext[ strlen((*ext)->_ext) - 1 ] = '\0';
	    	(*ext)->next = NULL;
	    	ext = &(*ext)->next; 
	    	flag = 0;
        }
        else{
	    	*ext = (exten_database_t*) calloc(1, sizeof(*ext));
	    	(*ext)->_ext = (char*)malloc(len*sizeof(char));
			memset(((*ext)->_ext), 0x00, len*sizeof(char));
	    	strncpy((*ext)->_ext,line,len);
            (*ext)->_ext[ strlen((*ext)->_ext) - 1 ] = '\0';
	    	(*ext)->next = NULL;
	    	ext = &(*ext)->next;    					    		    	
	    	//(*hash)->_hash = NULL;
        }
        count++;


    }

    fclose(fp);
    if (line)
        free(line);

    if(count > 0){
    	//(*ext)->next = *ext;
    	return 1;
    }

    return 0;
    //exit(EXIT_SUCCESS);
}
/**/
int _fileScan(char  *filepath,hash_database_t *hash,exten_database_t *ext){
    FILE *fp;
	char fileExt[20],str[10]; 
	int ctr,ctr2=0,di,countdot=0;
    md5_state_t state;
    md5_byte_t digest[16];
    unsigned char buffer[1024];
    char szHash[16*2 + 1];
    size_t size;
    unsigned long dwFileSize = 0;
    ssize_t read;

	memset(fileExt, 0x00, sizeof(fileExt));
    memset(buffer, 0x00, sizeof(buffer));

	//strncpy(fileExt,filepath,sizeof(filepath));

	for(ctr = 0; ctr < strlen(filepath);ctr++){
		if(filepath[ctr] == '.'){
			countdot++;
		}
		if(countdot >= 2){
			fileExt[ctr2] = filepath[ctr];
			ctr2++;
		}
	}

    if(countdot == 0){
        return 0;
    }

    while(ext != NULL){
    	if(strcasecmp(fileExt,ext->_ext) == 0)
    	return 0;
        ext = ext->next;
    }

    fp = fopen(filepath, "rb");
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
        md5_append(&state, (const md5_byte_t *)buffer, read);
    }
    md5_finish(&state, digest);

    for (di = 0; di < 16; ++di)
    sprintf(szHash + di * 2, "%02x", digest[di]);

    fclose(fp);

    while(hash != NULL){
        if (strcmp(szHash, hash->_hash) == 0){
            // Write output to console
            printf("Found a suspicious file: %s\n", filepath);

            // Delete file
            printf("do you want to delete file? \n yes or no?");
            scanf("%s",str);
            if(strcasecmp("yes",str)==0){
            	//remove(filepath);
                remove(filepath);
            	printf("file deleted\n");
            }

            return 1;                         // We found matched hash with database
        } 
        hash = hash->next; 	
    } 
    return 1; 
	
}
