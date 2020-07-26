// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <wait.h> 
#include <pthread.h>
#include <string.h> 
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#define NET AF_INET
#include <dirent.h>
#include <openssl/sha.h>
//********************************************NEED TO SEND************************************
#include <sys/types.h>  
#include <sys/stat.h>
#include <unistd.h>

//*******************************************************************************

pthread_mutex_t lock;
//********************************************************NEED TO SEND***********************************************************
typedef struct nodde_
{
  // nodde to store path and hash
  char* indicator;
  char* path;
  char* hash;
  char* vnum;
  struct nodde_ * next;
}nodde;
//********************************************************NEED TO SEND***********************************************************
char* proj_comm_path(char*project_name){
    char* path_Commit = (char*) malloc(sizeof(char)*(15+strlen(project_name)));
    sprintf(path_Commit,"repo/%s/.Commit",project_name);
    //printf("%s\n",path_manifest);
    return path_Commit;
}
//***********************************************************NEED TO SEND*******************************************************
char* pullFile(char *file){


int fileSize=strlen(file);
char *str = calloc((fileSize+1), sizeof str[0]);
//char str[fileSize];
//memset(str,'\0',(fileSize+1));


//printf("%s",file);
int i=strlen(file)-1;
//printf("%d",i);
int size=0;

while(i>0){

   if(file[i]=='/'){
      break;
   }
   //append
   char ch=file[i];
   strncat(str, &ch, 1); 

   
   size++;
   i--;
}



//printf("STR IS ::::::::::::;%s",str);
int k=0;
int j=0;

char *str2 = calloc((fileSize+1), sizeof str2[0]);
for(k=strlen(str)-1;k>=0;k--){

   str2[j]=str[k];
   //printf("STR2 %c",str2[j]);
      j++;
}

//printf("\n");

// //sub0/subsub92/file716
//printf("STR2:::::%s",str2);

return str2;

}

//***********************************************************NEED TO SEND*******************************************************
char* stripped(char* str){
    // gets versionnum (everything but the #)
    char* num = malloc(strlen(str));
    bzero(num,strlen(str));
    int i;
    for(i=1;i<strlen(str);i++){
        num[i-1]= str[i];
    }
    return num;
}
char* stripMan(char*path){
    int i;
    int firstoccur=0;
    for(i=0;i<strlen(path);i++){
        if(path[i]=='\n'){
            firstoccur = i;
            break;
        }
    }
    char* new_path = (char*) malloc((sizeof(char)*firstoccur)+1);
    for(i=0;i<firstoccur;i++){
        new_path[i]=path[i];
    }
    new_path[firstoccur]='\0';
    return new_path;
}
char* filetoarray(char* filename){
    int fd = open(filename,O_RDONLY);
    char buffer[1];
    char* array = (char*) malloc(sizeof(char)*1);
    int size = 0;
   while(read(fd,buffer,1)>0){
       array[size] = buffer[0];
       size++;
       array = realloc(array,(size+1)*sizeof(char));
   }
   array[size] = '\0';
   close(fd);
   return array;
}
///////////////////////////////////////////////////////////NEED TO SEND//////////////////////////////////////////////////

//helper method for add method
int checkDup(char *pathT, char*ManPath){

//printf("TARGET:::::::::%s",pathT);

   char* array=filetoarray(ManPath);

   char *end_str;
   char *token = strtok_r(array, "\n", &end_str);
   token = strtok_r(NULL, "\n", &end_str);


   while(token != NULL)
   {

   int size=strlen(token);

   char savePath[size];
   strcpy(savePath,token);
      char *end_token;
      char *token2 = strtok_r(token, "|", &end_token);
      int counter=1;

         while (token2 != NULL)
      {      
         if(counter==3){
            // printf("LLALALLA");
         // this means reached path for the first token 
               char* new_path = token2;

            //printf("%s\n",new_path);

               if(strcmp(pathT,new_path)==0){

                  return 1;
               }
                                       
         }
         counter++;
         token2 = strtok_r(NULL, "|", &end_token);    
         
      }
      token = strtok_r(NULL, "\n", &end_str);

   }
return 0;
}
//**********************************************************************UPDATE VERSION NUMBER////////////////////////////////
//manpath array needs to be sent here, projectNamewith the path 
void updateManVersion(char*ManPath,char *projectName, int VersionNumber){
    VersionNumber=VersionNumber+1;
    char vNumber[500];
    bzero(vNumber,500);
sprintf(vNumber, "%s%d","#", VersionNumber);
printf("VNUMBER IS %s \n",vNumber);

//char tempPath[300];
//sprintf(tempPath, "repo/%s/.tempMan", projectName);

   char* array=filetoarray(ManPath);


   //Created a temp manifest same place as the original manifest location
   int fp = open(ManPath, O_CREAT | O_RDWR | O_APPEND|O_TRUNC, S_IRWXU);
    printf("FP VALUE %d\n",fp);


   write(fp,vNumber,strlen(vNumber));
   write(fp,"\n",strlen("\n"));

   //Everything is stored in array now, now break the array in token with /n 


   char *end_str;
   char *token = strtok_r(array, "\n", &end_str);
   token = strtok_r(NULL, "\n", &end_str);


   while(token != NULL)
   {
   //fileversion|filename|path|Hash

   //printf("PP%s \n",token);

   int size=strlen(token);

   char savePath[size];
   strcpy(savePath,token);

      int counter=1;

       write(fp, savePath, strlen(savePath));
       write(fp, "\n", strlen("\n"));
               

      token = strtok_r(NULL, "\n", &end_str);

   }


   //first token will be called and save original str token and break that token further and check the path with token 3


   //if it is the target string is not same as the token3 then write original str into the newfile tempManifest 
   //delete the origional one and rename the temp one with .Manifest
}

//////////////////////////////////////////////////////////////////Need to send//////////////////////////////////////////////
//**************************************************************************************************************************************
int addM(char *projectName, char *path)

