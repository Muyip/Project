/*********************************************************************************************
	> File Name:      config.hpp
	> Author:         common
	> Mail:	          yp_abc2015@163.com 
	> Created Time:   2016-06-04 Сб. 23:31:02
	> Explanation :    
 ********************************************************************************************/
#pragma once

#include <iostream>
using namespace std;

//#define __DEBUG__
#define __TIME__

const unsigned int MAX_DIG_OF_COUNT = 10;
const char *FILE_NAME = "file";
const char *COMPRESS_FILE_NAME = "compressFile";
const char *CONF_NAME = "compress.conf";
const char *UNCOMPRESS_FILE_NAME = "uncompressFile";

void perr_exit(const char *str)
{
	perror("str");
	exit(1);
}

