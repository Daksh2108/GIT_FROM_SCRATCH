#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

typedef struct nodde_
{
  // nodde to store path and hash
  char* indicator;
  char* path;
  char* hash;
  struct nodde_ * next;
}nodde;

//#0|test1.txt|repo/updateTest/test1.txt|4e1eafa48600350c2f2891af3666ee4775942d76



//***************************************************************************************************************************
//remove helper method uses it 
char* anotherstrip(char*path){
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

//************************************************************NEED TO SEND*********************************************************************

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

//***************************************************************************************************************************


//checks if it is a path or not (add and remove both uses it)
int checkIfPath(char *array){

int i=0;

for(i=0;i<strlen(array);i++){

   if(array[i]=='/'){

      return 1;
   }

}
return 0;
}

//***************************************************************************************************************************

//pulls filename from path (add)
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
//**********************************************************************************************

//strip path and compare
//
//************************FOR SAME TEST FILE

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

//************************************************************NEED TO SEND*********************************************************************
nodde* upgradeP( char* manifest, nodde*uhead){
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
        int path_complete = 0;
        int hash_complete =0;
        int indicator_complete =0;
        while (token2 != NULL)
         {   

             if(counter ==1){
                // hash
                temp_indicator = (char*) malloc(sizeof(char)*strlen(token2));
                bzero(temp_indicator,sizeof(char)*strlen(token2)+1);
                strcpy(temp_indicator,token2);
                 indicator_complete=1;
                printf("temp_indocator:%s\n",temp_indicator);
            }

            if(counter==2){
            //path       
                temp_path = (char*) malloc(sizeof(char)*strlen(token2)+1);
                bzero(temp_path,sizeof(char)*strlen(token2)+1);
                strcpy(temp_path,token2);
                path_complete = 1;
                printf("PATH: %s",temp_path);
            } if(counter==3){
                // hash
                temp_hash = (char*) malloc(sizeof(char)*strlen(token2));
                bzero(temp_hash,sizeof(char)*strlen(token2)+1);
                strcpy(temp_hash,token2);
                hash_complete=1;
                printf("HASH:%s\n",temp_hash);
            }
           
            
            if(hash_complete==1 && path_complete==1 && indicator_complete==1){
                if(uhead==NULL){
                    uhead = (nodde*) malloc(sizeof(nodde));
                    uhead->indicator=temp_indicator;
                    uhead->path = temp_path;
                    uhead->hash = temp_hash;
                    uhead->next = NULL;
                }
                else{
                    nodde* temp  = (nodde*) malloc(sizeof(nodde));
                    temp->indicator=temp_indicator;
                    temp->path = temp_path;
                    temp->hash = temp_hash;
                    temp->next = uhead;
                    uhead = temp;
                }
            }
            
            token2 = strtok_r(NULL, "|", &end_token);
            counter++;
        }
        token = strtok_r(NULL, "\n", &end_str);
    }
    return uhead; 
}
//************************************************************NEED TO SEND*********************************************************************