{
char proName[255];
char projectN[255];
strcpy(projectN,projectName);
char projectPath[255];
strcpy(proName,projectName);
strcpy(projectPath,path);


   int kd = open(path, O_RDONLY);


   if(kd<0){

      printf("File does not exist in Project ");
      return 1;
   }

//Check for same filePath in the manifest and exit here
char manPath[255];
   //char filePath[255];

sprintf(manPath, "repo/%s/.Manifest", projectName);
   //printf("%s", manPath);

  int val=checkDup(path,manPath);

      if(val==1){

         printf("FILE ALREADY ADDED IN MANIFEST \n");
         return 1;
      }

DIR *dir;
struct dirent *sd;
unsigned char hash[SHA_DIGEST_LENGTH];
//test in here but need to append here
   

//project/test
int fd;
 
   fd = open(path, O_RDONLY);
   

   char *versionNumber = "#0";
   int fp = 0;
   fp = open(manPath, O_RDWR | O_APPEND);

   //printf("%d", fp);
   write(fp, versionNumber, strlen(versionNumber));
   write(fp, "|", strlen("|"));

          

     
   write(fp, pullFile(path), strlen(pullFile(path)));
      
    write(fp, "|", strlen("|"));


   char *array = (char *)malloc(sizeof(char));


   char buffer2[10000];
   
   write(fp, path, strlen(path));
   //***********************************************

   write(fp, "|", strlen("|"));
   // printf("%p",fp);

   int rd = 100;
   char buffer[10000];

   int arrSize = 0;

   while ((rd = read(fd, buffer, 10)) > 0)
   {
      array = realloc(array, (arrSize + rd) * sizeof(char));
      int i;
      for (i = 0; i < rd; i++)
      {
         array[arrSize] = buffer[i];
         //printf("%c", buffer[i]);
         (arrSize)++;
      }
   }

   memset(hash, '\0', strlen(hash));
   SHA1(array, strlen(array), hash);
   char code[41];
   memset(code, '\0', 41);
   int i = 0;
   for (i; i < 20; i++)
   {
      snprintf(code + i * 2, 3, "%02x ", hash[i]);
   }

   memset(array, '\0', strlen(array));

   write(fp, code, strlen(code));
   write(fp, "\n", strlen("\n"));
   memset(code, '\0', strlen(code));

return 0;
}

//****************************************************************NEED TO SEND*****************************************************************
nodde* pushLL( char* manifest, nodde*chead){
    char *end_str;
    char *token = strtok_r(manifest, "\n", &end_str); 
    //token = strtok_r(NULL, "\n", &end_str); // skipping manifest version
    while (token != NULL)
    {
        char *end_token;
        char *token2 = strtok_r(token, "|", &end_token);
        int counter =1;
        char*temp_indicator;
        char* temp_path;
        char* temp_hash;
        char* temp_vnum;
        int path_complete = 0;
        int hash_complete =0;
        int indicator_complete =0;
        int vnum_complete =0;
        while (token2 != NULL)
         {   

             if(counter ==1){
                // hash
                temp_indicator = (char*) malloc(sizeof(char)*strlen(token2));
                bzero(temp_indicator,sizeof(char)*strlen(token2)+1);
                strcpy(temp_indicator,token2);
                 indicator_complete=1;
                //printf("temp_indocator:%s\n",temp_indicator);
            }

            if(counter==2){
            //path       
                temp_path = (char*) malloc(sizeof(char)*strlen(token2)+1);
                bzero(temp_path,sizeof(char)*strlen(token2)+1);
                strcpy(temp_path,token2);
                path_complete = 1;
               // printf("PATH: %s",temp_path);
            } if(counter==3){
                // hash
                temp_hash = (char*) malloc(sizeof(char)*strlen(token2));
                bzero(temp_hash,sizeof(char)*strlen(token2)+1);
                strcpy(temp_hash,token2);
                hash_complete=1;
                //printf("HASH:%s\n",temp_hash);
            
           } if(counter==4){
                // hash
                temp_vnum = (char*) malloc(sizeof(char)*strlen(token2));
                bzero(temp_vnum,sizeof(char)*strlen(token2)+1);
                strcpy(temp_vnum,token2);
                vnum_complete=1;
                //printf("HASH:%s\n",temp_hash);
            }
            
            if(hash_complete==1 && path_complete==1 && indicator_complete==1&&vnum_complete==1){
                if(chead==NULL){
                    chead = (nodde*) malloc(sizeof(nodde));
                    chead->indicator=temp_indicator;
                    chead->path = temp_path;
                    chead->hash = temp_hash;
                    chead->vnum = temp_vnum;
                    chead->next = NULL;
                }
                else{
                    nodde* temp  = (nodde*) malloc(sizeof(nodde));
                    temp->indicator=temp_indicator;
                    temp->path = temp_path;
                    temp->hash = temp_hash;
                    temp->next = chead;
                     chead->vnum = temp_vnum;

                    chead = temp;
                }
            }
            
            token2 = strtok_r(NULL, "|", &end_token);
            counter++;
        }
        token = strtok_r(NULL, "\n", &end_str);
    }
    return chead; 
}


///////////////////////////////////////////////////////////////////Need TO SEND /////////////////////////////////////////////////////////

void print(nodde* n) 
{ 
    while (n != NULL) { 
       printf("|%s ",n->indicator);
        printf("|%s ",n->path);
        printf("%s|->",n->hash); 
        n = n->next; 
    } 
    printf("\n");
}
///////////////////////////////////////////////////////////////////Need TO SEND /////////////////////////////////////////////////////////

