#include "../4.3cd/include/mytime.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  const char* quelle; 
  const char* ziel;
  int buffsize;
  struct tstamp t;

  /*Quelle und Ziel aus Kommandozeilenparameter uebernehmen, 
    ggf. Verwendung anzeigen*/
  if(argc ==  3){
    buffsize = 1;
    quelle = argv[1];
    ziel = argv[2];
  }else if (argc < 3)
  {
    printf("Verwendung: ./mycp quelle ziel [buffersize]\n");
    return 1;
  }else{
    quelle = argv[1];
    ziel = argv[2];
    buffsize = atoi(argv[3]);
  }

  /*Timer starten*/
  start(&t);
  
  /*Dateigroesse der Quelldatei ermitteln*/
  struct stat buff;
  stat(quelle,&buff);
  long FILESIZE = buff.st_size;

  /*Falls keine regulaere Datei, Ende und Fehlermeldung*/
  if (!(buff.st_mode & S_IFREG)){
    printf("Die angegebene Quelldatei %s ist keine regulaere Datei!\n", quelle);
    return(1);
  }
  
  int fhq;
  fhq = open (quelle,O_RDONLY);

  /*Falls Fehler beim oeffnen der Quelldatei, Fehler melden, ansonsten weiter*/
  if (fhq==-1)
  {
    perror("open");
    return (1);
  }
  
  int fhz;
  fhz = creat(ziel, buff.st_mode);
  
  if (fhz==-1)
  {
    perror("creat");
    return (1);
  }
  int geles_bytes=0;
  int geschr_bytes=0;
  char * buf[BUFSIZ];
  int tmp;
  int rest;
  int durchlauf;

  if(buffsize > FILESIZE){
    buffsize = FILESIZE;
    printf("Puffergroesse auf Dateigroesse reduziert.\n");
    durchlauf = 1;
    rest = 0;
  }else{
    rest = FILESIZE % buffsize;
    durchlauf = FILESIZE / buffsize;
  }
  while (durchlauf > 0){
    tmp = read(fhq, buf, buffsize);
    if (tmp==-1)
      {
	perror("read");
	return(1);
      }
    geles_bytes += buffsize;
    int tmp2 = write(fhz, buf,buffsize);
    if (tmp2==-1){
      perror("write");
      return(1);
    }
    geschr_bytes +=buffsize;
    durchlauf--;
  }

  if(rest != 0){
      tmp = read(fhq, buf, rest);
      if(tmp == -1){
	perror("read");
	return 1;
      }
      geles_bytes += rest;

      int tmp2 = write(fhz, buf, rest);
      if(tmp == -1){
	perror("write");
	return 1;
      }
      geschr_bytes += rest;
  }
  
  stop(&t);


  printf("%d Bytes insgesamt gelesen...\n",geles_bytes);
  printf("%d Bytes insgesamt geschrieben...\n",geschr_bytes);


  close(fhq);
  close(fhz);
  chmod(ziel, buff.st_mode);
  printf("%lums dafür benötigt\n", extime(&t));
  return(0);


}
