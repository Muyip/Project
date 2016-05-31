/*************************************************************************
	> File Name: FileCompress.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 23:53:40
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

#include <queue>

#include "Huffman.hpp"
using namespace std;


typedef long LongType;

struct CharInfo
{
	CharInfo(LongType count = 0)
		:_count(count)
	{}

	bool operator<(CharInfo &rhs)
	{
		return _count < rhs._count;
	}

	CharInfo operator+(CharInfo &rhs)
	{
		return CharInfo(_count + rhs._count);
	}

	unsigned char _ch;
	LongType      _count;
	string        _code;
};

class FileCompress
{
	public:
		FileCompress()
		{}

	public:
		void Compress(const char *filename)
		{
			//打开要压缩的文件
			int fd_in = 0;
			if((fd_in = open(filename, O_RDONLY)) == -1)
			{
				perror("open");
				exit(1);
			}

			//读文件，统计字符信息
			char ch = '\0';
			int rsz = 0;
			while((rsz = read(fd_in, &ch, 1)))   //sz == 0  --> EOF
			{
				if(rsz == -1)    //read error
					continue;   //再读一次

				else            //OK
				{
					unsigned int index = (unsigned char)ch;
					_infos[index]._ch = ch;
					++_infos[index]._count;
				}
			}//统计完成
			
			//生成 HuffmanTree
			HuffmanTree<CharInfo> huffman(_infos, 256);    //数组，数组长度

#ifdef __DEBUG__
			Print(huffman.GetRoot());
#endif   /* __DEBUG__ */

			//生成 HuffmanCode
			string code = "";
			GeneralHuffmanCode(huffman.GetRoot(), code);

			int wsz = 0;            //每次写入压缩文件时的返回值 1 Byte
			int pos = 7;            //如果编码中有1，每次需要移动的位数
			unsigned char buf = 0;  //临时空间，只占一个字节，用来写入压缩文件
			ch = 0;	                //从待压缩文件读一个字节到ch中

			//建立新的文件，用来存放压缩内容
			creat("./CompressFile", S_IRUSR | S_IWUSR);
			int fd_out = open("./CompressFile", O_RDWR);

			//读文件，对每一个字符定位到infos[]，取到code，到8位，写到输出文件
			lseek(fd_in, 0, SEEK_SET);
			while((rsz = read(fd_in, &ch, 1)))   //sz == 0  --> EOF
			{
				if(rsz == -1)    //read error
					continue;    //再读一次

				else             //OK
				{
					unsigned int index = (unsigned char)ch;
					
					string code = _infos[index]._code;  //取到这个字符的哈夫曼编码
					const char *str = code.c_str();

					for(; *str != '\0'; --pos, ++str)   //依次查看这个编码中不为0的位
					{
						if(pos < 0)                     //够八位，写到文件
						{
							if((wsz = write(fd_out, &buf, 1)) < 0)
							{
								perror("write");
								exit(1);
							}
							pos = 7;	//开始写下一个字节
							buf = 0;
						}

						if(*str == '0')         //如果这个位为0，什么都不做，再读下一位
							continue;

						buf |= 0x1<<pos;        //把哈夫曼编码中为1的位写到buf中
					}  //for
				}  //else

			} //while

			if(pos != 7)   //如果最后读完文件，不足一个字节就返回了，或者最后一个字节已经写满，还没有写回压缩文件就返回了，这时要把最后一个字节写到压缩文件
			{
				if((wsz = write(fd_out, &buf, 1)) < 0)
				{
					perror("write");
					exit(1);
				}
			}
		}

		void UnCompress(const char *filename);
		//字符及其对应的个数放到配置文件中，解压缩时构建哈夫曼树
		
	protected:
		void GeneralHuffmanCode(HuffmanTreeNode<CharInfo> *root, string code)
		{
			if(root == NULL)
				return;

			if(root->_left)
				GeneralHuffmanCode(root->_left, code+"0");

			if(root->_right)
				GeneralHuffmanCode(root->_right, code+"1");

			if(root->_left == NULL && root->_right == NULL)             //PreOrder
				_infos[root->_weight._ch]._code = code;    //Huffman Code还是记录到_infos里面，Huffman Tree只是充当一个找到编码（路径）和这个叶子节点在_infos的下标
		}

		//void Print(HuffmanTreeNode<CharInfo> *root)
		//{
		//	queue<HuffmanTreeNode<CharInfo>* > q;
		//	q.push(root);
		//	while(!q.empty())
		//	{
		//		HuffmanTreeNode<CharInfo> *tmp = q.front();
		//		q.pop();
		//		cout<<tmp->_weight._count<<" ";
		//		if(tmp->_left)
		//			q.push(tmp->_left);
		//		if(tmp->_right)
		//			q.push(tmp->_right);
		//	}
		//}

	protected:
		CharInfo _infos[256];
};
