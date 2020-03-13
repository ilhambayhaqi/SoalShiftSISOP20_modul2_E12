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

int main(int argc, char const *argv[])
{
  	int signal;
  	pid_t child_id;
  	child_id = fork();

  	const char* myWorkdir = "/home/almond/modul2/";

  	if (child_id < 0) exit(EXIT_FAILURE);

  	if (child_id == 0) {
  		if(fork() == 0){
  			char *argv[] = {"mkdir", "/home/almond/modul2/indomie", NULL};
			execv("/bin/mkdir", argv);
  		}
  		else{
  			sleep(5);
  			char *argv[] = {"mkdir", "/home/almond/modul2/sedaap", NULL};
			execv("/bin/mkdir", argv);
  		}
  	}
  	else {
	    wait(&signal);
	    int boom;
	    if(fork() == 0){
	    	char *argv[] = {"unzip","-q","-o","/home/almond/modul2/jpg","-d","/home/almond/modul2/", NULL};
			execv("/usr/bin/unzip", argv);
	    }
	    else{
	    	wait(&boom);
	    	char currDir[100];
	    	char fullPath[500];
	    	strcat(currDir, myWorkdir);
	    	strcat(currDir,"jpg/");

	    	DIR* dir = opendir(currDir);
	    	DIR* pathDir;
	    	FILE* path;
	    	struct dirent* ls;
  			while ((ls = readdir (dir))) {
  				if(strcmp(ls->d_name, ".") !=0 && strcmp(ls->d_name,"..") != 0){
  					sprintf(fullPath, "%s%s", currDir, ls->d_name);

	    			pathDir = opendir(fullPath);
					path = fopen(fullPath,"r");

					if(pathDir){ // untuk Direktori
						if(fork() == 0){
							int boom2;
							if(fork() == 0){
								char *argv[] = {"mv", fullPath ,"/home/almond/modul2/indomie", NULL};
								execv("/bin/mv", argv);
							}
							else{
								wait(&boom2);
								int boom3;
								if(fork() == 0){
									sprintf(fullPath,"%sindomie/%s/coba1.txt", myWorkdir, ls->d_name);
									char *argv[] = {"touch", fullPath ,"/home/almond/modul2/indomie", NULL};
									execv("/bin/touch", argv);
								}
								else{
									wait(&boom3);
									sleep(3);
									sprintf(fullPath,"%sindomie/%s/coba2.txt", myWorkdir, ls->d_name);
									char *argv[] = {"touch", fullPath ,"/home/almond/modul2/indomie", NULL};
									execv("/bin/touch", argv);
								}
							}
						}
					}
					else if(path){ //file
						if(fork() == 0){
							char *argv[] = {"mv", fullPath ,"/home/almond/modul2/sedaap", NULL};
							execv("/bin/mv", argv);
						}
					}

					closedir(pathDir);
					fclose(path);
  				}
  			}		
	    	closedir(dir);
	    }
  	}
	return 0;
}

