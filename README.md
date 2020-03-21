# SoalShiftSISOP20_modul2_E12

## Soal 1
Pada soal 1, diminta untuk membuat program menyerupai crontab dengan 4 argumen yaitu detik, menit, jam dan path file .sh. Program dapat mengeluarkan pesan error jika argumen yang diberikan tidak sesuai dan program hanya menerima 1 config cron serta merupakan daemon.  
Pada awalnya dilakukan pengecekan terhadap argumen yang diberikan. Karena ./(nama_program) pada c dihitung sebagai argumen maka, dicek argumen yang diberikan harus sebanyak 5 argumen, jika tidak maka argumen bernilai tidak valid dan memanggil argErr()
```
if(argc != 5) argErr();
```
Untuk fungsi arrErr() sebagai berikut.
```
void argErr(){
	printf("Argument Error\n");
	exit(EXIT_FAILURE);
}
```
Selanjutnya dilakukan pengecekan validitas detik, menit, dan jam. Pengecekan tersebut dilakukan dengan melakukan parse pada tiap argumen dan mengecek nilai rentang dari tiap argumen yang telah di parse, jika valid maka pada array of boolean waktu tersebut diset sebagai true, dimana untuk setiap waktu yang diset sebagai true nantinya merupakan waktu yang akan digunakan sebagai eksekusi.
```
if(!toParse(argv[1], sec, 60)) argErr();
if(!toParse(argv[2], min, 60)) argErr();
if(!toParse(argv[3], hour, 24)) argErr();
```
fungsi toParse akan mereturn true bila argumen yang diberikan valid dan false bila argumen yang diberikan tidak valid.
```
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
		else if(temp == '*'){
			if(isA){
				a = 0;
				b = maximum-1;
				isValid(a,b, maximum, t);
			}
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
```
Pada toParse, terdapat pemanggilan fungsi isValid, pada fungsi tersebut dilakukan pengecekan nilai setelah argumen dilakukan parsing sekaligus digunakan untuk set pada array waktu yang ada.
```
bool isValid(int a, int b, int maximum, bool t[]){
	if (a > b) return false;
	if (a >= maximum || (b >= maximum && b != INT_MAX)) return false;
	
	if(b == INT_MAX) t[a] = true;
	else 
		for(a; a <= b; a++) t[a] = true;

	return true;
}
```
Setelah tiap argumen waktu dicek, maka hanya tinggal argumen path file yang perlu dicek. Untuk itu dilakukan pengecekan apakah file yang akan dieksekusi terdapat pada path yang diberikan atau tidak. Karena pointer DIR akan bernilai true bila hanya merupakan direktori dan pointer FILE akan true untuk direktori dan file, maka dilakukan pengecekan sebagai berikut.
```
DIR* dir = opendir(argv[4]);
FILE* path = fopen(argv[4],"r");
if(!path || dir) fileErr();
```
Karena program berjalan di background, maka dilakukan dengan cara sebagai berikut.
```
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

```
Selanjutnya untuk menentukan waktu eksekusi program selanjutnya yaitu dengan dibuat fungsi findNextTime. Pada fungsi ini dilakukan pencarian waktu selanjutnya secara linear dengan parameter yaitu waktu saat ini serta daftar waktu yang akan dieksekusi. Pada fungsi ini terdapat bool isUp, dimana boolean ini menandakan bahwa waktu selanjutnya akan kembali melewati 0 sehingga untuk waktu diatasnya dapat berubah bila, maksudnya misal waktu saat ini menunjukkan 19:57:00, dan parameter pada jam yaitu ```19-20```, dan menit ```5,52```. Pada kasus tersebut maka menit eksekusi selanjutnya yaitu pada menit 5 dan ini menyebabkan jam eksekusi selanjutnya bukan jam 19 melainkan jam 20. Fungsi findNextTime sebagai berikut.
```
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
```
Pada fungsi diatas, akan dilakukan pemanggilan nextTimeUtil yang merupakan fungsi utilitas untuk mencari waktu selanjutnya dari tiap jenis waktu.
```
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
```
Setelah dilakukan pencarian waktu, maka dicek berapa lama hingga waktu eksekusi berlangsung. Maka dari itu dibuat fungsi diff dengan parameter waktu saat ini dan waktu eksekusi selanjutnya.
```
int diff(struct tm currTime, struct tm nextTime){
	int second1 = nextTime.tm_hour*60*60 + nextTime.tm_min*60 + nextTime.tm_sec;
	int second2 = currTime.tm_hour*60*60 + currTime.tm_min*60 + currTime.tm_sec;

	return (second1-second2 + 24*3600)%(24*3600);
}
```
Pada diff tersebut bila waktu selanjutnya dalam 0 detik maka akan dilakakukan eksekusi terhadap file .sh, sedangkan bila tidak akan sleep dengan 2 opsi yaitu sleep tiap 1 detik atau minimal 5 detik hingga waktu eksekusi selanjutnya. Pada opsi pertama, akurasi program lebih tinggi namun lebih sering berjalan pada background process, sedangkan pada opsi 2 tingkat akurasi dapat berkurang bila terjadi perubahan waktu dalam localtime (dapat terjadi delay 5 detik karena program dalam keadaan sleep).
```
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
```
Pada pengerjaan soal 1 ini terjadi kendala yaitu lupa dengan parameter * sehingga dilakukan revisi serta pada daemon,  pesan argumen berjalan dalam background sehingga tidak nampak oleh pengguna bila terjadi error.

## Soal 2


## Soal 3
Kendala  
Untuk soal 2, belum selesai zipnya dan killer masih bug.  
Belum cukup waktunya untuk menyelesaikan