dellMan(char *pathT, char*ManPath,char *projectName,int serverVersion){

//printf("TARGET:::::::::%s",pathT);
//printf("HERE NOW");
serverVersion=serverVersion+1;
char vNumber[500];
bzero(vNumber,500);
sprintf(vNumber, "%s%d","#", serverVersion);

char tempPath[300];
sprintf(tempPath, "repo/%s/.tempMan", projectName);
//target path 

// printf("\n");

   char* array=filetoarray(ManPath);


   //Created a temp manifest same place as the original manifest location
   int fp = open(ManPath, O_CREAT | O_RDWR | O_APPEND |O_TRUNC, S_IRWXU);


   write(fp,vNumber,strlen(vNumber));
   write(fp,"\n",strlen("\n"));

   //Everything is stored in array now, now break the array in token with /n 


   char *end_str;
   char *token = strtok_r(array, "\n", &end_str);
   token = strtok_r(NULL, "\n", &end_str);


   while(token != NULL)
   {
   //fileversion|filename|path|Hash

   //printf("PP%s \n",token);

   int size=strlen(token);

   char savePath[size];
   strcpy(savePath,token);
      char *end_token;
      char *token2 = strtok_r(token, "|", &end_token);
  

      int counter=1;

         while (token2 != NULL)
      {      
         if(counter==3){
            // printf("LLALALLA");
         // this means reached path for the first token 
               char* new_path = token2;
      
              // printf("%s\n",new_path);

               if(strcmp(pathT,new_path)!=0){

                  write(fp, savePath, strlen(savePath));
                     write(fp, "\n", strlen("\n"));
               }
                                       
         }
         counter++;
         token2 = strtok_r(NULL, "|", &end_token);    
         
      }
      token = strtok_r(NULL, "\n", &end_str);

   }
 
}


