#include "../include/rwlock.h"

int main(){
	leser_lock();
	leser_unlock();
	writer_lock();
	writer_unlock();
	leser_lock();
	writer_lock();
	leser_unlock();
	writer_lock();
	writer_unlock();
	
	return 0;
}