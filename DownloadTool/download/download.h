#pragma once

//#ifndef __DOWNLOAD_H__
//#define __DOWNLOAD_H__

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <queue>
#include <pthread.h>
#include <locale.h>
#include "../window/window.h"
#include "../pool/threadpool.h"

using namespace std;

//#define __DEBUG__
#define DEFAULT_PORT 80
const int _BUF_SIZE = 40960;
const int _LINE_MAX = 4096;
const int _REQUEST_MAX = 1024;
const int _DOMAIN_MAX = 64;
const int _BREAK_MAX = 256; 
const int _FILENAME_MAX = 64;
const int _ADDR_MAX = 256; 
const int _RANGE_MAX = 64;


////////////////////////////////////////////////////////////////////
struct Download
{
	int m_fdCli;
	string m_url;
};

struct IpAndDomainAndAddr
{
	string m_ip;
	string m_domain;
	string m_addr;
};

struct NameAndFd
{
	string m_name;
	int m_fd;
};

////////////////////////////////////////////////////////////////////
void usage();

int GetLine(int sock, char *buf, int buf_len);
string GetFilenameFromAddr(string reqAddr);
IpAndDomainAndAddr AnalyseUrl(string & url);
void SendRequestGram(int sockfd, struct IpAndDomainAndAddr &info, int offset);
void ClearHead(int sock);
int RecvResponseGram(int sockfd);
int  ConnectServer(string & ip);
void DownloadAndDisplay(int sockfd, string fileName, int fileLeftSize, int offset);
void ReConnAndDown(struct IpAndDomainAndAddr & ida, int offset);
bool GetSuffer(const char *str, int len, char *suffer);
bool HasConfigFile(queue<int> & fdq);
void *ThreadDown(void *arg);
void BreakPointDown(queue<int> & fdq);
void *ThreadDisplayHeader(void *arg);

