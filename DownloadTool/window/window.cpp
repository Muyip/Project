#include "window.h"
#include <string.h>

/*
 * 窗口的构造函数
 * 初始化下载窗口类的部分成员
 * */
DownWindow::DownWindow()
{
	header = NULL;
	input = NULL; 
	output = NULL;
}

/*
 * 窗口的初始化
 * */
void DownWindow::init()
{
	pthread_mutex_init(&lock, NULL);
	//pthread_mutex_init(&outputLock, NULL);

	setlocale(LC_ALL, "");

	initscr();
}

/*
 * 清除指定窗口的一行信息
 * */
void DownWindow::clear_win_line(WINDOW* _win, int begin, int num)
{
	while(num-- > 0)
	{
		wmove(_win, begin++, 0);
		wclrtoeol(_win);
	}
}

/*
 * 创建一个窗口，在(y, x)位置开始，创建一个长wth，高hei的窗口
 * */
WINDOW* DownWindow::create_win(const int& hei,const int& wth, const int& y, const int& x)
{
	WINDOW* _win = newwin(hei, wth, y, x);

	return _win;
}

/*
 * 刷新指定窗口
 * */
void DownWindow::win_refresh(WINDOW* _win)
{
	box(_win, 0, 0);
	pthread_mutex_lock(&lock);
	wrefresh(_win);
	pthread_mutex_unlock(&lock);
}

/*
 * 画出头部窗口
 * */
void DownWindow::draw_header()
{
	int hei = LINES*3/10;
	int wth = COLS;
	int y = 0;
	int x = 0;

	this->header = create_win(hei, wth, y,x);
}

/*
 * 画出输入窗口
 * */
void DownWindow::draw_input()
{
	int hei = LINES*1/10;
	int wth = COLS;
	int y = (LINES*3/10) + 1;
	int x = 0;

	this->input = create_win(hei, wth, y, x);
}

/*
 * 画出输出窗口
 * */
void DownWindow::draw_output()
{
	int hei = LINES*6/10;
	int wth = COLS;
	int y = (LINES*4/10) + 1;
	int x = 0;

	this->output = create_win(hei, wth, y, x);
}

/* 
 * 往指定窗口的(y, x)处打印字符串_str
 * */
void DownWindow::put_str_to_win(WINDOW* _win, int y, int x, const string& _str)
{
	//pthread_mutex_lock(&outputLock);
	mvwaddstr(_win, y, x, _str.c_str());
	//pthread_mutex_unlock(&outputLock);
}

/*
 * 从指定窗口获取信息保存到_out中
 * */
void DownWindow::get_str_from_win(WINDOW* _win, string& _out)
{
	char buf[G_BLOCK_SIZE];
	memset(buf, '\0', sizeof(buf));

	wgetnstr(_win, buf, sizeof(buf));
	_out = buf;
}

/*
 * 窗口析构
 * */
DownWindow::~DownWindow()
{
	pthread_mutex_destroy(&lock);
	//pthread_mutex_destroy(&outputLock);
	delwin(this->header);
	delwin(this->input);
	delwin(this->output);
	endwin();
}

