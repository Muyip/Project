/*************************************************************************
	> File Name: Main.cpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 12:56:43
 ************************************************************************/

#include <iostream>
#include "FileCompress.hpp"
using namespace std;

int main()
{
	//int fd = open("./file", O_RDONLY);
	//cout<<fd<<endl;

	FileCompress fc;
	fc.Compress("./file");


	return 0;
}