////////////////////////////////////////////////////////////////////////NEED TO SEND///////////////////////////////////////////////////////
int writeint(int* socket,int x){
    int converted_number = htonl(x);
    if(write(*socket,&converted_number,sizeof(converted_number))<0){
        perror("fail write_int");
        exit(1);
    }
    return 0;
}
int read_conversion(int* socket){
    int received_int = 0;
    if(read(*socket,&received_int, sizeof(received_int))<0){
        perror("fail to read netcode");
    }
    int converted_int = ntohl(received_int);
    printf("CONVERSION READ_CONVERSION:%d\n",converted_int);
    return converted_int;
}
/////////////////////////////////////////////
void mkdir_recursive(const char *path)
{
    char *subpath, *fullpath;
    fullpath = strdup(path);
    subpath = dirname(fullpath);
    if (strlen(subpath) > 1)
        mkdir_recursive(subpath);
    mkdir(path,00700);
    free(fullpath);
}
/////////////////////////////////////////////////////
int transfer(int* socket,char*filename){
    char* array  = filetoarray(filename);
    //printf("this is size of :%d",sizeof(array));
    int i=0;
    while(array[i]!='\0'){
        i++;
    }
    if((write(*socket,array,i))<0){
        return -1;
    }
    return 1;
}
int comparethis(char* s1, char* s2) 
{ 
    int M = strlen(s1); // longer
    int N = strlen(s2); // shorter
    int i;
    /* A loop to slide pat[] one by one */
    for (i = 0; i <= N - M; i++) { 
        int j; 
  
        /* For current index i, check for pattern match */
        for (j = 0; j < M; j++) 
            if (s2[i + j] != s1[j]) 
                break; 
  
        if (j == M) 
            return -1; 
    } 
    return 0; 
}
char* proj_man_path(char*project_name){
    char* path_manifest = (char*) malloc(sizeof(char)*(15+strlen(project_name)));
    sprintf(path_manifest,"repo/%s/.Manifest",project_name);
    //printf("%s\n",path_manifest);
    return path_manifest;
}
int create(int* socket){
    printf("INSIDE CREATE CALL\n");
    int pnameint = read_conversion(socket);
    char project_name[pnameint+1];
    bzero(project_name,pnameint+1);
    if(read(*socket,project_name,pnameint)<0){
        perror("READING error AT project_name");
    }
    printf("%s\n",project_name);
    char path_manifest[255];
    bzero(path_manifest,255);
    sprintf(path_manifest,"repo/%s/.Manifest",project_name);
    int check =open(path_manifest,O_RDONLY);
    if(check>0){
        printf("writing no to client");
        write(*socket,"no",2);
        return 0;
    }
    write(*socket,"ye",2);
    char project_path[255];
    bzero(project_path,255);
    sprintf(project_path,"repo/%s",project_name);
    DIR* dir;
    dir = opendir(project_path);
    if(dir==NULL){
        int direc = mkdir(project_path,00700);
    }else{
        printf("impossible");
        write(*socket,"no",2);
        return -1;
    }
    int manfiestfd = open(path_manifest,O_CREAT|O_RDWR|O_APPEND,00700);
    write(manfiestfd,"#0\n",3);
    close(manfiestfd);
    char* manfiestarray = filetoarray(path_manifest);
    printf("%s\n",manfiestarray);
    int number_to_send = strlen(manfiestarray);
    printf("%d\n",number_to_send);
    int converted_number = htonl(number_to_send);
    printf("%d\n",converted_number);
    write(*socket,&converted_number,sizeof(converted_number));
    sleep(0.3);
    write(*socket,manfiestarray,number_to_send);
    return 0;
}
char* strip(char*path){
    int i;
    int lastslash=0;
    for(i=0;i<strlen(path);i++){
        if(path[i]=='/'){
            lastslash = i;
        }
    }
    char* new_path = (char*) malloc((sizeof(char)*lastslash)+1);
    for(i=0;i<lastslash;i++){
        new_path[i]=path[i];
    }
    new_path[lastslash]='\0';
    return new_path;
}
char** all_paths(int* manptr,char*manifest,int* sizearray){
    char *end_str;
    char *token = strtok_r(manifest, "\n", &end_str);
    token = strtok_r(NULL, "\n", &end_str);
    //printf("a = %s\n", token);
    char** array_new_paths =  malloc(0);
    int new_path_ptr=0;
    while (token != NULL)
    {
        char *end_token;
        char *token2 = strtok_r(token, "|", &end_token);
       // printf("a = %s\n", token);
        int counter =1;
        while (token2 != NULL)
         {   
            if(counter==3){
            // this means reached path
            //printf("run");
                char* new_path = strdup(token2);
                array_new_paths = (char**) realloc(array_new_paths,sizeof(char*)+1000);
                array_new_paths[new_path_ptr] = malloc(sizeof(char)*strlen(new_path)+1);
                array_new_paths[new_path_ptr] = new_path;
                //printf("PATH: %s\n",array_new_paths[new_path_ptr]);
                new_path_ptr++;
            }
            //printf("b = %s\n", token2);
            counter++;
            token2 = strtok_r(NULL, "|", &end_token);
        }
        token = strtok_r(NULL, "\n", &end_str);
        *sizearray = new_path_ptr;
    }
    return array_new_paths;
}
char** make_unqiue_path(int* manptr,char*manifest,int* sizearray){
    //fileversion|filename|path|Hash
    printf("\n");
    char *end_str;
    char *token = strtok_r(manifest, "\n", &end_str);
    token = strtok_r(NULL, "\n", &end_str);
    //printf("a = %s\n", token);
    char** array_new_paths = malloc(0);
    int new_path_ptr=0;
    while (token != NULL)
    {
        char *end_token;
        char *token2 = strtok_r(token, "|", &end_token);
       // printf("a = %s\n", token);
        int counter =1;
        while (token2 != NULL)
         {   
            if(counter==3){
            // this means reached path
                char* new_path = strip(token2);
                int i;
                int indicator = 0;
                for(i=0;i<new_path_ptr;i++){
                    if(strcmp(array_new_paths[i],new_path)==0){
                        indicator = -1;
                        break;
                    }
                }
                if(indicator<0){
                    counter++;
            token2 = strtok_r(NULL, "|", &end_token);
                   continue;
                }
                array_new_paths = (char**) realloc(array_new_paths,sizeof(char*));
                array_new_paths[new_path_ptr] = malloc(sizeof(char)*strlen(new_path)+1);
                array_new_paths[new_path_ptr] = new_path;
                //printf("PATH: %s\n",array_new_paths[new_path_ptr]);
                new_path_ptr++;


            }
            //printf("b = %s\n", token2);
            counter++;
            token2 = strtok_r(NULL, "|", &end_token);
        }
        token = strtok_r(NULL, "\n", &end_str);
        *sizearray = new_path_ptr;
    }
    return array_new_paths;
}
//char* path_create(char* project_name,)
int checkout(int*socket){
    printf("INSIDE CHECKOUT CALL\n");
    int pnameint = read_conversion(socket);
    char project_name[pnameint+1];
    bzero(project_name,pnameint+1);
    if(read(*socket,project_name,pnameint)<0){
        perror("READING error AT project_name");
    }
    printf("%s\n",project_name);
    int fd  = open(proj_man_path(project_name),O_RDONLY);
    if(fd<0){
        perror("Opening path to MANFIEST (CHECKOUT)");
        write(*socket,"no",2);
        return -1;
    }else{
        write(*socket,"ye",2);
    }
    // now we want array of unqiue paths..
    // to find directory
    char* manifest = filetoarray(proj_man_path(project_name));
    char* manifest_copy = strdup(manifest);
    char* manifest_copy2 = strdup(manifest);
    int sizearray = 0;
    int* ptr = &sizearray;
    char** array_unqiue_path  = make_unqiue_path(&fd,manifest,ptr);
    printf("this is num of array_unqiue_path: %d\n",*ptr);
    int converted_number = htonl(*ptr);
    write(*socket,&converted_number,sizeof(converted_number));
    sleep(0.5);
    int i = 0;
    printf("/////////////////\n");
    while(i<(*ptr)){
    int strlen_convert = htonl(strlen(array_unqiue_path[i]));
    write(*socket,&strlen_convert,sizeof(strlen_convert));
    sleep(0.3);
    printf("%s\n",array_unqiue_path[i]);
    write(*socket,array_unqiue_path[i],strlen(array_unqiue_path[i]));
    i++;
    }
    printf("/////////////////\n");
    // data transfer
    int all_path_size = 0; // number files/paths contained
    int* all_ptr = &all_path_size;
    char** array_all_paths = all_paths(&fd,manifest_copy,all_ptr);
    printf("NUMBER OF FILES %d\n",all_path_size);
    int num_file = htonl(all_path_size); // 
    write(*socket,&num_file,sizeof(num_file));
    i =0;
    printf("&&&&&&&&&&&&&&&&&&&&&&&&\n");
    while(i<(*all_ptr)){
        int stconvert = htonl(strlen(array_all_paths[i])); 
        write(*socket,&stconvert,sizeof(stconvert)); // writes the length_path to client
        sleep(0.3);
        write(*socket,array_all_paths[i],strlen(array_all_paths[i])); //write paths to client
        printf("written path:%s\tlength:%d\n",array_all_paths[i],strlen(array_all_paths[i]));
        sleep(0.3);
        char* file = filetoarray(array_all_paths[i]);
        int flength =htonl(strlen(file));
        write(*socket,&flength,sizeof(flength)); // write size of file -> client
        sleep(0.3);
        write(*socket,file,strlen(file));
        sleep(0.3); // write file to client
        i++;
    }
    int man_length = htonl(strlen(manifest_copy2)); 
    write(*socket,&man_length,sizeof(man_length));
    sleep(0.3);
    write(*socket,manifest_copy2,strlen(manifest_copy2));
    return 0;
}
int update(int*socket){
    int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    bzero(project_name,projlen+1);
    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein UPDATE");
        exit(1);
    }
    printf("project_name:%s\n",project_name);
    char* man_path = proj_man_path(project_name);
    char* manifest = filetoarray(man_path);
    int mlen_status= writeint(socket,strlen(manifest));
    if(write(*socket,manifest,strlen(manifest))<0){
        perror("error at writing manifest in update");
        exit(1);
    }
    
    return 0;
}
int current(int*socket){

    int proj_len = read_conversion(socket);
    char project_name[proj_len+1];
    bzero(project_name,proj_len+1);
    if(read(*socket,project_name,proj_len)<0){
        perror("FAIL TO READ project_name IN CURRENT");
    }
    printf("%s\n",project_name);
    char* manPath = proj_man_path(project_name);
   //repo/projectname/.Manifest
   char* manfiestarray = filetoarray(manPath);
    int check =open(project_name,O_RDONLY);
    if(check>0){
        printf("writing no to client");
        write(*socket,"no",2);
        return 0;
    }
    write(*socket,"ye",2);
    
    printf("%s\n",manfiestarray);
    int number_to_send = strlen(manfiestarray);
    printf("%d\n",number_to_send);
    int converted_number = htonl(number_to_send);
    printf("%d\n",converted_number);
    write(*socket,&converted_number,sizeof(converted_number));
    sleep(0.3);
    write(*socket,manfiestarray,number_to_send);
    return 0;
}
int deleteProject(const char path[])
{
    size_t path_len;
    char *pathAppend;
    DIR *dir;
    struct stat stat_path, stat_sd;
    struct dirent *sd;
    
    stat(path, &stat_path);
    if (S_ISDIR(stat_path.st_mode) == 0) {
        printf("Is not directory\n");
        return 1;
    }
    if ((dir = opendir(path)) == NULL) {
        printf("Can`t open directory\n");
        return 1;
    }
    path_len = strlen(path);
    // iteration through entries in the directory
    while ((sd = readdir(dir)) != NULL) {

        // skip entries "." and ".."
        if (!strcmp(sd->d_name, ".") || !strcmp(sd->d_name, ".."))
            continue;

        // determinate a full path of an sd
        pathAppend = calloc(path_len + strlen(sd->d_name) + 1, sizeof(char));

        if(pathAppend==NULL){

            printf("Cannot allocate enough memory\n");
        }

        //appending path now
        sprintf(pathAppend,"%s/%s",path,sd->d_name);

        stat(pathAppend, &stat_sd);

        // using recursion to remove subdirectories within directories
        if (S_ISDIR(stat_sd.st_mode) != 0) {
            deleteProject(pathAppend);
            continue;
        }

        // remove a file object
        if (unlink(pathAppend) == 0)
            printf("File deleted: %s\n", pathAppend);
        else
            printf("Can`t delete file: %s\n", pathAppend);
        free(pathAppend);
    }
    if (rmdir(path) == 0)
        printf("Removed file: %s\n", path);
    else
        printf("Error:Can`t delete  directory: %s\n", path);
  
    closedir(dir);
}
int destroy(int* socket){
  int proj_len = read_conversion(socket);
    char project_name[proj_len+1];
    bzero(project_name,proj_len+1);
    if(read(*socket,project_name,proj_len)<0){
        perror("FAIL TO READ project_name IN CURRENT");
    }
    printf("%s\n",project_name);
    char* manPath = proj_man_path(project_name);
   //delete the manpath first 
    char projectPath[5+strlen(project_name)];
    sprintf(projectPath,"repo/%s",project_name);
    int check =open(projectPath,O_RDONLY);
    if(check<0){
        printf("writing no to client");
        write(*socket,"no",2);
        return 0;
    }
    if(write(*socket,"ye",2)<0){
        perror("VALID FAIL WRITE->destroy");
    }
    //removing manifest file first
    remove(manPath);
    deleteProject(projectPath);
    return 0;

}
int upgrade(int*socket){
    int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    bzero(project_name,projlen+1);
    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein upgrade");
        exit(1);
    }
    printf("project_name:%s\n",project_name);
    char* man_path = proj_man_path(project_name);
    char* manifest = filetoarray(man_path);
    int mlen_status= writeint(socket,strlen(manifest));
    if(write(*socket,manifest,strlen(manifest))<0){
        perror("error at writing manifest in upgrade");
        exit(1);
    }

    int num_mods = read_conversion(socket); // number of mod occurances
    int num_adds = read_conversion(socket); // number of add occurances
    int i =0;
    while(i<num_mods){
        int path_size = read_conversion(socket);
        char path[path_size+1];
        bzero(path,path_size+1);
        if(read(*socket,path,path_size)<0){
            perror("ERROR AT READING PATH IN UPGRADE->numods");
            exit(1);
        }
        printf("%s",path);
        int filefd = open(path,O_RDONLY);
        if(filefd<0){
            perror("open failed in upgrade");
            exit(1);
        }
        char* modifile = filetoarray(path);
        int write_modsize = writeint(socket,strlen(modifile));
        write(*socket,modifile,strlen(modifile));
        close(filefd);
        i++;
        sleep(0.3);
    }
    i=0;
     while(i<num_adds){
        int path_size = read_conversion(socket);
        char path[path_size+1];
        bzero(path,path_size+1);
        if(read(*socket,path,path_size)<0){
            perror("ERROR AT READING PATH IN UPGRADE->numadd");
            exit(1);
        }
        int filefd = open(path,O_RDONLY);
        if(filefd<0){
            perror("open failed in upgrade ->numadd");
            exit(1);
        }
        char* addfile = filetoarray(path);
        int write_addsize = writeint(socket,strlen(addfile));
        write(*socket,addfile,strlen(addfile));
        close(filefd);
        i++;
        sleep(0.3);
    }
     printf("UPGRADE FINISH");
    return 0;
}

