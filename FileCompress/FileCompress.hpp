/*************************************************************************
	> File Name: FileCompress.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 23:53:40
 ************************************************************************/

#include <iostream>
#include <string>
using namespace std;


typedef long LongType;

struct CharInfo
{
	CharInfo()
		:_count(0)
	{
	}

	unsigned char _ch;
	LongType      _count;
	string        _code;
};

class FileCompress
{
	public:
		void Compress(const char *filename)
		{
			int fd_in = 0;
			if(fd_in = open(filename, O_RDONLY) == -1)
			{
				perror("open");
				exit(1);
			}

			//读文件，统计字符信息
			char ch = '\0';
			int sz = 0;
			while((sz = read(fd_in, &ch, 1)))   //sz == 0  --> EOF
			{
				if(sz == -1)    //read error
					continue;   //再读一次

				else            //OK
				{
					unsigned int index = (unsigned char)ch;
					_infos[index]._ch = ch;
					++_infos[index]._count;

				}
			}

			//构建HuffmanTree
			HuffmanTree

			//生成HuffmanCode



			//读文件，对每一个字符定位到infos[]，取到code，到8位，写到输出文件


		}

		void UnCompress(const char *filename);
	protected:
		CharInfo _infos[256];
};
