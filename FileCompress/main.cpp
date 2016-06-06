/*************************************************************************
	> File Name: Main.cpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 12:56:43
 ************************************************************************/

#include <iostream>
#include "FileCompress.hpp"
#include "config.hpp"

using namespace std;


void TestCompress()
{
	FileCompress fc;
	fc.Compress(FILE_NAME);
}

void TestUnCompress()
{
	FileCompress fc;
	fc.UnCompress(UNCOMPRESS_FILE_NAME);
}


int main()
{
	TestCompress();

	TestUnCompress();

	return 0;
}