int commit(int*socket){
    int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    bzero(project_name,projlen+1);
    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein COMMIT");
        exit(1);
    }
    printf("project_name:%s\n",project_name);
    char* man_path = proj_man_path(project_name);
    char* manifest = filetoarray(man_path);
    int mlen_status= writeint(socket,strlen(manifest));
    if(write(*socket,manifest,strlen(manifest))<0){
        perror("error at writing manifest in COMMIT");
        exit(1);
    }

    int yesno = read_conversion(socket);
    int req_size = strlen(project_name)+14;
    char compath[req_size];
    bzero(compath,req_size);
    sprintf(compath,"repo/%s/.Commit",project_name);
    if(yesno==1){
        int commit_size = read_conversion(socket); // commit size
        char commit_file[commit_size+1];
        bzero(commit_file,commit_size+1);
        if(read(*socket,commit_file,commit_size)<0){
            perror("fail to read commit fail");
        }   
        printf("\nCONTENTS OF COMMIT FILE\n");
        printf("%s",commit_file);
        printf("\nCONTENTS OF COMMIT FILE\n");

        int fd = open(compath,O_CREAT|O_RDWR,00600);
        if(fd<0){
            perror("fail to open");
           // updateManVersion(char*ManPath,char *projectName, int VersionNumber);
            exit(1);
        }
        if(write(fd,commit_file,commit_size)<0){
            perror("commit file failed to write");
        }
    }else{
        printf("FAIL TO COMMIT");
    }
    printf("sucessful commit");
    return 0;
}
//////////////////////////////////////////////////NEEDTOSEND//////////////////////////////////////////////////////////
int push(int*socket){

    int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    memset(project_name,'\0',projlen+1);


    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein COMMIT");
        exit(1);
    }

    printf("project_name:%s\n",project_name);

    char Project2Name[500];
    memset(Project2Name,'\0',strlen(project_name));

    strcpy(Project2Name,project_name);
    

   //sned the array of commit from client and access it here
   int clientComSize = read_conversion(socket);
    printf("clientComSize is %d\n",clientComSize);
        char clientCom[clientComSize+1];
        bzero(clientCom,clientComSize+1);

        if(read(*socket,clientCom,clientComSize)<0){
        perror("FAIL TO READ project_namein COMMIT");
        exit(1);
    }
    printf("HELLO 1 \n");
    printf("clientComArray is :::::%s\n",clientCom);
    printf("HELLO 2 \n");

   char *serverPath=proj_comm_path(project_name);

   char *serverCommit=filetoarray(serverPath);


   if(strcmp(clientCom,serverCommit)==0){
         //open the server manifest 

       char *manpath=proj_man_path(project_name);

        char*manarray=filetoarray(manpath);
        char* manarray_copy = strdup(manarray);
       // printf("MANARRAY%s \n",manarray);
        char *VersionNo=malloc(sizeof(char)*10000);
        memset(VersionNo,'\0',strlen(VersionNo));
        VersionNo=stripMan(manarray);
        //printf("Version Number%s\n",VersionNo);
        char originalPro[500];
        bzero(originalPro,500);
        sprintf(originalPro,"repo/%s",project_name);

        char duProName[500];
        memset(duProName,'\0',strlen(duProName));
      //  printf("%s\n",project_name);
        strcat(project_name,VersionNo);
      //  printf("%s\n",project_name);
      struct stat st = {0};

     if (stat("ProjectHistory", &st) == -1) {
       mkdir("ProjectHistory", 0700);
     }
     
      
        sprintf(duProName,"ProjectHistory/%s",project_name); 
           

        int duSize=strlen(duProName);
        int OSize=strlen(originalPro);

        int totalSize=duSize+OSize+6;
        
        printf("Origonal Pro Path ::: %s\n",originalPro);
        printf("Duplicate Pro Path ::: %s\n",duProName);

        char * pro5 = (char*)malloc(sizeof(char) * totalSize);
        memset(pro5, '\0', totalSize);

           //duplicated the directory 
            strcat(pro5, "cp -R ");
            strcat(pro5, originalPro);
            strcat(pro5, " ");
            strcat(pro5, duProName);
            system(pro5);

            //now work on incrementing the manifest of server
            //increment version number
           
           //Now create a new directory with a Manifest in it 


           //subdirectory part is done now 
           char*serverComCopy=strdup(serverCommit);
           nodde*dhead=NULL;
             dhead=pushLL(serverCommit,dhead);
             print(dhead);

              nodde* ptr2=dhead;

              while(ptr2!=NULL){

                char*stripPath= strip(ptr2->path);
                mkdir_recursive(stripPath);

                ptr2=ptr2->next;
              }

           char duManPath[500];
           memset(duManPath,'\0',strlen(duManPath));
           //repo
           sprintf(duManPath,"%s/.Manifest",duProName);
           printf("DuManPath %s\n",duManPath);
           int val = atoi(VersionNo+1);
           updateManVersion(manpath,Project2Name,val);

           nodde*chead=NULL;
             chead=pushLL(serverComCopy,chead);
             
            printf("Printing linked list now \n");
            
             print(chead);
           
           int counterA = 0;
         int counterM = 0;
      nodde* chead_ptr = chead;

        while(chead_ptr!=NULL){
    if(strcmp(chead_ptr->indicator,"M")==0){
        counterM++;
      }
     if(strcmp(chead_ptr->indicator,"A")==0){
        counterA++;
    }
    chead_ptr=chead_ptr->next;

}
            printf("Counter A%d\n",counterA);
             printf("Counter M%d\n",counterM);
int writeM_status = writeint(socket,counterM); // num mods
int writeA_status = writeint(socket,counterA); // num adds
char*manPath=proj_man_path(Project2Name);
printf("MANPATH IS :::%s\n",manpath);

nodde*ptr=chead; // ptr  = update nodelist

while(ptr!=NULL){
    if(strcmp(ptr->indicator,"D")==0){
        printf("Target HIT\n");
        printf("PTR PATH::::::::::::::::%s\n",ptr->path);
        //i send what???

        //send the man path of the created duplicate directory 
       dellMan(ptr->path,manpath,Project2Name,val);
                         //need to pull up the filename
        //remove the file from the duplicate copied directory

        char removePath[500];
        memset(removePath,'\0',strlen(removePath));
       char*fileName= pullFile(ptr->path);

       printf("Remove file is %s\n",fileName);
       
        remove(ptr->path);
    }
    ptr=ptr->next;
}
ptr=chead;
while(ptr!=NULL){
    if(strcmp(ptr->indicator,"M")==0){

        printf("Target HIT for M \n");
        printf("PTR PATH::::::::::::::::%s\n",ptr->path);
        //i send what???

        //send the man path of the created duplicate directory 
        dellMan(ptr->path,manpath,Project2Name,val);
        char*path=strdup(ptr->path);
        //sending the path to client 
        int path_size=writeint(socket,strlen(path));
         


        if(write(*socket,path,strlen(path))<0){
            perror("fail to write path in upgrade ->modify");
            exit(1);
        }

        int fileSize=read_conversion(socket);


        char file_M[fileSize+1];
        bzero(file_M,fileSize+1);
        printf("This is the file %s\n",file_M);
        if(read(*socket,file_M,fileSize+1)<0){

            perror("Failed to Read FileSize M");
        }

        printf("This is the file %s\n",file_M);


        int fp=open(path,O_TRUNC|O_RDWR);

        write(fp,file_M,strlen(file_M));

        
        
       //get the file from the client 
       //store it here 
       //call add method here 
        //remove the file from the duplicate copied directory

        char removePath[500];
        memset(removePath,'\0',strlen(removePath));
        char*fileName= pullFile(ptr->path);
        printf("Project2Name%s\n",Project2Name);
        

       addM(Project2Name,path);
        //add the file here 
    }
    ptr=ptr->next;
}
ptr=chead;
while(ptr!=NULL){
    if(strcmp(ptr->indicator,"A")==0){
        //simply add the file here in the manifest
         char* path = strdup(ptr->path);
         int path_size = writeint(socket,strlen(path));
          if(write(*socket,path,strlen(path))<0){
                perror("fail to write path in clientPush->Modify");
                exit(1);
        }

        int fileSize=read_conversion(socket);


        char file_M[fileSize+1];
        bzero(file_M,fileSize+1);
        printf("This is the file %s\n",file_M);
        if(read(*socket,file_M,fileSize+1)<0){

            perror("Failed to Read FileSize M");
        }

        printf("This is the file %s\n",file_M);

        char*fileName=pullFile(path);
        printf("%s\n",fileName);

       // char addFilePath[500];
       // bzero(addFilePath,500);

        //problem is right here
        //projectname pro3

       // sprintf(addFilePath,"repo/%s/%s",Project2Name,fileName);
       // printf("ADDFILE PATH IS  %s\n",addFilePath);

        int fp=open(path,O_CREAT|O_RDWR|O_APPEND,00600);

        write(fp,file_M,strlen(file_M));

        addM(Project2Name,path);
       
    }
    ptr=ptr->next;
    
}       
        char* vnum = stripped(stripMan(manarray_copy));
        char*commitPath=strdup(proj_comm_path(Project2Name));
        char HistoryPath[655];
        bzero(HistoryPath,655);
        sprintf(HistoryPath,"repo/History/%s",Project2Name);
        mkdir_recursive(HistoryPath);
        bzero(HistoryPath,655);
        sprintf(HistoryPath,"repo/History/%s/.History",Project2Name);
        int ft=open(HistoryPath,O_CREAT|O_RDWR|O_APPEND,00600);
        if(ft<0){
            printf("Failed to create HistoryFile\n");
            exit(1);
        }
        char*comarray=filetoarray(commitPath);
        int req_size = strlen(vnum)+strlen("VERSION: \n");
        char vez[req_size+1];
        bzero(vez,req_size+1);
        sprintf(vez,"VERSION: %s\n",vnum);
        write(ft,vez,req_size);
        write(ft,"Successful Push History:\n",strlen("Successful Push History:\n"));
        write(ft,"\n",strlen("\n"));
        write(ft,comarray,strlen(comarray));
        write(ft,"\n",strlen("\n"));
        close(ft);
        remove(proj_comm_path(Project2Name));

   }else{
           //send the message here back to client that it failed 
           
           int i=writeint(socket,1);
   }
    
   printf("Success\n");
   int f=writeint(socket,0);


   
    return 0;
}

