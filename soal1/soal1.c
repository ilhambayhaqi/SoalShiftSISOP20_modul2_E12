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

typedef long long int LL;

int comMIN(int a, int b){ 
	if(a == 0 || b == 0 ) return 1;
	if(a < b) return a; 
	else return b; 
}

bool isValid(int a, int b, int maximum, bool t[]){
	if (a > b) return false;
	if (a >= maximum || (b >= maximum && b != INT_MAX)) return false;
	
	if(b == INT_MAX) t[a] = true;
	else 
		for(a; a <= b; a++) t[a] = true;

	return true;
}

bool toParse(const char* argv, bool t[], int maximum){
	int a = 0, b = INT_MAX, i, j = 0;
	bool isA = true;
	for (i = 0; *(argv+i) != '\0'; ++i){
		char temp = *(argv + i);
		if(temp == ','){
			if(*(argv+i+1) == '\0') return false;
			if(!isValid(a,b,maximum,t)) return false;
			j = 0;
			a = 0;
			b = INT_MAX;
			isA = true;
		}
		else if(temp == '-'){
			if(*(argv + i + 1) == '\0') return false;
			if(isA) isA = false; 
			else return false;
		}
		else if(isdigit(temp) != 0){
			if(isA) a = a * 10 + (int)(temp - '0');
			else{
				if (b == INT_MAX) b = 0;
				b = b * 10 + (int)(temp - '0');
			} 
			j++;
		}
		else return false;
	}
	if(!isValid(a,b,maximum,t)) return false;
	return true;
}

int nextTimeUtil(bool t[], bool* isUp,int tnow, int maximum){
	*isUp = false;
	tnow = tnow % maximum;
	while(!t[tnow]){
		tnow = tnow + 1;
		if(tnow == maximum) *isUp = true;
		tnow = tnow % maximum;
	}
	return tnow;
}

int diff(struct tm currTime, struct tm nextTime){
	int second1 = nextTime.tm_hour*60*60 + nextTime.tm_min*60 + nextTime.tm_sec;
	int second2 = currTime.tm_hour*60*60 + currTime.tm_min*60 + currTime.tm_sec;

	return (second1-second2 + 24*3600)%(24*3600);
}

struct tm findNextTime(struct tm currTime, bool hour[], bool min[], bool sec[]){
	bool isUp;
	int nextHour, nextMin, nextSec;
	
	
	nextHour = nextTimeUtil(hour, &isUp, currTime.tm_hour, 24);
	if(nextHour == currTime.tm_hour){
		nextMin = nextTimeUtil(min, &isUp, currTime.tm_min, 60);
		if(isUp){
			nextHour = nextTimeUtil(hour, &isUp, currTime.tm_hour + 1, 24);
			nextSec = nextTimeUtil(sec, &isUp, 0, 60);
		}
		else{
			if(nextMin == currTime.tm_min){
				nextSec = nextTimeUtil(sec, &isUp, currTime.tm_sec, 60);
				if(isUp){
					nextMin = nextTimeUtil(min, &isUp, currTime.tm_min + 1, 60);					
					if(isUp) nextHour = nextTimeUtil(hour, &isUp, currTime.tm_hour + 1, 24);
				}
			}
			else{
				nextSec = nextTimeUtil(sec, &isUp, 0, 60);
				if(isUp){
					nextMin = nextTimeUtil(min, &isUp, currTime.tm_min + 1, 60);					
					if(isUp) nextHour = nextTimeUtil(hour, &isUp, currTime.tm_hour + 1, 24);
				}
			}
		}

	}
	else{
		nextMin = nextTimeUtil(min, &isUp, 0, 60);
		nextSec = nextTimeUtil(sec, &isUp, 0, 60);
	}

	struct tm nextTime;
	nextTime.tm_hour = nextHour;
	nextTime.tm_min = nextMin;
	nextTime.tm_sec = nextSec;

	return nextTime;
}
	

void argErr(){
	printf("Argument Error\n");
	exit(EXIT_FAILURE);
}

void fileErr(){
	printf("File not exist\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
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

  	bool sec[60] = {false};
  	bool min[60] = {false};
  	bool hour[24] = {false};

  	if(argc != 5) argErr();
  	if(!toParse(argv[1], sec, 60)) argErr();
  	if(!toParse(argv[2], min, 60)) argErr();
  	if(!toParse(argv[3], hour, 24)) argErr();

  	DIR* dir = opendir(argv[4]);
  	FILE* path = fopen(argv[4],"r");
  	if(!path || dir) fileErr();



  	while(true){
	  	time_t t;
	  	t = time(NULL);

	  	struct tm currTime = *localtime(&t);
	  	struct tm nextTime = findNextTime(currTime, hour, min, sec);

	  	int nextIn = diff(currTime, nextTime);

	  	if(nextIn == 0){
	  		if(fork() == 0){
	  			char curpath[100];
  				sprintf(curpath,"%s", argv[4]);
	  			char *hehe[] = {"bash", curpath, NULL};
				execv("/bin/bash", hehe);
		  	}
	  	}
	  	sleep(comMIN(nextIn, 5));
	  	// sleep(1);  //bila ingin tanpa delay
	}

	return 0;
}

