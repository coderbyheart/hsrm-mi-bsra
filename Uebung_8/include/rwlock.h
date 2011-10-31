#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#define SEM_READ 0

#define SEM_DB 1

#define SEM_NBR 2

void leser_lock();
void leser_unlock();
void writer_lock();
void writer_unlock();
void sem_init();

#endif