int rollback(int*socket){ 
     int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    memset(project_name,'\0',projlen+1);


    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein COMMIT");
        exit(1);
    }

    printf("project_name:%s\n",project_name);



    int rollbackVersion=read_conversion(socket);

    char rollTarget[500];
     bzero(rollTarget,500);
     
    //project#0
    sprintf(rollTarget,"%s#%d",project_name,rollbackVersion);
    printf("ROLLTARGET:%s\n",rollTarget);

   char grabProject[655];
    bzero(grabProject,655);
    sprintf(grabProject,"ProjectHistory/%s",rollTarget);

    DIR*dir;
    dir=opendir(grabProject);
   int x;
    if(dir==NULL){
    printf("RollbackVersion Doesn't exist\n");
     x=writeint(socket,1);
    exit(1);

    }
      x=writeint(socket,0);

 //get the projectname 
 char deleteCurrent[700];
 bzero(deleteCurrent,700);
 sprintf(deleteCurrent,"repo/%s",project_name);

 deleteProject(deleteCurrent);


  //duplicate the rollback project here

    

  int size1=strlen(grabProject);
  int size2=strlen(deleteCurrent);
  int totalSize=size1+size2;
  char * pro5 = (char*)malloc(sizeof(char) * totalSize+6);
        memset(pro5, '\0', totalSize);

           //duplicated the directory 
            strcat(pro5, "cp -R ");
            strcat(pro5, grabProject);
            strcat(pro5, " ");
            strcat(pro5, deleteCurrent);
            system(pro5);

  //delete from the projectHistory

  int i=rollbackVersion;
  DIR* dir2;
 // projecthistory path

