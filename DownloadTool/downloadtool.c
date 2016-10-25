#include "downloadtool.h" 


// @ 从sock所指向的文件读取一行到buf中（包括换行），buf容量有buf_len指定
int GetLine(int sock, char *buf, int buf_len)
{
	if(!buf || buf_len < 0)
		return -1;
	int i = 0;
	int n = 0;
	char c = '\0';
	while(i < buf_len && c != '\n')
	{
		n = recv(sock, &c, 1, 0);
		if(n > 0)
		{
			if(c == '\r')
			{
				n = recv(sock, &c, 1, MSG_PEEK);
				if(n > 0 && c == '\n')    // @ 如果遇到\r\n，就把两个字符替换成一个\n
					recv(sock, &c, 1, 0);
				else                      // @ 只有一个\n，直接写
					c = '\n';
			}
			buf[i++] = c;
		}
		else   // @ 读到文件尾
		{
			c = '\n';
		}
	}
	buf[i] = '\0';
	return i;
}

// @ 由一个请求路径，得到最后面的文件名
string GetFilenameFromAddr(string reqAddr)
{
	const char *addr = reqAddr.c_str();
	assert(addr);

	int right = strlen(addr) - 1;
	while(right >= 0)
	{
		if('/' == addr[right])
			return string(addr+right+1);

		--right;
	}
}

// @ 由一个url，得到该地址对应的 ip、域名(domain)和路径(addr)
IpAndDomainAndAddr AnalyseUrl(string & url)
{
	//assert("" != url);
	
	char str[128];
	char tmp[30];
	char ip[16];
	bzero(str, sizeof(str)); 
	bzero(tmp, sizeof(tmp));
	bzero(ip, sizeof(ip));
	strcpy(str, url.c_str());

	IpAndDomainAndAddr da;   // 用于保存url的信息

	int index = 0;
	int len = strlen(str);
	int domainBegin = 0;
	while(index < len)
	{
		if(str[index] == '/')
		{
			if(index + 1 < len && str[index + 1] == '/')
			{
				++index;
				domainBegin = index + 1;
			}
			else
			{
				char domain[_DOMAIN_MAX];
				bzero(domain, _DOMAIN_MAX);

				struct hostent *hent = new struct hostent;
				// @ 由domain得到ip
				hent = gethostbyname((const char *)memcpy(tmp, str+domainBegin, index - domainBegin));
				memcpy(domain, str+domainBegin, (index - domainBegin));
				da.m_ip = string(inet_ntop(AF_INET, *hent->h_addr_list, ip, 16));
				da.m_domain = string(domain);
				da.m_addr = string(str+index);

				return da;
			}
		}

		++index;
	}
	
	cout << "wrong url..." << endl;
}

// @ 给服务器发送请求报文
// @ 参数意义: sockfd(连接服务器的文件描述符)
//             info(所请求的资源的信息, ip、domain、addr)
//             offset(请求相应报文从offset开始发送, 用来实现断点续传)
void SendRequestGram(int sockfd, struct IpAndDomainAndAddr &info, int offset)
{
	const char *addr = info.m_addr.c_str();
	const char *domain = info.m_domain.c_str();

	char range[_RANGE_MAX];
	bzero(range, _RANGE_MAX);
	sprintf(range, "bytes=%d-", offset);

	char buffer[_REQUEST_MAX];
	bzero(buffer, _REQUEST_MAX);

	// @ buffer内容是请求报文的内容
	sprintf(buffer, "%s %s %s\nHost: %s\nAccept: %s\nAccept-Encoding: %s\nAccept-Language: %s\nUser-Agent: %s\nRange: %s\nProxy-Connection: %s\n\n",
			"GET", addr, "HTTP/1.1",
			domain,
			"text/html, application/xhtml+xml, application/xml;q=0.9,image/webp,*/*;q=0.8",
			"gzip,deflate,sdch",
			"zh-CN,zh;q=0.8",
			"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0",
			range,
			"close");
#ifdef __DEBUG__
	cout << "send buf : " << buffer << endl;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//sprintf(buffer, "%s %s %s\nHost: %s\nUser-Agent: %s\nRange: %s\n\n", "GET", addr, "HTTP/1.1", domain, "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0", range);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// @ 发送报文
	send(sockfd, buffer, strlen(buffer), 0);
}

// @ 清除sock所收到相应报文的头部信息
void ClearHead(int sock)
{
	char buf[_LINE_MAX];
	int ret = 0;
	do
	{
		bzero(buf, _LINE_MAX);
		ret = GetLine(sock, buf, _LINE_MAX);    // @ 只是读取，并不做处理
	}while(ret > 0 && 0 != strcmp(buf, "\n"));
}