//************************************************************************************************************************************
//helper method for remove method
int chkVersion(char*array){
   char *end_str;
   char *token = strtok_r(array, "\n", &end_str);
   printf("PROJECT VERSION\t");
   printf("%s\n",token);
   printf("FileVersion\tFileName\n");
   token = strtok_r(NULL, "\n", &end_str);

   while(token != NULL)
   {

   int size=strlen(token);

   char savePath[size];
   strcpy(savePath,token);
      char *end_token;
      char *token2 = strtok_r(token, "|", &end_token);

      int counter=0;

         while (token2 != NULL)
      {      
        
        if((counter==0)  || (counter==1)){

         printf("%s",token2);
         printf("\t");
         printf("\t");
        }

         counter++;
         token2 = strtok_r(NULL, "|", &end_token);    
         
      }

      printf("\n");
      token = strtok_r(NULL, "\n", &end_str);

   }

return 0;
}
      void  recurseDirectories(char *d_name,char **array, int *arrSize,char *temppath){

          int fp = open(temppath, O_RDWR | O_APPEND, S_IRWXU);

     struct dirent*sd;
     DIR *dir = opendir(d_name);  

     while((sd=readdir(dir))!=NULL){
     
     //if it is directory  then recurse 
     if(sd->d_type==DT_DIR){
     if((strcmp(sd->d_name, ".")==0) || (strcmp(sd->d_name, "..") == 0)){
          printf("How many times???");
          continue;
          }
     //update prefix according 
     char prefix2[100];
     snprintf(prefix2, sizeof(prefix2), "%s%s%s", d_name, "/", sd->d_name);

     //livehash

     recurseDirectories(prefix2,array,arrSize,temppath);
     }

     //Systems/test5.txt
     //else while loop work
     int skip = 0;

          if((strcmp(sd->d_name, ".")==0) ||  (strcmp(sd->d_name, "..") == 0)){
          skip = 1;
          }
          if(skip == 0){
     
          int fd=0;
     char buffer2[10000];
     memset(buffer2, '\0', 10000);

     //argv[1]+"/"+"sd->name"
     //Change this according to recursion
     char prefix[100];
     snprintf(prefix, sizeof(prefix), "%s%s%s", d_name, "/", sd->d_name);
     char* pro=sd->d_name;
     

     

     if(strstr(prefix, ".") != NULL && strstr(sd->d_name, ".") != NULL ) {

          if(pro[0]!='.'){

          write(fp,sd->d_name,strlen(sd->d_name));
          write(fp,"|",strlen("|"));
          write(fp,prefix,strlen(prefix));
          write(fp,"|",strlen("|"));


     printf("\nPrefix is ::::\t%s\n",prefix);
     printf("\n FILE NAME : \t %s\n",sd->d_name);

   
     unsigned char hash[SHA_DIGEST_LENGTH];

          fd = open(prefix,O_RDONLY);
          int rd=100; // end of file integer if return 0, means eof
          //char buffer[10000];
          //memset(buffer, '\0', 10000);
        

     //putting 
          
          



char buffer[1];
   // char* array = (char*) malloc(sizeof(char)*1);
while(read(fd,buffer,1)>0){
      (*array)[(*arrSize)] = buffer[0];
      (*arrSize)++;
      (*array) = realloc((*array),((*arrSize)+1)*sizeof(char));
}
(*array)[(*arrSize)] = '\0';










 
 // printf("ARRAY FOR LIVE HASH IS  %s \t",*array);
   memset(hash, '\0', strlen(hash));
   SHA1(*array, strlen(*array), hash);
   char code[41];
   memset(code, '\0', 41);
   int i = 0;
   for (i; i < 20; i++)
   {
      snprintf(code + i * 2, 3, "%02x ", hash[i]);
   }
  printf("code is :::%s",code);

   memset(*array, '\0', strlen(*array));

   write(fp, code, strlen(code));
   write(fp, "\n", strlen("\n"));
   memset(code, '\0', strlen(code));

   *arrSize=0;
     
     //array to hash method and then you get the hash of that
     //d0b4c8ae7563d1149c0c147c3a8a6d72edc67bdc
     
     //reset arrSize back to 0 
         
     }
          }
     }
     }
     
      }
delMan(char *pathT, char*ManPath,char *projectName){

//printf("TARGET:::::::::%s",pathT);


//printf("HERE NOW");

char tempPath[300];
sprintf(tempPath, "repo/%s/.tempMan", projectName);
//target path 

// printf("\n");

   char* array=filetoarray(ManPath);
   char* array_copy = strdup(array);
   char* vnum = anotherstrip(array_copy);

   //Created a temp manifest same place as the original manifest location
   int fp = open(tempPath, O_CREAT | O_RDWR | O_APPEND, S_IRWXU);


   write(fp,vnum,strlen(vnum));
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
      //0
      //fileversion 1
      //|filename|
      //path 3
      //|Hash

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


   //first token will be called and save original str token and break that token further and check the path with token 3


   //if it is the target string is not same as the token3 then write original str into the newfile tempManifest 
   //delete the origional one and rename the temp one with .Manifest

//delete file here
remove(ManPath);
//rename file back to Manifest
rename(tempPath, ManPath); 
}


