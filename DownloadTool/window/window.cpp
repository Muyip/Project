#include "window.h"
#include <string.h>


DownWindow::DownWindow()
{
	header = NULL;
	input = NULL; 
	output = NULL;
}

void DownWindow::init()
{
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&outputLock, NULL);

	setlocale(LC_ALL, "");

	initscr();
}

void DownWindow::clear_win_line(WINDOW* _win, int begin, int num)
{
	while(num-- > 0)
	{
		wmove(_win, begin++, 0);
		wclrtoeol(_win);
	}
}


WINDOW* DownWindow::create_win(const int& hei,const int& wth, const int& y, const int& x)
{
	WINDOW* _win = newwin(hei, wth, y, x);

	return _win;
}

void DownWindow::win_refresh(WINDOW* _win)
{
	box(_win, 0, 0);
	pthread_mutex_lock(&lock);
	wrefresh(_win);
	pthread_mutex_unlock(&lock);
}

void DownWindow::draw_header()
{
	int hei = LINES*2/10;
	int wth = COLS;
	int y = 0;
	int x = 0;

	this->header = create_win(hei, wth, y,x);
}

void DownWindow::draw_input()
{
	int hei = LINES/10;
	int wth = COLS;
	int y = LINES*2/10;
	int x = 0;

	this->input = create_win(hei, wth, y, x);
}

void DownWindow::draw_output()
{
	int hei = LINES*7/10;
	int wth = COLS;
	int y = LINES*3/10;
	int x = 0;

	this->output = create_win(hei, wth, y, x);
}


void DownWindow::put_str_to_win(WINDOW* _win, int y, int x, const string& _str)
{
	pthread_mutex_lock(&outputLock);
	mvwaddstr(_win, y, x, _str.c_str());
	pthread_mutex_unlock(&outputLock);
}

void DownWindow::get_str_from_win(WINDOW* _win, string& _out)
{
	char buf[G_BLOCK_SIZE];
	memset(buf, '\0', sizeof(buf));

	wgetnstr(_win, buf, sizeof(buf));
	_out = buf;
}

DownWindow::~DownWindow()
{
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&outputLock);
	delwin(this->header);
	delwin(this->input);
	delwin(this->output);
	endwin();
}

