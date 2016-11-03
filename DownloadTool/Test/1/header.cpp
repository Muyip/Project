#include "header.h"

int h1 = 1;
int h2 = 2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int Add(int x, int y)
{
	return x+y;
}
