#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>



int main (int argc, char *argv[]){
  const char* quelle = argv[1];
  const char* ziel = argv[2];
  
   if (argc!=3){
    printf("Verwendung: ./mybappend quelle ziel\n");
    return 1;
  }
  
  struct stat buff;
  stat(quelle,&buff);
  long FILESIZE = buff.st_size;

  int fhq;
  fhq = open (quelle,O_RDONLY);
  
  if(fhq == -1){
    perror("open");
    return(1);
  } 
  
  int fhz;
  fhz = open(ziel, O_WRONLY | O_APPEND);
  
  if (fhz == -1) {
    perror("creat");
    return (1);
  }
  int geles_bytes=0;
  int geschr_bytes=0;
  char * buf[BUFSIZ];
  int tmp;
  while (tmp = read (fhq,buf,1)>0)
  {
    if (tmp==-1)
      {
	perror("read");
	return(1);
      }
    geles_bytes +=1;
    int tmp2 = write(fhz, buf,1);
    if (tmp2==-1){
      perror("write");
    }
    geschr_bytes +=1;
  }
  printf("%d Bytes insgesamt gelesen...\n",geles_bytes);
  printf("%d Bytes insgesamt geschrieben...\n",geschr_bytes);

  close(fhq);
  close(fhz);
  return(0);
  
}
