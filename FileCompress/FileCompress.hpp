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
		{
		}

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
			}//统计完成
			
			//生成 HuffmanTree
			HuffmanTree<CharInfo> huffman(_infos, 256);    //数组，数组长度

			Print(huffman.GetRoot());
		
			//生成 HuffmanCode
			string code = "";
			GeneralHuffmanCode(huffman.GetRoot(), code);

			//读文件，对每一个字符定位到infos[]，取到code，到8位，写到输出文件
			
		}

		void UnCompress(const char *filename);
		//字符及其对应的个数放到配置文件中，解压缩时构建哈夫曼树
		
	protected:
		void GeneralHuffmanCode(HuffmanTreeNode<CharInfo> *root, string code)
		{
			if(root == NULL)
				return;

			if(root->_left)
			{
				GeneralHuffmanCode(root->_left, code+"0");
				//cout<<1<<endl;
			}
			if(root->_right)
			{
				GeneralHuffmanCode(root->_right, code+"1");
				//cout<<2<<endl;
			}
			if(root->_left == NULL && root->_right == NULL)    //PreOrder
			{
				_infos[root->_weight._ch]._code = code;    //Huffman Code还是记录到_infos里面，Huffman Tree只是充当一个找到编码（路径）和这个叶子节点在_infos的下标
				//cout<<3<<endl;
			}
		}

		void Print(HuffmanTreeNode<CharInfo> *root)
		{
			queue<HuffmanTreeNode<CharInfo>* > q;
			q.push(root);
			while(!q.empty())
			{
				HuffmanTreeNode<CharInfo> *tmp = q.front();
				q.pop();
				cout<<tmp->_weight._count<<" ";
				if(tmp->_left)
					q.push(tmp->_left);
				if(tmp->_right)
					q.push(tmp->_right);
			}
		}

	protected:
		CharInfo _infos[256];
};