dellMan(char *pathT, char*ManPath,char *projectName,char*serverVersion){

//printf("TARGET:::::::::%s",pathT);


//printf("HERE NOW");

char tempPath[300];
sprintf(tempPath, "repo/%s/.tempMan", projectName);
//target path 

// printf("\n");

   char* array=filetoarray(ManPath);


   //Created a temp manifest same place as the original manifest location
   int fp = open(ManPath, O_CREAT | O_RDWR | O_APPEND |O_TRUNC, S_IRWXU);


   write(fp,serverVersion,strlen(serverVersion));
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
      //0
      //fileversion 1
      //|filename|
      //path 3
      //|Hash

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
//********************************************************************************************************************
//WORKING ON DEL MANIFEST
int rem(char *projectName, char *path)
{

//printf("DOES IT COMER ");
char proName[255];
char project[255];
strcpy(proName,projectName);
strcpy(project,projectName);
//printf("%s",proName);

int checkPos=checkIfPath(path);


//file
if(checkPos==0){
   char fileP[500];
   sprintf(fileP,"repo/%s/%s",proName,path);

   int kd = open(fileP, O_RDONLY);


   if(kd<0){

      printf("Cannot remove:file does not exist in Manifest\n");
      return 1;
   }


//directory
}else if(checkPos==1){

   char fileP[500];
   
   //projectName/directory
   sprintf(fileP,"repo/%s/%s",proName,path);
   int kd = open(fileP, O_RDONLY);

   if(kd<0){

      printf("Cannot remove:file does not exist in Manifest\n");
      return 1;
   }

}


DIR *dir;
struct dirent *sd;
//test in here but need to append here

//project/test
int fd;
char proPath[500];
char *file = path;
int chck=checkIfPath(file);
if(chck==0){

   char filePath[500];
   //projectname/
   sprintf(filePath,"repo/%s/%s",proName,file);
   fd = open(filePath, O_RDONLY);
//   printf("%s",filePath);
   //projectname/test2.txt
   // printf("%s",proName);
}else{

   // /daskh/test1.txt
      sprintf(proPath,"repo/%s/%s",proName,file);

   //  printf("PRO PATH %s",proPath);

   fd = open(proPath, O_RDONLY);
   
}

// dir=opendir(file/orpathname);
//textfile
dir = opendir(path);

if (dir == NULL)
{

   //not a directory
   char manPath[255];
   //char filePath[255];

   sprintf(manPath, "repo/%s/.Manifest", projectName);
   //printf("%s", manPath);


   char root[100] = "repo/";

   int ck=checkIfPath(file);
if(ck==0){
   char *path = strcat(root, proName);
   // printf("FILE Path is ::: %s /t ",path);
   char filePath[500];

   sprintf(filePath,"%s/%s",path,file);

   delMan(filePath,manPath,project);

   //call the del man function here
   //exit(0);

   //  write(fp, path, strlen(path));

}else{
   //char *path = strcat(root, proPath);
      
      // printf("PRO Path is ::: %s",path);
      delMan(proPath,manPath,project);
   
   //  write(fp, path, strlen(path));

}
   
}
else
{
   printf("Not a file Path \n");
}

return 0;
}





//***************************************************************************************************************************
//filename or path
//daksh sub2/test12.txt 

//repo/daksh/test12.txt
int add(char *projectName, char *path)

{
char proName[255];
char projectN[255];
strcpy(projectN,projectName);
char projectPath[255];
strcpy(proName,projectName);
strcpy(projectPath,path);
//printf("%s",proName);

int checkPos=checkIfPath(path);


//file that is not in directory should not be added 
if(checkPos==0){
   char fileP[500];
   sprintf(fileP,"repo/%s/%s",proName,path);

   int kd = open(fileP, O_RDONLY);


   if(kd<0){

      printf("File does not exist in Project ");
      return 1;
   }


//directory
}else if(checkPos==1){

   char fileP[500];
   
   //projectName/directory
   sprintf(fileP,"repo/%s/%s",proName,path);
   int kd = open(fileP, O_RDONLY);

   if(kd<0){

      printf("File does not exist in Project");
      return 1;
   }


}


//Check for same filePath in the manifest and exit here
char manPath[255];
   //char filePath[255];

sprintf(manPath, "repo/%s/.Manifest", projectName);
   //printf("%s", manPath);
int checkPosition=checkIfPath(path);


//file that is not in directory should not be added 
if(checkPosition==0){
   char fileP[500];
   sprintf(fileP,"repo/%s/%s",proName,path);

   //int kd = open(fileP, O_RDONLY);
      int val=checkDup(fileP,manPath);

      if(val==1){

         printf("FILE ALREADY ADDED IN MANIFEST \n");
         return 1;
      }

}else if(checkPosition==1){

   char fileP[500];
   
   //projectName/directory
   sprintf(fileP,"repo/%s/%s",proName,path);
   //int kd = open(fileP, O_RDONLY);

int val=checkDup(fileP,manPath);

      if(val==1){

         printf("FILE ALREADY ADDED IN MANIFEST \n");
         return 1;
      }

}


DIR *dir;
struct dirent *sd;

//test in here but need to append here
   

//project/test
int fd;
char proPath[500];
char *file = path;
int chck=checkIfPath(file);
if(chck==0){

   char filePath[500];
   //projectname/
   sprintf(filePath,"repo/%s/%s",proName,file);
   
   fd = open(filePath, O_RDONLY);
   //printf("%s",filePath);
   //projectname/test2.txt
   //printf("%s",proName);
}else{

   // /daskh/test1.txt
      sprintf(proPath,"repo/%s/%s",proName,file);

      //printf("PRO PATH %s",proPath);

   fd = open(proPath, O_RDONLY);

   
}

// dir=opendir(file/orpathname);
//textfile
dir = opendir(path);

if (dir == NULL)
{
   
   

   //printf("IS IT GOING HERE???");
   //not a directory
 char roott[100] = "repo/";

   int check=checkIfPath(file);
if(check==0){
   char *path = strcat(roott, file);
   if(checkDup(path,manPath)==1){

   printf("This file already exist in Manifest\n");
   return 1;     
   }

}else{
   char *path = strcat(roott, proPath);

   if(checkDup(path,manPath)==1){

   printf("This file already exist in Manifest\n");
   return 1;    
   }

}

   //************
   char *versionNumber = "#0";
   int fp = 0;
   fp = open(manPath, O_RDWR | O_APPEND);

   //printf("%d", fp);
   write(fp, versionNumber, strlen(versionNumber));
   write(fp, "|", strlen("|"));



   //************************FILENAME WORK HERE 
   //Write filename else if path pull last from the last string part
   if(checkIfPath(file)==0){

      write(fp, file, strlen(file));
      
   }else if(checkIfPath(file)==1){
   //call string pull out 
   char *str;
   str= pullFile(file);
   write(fp, str, strlen(str));


   }
      write(fp, "|", strlen("|"));
   //exit(0);

  

   //printf("FD :::::%d",fd);



//*****************WRITING THE PATH FILE TEXT

   //
   char root[100] = "repo/";
   //repo/test1.txt/daksh

   int ck=checkIfPath(file);
if(ck==0){
   char *path = strcat(root, proName);
   //repo/file
   //projectname/testfile
   char filePath[500];

   sprintf(filePath,"%s/%s",path,file);
   write(fp, filePath, strlen(filePath));
   //***********************************************

}else{

   printf("does it go here?????");
   char projectPathNew[500];
   char *path = strcat(root, proName);
   //repo/daksh
   sprintf(projectPathNew,"%s/%s",path,projectPath);

   //now use sprinf

   write(fp, projectPathNew, strlen(projectPathNew));

}
   
   write(fp, "|", strlen("|"));
   // printf("%p",fp);

   int rd = 100;

   int arrSize = 0;
   //int *ptr = calloc(5, sizeof *ptr);


char buffer[1];
   char* array = (char*) malloc(sizeof(char)*1);
   int size = 0;
while(read(fd,buffer,1)>0){
      array[size] = buffer[0];
      size++;
      array = realloc(array,(size+1)*sizeof(char));
}
array[size] = '\0';


   
unsigned char hash[SHA_DIGEST_LENGTH];

   printf("Array:%s\n",array);

   memset(hash, '\0', strlen(hash));
   SHA1(array, strlen(array), hash);
   char code[41];
   memset(code, '\0', 41);
   int i = 0;
   for (i; i < 20; i++)
   {
      snprintf(code + i * 2, 3, "%02x ", hash[i]);
   }

   printf("code:::::%s",code);
   memset(array, '\0', strlen(array));

   write(fp, code, strlen(code));
   write(fp, "\n", strlen("\n"));
   memset(code, '\0', strlen(code));
   

   
}
else
{
   printf("Not a file Path \n");
}

return 0;
}
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


   //char *array = (char *)malloc(sizeof(char)*10000);


   //char buffer2[10000];
   
   write(fp, path, strlen(path));
   //***********************************************

   write(fp, "|", strlen("|"));
   // printf("%p",fp);

 
   
unsigned char hash[SHA_DIGEST_LENGTH];
int rd = 100;

   int arrSize = 0;
   //int *ptr = calloc(5, sizeof *ptr);


char buffer[1];
   char* array = (char*) malloc(sizeof(char)*1);
   int size = 0;
while(read(fd,buffer,1)>0){
      array[size] = buffer[0];
      size++;
      array = realloc(array,(size+1)*sizeof(char));
}
array[size] = '\0';
  memset(hash, '\0', strlen(hash));
   SHA1(array, strlen(array), hash);
   char code[41];
   memset(code, '\0', 41);
   int i = 0;
   for (i; i < 20; i++)
   {
      snprintf(code + i * 2, 3, "%02x ", hash[i]);
   }

   printf("Code::%s",code);

   memset(array, '\0', strlen(array));

   write(fp, code, strlen(code));
   write(fp, "\n", strlen("\n"));
   memset(code, '\0', strlen(code));

   close(fp);

return 0;
}