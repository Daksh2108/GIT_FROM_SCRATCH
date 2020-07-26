#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <netdb.h>
#include <dirent.h>
#include <openssl/sha.h>
#include "remadd.h"
#define MAX_PATHNAME_LEN 255
#define NET AF_INET
#include <libgen.h>
typedef struct Node_
{
  // node to store path and hash
  char* path;
  char* hash;
  char* vnum;
  struct Node_ * next;
}node;
typedef struct strarray_{
    char* path;
    struct  strarray_ *next;
}strarray;
char* proj_man_path(char*project_name){
    char* path_manifest = (char*) malloc(sizeof(char)*(15+strlen(project_name)));
    sprintf(path_manifest,"repo/%s/.Manifest",project_name);
    //printf("%s\n",path_manifest);
    return path_manifest;
}
char* proj_con_path(char*project_name){
    char* path_manifest = (char*) malloc(sizeof(char)*(15+strlen(project_name)));
    sprintf(path_manifest,"repo/%s/.Conflict",project_name);
    //printf("%s\n",path_manifest);
    return path_manifest;
}
char* proj_upd_path(char*project_name){
    char* path_manifest = (char*) malloc(sizeof(char)*(13+strlen(project_name)));
    sprintf(path_manifest,"repo/%s/.Update",project_name);
    //printf("%s\n",path_manifest);
    return path_manifest;
}
char* proj_comm_path(char*project_name){
    char* path_Commit = (char*) malloc(sizeof(char)*(15+strlen(project_name)));
    sprintf(path_Commit,"repo/%s/.Commit",project_name);
    //printf("%s\n",path_manifest);
    return path_Commit;
}
char* proj_path(char*project_name){
    char* path_manifest = (char*) malloc(sizeof(char)*(5+strlen(project_name)));
    sprintf(path_manifest,"repo/%s",project_name);
    //printf("%s\n",path_manifest);
    return path_manifest;
}
int writeint(int* socket,int x){
    printf("num sent:%d\n",x);
    int converted_number = htonl(x);
    if(write(*socket,&converted_number,sizeof(converted_number))<0){
        perror("fail write_int");
        exit(1);
    }
    return 0;
}
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
char* strip(char*path){
    // find first \n
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
char* strip0(char*path){
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
int connectServer(int *client_socket, struct sockaddr_in server_address, char* ip, int PORT){

 
    int check=isdigit(*ip);
    if(check>0){
    //connect with static ip
    *client_socket = socket(AF_INET,SOCK_STREAM,0);
    if(*client_socket < 0){ 
    printf("Error in creating socket\n");
    return -1;
    }
    bzero((char*)&server_address,sizeof(server_address));
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(PORT); 
     server_address.sin_addr.s_addr=inet_addr(ip);
    }else{
        //Connect with hostname

    *client_socket = socket(NET,SOCK_STREAM,0);

    //struct sockaddr_in server_address;
    bzero((char*)&server_address,sizeof(server_address));
    server_address.sin_family = NET; // what kind of client adresds
    server_address.sin_port = htons(PORT); // what port does this adress acesss: will change to command line
    struct hostent* result = gethostbyname(ip);
    bcopy( (char*)result->h_addr, (char*)&server_address.sin_addr.s_addr, result->h_length);

    }
    int connect_status = 0;
//keep reconnecting 3 times until it gets connect 
    int counter =0;
    do{
       connect_status = connect(*client_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(connect_status<0){
        perror("connect");
        sleep(3);
    }
    counter++;
    }while(connect_status<0&&counter<3);
    
    char buffer[1024];
    bzero(buffer,1024);
    if((write(*client_socket,"I AM CONNECTED",14))<0){
        perror("write");
    }

    if((read(*client_socket,buffer,400))<0){
        perror("read");
    }
    printf("SERVER RESPONSE WAS: %s",buffer);

return 0;
}
int lengthof(char* array){
int i=0;
    while(array[i]!='\0'){
        i++;
    }
    return i;
}
int transfer(int* socket,char*filename){
    char* array  = filetoarray(filename);
    //printf("this is size of :%d",sizeof(array));
    int i=lengthof(array);
    if((write(*socket,array,i))<0){
        return -1;
    }
    return 1;
}
void printList(node* n) 
{ 
    while (n != NULL) { 
        printf("|%s ",n->path);
        printf("%s ",n->hash); 
        printf(" %s|->",n->vnum); 
        n = n->next; 
    } 
    printf("\n");
} 
void updateManVersion(char*ManPath,char *projectName, int VersionNumber){
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
int create(char* project_name,int*client_socket){
/*
The create command will fail if the project name already exists on the server or the client can not communicate
with the server. Otherwise, the server will create a project folder with the given name, initialize a .Manifest for it
and send it to the client. The client will set up a local version of the project folder in its current directory and
should place the .Manifest the server sent in it.
*/
    int write_create_len_status = writeint(client_socket,strlen("create"));
    if(write(*client_socket,"create",strlen("create"))<0){
        perror("ERROR AT write_create");
    }
    sleep(0.3);
    int write_pname_len_status = writeint(client_socket,strlen(project_name));
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("passing project_name error");
    }
     // passing project_name;
    // 1. ye 2
    // 2. no  2
    char valid[3];
    bzero(valid,3);
    if((read(*client_socket,valid,2))<0){
        perror("reading valid or not in CREATE");
    }
    if(strcmp("no",valid)==0){
        return -1;
    }
    if(strcmp("ye",valid)==0){
        printf("we in bois\n");
    }
    DIR* dir;
    char repo_projectname_path[strlen(project_name)+5];
    sprintf(repo_projectname_path,"repo/%s",project_name);
    dir = opendir(project_name);
    if(dir==NULL){
        int direc = mkdir(repo_projectname_path,00700);
    }
    struct dirent *dp;
    // char pathFile[MAX_PATHNAME_LEN]; // path to manifest
    // sprintf(pathFile,"%s/.Manifest",project_name);
    char * pathFile = proj_man_path(project_name);
    printf("%s\n",pathFile);
    int direcMAN = open(pathFile, O_RDWR | O_APPEND | O_CREAT,00700);
    int received_int = 0;
    if(read(*client_socket,&received_int, sizeof(received_int))<0){
        perror("fail to read netcode manfiest_size");
    }
    int converted_int = ntohl(received_int);
    printf("%d",converted_int);
    char manfiest_array[converted_int];
    if(read(*client_socket,manfiest_array,converted_int)<0){
        perror("fail reading manfiestarray");
    }
    printf("%s\n",manfiest_array);
    write(direcMAN,manfiest_array,converted_int);
    return 0;
}
int read_conversion(int* client_socket){
    int received_int = 0;
    if(read(*client_socket,&received_int, sizeof(received_int))<0){
        perror("fail to read netcode int");
    }
    int converted_int = ntohl(received_int);
    printf("CONVERSION READ_CONVERSION:%d\n",converted_int);
    return converted_int;
}
int checkout(char* project_name,int*client_socket){
    // char snum[2];
    // bzero(snum,2);
    // snum[0]='8';
    // write(*client_socket,snum,1); // create size 
    // sleep(0.5);
    // write(*client_socket,"checkout",8); // passing "create"
    // sleep(0.5);
    //char namelen = strlen(project_name)+'0';
    //char namelenstr[1];
    int write_checkout_len_status = writeint(client_socket,strlen("checkout"));
    if(write(*client_socket,"checkout",strlen("checkout"))<0){
        perror("ERROR AT write_checkout");
    }
    sleep(0.3);
    int write_pname_len_status = writeint(client_socket,strlen(project_name));
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("passing project_name error");
    }
     //passing project_name;
    // 1. ye 2
    // 2. no  2
    char valid[3];
    bzero(valid,3);
    if((read(*client_socket,valid,2))<0){
        perror("reading valid or not in CHECKOUT");
    }
    if(strcmp("no",valid)==0){
        return -1;
    }
    if(strcmp("ye",valid)==0){
        printf("we in bois\n");
    }
    printf("num of paths:");
    int num_paths = read_conversion(client_socket);
    int i =0;
    printf("\n");
    while(i<num_paths){
        int str_length = read_conversion(client_socket);
        char str[str_length+1];
        bzero(str,str_length+1);
        if((read(*client_socket,str,str_length))<0){
        perror("reading STRING");
        }
        mkdir_recursive(str);
        i++;
    }
    i=0;
    printf("num of files:");
    int num_file = read_conversion(client_socket);
    printf("\n");
    while(i<num_file){
        int size_path,size_file;
        size_path = read_conversion(client_socket);
        char path[size_path+1];
        bzero(path,size_path+1);
        if(read(*client_socket,path,size_path)<0){
            perror("READ FAILED AT PATH_NUM_FILE");
        }
        printf("full_path:%s\n",path);
        int fd = open(path,O_CREAT|O_RDWR,00700);
        size_file = read_conversion(client_socket);
        //printf("^^ convert size_file\n");
        char file_content[size_file+1];
        bzero(file_content,size_file+1);
        if(read(*client_socket,file_content,size_file)<0){
            perror("READ FAILED AT FILE_NUM_FILE");
        }
        printf("\ncontents of file:\n%s\n",file_content);
        write(fd,file_content,size_file);
        close(fd);
        i++;
    }
    // send the manifest
    int man_length = read_conversion(client_socket);
    char mani[man_length+1];
    bzero(mani,man_length+1);
    if(read(*client_socket,mani,man_length)<0){
        perror("FAIL TO READ MANI");
    }
    char path_man[MAX_PATHNAME_LEN]; // path to manifest
    sprintf(path_man,"repo/%s/.Manifest",project_name);
    int manfd = open(path_man,O_CREAT|O_RDWR|O_APPEND,00600);
    if(manfd<0){
        perror("fail to open .Manifest in checkout");
    }
    printf("%s",mani);
    write(manfd,mani,man_length);
    return 0;
}
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

node* pash( char* manifest, node*chead,int*version){
    char *end_str;
    char *token = strtok_r(manifest, "\n", &end_str);
    *version = atoi(stripped(token));
    token = strtok_r(NULL, "\n", &end_str); // skipping manifest version
    while (token != NULL)
    {
        char *end_token;
        char *token2 = strtok_r(token, "|", &end_token);
        int counter =1;
        char* temp_path;
        char* temp_hash;
        char* temp_vnum;
        int path_complete = 0;
        int hash_complete =0;
        int vnum_complete = 0;
        // vnum|filename|path|hash
        while (token2 != NULL)
         {   
            if(counter==3){
            //path       
                temp_path = (char*) malloc(strlen(token2)+1);
                bzero(temp_path,strlen(token2)+1);
                strcpy(temp_path,token2);
                path_complete = 1;
                //printf("PATH: %s",temp_path);
            }
            if(counter ==4){
                // hash
                temp_hash = (char*) malloc(strlen(token2)+1);
                bzero(temp_hash,sizeof(char)*strlen(token2)+1);
                strcpy(temp_hash,token2);
                hash_complete=1;
                //printf(" HASH:%s\n",temp_hash);
            }
            if(counter == 1){
                temp_vnum = (char*) malloc(strlen(token2)+1);
                bzero(temp_vnum,sizeof(char)*strlen(token2)+1);
                strcpy(temp_vnum,token2);
                vnum_complete=1;
            }
            counter++;
            if(hash_complete==1&&path_complete==1&&vnum_complete==1){
                if(chead==NULL){
                    chead = (node*) malloc(sizeof(node));
                    chead->path = temp_path;
                    chead->hash = temp_hash;
                    chead ->vnum = temp_vnum;
                    chead->next = NULL;
                }
                else{
                    node* temp  = (node*) malloc(sizeof(node));
                    temp->path = temp_path;
                    temp->hash = temp_hash;
                    temp->vnum = temp_vnum;
                    temp->next = chead;
                    chead = temp;
                }
            }
            token2 = strtok_r(NULL, "|", &end_token);
        }
        token = strtok_r(NULL, "\n", &end_str);
    }
    return chead;
    //printf("THIS IS THE END OF PASH");
}
node* pash_temp( char* manifest, node*chead){
    char *end_str;
    char *token = strtok_r(manifest, "\n", &end_str); 
    //token = strtok_r(NULL, "\n", &end_str); // skipping manifest version
    // vnum|path|hash
    while (token != NULL)
    {
        char *end_token;
        char *token2 = strtok_r(token, "|", &end_token);
        int counter =1;
        char* temp_path;
        char* temp_hash;
        char* temp_vnum;
        int path_complete = 0;
        int hash_complete =0;
        int vnum_complete =0;
        while (token2 != NULL)
         {   
             if(counter == 1){
                temp_vnum = (char*) malloc(strlen(token2)+1);
                bzero(temp_vnum,sizeof(char)*strlen(token2)+1);
                strcpy(temp_vnum,token2);
                vnum_complete=1;
            }
            if(counter==2){
            //path       
                temp_path = (char*) malloc(strlen(token2)+1);
                bzero(temp_path,strlen(token2)+1);
                strcpy(temp_path,token2);
                path_complete = 1;
                //printf("PATH in: %s",temp_path);
            }
            if(counter ==3){
                // hash
                temp_hash = (char*) malloc(strlen(token2)+1);
                bzero(temp_hash,strlen(token2)+1);
                strcpy(temp_hash,token2);
                hash_complete=1;
                //printf(" HASH:%s ->>>>> phashtemp\n",temp_hash);
            }
            counter++;
            if(hash_complete==1&&path_complete==1&&vnum_complete==1){
                if(chead==NULL){
                    chead = (node*) malloc(sizeof(node));
                    chead->path = temp_path;
                    chead->hash = temp_hash;
                    chead->vnum = temp_vnum;
                    chead->next = NULL;
                }
                else{
                    node* temp  = (node*) malloc(sizeof(node));
                    temp->path = temp_path;
                    temp->hash = temp_hash;
                    chead->vnum = temp_vnum;
                    temp->next = chead;
                    chead = temp;
                }
            }
            token2 = strtok_r(NULL, "|", &end_token);
        }
        token = strtok_r(NULL, "\n", &end_str);
    }
    return chead;
    //printf("THIS IS THE END OF PASH");
}

void del(node**gen,char*tar_path,char* tar_hash,int mode){
    node* ptr = (*gen);
    node* prev;
    if(mode==0){
        if(ptr!=NULL&&strcmp(ptr->path,tar_path)==0&&strcmp(ptr->hash,tar_hash)==0){
            *gen = (*gen)->next;
            free(ptr);
            return;
        }
    }
    if(ptr!=NULL&&strcmp(ptr->path,tar_path)==0&&mode==1){
        *gen = (*gen)->next;
        free(ptr);
        return;
    }
    while(ptr!=NULL){
        if(mode ==0){
            if(strcmp(ptr->path,tar_path)==0&&strcmp(ptr->hash,tar_hash)==0){
                printf("found %s matches %s",ptr->path,tar_path);
                break;
            }else{

                if(strcmp(ptr->path,tar_path)==0){
                    printf("found %s matches %s and mode = 1",ptr->path,tar_path);
                    break;
                }
            }
        }
            prev = ptr;
            ptr = ptr->next;
    }
    if(ptr==NULL)return;
    prev->next = ptr->next;
    free(ptr);
    return;
}
void compare_node(node**chead,node**lhead){
   // we compare the nodes and remove them if they are the same
    printf("inside compare_node\n");
    node* ptr_c = (*chead);
    node* ptr_l = (*lhead);
    int indicator = 0;
    while(ptr_c!=NULL){
        while(ptr_l!=NULL){
            if(strcmp(ptr_c->path,ptr_l->path)==0){
                if(strcmp(ptr_c->hash,ptr_l->hash)==0){
                    printf("calling delete on path\n");
                    del(chead,ptr_c->path,ptr_c->hash,0);
                    del(lhead,ptr_l->path,ptr_c->hash,0);
                    ptr_c = (*chead); // to reset the positions
                    ptr_l = (*lhead); // to reset the postions (compare from head again);
                    indicator = 1;
                    break;
                }
            }
            ptr_l = ptr_l->next;
        }
        if(indicator==1){
            indicator =0;
            continue;
        }else{
            ptr_c = ptr_c->next;
            ptr_l = (*lhead);
        }

    }
    return;
}
void diff_trav_write(node*lhead,node*chead,int*fd){
    node* lhead_ptr = lhead;
    while(lhead_ptr!=NULL){
        //'C <file/path> <live hash>'
        // C|PATH|Hash
        int req_size = strlen(lhead_ptr->path)+strlen(lhead_ptr->hash)+(sizeof(char)*4);
        char conflict[req_size];
        bzero(conflict,req_size);
        //printf("HASH: %s",lhead_ptr->hash);
        sprintf(conflict,"C|%s|%s\n",lhead_ptr->path,lhead_ptr->hash);

        write(*fd,conflict,req_size);
        lhead_ptr = lhead_ptr->next;
    }
    node*chead_ptr = chead;
    while(chead_ptr!=NULL){
        //'C <file/path> <live hash>'
        // C|PATH|Hash
        int req_size = strlen(chead_ptr->path)+strlen(chead_ptr->hash)+(sizeof(char)*4);
        char conflict[req_size];
        bzero(conflict,req_size);
        sprintf(conflict,"C|%s|%s\n",chead_ptr->path,chead_ptr->hash);
        write(*fd,conflict,req_size);
        chead_ptr = chead_ptr->next;
    }
}
void diff_trav_write_v2(node**shead,node**nchead,int*fd){
    node* shead_ptr = *(shead);
    node* nchead_ptr = *(nchead);
    int indicator=0;
    while(shead_ptr!=NULL){
        while(nchead_ptr!=NULL){
            if(strcmp(shead_ptr->path,nchead_ptr->path)==0){
                if(strcmp(shead_ptr->hash,nchead_ptr->hash)==0){
                    printf("IMPOSSIBLEEEE\n");
                    exit(1);
                }
                // write "modify" 'M|path|server's hash\n'
                int size = strlen(shead_ptr->path)+strlen(shead_ptr->hash)+((sizeof(char))*4);
                char mstring[size];
                bzero(mstring,size);
                sprintf(mstring,"M|%s|%s\n",shead_ptr->path,shead_ptr->hash);
                write(*fd,mstring,size);
                del(shead,shead_ptr->path,shead_ptr->hash,0);
                del(nchead,nchead_ptr->path,nchead_ptr->hash,0);
                shead_ptr = *(shead);
                nchead_ptr = *(nchead);
                indicator =1;
                break;
            }
            nchead_ptr = nchead_ptr->next;
        }
        if(indicator ==1){
            indicator=0;
            continue;
        }else{
            shead_ptr = shead_ptr->next;
            nchead_ptr = (*nchead);
        }
    }
    if(*(nchead)!=NULL){
        // del
        node* nptr = *(nchead);
        while(nptr!=NULL){
            int size2 = strlen(nptr->path)+strlen(nptr->hash)+((sizeof(char))*4);
            char dstring[size2];
            bzero(dstring,size2);
            sprintf(dstring,"D|%s|%s\n",nptr->path,nptr->hash);
            write(*fd,dstring,size2);
            nptr = nptr->next;
        }
    }
     if(*(shead)!=NULL){
        // add
        node* sptr = *(shead);
        while(sptr!=NULL){
            int size2 = strlen(sptr->path)+strlen(sptr->hash)+((sizeof(char))*4);
            char astring[size2];
            bzero(astring,size2);
            sprintf(astring,"A|%s|%s\n",sptr->path,sptr->hash);
            write(*fd,astring,size2);
            sptr = sptr->next;
        }
    }
    // check if the path is the same, if it is write modify
    // if not the same in server.. write add
    // if node still remain in nchead-> del
}
int check(node* noded,char* tar_path,char* tar_hash){
    node* node_ptr = noded;
    while(node_ptr!=NULL){
        if(strcmp(node_ptr->path,tar_path)==0){
            if(strcmp(node_ptr->hash,tar_hash)!=0){
                return 1;
            }
        }
        node_ptr = node_ptr->next;
    }
    return 0;
}
void triple_conflict_write(node*shead,node*chead,node*lhead,int *fd,int*created){
    // left over shead, chead, lhead
    // conflict only happens when all 3 nodes have the same path but different hash
    int check_chead =0;
    int check_shead =0;
    node* lhead_ptr = lhead;
    while(lhead_ptr!=NULL){
        char* path = strdup(lhead_ptr->path);
        char* hash = strdup(lhead_ptr->hash);
        int check_chead =check(chead,path,hash);
        int check_shead = check(shead,path,hash);
        if(check_chead==1&&check_shead==1){
        (*created)+=1;
        int req_size = strlen(lhead_ptr->path)+strlen(lhead_ptr->hash)+(sizeof(char)*4);
        char conflict[req_size];
        bzero(conflict,req_size);
        sprintf(conflict,"C|%s|%s\n",lhead_ptr->path,lhead_ptr->hash);
            if(write(*fd,conflict,req_size)<0){
                perror("FAIL TO WRITE IN TRIPLE CONFLICT");
                exit(1);
            }
        }
        lhead_ptr = lhead_ptr->next;
    }
}
int checkstrarray(strarray* array,char* patg){
    strarray* array_ptr = array;
    while(array_ptr!=NULL){
        if(strcmp(patg,array_ptr->path)==0){
            return 1;
        }
        array_ptr = array_ptr->next;
    }
    return 0;
}
int write_commit_file(node*shead,node*chead,node*lhead,char* project_name){
    int req_size = strlen(project_name)+14;
    char compath[req_size];
    bzero(compath,req_size);
    sprintf(compath,"repo/%s/.Commit",project_name);
    int fd  = open(compath,O_CREAT|O_RDWR,00700);
    int indicator =0;
    strarray* headstrings=NULL;
    node*chead_ptrx =chead;
    // guarentee that files on server is the same version as client manifest
    while(chead_ptrx!=NULL){
        char * fpath  = chead_ptrx->path;
        char* fhash = chead_ptrx ->hash;
        char* fvnum  = stripped(chead_ptrx -> vnum);
        int check_shead = check(shead,fpath,fhash);
        if(check_shead==1){
            printf("server and client manifest is the same yet there is a file that both contain but have different hash");
            // +1
            int x = atoi(fvnum);
              x=x+1;
                char vNumber[500];
                bzero(vNumber,500);
            sprintf(vNumber,"%d", x);
                printf("VNUMBER IS %s \n",vNumber);
            int modify_line_size_diff  = strlen(fpath)+5+strlen(fhash)+strlen(fvnum);
            char modify_line_diff[modify_line_size_diff];
            bzero(modify_line_diff,modify_line_size_diff);
            sprintf(modify_line_diff,"M|%s|%s|%s\n",fpath,fhash,vNumber);
            if(write(fd,modify_line_diff,modify_line_size_diff)<0){
                perror("FAIL TO WRITE IN WRITE_COMMIT_FILE ->modify version");
                exit(0);
            }
            strarray* temp_string = (strarray*) malloc(sizeof(strarray));
            temp_string->path = fpath;
            if(headstrings==NULL){
                headstrings = temp_string;
            }
            else{
                temp_string->next =headstrings;
                headstrings = temp_string;
            }
            del(&chead,fpath,"w/e",1);
        }

        chead_ptrx = chead_ptrx->next;
    }

    // Writing M to commit file;
    // find diff files hashes between
    node* lhead_ptr = lhead;
    //node* lhead_ptr = lhead;
    while(lhead_ptr!=NULL){
        char * fpath  = lhead_ptr->path;
        char* fhash = lhead_ptr ->hash;
        int check_chead = check(chead,fpath,fhash); // if same path but diff hash -> return 1;
        if(check_chead==1){
            printf("\nFOUND A MODIFICATION\n");
            indicator++;
            int modify_line_size  = strlen(fpath)+7+strlen(fhash);
            char modify_line[modify_line_size];
            bzero(modify_line,modify_line_size);
            sprintf(modify_line,"M|%s|%s|-1\n",fpath,fhash);
            if(write(fd,modify_line,modify_line_size)<0){
                perror("FAIL TO WRITE IN WRITE_COMMIT_FILE");
                exit(0);
            }
        }
        lhead_ptr= lhead_ptr->next;
    }

    compare_node(&chead,&shead);
    printf("this is after compare\n");
    printf("\nTHIS IS CHEAD AFTER COMPARE\n");
    printList(chead);
    printf("\nTHIS IS CHEAD AFTER COMPARE\n");
    printf("\n");
    printf("\nTHIS IS SHEAD AFTER COMPARE\n");
    printList(shead);
    printf("\nTHIS IS SHEAD AFTER COMPARE\n");
    printf("\n");
    printf("this is after compare\n");
    
    printf("\nNOT HERE \n");
    // stuff client wants to add to server
    node* chead_ptr = chead;
    while(chead_ptr!=NULL){
        indicator++;
        char * fpath  = chead_ptr->path;
        char* fhash = chead_ptr ->hash;
        int Add_line_size  = strlen(fpath)+7+strlen(fhash);
            char Add_line[Add_line_size];
            bzero(Add_line,Add_line_size);
            sprintf(Add_line,"A|%s|%s|-1\n",fpath,fhash);
            if(write(fd,Add_line,Add_line_size)<0){
                perror("FAIL TO WRITE IN WRITE_COMMIT_FILE");
                exit(0);
            }
        chead_ptr = chead_ptr -> next;
    }

    printf("\nNOT HERE2 \n");
    node* shead_ptr = shead;
    while(shead_ptr!=NULL){
        printf("REACH THIS?????");
        indicator++;
        char * fpath  = shead_ptr->path;
        char* fhash = shead_ptr ->hash;
        int Del_line_size  = strlen(fpath)+6+strlen(fhash);
            char Del_line[Del_line_size];
            bzero(Del_line,Del_line_size);
            sprintf(Del_line,"D|%s|%s|-1\n",fpath,fhash);
            int checstrarry = checkstrarray(headstrings,fpath);
            if(checstrarry!=1){
            if(write(fd,Del_line,Del_line_size)<0){
                perror("FAIL TO WRITE IN WRITE_COMMIT_FILE");
                exit(0);
                }
            }
            printf("let this work");
        shead_ptr = shead_ptr -> next;
    }
    printf("\nNOT HERE3\n");
    close(fd);
if(indicator<=0){
    return 1;
    remove(compath);
}

    return 0;
}
int update(char*project_name,int*client_socket){
    // // request manifest
    int command_len_write = writeint(client_socket,strlen("update"));
    sleep(0.3);

    if(write(*client_socket,"update",strlen("update"))<0){
        perror("fail to write \'update\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write project_name to server");
    }
    int man_length = read_conversion(client_socket);
    char server_manifest[man_length+1];
    bzero(server_manifest,man_length+1);
    if(read(*client_socket,server_manifest,man_length)<0){
        perror("error reading server_manifest in update");
    }
    printf("Server MANIFEST contents:\n\n%s\n",server_manifest);
    char* server_manifest_dup = strdup(server_manifest); //dup of server mani
    char* client_mani_path = proj_man_path(project_name);
    char* client_manifest = filetoarray(client_mani_path);
    char* client_manifest_copy = strdup(client_manifest); //copy because SKTOK destroys the string
    printf("contents->client:\n\n%s\n",client_manifest); 
    // compare with temp b4 comparing with server_manifest
    // calculate the live path and each file
    int arrSize=0;
     
          char *array = (char*) malloc(sizeof(char));
          char tempPath[500];
          char ProjectPath[500];

          sprintf(tempPath,"repo/%s/.temp",project_name);
          sprintf(ProjectPath,"repo/%s",project_name);

          //repo/Systems2/
        int fp = open(tempPath, O_CREAT | O_RDWR | O_APPEND, S_IRWXU);
        
          //tempfile
    recurseDirectories(ProjectPath,&array,&arrSize,tempPath);


int req_size_update = strlen(project_name)+13;
char update_path[req_size_update];
bzero(update_path,req_size_update); 
    sprintf(update_path,"repo/%s/.Update",project_name);
    int upfd = open(update_path,O_RDONLY);
    if(upfd>0){
    close(upfd);
    remove(update_path);
    }
// conflict path
int req_size = strlen(project_name)+15;
    char conflict_path[req_size];
    bzero(conflict_path,req_size);  // CONFLICT
    sprintf(conflict_path,"repo/%s/.Conflict",project_name);
    int cfd = open(conflict_path,O_RDONLY);
    if(cfd>0){
    close(cfd);
    remove(conflict_path);
    }
    
// live path
    char temp_path[strlen(project_name)+11];
    sprintf(temp_path,"repo/%s/.temp",project_name);
    // repo/ /.temp
    printf("This is temp path: %s\n",temp_path);
    char* temp = filetoarray(temp_path); //live
    printf("\nTHIS IS THE LIVE HASH\n");
    printf("\n%s\n",temp);
    printf("\nTHIS IS THE LIVE HASH\n");
    close(fp);
    remove(temp_path);    
    //client_manifest,
    node* chead=NULL; // client 
    node* lhead=NULL; // live
    int version_num_chead;
    chead = pash(client_manifest,chead,&version_num_chead);
    printf("\n"); 
    lhead = pash_temp(temp,lhead);
    compare_node(&chead,&lhead);
    //printf("|HASH: %s|pATH:%s",lhead->hash,lhead->path);
    
    
    // else compare with the server manifest
    printf("\nNAJSDNASJKNAJSKNJDKSANJKDNASDKJSANDJKANDASNKDKNSAJKN\n%s\nnNAJSDNASJKNAJSKNJDKSANJKDNASDKJSANDJKANDASNKDKNSAJKN",server_manifest_dup);
    node* shead = NULL; //server LL
    int version_num_shead; // version number on server manifest
    shead = pash(server_manifest_dup,shead,&version_num_shead);
    printf("\nnow we worry about server\n");
    printList(shead);
    printf("\n");
    node* nchead = NULL;
    version_num_chead =-1; // client manifest version prev declaration
    nchead = pash(client_manifest_copy,nchead,&version_num_chead); 
    printList(nchead);
    // CHECK VERSION NUM SERVER=CLIENT?
    if(version_num_chead==version_num_shead){
        printf("NO UPDATES REQURIED ->exiting");
        return 0;
    }
    compare_node(&nchead,&shead);
    printf("\nNC HEAD  and S HEAD\n");
    printList(nchead);
    printList(shead);
    printf("\nNC HEAD  and S HEAD\n");
    // else


    int created =0;
    if(lhead!=NULL&&chead!=NULL&&shead!=NULL){
        int conflict_fd = open(conflict_path,O_CREAT|O_RDWR,00600);
        triple_conflict_write(shead,chead,lhead,&conflict_fd,&created);
        close(conflict_fd);
        if(created<=0){
            remove(conflict_path);
        }
    }
    if(open(conflict_path,O_RDONLY)>0){
        printf(" YOU HAVE NOT RESOLVED YOUR CONFLICT/there is still conflict ;(");
        printf(" BEGONE");
        return 0;
    }

     if(nchead==NULL&&shead==NULL){
        int update_fd2 = open(update_path,O_CREAT|O_RDWR,00600);
        if(write(update_fd2,"EMPTY",strlen("EMPTY"))){
            perror("writng EMPTY IN BLANK UPDATE FILE");
        }
        printf("CREATED A BLANK UPDATE just so you can increment manifest version ;)");
        return 0;
    }
    if(nchead!=NULL||shead!=NULL){
        int update_fd = open(update_path,O_CREAT|O_RDWR,00600);
        // exit(0);
        printf("STATUS OF UPDATEFILE:%d\n",update_fd);
        diff_trav_write_v2(&shead,&nchead,&update_fd);
        close(update_fd);
    }
   
    return 0;

}
int upgrade(char *projectname,int*client_socket ){
//open the .upgrade file first

       char* conpath = proj_con_path(projectname);
        int conflict_fd = open(conpath,O_RDONLY); 

        if(conflict_fd>0){
        
        printf("Fail to upgrade :Conflict files Exist");
            exit(0);
        }

         char* upgradePath = proj_upd_path(projectname);
          int upgrade_fd = open(upgradePath,O_RDONLY); 
            if(upgrade_fd< 0){
             printf("Fail to upgrade :.Update file does not Exist");
            exit(0);
        }else{
            char* array = filetoarray(upgradePath);
            if(strcmp(array,"EMPTY")==0){
                char *VersionNo=malloc(sizeof(char)*10000);
        memset(VersionNo,'\0',strlen(VersionNo));
        char* manarray = filetoarray(proj_man_path(projectname));
        VersionNo=strip(manarray);
        int val = atoi(VersionNo+1);
           updateManVersion(proj_man_path(projectname),projectname,val);
            printf("found EMPTY FILE, Now incrementing manifest version");
            }

        }
        close(upgrade_fd);
//****************************************************************************************
int command_len_write = writeint(client_socket,strlen("upgrade"));
    sleep(0.3);

    if(write(*client_socket,"upgrade",strlen("upgrade"))<0){
        perror("fail to write \'upgrade\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(projectname));
    sleep(0.3);
    if(write(*client_socket,projectname,strlen(projectname))<0){
        perror("fail to write project_name to server");
    }
    int man_length = read_conversion(client_socket);
    char server_manifest[man_length+1];
    bzero(server_manifest,man_length+1);
    if(read(*client_socket,server_manifest,man_length)<0){
        perror("error reading server_manifest in upgrade");
    }
    printf("Server MANIFEST contents:\n\n%s\n",server_manifest);

    char* server_version = strip(server_manifest);
    printf("THIS IS THE SERVER VERSION:%s\n",server_version);
    printf("\n");
    char *server_version_copy = strdup(server_version);


//************************************************************************************
char*manPath=proj_man_path(projectname);
char updatePath[500];
bzero(updatePath,500);
sprintf(updatePath,"repo/%s/.Update",projectname);
//printf("UPDATE PATH %s",updatePath);
int fd=open(updatePath,O_RDONLY);
printf("FD::::%d",fd);
char *updateArray=filetoarray(updatePath);
printf("%s",updateArray);
close(fd);
remove(updatePath);
nodde* uhead=NULL; // client 

uhead=upgradeP(updateArray,uhead); // LL with indicator
//print(chead);

int counterA = 0;
int counterM = 0;
nodde* uhead_ptr = uhead;

while(uhead_ptr!=NULL){
    if(strcmp(uhead_ptr->indicator,"M")==0){
        counterM++;
    }
    if(strcmp(uhead_ptr->indicator,"A")==0){
        counterA++;
    }
    uhead_ptr=uhead_ptr->next;
}
int writeM_status = writeint(client_socket,counterM); // num mods
int writeA_status = writeint(client_socket,counterA); // num adds
printf(" AMOUNT OF ADDS: %d\n",counterA);
printf(" AMOUNT OF MODS: %d\n",counterM);



nodde*ptr=uhead; // ptr  = update nodelist
while(ptr!=NULL){
    if(strcmp(ptr->indicator,"D")==0){
        printf("Target HIT");
        dellMan(ptr->path,manPath,projectname,server_version);
    }
    ptr=ptr->next;
}
ptr =uhead;
while(ptr!=NULL){
    if(strcmp(ptr->indicator,"M")==0){
  //DO modification
        dellMan(ptr->path,manPath,projectname,server_version); // removes the path+hash(ptr) on manifest
        char* path = strdup(ptr->path);
        printf("THIS IS THE PATH FROM PTR\n%s\n",path); 
        int path_size = writeint(client_socket,strlen(path));
        if(write(*client_socket,path,strlen(path))<0){
        perror("fail to write path in upgrade->Modify");
        exit(1);
    }
    int file_size = read_conversion(client_socket);
    char file_M[file_size+1];
    bzero(file_M,file_size+1);
    if(read(*client_socket,file_M,file_size+1)<0){
        perror("FAIL TO READ FILE_SIZE_M");
    }
    printf("\nTHIS THE FILE:\n%s\n\n",file_M);
    int fd = open(path,O_RDWR|O_TRUNC);
    if(fd<0){
        perror("FAILED TO OPEN->MODI PATH");
        exit(1);
    }
    if(write(fd,file_M,strlen(file_M))<0){
        perror("FAIL TO WRITE in file");
        exit(1);
    }
    addM(projectname,path);
    close(fd);
    }
    ptr=ptr->next;
}
ptr =uhead;

    while(ptr!=NULL){
        if(strcmp(ptr->indicator,"A")==0){
   //Add the newadded file that just got downloaded from server
            char* path = strdup(ptr->path);
            printf("THIS IS THE PATH FROM PTR\n%s\n",path); 
            int path_size = writeint(client_socket,strlen(path));
            if(write(*client_socket,path,strlen(path))<0){
                perror("fail to write path in upgrade->Modify");
                exit(1);
        }
        int file_size = read_conversion(client_socket);
        char file_A[file_size+1];
        bzero(file_A,file_size+1);
            if(read(*client_socket,file_A,file_size+1)<0){
                perror("FAIL TO READ FILE_SIZE_A");
            }
            printf("\nTHIS THE FILE:\n%s\n\n",file_A);
            //char* stripped_path = strip2(path);
            char * stripped_directorypath = strip0(path);
            printf("STRIPPED PATH:%s\n",stripped_directorypath);
            mkdir_recursive(stripped_directorypath);
            int add_fd = open(path,O_CREAT|O_RDWR,00600);
            if(add_fd<0){
                perror("FAIL TO OPEN FILE FOR ADD");
            }
            if(write(add_fd,file_A,strlen(file_A))<0){
                perror("FAIL TO WRITE IN THE ADD FILE");
            }
        close(add_fd);
        addM(projectname,path);
        }
    ptr=ptr->next;
    } 
    int vnum = atoi(stripped(server_version_copy));
    //updateManVersion(char*ManPath,char *projectName, int VersionNumber){
    updateManVersion(proj_man_path(projectname),projectname,vnum);
    return 0;
}

int commit(char* project_name,int*client_socket){
    printf("inside commit//////////////////////////////////////////////////////\n");
    int command_len_write = writeint(client_socket,strlen("commit"));
    sleep(0.3);

    if(write(*client_socket,"commit",strlen("commit"))<0){
        perror("fail to write \'commit\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write project_name to server");
    }
    int man_length = read_conversion(client_socket);
    char server_manifest[man_length+1];
    bzero(server_manifest,man_length+1);
    if(read(*client_socket,server_manifest,man_length)<0){
        perror("error reading server_manifest in commit");
    }
    printf("Server MANIFEST contents:\n\n%s\n",server_manifest);

    char* server_manifest_copy = strdup(server_manifest);
    char* server_version = stripped(strip(server_manifest_copy));
    printf("THIS IS THE SERVER VERSION:%s\n",server_version);

    /*
The commit command will fail if the project name doesn’t exist on the server, if the server can not be contacted, if the client can not fetch the server's .Manifest file for the project, if the client has a .Update file that isn't empty (no .Update is fine) or has a .Conflict file. After fetching the server's .Manifest, the client should should first check to make sure that the .Manifest versions match. If they do not match, the client can stop immediatley and ask the user to update its local project first. If the versions match, the client should run through its own .Manifest and compute a live hash for each file listed in it. Every file whose live hash is different than the stord hash saved in the client's local .Manifest should have an entry wri tten out to a .Commit with its file version number incremented. The commit should be successful if the only differences between the server's .Manifest and the client's are:
    */
//////// checks existance of update file
    char* update_path = proj_upd_path(project_name);
    printf("update path %s\n",update_path);
    int up_fd = open(update_path,O_RDONLY);
    if(up_fd>0){ // checking updatefile/ empty update file
        char* updatefile = filetoarray(update_path);
        if(strlen(updatefile)>0){
            printf("AN UPDATE FILE EXIST AND NEEDS TO BE RESOLVED ");
            close(up_fd);
            exit(0);
        }
        close(up_fd);
    }
/// checks existance of conflict file
    char* conflict_path = proj_con_path(project_name);
    printf("conflict path %s\n",conflict_path);
    int con_fd = open(conflict_path,O_RDONLY);
    if(con_fd>0){ // checking updatefile/ empty update file
        char* conflictfile = filetoarray(conflict_path);
        if(strlen(conflictfile)>0){
            printf("AN conflict FILE EXIST AND NEEDS TO BE RESOLVED ");
            close(con_fd);
            exit(0);
        }
        close(con_fd);
    }

// check the server version manifest:
    char* client_manifest_path = proj_man_path(project_name);
    char* client_manifest = filetoarray(client_manifest_path);
    char* client_version = stripped(strip(client_manifest));
    if(strcmp(server_version,client_version)!=0){
        printf("PLEASE UPDATE THE PROJECT -> server and client manifest version don't match\n");
        int writing_yesno = writeint(client_socket,0);
        exit(0);
    }
// compute the live hash and check for the differences between client and live hash
    int arrSize =0;
    char* livehash_array = (char*) malloc(sizeof(char));
    char tempPath[11+strlen(project_name)];
    char ProjectPath[5+strlen(project_name)];
    sprintf(tempPath,"repo/%s/.temp",project_name);
    sprintf(ProjectPath,"repo/%s",project_name);
    int fp =open(tempPath,O_CREAT|O_RDWR|O_APPEND,00700);
    recurseDirectories(ProjectPath,&livehash_array,&arrSize,tempPath); // creates a temp file
    

    char temp_path[strlen(project_name)+11];
    sprintf(temp_path,"repo/%s/.temp",project_name);
    char* live =  filetoarray(temp_path);
    printf("\n%s\n",live);
    close(fp);
    remove(temp_path);
    // strip = get first \n and stripped = remove # from version num
    int version_num_chead =-1;
    //int version_num_shead =-1;
//node* pash_temp( char* manifest, node*chead) node* pash( char* manifest, node*chead,int*version)
    node* chead =NULL;
    chead = pash(client_manifest,chead,&version_num_chead); // strk_r tok
    node* lhead = NULL;
    lhead = pash_temp(live,lhead); // str_r tok
    printf("\nTHIS IS THE CLIENT LINKED LIST\n");
    printList(chead);
    printf("\nTHIS IS THE CLIENT LINKED LIST\n");
    printf("\n");
    printf("\nTHIS IS THE LIVE LINKED LIST\n");
    printList(lhead);
    printf("\nTHIS IS THE LIVE LINKED LIST\n");
    node* shead =NULL;
    int version_num_shead =-1;
    shead = pash(server_manifest,shead,&version_num_shead);
     printf("\nTHIS IS THE server LINKED LIST\n");
    printList(shead);
    printf("\nTHIS IS THE server LINKED LIST\n");
   int status= write_commit_file(shead,chead,lhead,project_name);
   int yesno;
   // 1 means to send commit
    if(status==0){
        int req_size = strlen(project_name)+14;
        char compath[req_size];
        bzero(compath,req_size);
        sprintf(compath,"repo/%s/.Commit",project_name);
        char* commit_file = filetoarray(compath);
        yesno = writeint(client_socket,1);
        sleep(0.2);
        int write_commit_size = strlen(commit_file);
        int send_size = writeint(client_socket,write_commit_size);
        sleep(0.2);
        if(write(*client_socket,commit_file,strlen(commit_file))<0){
            perror("fail to write commit file");

        }
    }
    yesno = writeint(client_socket,0);
    return 0;   

}
int push(char* project_name,int*client_socket){

 char*commitPath=proj_comm_path(project_name);

    int fd=open(commitPath,O_RDONLY);

    if(fd < 0){
     printf(" Cannot push:: Commit File does not exist");
     exit(1);
    }
   //send server the projectname length and name

  int command_len_write = writeint(client_socket,strlen("push"));
    sleep(0.3);

    if(write(*client_socket,"push",strlen("push"))<0){
        perror("fail to write \'push\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);

    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write project_name to server");
    }

    
    

   char*commitFile= filetoarray(commitPath);

   int write_arraySize = writeint(client_socket,strlen(commitFile));
  
   printf("Commit file array is %s\n",commitFile);

     if(write(*client_socket,commitFile,strlen(commitFile))<0){
        perror("fail to write project_name to server");
    }


    int num_mods=read_conversion(client_socket);
    int num_add=read_conversion(client_socket);
     int i=0;

     printf("Num_mods %d",num_mods);
      printf("Num_add %d",num_add);
    while(i<num_mods){

        int path_size=read_conversion(client_socket);
        char path[path_size+1];
        bzero(path,path_size+1);

        if(read(*client_socket,path,path_size)<0){

            perror("Error reading path in push->nummods");
        }

        int filefd=open(path,O_RDONLY);

        if(filefd<0){

            perror("open failed in push");
            exit(1);
        }

        char*modifile=filetoarray(path);

        printf("%s",modifile);


        int write_modsize=writeint(client_socket,strlen(modifile));
        write(*client_socket,modifile,strlen(modifile));
        sleep(0.3);
        close(fd);
        i++;  

    }
    while(i<num_add){

        int path_size=read_conversion(client_socket);
        char path[path_size+1];
        bzero(path,path_size+1);

        if(read(*client_socket,path,path_size)<0){

            perror("Error reading path in push->nummods");
        }

        int filefd=open(path,O_RDONLY);

        if(filefd<0){

            perror("open failed in push");
            exit(1);
        }

        char*addfile=filetoarray(path);

        printf("%s",addfile);
        int write_modsize=writeint(client_socket,strlen(addfile));
        write(*client_socket,addfile,strlen(addfile));
        sleep(0.3);
        close(fd);
        i++;  

    }
    

     
        int x=read_conversion(client_socket);
        printf("X value is %d",x);

        if(x==1){

            printf("Failed to Push\n");
            exit(1);
        }
        printf("Success");
        close(fd);
        remove(commitPath);
    }

int currentversion(char* project_name,int*client_socket){

    //send in the projectname to the server socket
    //send in the servers minfest back to clients socket 
    //and print out the buffer 
    // sent command

    int command_len_write = writeint(client_socket,strlen("current"));
    sleep(0.3);

    if(write(*client_socket,"current",strlen("current"))<0){
        perror("fail to write \'current\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write project_name to server");
    }

    // sent proj_name


    //passing project_name;
    // 1. ye 2
    // 2. no  2
  // exit(0);
    char valid[3];
    bzero(valid,3);
    if((read(*client_socket,valid,2))<0){
        perror("reading valid or not in Checkversion");
    }
    if(strcmp("no",valid)==0){
        return -1;
    }
    int str_length = read_conversion(client_socket);
 char currentVersion[str_length+1];
 bzero(currentVersion,str_length+1);

  if(read(*client_socket,currentVersion,str_length)<0){
            perror("READ FAILED AT FILE_NUM_FILE");
        }
        //printf("\ncontents of file:\n%s\n",currentVersion);

        chkVersion(currentVersion);


        return 0;
}
int destroy(char* project_name,int*client_socket){

    //send in the projectname to the server socket
    //send in the servers minfest back to clients socket 
    //and print out the buffer 
    int command_len_write = writeint(client_socket,strlen("destroy"));
    sleep(0.3);
    if(write(*client_socket,"destroy",strlen("destroy"))<0){
        perror("fail to write \'destroy\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write project_name to server");
    }
    //passing project_name;
    // 1. ye 2
    // 2. no  2
  // exit(0);
    char valid[3];
    bzero(valid,3);
    if((read(*client_socket,valid,2))<0){
        perror("reading valid or not in Checkversion");
    }
    if(strcmp("no",valid)==0){
        return -1;
    }
        return 0; 

}

int rollback(char* project_name,char*projectVersion,int*client_socket){

    int command_len_write = writeint(client_socket,strlen("rollback"));
    sleep(0.3);

    if(write(*client_socket,"rollback",strlen("rollback"))<0){
        perror("fail to write \'rollback\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write projectversion to server");
    }


int projectnumber=atoi(projectVersion);
 int write_versionlen_status = writeint(client_socket,projectnumber);
 sleep(0.3);


int x=read_conversion(client_socket);

if(x==1){
printf("Failed to Rollback::Rollback Version doesn't exist\n");
exit(1);
}
printf("Success\n");

return 0;
}

int history(char*project_name,int *client_socket){

int command_len_write = writeint(client_socket,strlen("history"));
    sleep(0.3);

    if(write(*client_socket,"history",strlen("history"))<0){
        perror("fail to write \'history\'");
    }
    int write_projlen_status = writeint(client_socket,strlen(project_name));
    sleep(0.3);
    if(write(*client_socket,project_name,strlen(project_name))<0){
        perror("fail to write projectversion to server");
    }
    


return 0;
}



int main(int argc, char *argv[]){
int sockfd;
struct sockaddr_in servaddr;
int portNum;
char* ip;
//WTF Configure ipNumber PortNumber need to be written in a file
if(strcmp(argv[1], "configure")==0){

    if(argc < 4){
			printf("Error, Correct command is : ./WTF configure <IP> <port>\n");
			return -1;
		}
		
portNum=atoi(argv[3]);
ip=argv[2];
int fd = open("config.txt", O_CREAT| O_RDWR | O_APPEND|O_TRUNC, 00600);
//writing it the ipAddress and config to file
write(fd, ip, strlen(ip));
write(fd,  " ",strlen(" "));
write(fd,  argv[3],strlen(argv[3]));
write(fd,  "\n",strlen("\n")); 
close(fd);
return 0;       
}
char* configfile = filetoarray("config.txt");
int count=0;
char* token = strtok(configfile, "\n ");
    while( token != NULL ) {
        printf( " %s\n", token );
        if(count!=0){
            portNum = atoi(token);
            break;
        }
        ip = token;
        count++;
        token = strtok(NULL, "\n ");
   }
if(strcmp(argv[1],"add")==0){
    if(argc < 4){
			printf("Error, Correct command ./WTF add </project name> </filename>\n");
			return -1;
		}
    add(argv[2],argv[3]);
    return 0;
}
if(strcmp(argv[1],"remove")==0){
    if(argc < 4){
			printf("Error, Correct command ./WTF remove </project name> </filename>\n");
			return -1;
		}
    rem(argv[2],argv[3]);
    return 0;
}
int connect_status = connectServer(&sockfd,servaddr,ip,portNum);
 DIR* dir;
    dir = opendir("repo");
    if(dir==NULL){
        int direc = mkdir("repo",00700);
    }
char buffer[1024];
bzero(buffer,1024);
if((read(sockfd,buffer,8))<0){
        perror("read");
        exit(1);
    } //command
printf("SERVER asked: %s",buffer);
if(strcmp(argv[1],"create")==0){
    if(argc < 3){
			printf("Error,Please enter  Correct command ./WTF create </project name>\n");
			return -1;
		}
   int creat_status = create(argv[2],&sockfd);
   if(creat_status<0){
        printf("SERVER HAS THE PROJECT ALREADY");
    }else{
        printf("CREATE IS SUCESSFUL");
    }
    return 0;
}
if(strcmp(argv[1],"checkout")==0){

    if(argc < 3){
			printf("Error, Please enter  Correct command ./WTF checkout <project name>\n");
			return -1;
		}
        
    int checkout_status = checkout(argv[2],&sockfd);
    if(checkout_status<0){
        printf("CHECKOUT FAILED");
        
    }else{
        printf("CHECKOUT IS SUCESSFUL");
    }
}
if(strcmp(argv[1],"update")==0){

    if(argc < 3){
			printf("Error,Please enter Correct command ./WTF update <project name>\n");
			return -1;
		}

 int update_status = update(argv[2],&sockfd);
}
if(strcmp(argv[1],"currentversion")==0){

 int current_status = currentversion(argv[2],&sockfd);
}
if(strcmp(argv[1],"destroy")==0){
    if(argc < 3){
			printf("Error,Please enter  Correct command ./WTF destroy </project name>\n");
			return -1;
		}
 int destroy_status = destroy(argv[2],&sockfd);
} 
if(strcmp(argv[1],"upgrade")==0){
    if(argc < 3){
			printf("Error, Please enter Correct command ./WTF upgrade </project name>\n");
			return -1;
		}
upgrade(argv[2],&sockfd);
}
if(strcmp(argv[1],"commit")==0){
    if(argc < 3){
			printf("Error, Please enter Correct command ./WTF commit </project name>\n");
			return -1;
		}

int commit_status = commit(argv[2],&sockfd);
}
if(strcmp(argv[1],"push")==0){
    if(argc < 3){
			printf("Error, Please enter Correct command ./WTF push </project name>\n");
			return -1;
		}

int push_status = push(argv[2],&sockfd);
}if(strcmp(argv[1],"rollback")==0){
    if(argc < 4){
			printf("Error, Please enter Correct command ./WTF rollback </project name>\n");
			return -1;
		}

int push_status = rollback(argv[2],argv[3],&sockfd);
}if(strcmp(argv[1],"history")==0){
    if(argc < 3){
			printf("Error, Please enter Correct command ./WTF history </project name>\n");
			return -1;
		}
int push_status = history(argv[2],&sockfd);
}
//int status = transfer(&sockfd,"sample.c");
close(sockfd);
 return 0;
} 
