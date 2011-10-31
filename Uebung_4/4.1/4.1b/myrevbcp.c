#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


int main(int argc, char *argv[]){
  const char* quelle = argv[1];
  const char* ziel = argv[2];
  
  if(argc != 3){
    printf("Verwendun: ./myrevbcp quelle ziel\n ");
    return 1;
  }
  
  struct stat buff;
  stat(quelle,&buff);
  long FILESIZE = buff.st_size;

  int fhq;
  fhq = open(quelle, O_RDONLY);

  if(fhq == -1){
    perror("open");
    return (1);
  }

  int fhz;
  fhz = creat(ziel, S_IRUSR | S_IWUSR);

  if(fhz == -1){
    perror("creat");
    return (1);
  }

  int read_bytes = 0;
  int write_bytes = 0;
  char *buf[BUFSIZ];

  long counter = FILESIZE-1;
  int tmp;

  // lseek(fhq, 0, SEEK_END);
  
  lseek(fhq, counter, SEEK_SET);
  while(counter >= 0){
    tmp = read(fhq, buf, 1);
    if(tmp == -1){
      perror("read");
    }
    read_bytes += 1;
    
    int tmp2 = write(fhz, buf, 1);
    if(tmp2 == -1){
      perror("write");
    }
    write_bytes += 1;
    counter --;
    lseek(fhq, counter, SEEK_SET );
    
  }
  
  printf("%d Bytes insgesamt gelesen. \n", read_bytes);
  printf("%d Bytes insgesamt geschrieben \n", write_bytes);

  close(fhq);
  close(fhz);
  return (0);
}