// @ 从sockfd接收响应报文
int RecvResponseGram(int sockfd)
{
	char buf[_LINE_MAX];
	bzero(buf, _LINE_MAX);
	int numChars = 0;
	int fileSize = 0;
	do
	{
		bzero(buf, _LINE_MAX);
		numChars = GetLine(sockfd, buf, _LINE_MAX);

		// @ 匹配到Content-Length: 这一行，拿出长度
		if(0 == strncasecmp(buf, "Content-Length: ", strlen("Content-Length: ")))
		{
			fileSize = atoi(buf+16);
#ifdef __DEBUG__
			cout << "ContentLength : " << fileSize << endl;
#endif
			return fileSize;
		}
	}while(numChars > 0 && strcmp(buf, "\n") != 0);
	
	cout << "response gram error" << endl;

	exit(1);
}

// @ 由ip连接上远端服务器
// @ 返回创建的socket文件描述符
int  ConnectServer(string & ip)
{
	// @ connect to the server 
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > sockfd)
	{
#ifdef __DEBUG__
		cout << "create socket error" << endl;
#endif
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), (void *)&(serAddr.sin_addr));
	serAddr.sin_port = htons(DEFAULT_PORT);

	if(-1 == connect(sockfd, (struct sockaddr *)&serAddr, socklen_t(sizeof(serAddr))))
	{
		cout << "conncet error :(, please input url again :)" << endl;
		exit(1);
	}

	return sockfd;
}


