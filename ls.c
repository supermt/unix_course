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

void print_type(struct dirent * current_dirent){
  if (current_dirent->d_type==4)  printf("d");
  if (current_dirent->d_type==8)  printf("-");
}
void print_permission(struct stat statbuf){
  //Root用户权限
  if (statbuf.st_mode & S_IRUSR) printf("r");
  else printf("-");
  if (statbuf.st_mode & S_IWUSR) printf("w");
  else printf("-");
  if (statbuf.st_mode & S_IXUSR) printf("x");
  else printf("-");
  //群组权限
  if (statbuf.st_mode & S_IROTH) printf("r");
  else printf("-");
  if (statbuf.st_mode & S_IWOTH) printf("w");
  else printf("-");
  if (statbuf.st_mode & S_IXOTH) printf("x");
  else printf("-");
  //一般用户权限
  if (statbuf.st_mode & S_IRGRP) printf("r");
  else printf("-");
  if (statbuf.st_mode & S_IWGRP) printf("w");
  else printf("-");
  if (statbuf.st_mode & S_IXGRP) printf("x");
  else printf("-");

  printf("\t");
}
void print_link_count(struct stat statbuf){
    printf("%d\t",statbuf.st_nlink);
}
void print_user(struct stat statbuf){
  int uid = statbuf.st_uid;//获取用户uid
  struct passwd * current_user = getpwuid (uid);
  printf("%s\t",current_user->pw_name );
}
void print_group(struct stat statbuf){
  int gid = statbuf.st_gid;
  struct group *data =  getgrgid(gid);
  printf("%s\t",data->gr_name);
}
void print_size(struct stat statbuf){
  printf("%lld\t", statbuf.st_size);
}
void print_modify_date(struct stat statbuf){
  struct tm * timeData = localtime(&statbuf.st_mtime);
  printf("%d月 %d日 %d:%d %d\t",timeData->tm_mon+1,timeData->tm_mday,timeData->tm_hour,timeData->tm_min,timeData->tm_sec);
}

void scan_dir(char * dir){
	DIR * dp;
	struct dirent * entry;//目录文件，存储了一张表，该表就是该目录文件下，所有文件名和inode的映射关系
  /*
  1.-，普通文件。
  2.d，目录文件，d是directory的简写。
  3.l，软连接文件，亦称符号链接文件，s是soft或者symbolic的简写。
  4.b，块文件，是设备文件的一种（还有另一种），b是block的简写。
  5.c，字符文件，也是设备文件的一种（这就是第二种），c是character的文件。
  */
  /**
  struct dirent
  {
  　long d_ino; inode number 索引节点号
      off_t d_off; offset to this dirent 在目录文件中的偏移
      unsigned short d_reclen;  length of this d_name 文件名长
      unsigned char d_type;  the type of d_name 文件类型
      char d_name [NAME_MAX+1]; file name (null-terminated) 文件名，最长255字符
  }
  */
  struct stat statbuf;
	if ((dp = opendir(dir)) == NULL ) {
		puts("Can not Open Dir");
		return ;
	}
	chdir(dir);//切换工作目录
  //下面的代码是用来从目录文件维护的表中循环读取每个文件的
	while(1){
	entry = readdir(dp);//根据目录文件读取内容
		if (entry != NULL){
			if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
					continue;
      char cwd[255+1];
      getcwd(cwd,255);
      strcat(cwd,"/");
      strcat(cwd,entry->d_name);
      stat(entry->d_name,&statbuf);
      //获取目录文件之后，找到源文件后获取各类信息
      print_type(entry);//文件类型
      print_permission(statbuf);//文件权限
      print_link_count(statbuf);//文件节点
      print_user(statbuf);
      print_group(statbuf);
      print_size(statbuf);
      print_modify_date(statbuf);
      puts(entry->d_name);
		}
		else return ;
	}
}

int main(){
	char cwd[255+1];
	getcwd(cwd,255);
	puts(cwd);
	scan_dir(cwd);
}
