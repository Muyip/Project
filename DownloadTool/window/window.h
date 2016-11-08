#pragma once

#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <pthread.h>
using namespace std;

const int G_BLOCK_SIZE = 256;

class DownWindow
{
	public:
		DownWindow();
		~DownWindow();
		void init();
		WINDOW* create_win(const int& hei,const int& wth, const int& y, const int& x);

		void win_refresh(WINDOW* _win);
		void put_str_to_win(WINDOW* _win, int y, int x, const string& _str);
		void get_str_from_win(WINDOW* _win, string& _out);

        void clear_win_line(WINDOW* _win, int begin, int num);

		void draw_header();
		void draw_input();
		void draw_output();

		WINDOW* get_header()
		{
			return this->header;
		}

		WINDOW* get_input()
		{
			return this->input;
		}

		WINDOW* get_output()
		{
			return this->output;
		}

	private:
		WINDOW* header;         // @ 头部窗口的句柄
		WINDOW* input;          // @ 输入窗口的句柄
		WINDOW* output;         // @ 输出窗口的句柄
		pthread_mutex_t lock;   // @ 刷新窗口时需要互斥访问窗口句柄
};
