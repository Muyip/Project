#include "download.h"

extern Threadpool tp;
extern DownWindow dw;

// @ 下载文件，默认端口号 80
int main(int argc, char *argv[])
{
	if(1 != argc)
	{
		usage();
		exit(1);
	}

	// @ 窗口初始化
	dw.init();

	dw.draw_header();
	dw.win_refresh(dw.get_header());

	dw.draw_input();
	dw.win_refresh(dw.get_input());

	dw.draw_output();
	dw.win_refresh(dw.get_output());

	// @ 刚进入程序，首先看有没有配置文件
	queue<int> fdq;
	bool hasconfig = HasConfigFile(fdq);

	// @ 创建一个线程用来刷新头部窗口
	pthread_t headTid;
	pthread_create(&headTid, NULL, ThreadDisplayHeader, NULL);
	// @ 有配置文件，开始断点续传
	if(hasconfig)    
		BreakPointDown(fdq);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// @ 下面开始正常下载一个文件	
	char *buf[128];
	bzero(buf, sizeof(buf));	

	// @ 存储客户端的地址
	struct sockaddr_in client;
	socklen_t len = 0;
	string url;
	pthread_t tid;

	while(1)
	{
		// @ 初始化输入栏的提醒
		string input = "input url > ";
		dw.put_str_to_win(dw.get_input(), 1, 1, input);
		dw.win_refresh(dw.get_input());

		dw.get_str_from_win(dw.get_input(), url);
		dw.clear_win_line(dw.get_input(), 1, 12);
		dw.win_refresh(dw.get_input());

#ifdef __DEBUG__
		cout << "url : " << url << endl;
#endif
		// url = "http://www.baidu.com/";
		// http://sw.bos.baidu.com/sw-search-sp/software/84b5fcf50a3de/QQ_8.7.19083.0_setup.exe

		string *url_arg = new string(url);
		//pthread_create(&tid, NULL, ThreadDown, (void*)url_arg);
		tp.AddWorkToWorkQueue(ThreadDown, (void*)url_arg);

	} /* while(1) */
	return 0;
}
