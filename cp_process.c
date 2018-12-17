#include <fcntl.h>
#include <pwd.h>//解析用户
#include <grp.h>//解析用户组
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#ifndef MAXBUFF
#define MAXBUFF 255
#endif

char workDir[255+1];
char sourceDir[255+1];
char destDir[255+1];

void copyFile(char * source , char * destinationDir , char * destName){
  struct stat statbuf;
  lstat(source,&statbuf);
  char * readBuff[MAXBUFF];
  DIR * dp = opendir(destinationDir);
  if(dp == NULL) {
    if (mkdir(destinationDir,0777) ==-1 ){
      //puts("permission denied");
      return ;
    }
  }
  char destination[255+1];
  strcpy(destination,destinationDir);
  strcat(destination,destName);
  mode_t f_attrib;
  f_attrib = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
  int sourceFile = open(source,O_RDONLY);
  int destFile = open(destination, O_WRONLY|O_CREAT,0664);
  //if (destFile == -1) puts("failed to create,check your permission");
  ssize_t readSize=0;
  while(1){//读到尾部为止
      readSize = read(sourceFile,(void*)readBuff,readSize+MAXBUFF);
      if (readSize == 0 || readSize == -1) break;
      // //printf("%s\n",readBuff);
      // //printf("%zd\n", readSize);
      write(destFile,readBuff,readSize);
   }
  close(sourceFile);
  close(destFile);
}

void scan_dir(char * dir){
  DIR * dp;
  struct dirent * entry;
  struct stat statbuf;
  if ((dp=opendir(dir))==NULL){
    puts("can not open dir");
    return ;
  }
  chdir(dir);
  while((entry = readdir(dp)) != NULL){
    lstat(entry->d_name,&statbuf);
    if (S_IFDIR & statbuf.st_mode){
      if (strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0) continue;
 			strcpy(workDir,entry->d_name);
     	scan_dir(entry->d_name);
    }
    getcwd(workDir,255);
    char source[255]="";
    sprintf(workDir,"%s/%s",workDir,entry->d_name);
    strcpy(source,workDir);
    //puts(source);
    char test[255]="";
    int count = strlen(workDir)-strlen(sourceDir);
    //printf("%d\n",count );
    strncpy(test,&workDir[strlen(sourceDir)],count-strlen(entry->d_name));
    sprintf(workDir,"%s%s",destDir,test);
    //puts(workDir);

    pid_t pid = 0;

    pid_t child_id = 0;

    if ((pid=fork())>0){
      while(1){
        child_id = waitpid(pid,NULL,0);
        if (child_id == pid) printf("process %d which copys file: %s has just exit\n",child_id,workDir);
        break;
      }
    }
    else if (pid==0) {
      copyFile(source,workDir,entry->d_name);
      exit(0);
    }
    else {
      printf("fork()error\n");
      exit(0);
    }



	}
  chdir("..");
  closedir(dp);
}

int main(int argc,char * argv[]){

	char cwd[255+1];
	getcwd(cwd,255);
	// puts(argv[1]);
  chdir(argv[1]);
  getcwd(sourceDir,255);
  chdir(cwd);
	DIR * dp=opendir(argv[2]) ;
  if (dp == NULL){//没有此文件夹
    //puts("no dir");
    if (mkdir(argv[2],0777) == -1) return 1;
    else puts("dir created");
  }
  chdir(argv[2]);
	getcwd(destDir,255);
  chdir(sourceDir);
  scan_dir(argv[1]);
  //copyFile("/Users/MT/git/Unix Linux/linux/ls.c","/Users/MT/git/Unix Linux/copy/","1.c");
  //printf("%lu\n",strlen("/Users/MT/git/Unix Linux/linux/信软学院实验报告撰写规范及模板（试行）/电子科技大学信软学院标准实验报告模板（试行）.docx") );
}