// @ 下载并显示进度情况
// @ 参数意义：sockfd(连接服务器的文件描述符)
//             localfd(要下载的文件对应的本地文件描述符)
//             configfd(要下载的配置文件描述符)
//             fileLeftSize(要下载的文件剩余大小，开始时是文件总大小)
//             offset(要下载的文件偏移，用来实现断点续传)
void DownloadAndDisplay(int sockfd, int localfd, int configfd, int fileLeftSize, int offset)
{
	// @ 本地文件重定位到offset处	
	lseek(localfd, offset, SEEK_SET);

	lseek(configfd, 0, SEEK_SET);
	int infoLen = 0;
	int c = '\0';
	while(read(configfd, &c, 1) == 1)
	{
		++infoLen;
		if('\n' == c)
			break;
	}

	char buf[_BUF_SIZE];    // @ 读写网络文件的临时缓冲区
	bzero(buf, _LINE_MAX);
	char offsetBuf[32];
	bzero(offsetBuf, 32);

	// @ 进度条初始化
	char proBar[102];
	bzero(proBar, sizeof(proBar));

	int hasCompletedPercent = ((float)(offset) / (float)(offset + fileLeftSize)) * 100;   // @ 已完成的百分比
	float rate = 1.1;       // @ 下载速率
	int spacing = 2;        // @ 时间间隔

	// @ 记录时间
	int beginDown = time(NULL);
	int beginOnceRecv = beginDown;
	int endOnceRecv = 0;

	int rsz = 0;					   // @ 每次读到的数据个数
	int totalRead = offset;  		   // @ 已经读到的总的字节数

	int moveGap = (fileLeftSize + offset) / 100;  // @ 总文件的百分之一

	int gap = totalRead % moveGap;                       // how many characters begin to write to file

	int count = 0;	                   
	
	// @ 已经达到了多少百分比，就初始化多少进度条
	while(hasCompletedPercent--)
		proBar[count++] = '*';

	while(fileLeftSize > 0)
	{
		bzero(buf, _BUF_SIZE);
		rsz = recv(sockfd, buf, _BUF_SIZE, 0);
		if(rsz < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
			continue;

		write(localfd, buf, rsz);
		gap += rsz;    
		
		totalRead += rsz;

	// @ 记录最近的一次写入信息，如果程序退出，从这个偏移开始下载
		bzero(offsetBuf, sizeof(offsetBuf));
		sprintf(offsetBuf, "%d", totalRead);     
		// @ 文件指针重定位到固定位置去写覆盖offset
		lseek(configfd, infoLen, SEEK_SET);         
		// @ 把文件偏移写到配置文件
		write(configfd, offsetBuf, strlen(offsetBuf));    

		// @ 只有在读到了总文件的百分之一，才会去更改状态条
		if(gap >= moveGap && count < 100)   // @ 每超过 1% 更新显示下载状况
		{
			endOnceRecv = time(NULL);
			spacing = endOnceRecv - beginOnceRecv;
			
			// @ 防止程序执行过快，导致时间间隔为0,除0会导致段错误
			if(0 == spacing)
				spacing = 1.2;
			
			rate = gap / spacing;
			beginOnceRecv = endOnceRecv;

			int percent = gap / moveGap;	
			while(percent--)
				proBar[count++] = '*';
			--count;

			printf("[%-100s] [%d%%] [%.1f B/s] [%-ds]\r", proBar, ++count, rate, endOnceRecv - beginDown);
			fflush(stdout);

			//sleep(1);

			gap = gap % moveGap;
		}

		fileLeftSize -= rsz;
		//if(100 <= count)
		//	break;
	}
	cout << endl;
	
	char configPath[_FILENAME_MAX];
	bzero(configPath, sizeof(configPath));
	
	// @ 关闭本地下载文件
	close(localfd);
}

// @ 重新连接服务器并下载文件
// 参数信息：ida(要下载的资源信息，ip、domain、addr)
//           offset(要下载的文件偏移量，开始时给0)
void ReConnAndDown(struct IpAndDomainAndAddr & ida, int offset)
{
	string ip = ida.m_ip;
	string domain = ida.m_domain;
	string addr = ida.m_addr;

	int sockfd = ConnectServer(ip);

	// @ 重新发送请求报文
	SendRequestGram(sockfd, ida, offset);

	// @ 解析响应报文
	int fileSize = RecvResponseGram(sockfd);
#ifdef __DEBUG__
	cout << "in func ReConnServer, fileSize : " << fileSize << endl;
#endif

	ClearHead(sockfd);	

	// get local file name used for open it
	string fileName = GetFilenameFromAddr(addr);
	int localfd = 0;
	if((localfd = open(fileName.c_str(), O_WRONLY)) < 0)
	{
		cout << "open " << fileName << " error" << endl;
		exit(1);
	}

	
	// @ 得到配置文件的路径 --> ./ConfFileName.config
	char configPath[_FILENAME_MAX];
	bzero(configPath, _FILENAME_MAX);
	sprintf(configPath, "./%s.config", fileName.c_str());

	// @ 打开配置文件
	int configfd = 0;
	if ((configfd = open(configPath, O_RDWR)) < 0)
	{
		cout << "open " << configPath << " error" << endl;
		exit(1);
	}

	// @ 开始下载
	DownloadAndDisplay(sockfd, localfd, configfd, fileSize, offset);

	close(localfd);
	close(configfd);

	//remove(configPath);	
}

// @ 得到文件的后缀名(包括.号)
// @ 参数意义：str(文件名)
//             len(文件名长度)
//             suffer(输出型参数，拿出后缀)
bool GetSuffer(const char *str, int len, char *suffer)
{
#ifdef __DEBUG__
	cout << "get in the GetSuffer" << endl;
	cout << "str : " << str << endl;
	cout << "len : " << len << endl;
#endif

	if(NULL == str || 0 >= len)
		return false;
	int end = len - 1;
	while(end >= 0)
	{
		if(str[end] == '.')
		{
			strncpy(suffer, str + end, len - end);
			return true;
		}
		--end;
	}

	return false;
}

// @ 查看当前目录有没有含.config后缀的配置文件
// @ 如果有，打开这个文件，把文件描述符放到fdq队列中，返回真
// @ 如果没有，返回假
bool HasConfigFile(queue<int> & fdq)
{
	DIR *dir;	
	struct dirent *ptr;
	int fd = 0;
	char fileName[_FILENAME_MAX];
	bzero(fileName, _FILENAME_MAX);
	
	dir = opendir("./");
	// @ 每次从dir目录读一个文件到ptr中
	while((ptr = readdir(dir)) != NULL)
	{
		bzero(fileName, sizeof(fileName));

		// @ 从ptr(struct dirent *类型)里取出文件名放到fileName中
		sprintf(fileName, "%s", ptr->d_name);

		char suffer[_FILENAME_MAX];
		bzero(suffer, _FILENAME_MAX);
		bool ret = GetSuffer(fileName, strlen(fileName), suffer);
		if(!ret)
			continue;

		if(strncmp(suffer, ".config", strlen(".config")) == 0)
		{
			int fd = open(fileName, O_RDWR); 
			assert(fd > 0);
			fdq.push(fd);
		}
	}

	return fdq.size() != 0;
}

// @ 下载文件，默认端口号 80
int main(int argc, char *argv[])
{
	if(1 != argc)
	{
		usage();
		exit(1);
	}
	struct IpAndDomainAndAddr ida;

	char infoBreak[_BREAK_MAX];
	bzero(infoBreak, _BREAK_MAX);

	// @ 刚进入程序，首先看有没有配置文件
	queue<int> fdq;
	bool hasconfig = HasConfigFile(fdq);

	// @ 有配置文件，开始断点续传
	if(hasconfig)    
	{
		while(!fdq.empty())
		{
			int configfd = fdq.front();
			fdq.pop();

			string ip = "";
			string domain = "";
			string addr = "";
			string offset = "";
			char c = '\0';
			int i = 0;

			// @ 从配置文件取出ip，重新连接服务器需要用到
			while(read(configfd, &c, 1) == 1)
			{
				if(' ' == c)
					break;
				ip += c;
			}
			
			// @ 从配置文件取出域名，在请求报文头部中Host:字段用到
			while(read(configfd, &c, 1) == 1)
			{
				if(' ' == c)
					break;
				domain += c;				
			}

			// @ 从配置文件取出addr，因为需要打开本地下载文件，就需要知道文件名
			while(read(configfd, &c, 1) == 1)
			{
				if('\n' == c)
					break;
				addr += c;
			}

			// @ 从配置文件取出offset，在断点续传中Range:字段用到 
			// exit loop when EOF
			while(read(configfd, &c, 1) == 1)
			{
				offset += c;
			}

			struct IpAndDomainAndAddr ida;
			ida.m_ip = ip;
			ida.m_domain = domain;
			ida.m_addr = addr;
			
			// @ 重新连接并且从上次离开处下载文件
			ReConnAndDown(ida, atoi(offset.c_str()));

			string confAddr = ida.m_addr + ".config";
			string confName = GetFilenameFromAddr(confAddr);
			remove(confName.c_str());
		}
	}
	
/////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// @ 下面开始正常下载一个文件	
	char *buf[128];
	bzero(buf, sizeof(buf));	
	string ip, addr, domain;

	// @ 存储客户端的地址
	struct sockaddr_in client;
	socklen_t len = 0;
	string url;
	while(1)
	{
		cout << "input url > ";
		cin >> url;
		// url = "http://www.baidu.com/";
		// http://sw.bos.baidu.com/sw-search-sp/software/84b5fcf50a3de/QQ_8.7.19083.0_setup.exe

#ifdef __DEBUG__
		cout << "url : " << url << endl;
#endif
/////////////////////////////////////////////////////////////////////////////////////////// belong to a thread, only need a url
		ida = AnalyseUrl(url);
		string fileName = GetFilenameFromAddr(ida.m_addr);

#ifdef __DEBUG__
		cout << "ip : " << ida.m_ip << endl;
		cout << "domain : " << ida.m_domain << endl;
		cout << "addr : " << ida.m_addr << endl;
#endif
		// @ config file
		char configPath[_FILENAME_MAX];
		bzero(configPath, _FILENAME_MAX);
		sprintf(configPath, "./%s.config", fileName.c_str());

		// @ 记录断点信息
		sprintf(infoBreak, "%s %s %s\n", ida.m_ip.c_str(), ida.m_domain.c_str(), ida.m_addr.c_str());

		// @ 创建配置文件 
		creat(configPath, S_IWUSR | S_IRUSR);
		int configfd = open(configPath, O_RDWR); 
		int infoLen = strlen(infoBreak);        // @ 断点信息长度，方便文件指针越过断点信息一行 重定位到offset处
		write(configfd, infoBreak, infoLen);    // @ ip domain addr \n offset

		ip = ida.m_ip;
		domain = ida.m_domain;
		addr = ida.m_addr;

		// @ 连接服务器
		int sockfd = ConnectServer(ip);

		// @ 发送请求报文
		SendRequestGram(sockfd, ida, 0); 

		// @ 解析响应报文
		int fileSize = RecvResponseGram(sockfd);

		if(fileSize == 0)
		{
			cout << "nothing to story, input again :)" << endl;
			exit(1);
		}

		// @ 忽略其他头部信息，定位到相应消息体
		ClearHead(sockfd);

		char localPath[_FILENAME_MAX];
		bzero(localPath, _FILENAME_MAX);
		sprintf(localPath, "./%s", fileName.c_str());

		// @ 创建一个和要下载的文件同名的本地文件
		creat(localPath, S_IWUSR | S_IRUSR);
		int localfd = open(localPath, O_RDWR); 

		DownloadAndDisplay(sockfd, localfd, configfd, fileSize, 0);

		remove(configPath);

	} /* while(1) */

	return 0;
}
