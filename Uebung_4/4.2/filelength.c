#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main (int argc, char *argv[]){
  const char* quelle = argv[1];

  if (argc!=2)
  {
    perror("usage");
    printf("Verwendung: ./filelength dateiname\n");
    return 1;
  }
   /*Dateigroesse der Quelldatei ermitteln*/
  struct stat buff;
  stat(quelle,&buff);
  long FILESIZE = buff.st_size;

  printf("Die Datei %s ist %ld Byte gross\n", quelle ,FILESIZE);

}