totalSize =255;
char * array = (char*)malloc(totalSize+6); 
bzero(array,sizeof(char) * totalSize+6);
           //duplicated the directory 
dir2=opendir(grabProject);
while(dir2!=NULL){
    printf("GrabProject:%s\n",grabProject); 
    strcat(array, "rm -R ");
    strcat(array,grabProject);
    system(array);
   //sprintf(rollTarget,"%s#%d",project_name,rollbackVersion);
    bzero(array,totalSize+6);
    bzero(grabProject,655);
    i++;
   sprintf(grabProject,"ProjectHistory/%s#%d",project_name,i);
   dir2=opendir(grabProject);
   
  }
printf("Rollback Success\n");
return 0;
}

int history(int*socket){
 int projlen = read_conversion(socket);
    printf("proj_name_len:%d\n",projlen);
    char project_name[projlen+1];
    bzero(project_name,projlen+1);
    if(read(*socket,project_name,projlen)<0){
        perror("FAIL TO READ project_namein COMMIT");
        exit(1);
    }
    printf("project_name:%s\n",project_name);
    

}

//////////////////////////////////////////////////NEEDTOSEND//////////////////////////////////////////////////////////


void* thread_handle(void* para){
    
    int client_socket = *(int*)para;
    int *client_socket_ptr = &client_socket;

char server_message[] = "You have reached the server!\n";
 // send(client_socket,server_message,sizeof(server_message),0);
   char buffer[1024];
   bzero(buffer,1024);
   if(read(client_socket,buffer,1024)<0){
       perror("read");
       exit(1);
   }
   printf("Client:%s\n",buffer);
   if(write(client_socket,server_message,29)<0){
       perror("write");
       exit(1);
   }
   bzero(buffer,1024);
    DIR* dir;
    dir = opendir("repo");
    if(dir==NULL){
        int direc = mkdir("repo",00700);
    }
     DIR* dird;
    dird = opendir("ProjectHistory");
    if(dird==NULL){
        int direc = mkdir("ProjectHistory",00700);
    }
    //sendfile:2:9:thing.txt:21:8:stuf.txt:
    //9:0f009fflll1l100JIAlz0&89*1H9s0
    char commandquestion[]= "Command\n";
    sleep(1);
    if(write(client_socket,commandquestion,8)<0){
       perror("write");
       exit(1);
    }
    sleep(1);
    int commandsize = read_conversion(&client_socket);
    char command[commandsize+1];
    bzero(command,commandsize+1);
    if(read(client_socket,command,commandsize)<0){
       perror("read"); // actual reading of command
       exit(1);
   }
   if(strcmp("create",command)==0){
       printf("create has be called");
      int creat_status = create(client_socket_ptr);
      if(creat_status<0){
      printf("CLIENT TRIED TO CREATE EXISTING PROJECT\n");
      }
   }
   if(strcmp("checkout",command)==0){
    printf("checkout has be called");
      int checkout_status = checkout(client_socket_ptr);

      if(checkout_status<0){
      printf("FAIL TO CHECKOUT\n");
      }
   }
   if(strcmp("update",command)==0){
    printf("update has be called");
      int update_status = update(client_socket_ptr);
      if(update_status<0){
      printf("FAIL TO UPDATE\n");
      }
   }
   if(strcmp("current",command)==0){
       printf("current has be called ");
       int current_status = current(client_socket_ptr);
       if(current_status<0){
           printf("FAIL TO FIND CURRENT VERSION");
       }
   }
   if(strcmp("destroy",command)==0){
       printf("destroy has be called ");
       if(pthread_mutex_init(&lock, NULL) != 0)
				printf("Error in Mutex Lock\n");
	//lock it first 		
	pthread_mutex_lock(&lock);
       int destroy_status = destroy(client_socket_ptr);
       if(destroy_status<0){
           printf("FAIL TO destroy");
       }
    //unlock now
    pthread_mutex_unlock(&lock);
   }
   //printf("THIS IS THE CODE\n%s\n",buffer);
   if(strcmp("upgrade",command)==0){
       printf("upgrade has be called ");
       int upgrade_status = upgrade(client_socket_ptr);
       if(upgrade_status<0){
           printf("FAIL TO upgrade");
       }
   }
   if(strcmp("commit",command)==0){
       printf("commit has be called ");
       int commit_status = commit(client_socket_ptr);
       if(commit_status<0){
           printf("FAIL TO commit");
       }
   }

   
     if(strcmp("push",command)==0){
         //Mutex_Lock
		if(pthread_mutex_init(&lock, NULL) != 0)
		printf("Error in Mutex Lock\n");	
		pthread_mutex_lock(&lock);

       printf("push has be called ");
       int push_status= push(client_socket_ptr);
       if(push_status<0){
           printf("FAIL TO push");
       }
       //mutex_unlock
		pthread_mutex_unlock(&lock);
   }
    //projectname versionnumber 
    if(strcmp("rollback",command)==0){
       printf("rollback has be called ");
       int push_status= rollback(client_socket_ptr);
       if(push_status<0){
           printf("FAIL TO rollback");
       }
   }if(strcmp("history",command)==0){
       printf("history has be called ");
       int push_status= history(client_socket_ptr);
       if(push_status<0){
           printf("FAIL TO history");
       }
   }
   
}
 

int main(int argc, char const *argv[]) 
{ 
  
int portNUM = atoi(argv[1]);
    
    int server_socket;
    server_socket = socket(NET,SOCK_STREAM,0);
    struct sockaddr_in server_address;
    server_address.sin_family = NET;
    server_address.sin_port = htons(portNUM);
    server_address.sin_addr.s_addr = INADDR_ANY;
    // char myname[256];
    //  gethostname(myname,255);
   //printf("Host Name: %s\n",myname);
    bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(listen(server_socket,5)<0){
        perror("listen");
        exit(1);
    }


int client_socket;
int *client_socket_ptr;
pthread_t tid;

    //pthreading news to be done here
  
while(1){
    
    //keep creating new Socket
    client_socket = accept(server_socket,NULL,NULL);
    client_socket_ptr = &client_socket; 

    //create threads to allow multiple clients to connect
    pthread_create(&tid, NULL,(void*) &thread_handle, (void*) &client_socket);
}

close(client_socket);
close(*client_socket_ptr);
 //Wait for the threads to complete
	pthread_exit(NULL);
    return 0;
} 
