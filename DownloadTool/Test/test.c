#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void *ReadSameFile1(void *arg)
{
	int fd = open("./file", O_RDONLY);
	assert(fd);

	creat("./w1.txt", S_IWUSR | S_IRUSR);
	int fd_w = open("./w1.txt", O_WRONLY);
	assert(fd_w);

	char buf[1024];
	bzero(buf, sizeof(buf));

	int rsz = 0;
	while((rsz = read(fd, buf, sizeof(buf))) > 0)
	{
		//if(recv(sockfd, &c, 1, 0)<0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
		write(fd_w, buf, rsz);
	}
	close(fd);
	close(fd_w);

	return NULL;
}

void *ReadSameFile2(void *arg)
{
	int fd = open("./file", O_RDONLY);
	assert(fd);

	creat("./w2.txt", S_IWUSR | S_IRUSR);
	int fd_w = open("./w2.txt", O_WRONLY);
	assert(fd_w);

	char buf[1024];
	bzero(buf, sizeof(buf));

	int rsz = 0;
	while((rsz = read(fd, buf, sizeof(buf))) > 0)
	{
		write(fd_w, buf, rsz);
	}

	close(fd);
	close(fd_w);

	return NULL;
}

<<<<<<< HEAD
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int main()
{
	DIR *dir;
	struct dirent *ptr;
	string s;
	dir = opendir(".");
	while((ptr = readdir(dir)) != NULL)
	{
		cout << ptr->d_name << endl;
	}
=======
int main()
{
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, ReadSameFile1, NULL);
	pthread_create(&tid2, NULL, ReadSameFile2, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
>>>>>>> c50521db89f14623e431e5f864a090fa9f155334
	return 0;
}
