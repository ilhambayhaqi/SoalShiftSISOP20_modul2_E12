#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<syslog.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<ctype.h>
#include<limits.h>
#include<dirent.h>
#include<math.h>
#include<wait.h>
typedef long long int LL;

void argErr(){
	exit(EXIT_FAILURE);
}

void download(char path[]){
	time_t t = time(NULL);
	int width = (int)(t % 1000) + 100;
	
	int signal;
	struct tm currTime = *localtime(&t);
	if(fork() == 0){
		int signal;

		char link[100], fileName[100], currFileName[100];
		sprintf(link,"https://picsum.photos/%d", width);
		sprintf(path,"%s/", path);
      	sprintf(fileName,"%s%04d-%02d-%02d_%02d:%02d:%02d", path ,currTime.tm_year + 1900,
       		currTime.tm_mon + 1, currTime.tm_mday, currTime.tm_hour, currTime.tm_min, currTime.tm_sec);
      	sprintf(currFileName, "%s%d", path,width);

		if(fork() == 0){
			char *hehe[] = {"wget","-P", path, link, NULL};
			execv("/usr/bin/wget", hehe);
		}
		wait(&signal);

		char *hehe[] = {"mv", currFileName, fileName, NULL};
		execv("/bin/mv", hehe);
	}
	wait(&signal);
}

/*
void makeZip(char path[]){
	char dir[100];
	sprintf(dir, "./%s", path);
	
	if(fork() == 0){
		chdir(dir);
		char zipName[100], targetZip[100];
		sprintf(zipName, "../%s.zip", path);
		sprintf(targetZip, "./*");

		char *hehe[] = {"zip",zipName, targetZip, NULL};
		execv("/usr/bin/zip", hehe);
	}
}
*/

void getKiller(const char hehe[]){
	FILE *killer = fopen("killer.txt", "w");
	fprintf(killer, "hehe\n");
	/*
	if(!strcmp(argv, "-a")){
		fprintf(file, "#!/bin/bash\nkill -9 -%d", getpid());

		if(fork() == 0){
			char *hehe[] = {"chmod","-x","killer", NULL};
			execv("/bin/chmod", hehe);
		}
	}
	else if(!strcmp(argv, "-b")){
		fprintf(file, "#!/bin/bash\nkill -%d", getpid());

		if(fork() == 0){
			char *hehe[] = {"chmod","-x","killer", NULL};
			execv("/bin/chmod", hehe);
		}
	}
	else argErr();
	*/
	fclose(killer);
}

int main(int argc, char const *argv[])
{
    /*
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    else if (pid > 0) exit(EXIT_SUCCESS);
  
    umask(0);
  
    sid = setsid();
    if (sid < 0) exit(EXIT_FAILURE);
  
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO); 
    */

    if(argc != 2) argErr();

   	getKiller(argv[1]);

   	FILE *killer = fopen("killer.sh", "w");
	
	if(strcmp(argv[1], "-a") == 0){

		fprintf(killer, "#!/bin/bash\n");
		fprintf(killer, "kill -9 %d\n", getpid());

		if(fork() == 0){
			char *hehe[] = {"chmod","-x","killer.sh", NULL};
			execv("/bin/chmod", hehe);
		}
	}
	else if(strcmp(argv[1], "-b") == 0){
		fprintf(killer, "#!/bin/bash\n");
		fprintf(killer, "kill %d\n", getpid());

		if(fork() == 0){
			char *hehe[] = {"chmod","-x","killer.sh", NULL};
			execv("/bin/chmod", hehe);
		}
	}
	else argErr();
	
	fclose(killer);

    while(true){
      	time_t t = time(NULL);
	    struct tm currTime = *localtime(&t);

      	char folderName[100];
      	sprintf(folderName,"%04d-%02d-%02d_%02d:%02d:%02d", currTime.tm_year + 1900,
       		currTime.tm_mon + 1, currTime.tm_mday, currTime.tm_hour, currTime.tm_min, currTime.tm_sec);

	    if(fork() == 0){
        	int signal;
        	if(fork() == 0){
        		char *argv[] = {"mkdir", folderName, NULL};
          		execv("/bin/mkdir", argv);
          	}
          	else{
          		wait(&signal);
          		int i;
          		for (i = 0; i < 20; ++i)
          		{
          			download(folderName);
          			sleep(5);
          		}
          		//makeZip(folderName);
          	}
	    }
      	
      	sleep(30);
    }
    return 0;
}

