/*************************************************************************
	> File Name: FileCompress.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 23:53:40
 ************************************************************************/
#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <queue>

#include "Huffman.hpp"
#include "config.hpp"
using namespace std;

typedef long LongType;

struct CharInfo
{
	CharInfo(LongType count = 0)
		:_count(count)
	{}

	CharInfo(unsigned char ch, LongType count = 0)
		:_ch(ch)
		 ,_count(count)
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

#ifdef __TIME__
			clock_t start, finish;
			double duration;
			start = clock();
#endif  /* __TIME__ */

			//打开要压缩的文件
			FILE *fp_in = NULL;
			if((fp_in = fopen(filename, "r")) == NULL)
				perr_exit("open fp_in");

			//读文件，统计字符信息(字符及其出现次数)
			char ch = '\0';
			int rsz = 0;
			while((rsz = fread(&ch, 1, 1, fp_in)))    //sz == 0  --> EOF
			{
				unsigned int index = (unsigned char)ch;
				_infos[index]._ch = ch;
				++_infos[index]._count;

			}  /* while */  //统计完成
			
			//根据统计信息_infos[256]，生成 HuffmanTree
			HuffmanTree<CharInfo> huffman(_infos, 256);    //数组，数组长度

#ifdef __DEBUG__
			Print(huffman.GetRoot());
#endif   /* __DEBUG__ */

			//生成 HuffmanCode
			string code = "";
			GeneralHuffmanCode(huffman.GetRoot(), code);

			int wsz = 0;            //每次写入压缩文件时的返回值  -> 1 Byte
			int pos = 7;            //如果编码中有1，每次需要左移的位数
			unsigned char buf = 0;  //临时空间，只占一个字节，用来写入压缩文件
			ch = 0;	                //从待压缩文件读一个字节到ch中

			//建立新的文件，用来存放压缩内容
			FILE* fp_out = fopen(COMPRESS_FILE_NAME, "w+");
			if(fp_out < 0)
				perr_exit("creat_open fp_out");

			//真正的压缩从这里开始
			//读文件，对每一个字符定位到infos[]，取到code，到8位，写到输出文件
			fseek(fp_in, 0, SEEK_SET);
			while((rsz = fread(&ch, 1, 1, fp_in)))   //sz == 0  --> EOF
			{
				unsigned int index = (unsigned char)ch;
					
				string code = _infos[index]._code;  //取到这个字符的哈夫曼编码
				const char *str = code.c_str();

				for(; *str != '\0'; --pos, ++str)   //依次查看这个编码中不为0的位
				{
					if(pos < 0)                     //够八位，写到文件
					{
						if((wsz = fwrite(&buf, 1, 1, fp_out)) < 0)
							perr_exit("write fp_out");

						pos = 7;	//开始写下一个字节
						buf = 0;
					}
					if(*str == '0')         //如果这个位为0，什么都不做，再读下一位
						continue;
					buf |= 0x1<<pos;        //把哈夫曼编码中为1的位写到buf中
				}  //for
			} //while
			fflush(fp_out);

			//如果最后读完文件，不足一个字节就返回了，或者最后一个字节已经写满，还没有写回压缩文件就返回了，
			//这时要把最后一个字节写到压缩文件
			if(pos != 7)   
			{
				if((wsz = fwrite(&buf, 1, 1, fp_out)) <= 0)
					perr_exit("write fd_out");
			}


#ifdef __TIME__
			finish = clock();
			duration = (double)(finish-start);
			cout<<"Compress used "<<duration / CLOCKS_PER_SEC <<" seconds"<<endl;
#endif  /* __TIME__ */


			//创建配置文件，里面记录压缩的信息（每个字符及其出现的次数，等到解压缩时，哈夫曼树可能已经不在，这时要用这个文件重建哈夫曼树）
			//creat(CONF_NAME, S_IRUSR | S_IWUSR);
			FILE* fp_conf = fopen(CONF_NAME, "w+");
			if(fp_conf == NULL)
				perr_exit("creat_open fp_conf");

			char conf_buf[2+MAX_DIG_OF_COUNT];    // * ch + ',' + count
			bzero(conf_buf, sizeof(conf_buf));

			for(int i = 0; i < 256; ++i)    // ****** 只需要记录出现次数不为0的字符信息
			{
				if(_infos[i]._count == 0)
					continue;

				sprintf(conf_buf, "%c,%d\n\0", _infos[i]._ch, _infos[i]._count);   // ****** 配置文件按一定格式写入
				fwrite(conf_buf, 1, strlen(conf_buf), fp_conf);
				bzero(conf_buf, sizeof(conf_buf));	
			}
			fflush(fp_conf);
	
			fclose(fp_conf);
			fclose(fp_in);
		}


		// * 压缩文件  -->  解压缩文件(与原文件相同)
		void UnCompress(const char *filename)
		{

#ifdef __TIME__
			clock_t start, finish;
			double duration;
			start = clock();
#endif  /* __TIME__ */

			//配置文件中记录的字符及其对应的个数，解压缩时构建哈夫曼树
			//creat(UNCOMPRESS_FILE_NAME,S_IRUSR | S_IWUSR);
			FILE* fp_un = fopen(filename, "w+");              // * 解压缩文件描述符
			if(fp_un == NULL)
				perr_exit("creat_open fp_un");
			FILE* fp_com = fopen(COMPRESS_FILE_NAME, "r");   // * 压缩文件描述符 
			FILE* fp_conf = fopen(CONF_NAME, "r");           // * 配置文件描述符
			
			char buf_line[2+MAX_DIG_OF_COUNT];
			bzero(buf_line, sizeof(buf_line));
			int count = 0; 
			unsigned char ch = '\0';
			Heap<struct HuffmanTreeNode<CharInfo>* > hp;
			while(ReadLine(fp_conf, buf_line))    //每次从配置文件读一行
			{
				if(strlen(buf_line) == 1)     // * 说明只读到一个'\n'
				{
					bzero(buf_line, sizeof(buf_line));
					ReadLine(fp_conf, buf_line);	// * 接着拿到'\n'出现的次数
					count = atoi(buf_line+1);
					ch = '\n';
				}
				else         // * 正常地从配置文件中读到一个字符及其出现的次数
				{
					ch = buf_line[0];
					count = atoi(buf_line+2);
				}
				bzero(buf_line, sizeof(buf_line));
				hp.Push(new HuffmanTreeNode<CharInfo>(CharInfo(ch, count)));   // * 新建一个树节点，放到一个堆中
			}

			HuffmanTree<CharInfo> hft(hp);      // ****** 由堆中的记录信息构建一个哈夫曼树

			ch = '\0';
			int sz = 0;
			int pos = 7;
			int direction = 0;
			HuffmanTreeNode<CharInfo> *cur = hft.GetRoot();

			LongType totalChar = cur->_weight._count;      // ****** 哈夫曼树的根节点存放的数据就是所有字符出现的总数，可以据此为解压缩的终止条件

#ifdef __DEBUG__
			cout<<"总共处理了 "<<totalChar<<" B"<<endl;
#endif  /* __DEBUG__ */

			// × 从压缩文件读一个字节，每次取左边一位，判断往左还是往右走
			while(1)
			{
				sz = fread(&ch, 1, 1, fp_com);
				pos = 7;
				while(pos >= 0)	
				{
					direction = ch & (0x1 << pos);    // * direction的0/1值是指导cur往左还是往右走的方向
					pos--;                            // * 下次取ch的下一个 1,就要少左移一位
					if(direction == 0)
						cur = cur->_left;
					else
						cur = cur->_right;

					if(cur && NULL == cur->_left && NULL == cur->_right)   // ****** 找到一个叶子节点，说明之前读到的几个位是一个字符的哈夫曼编码, 写入解压缩文件中，并且cur再次从根节点开始往下走
					{
						fwrite(&cur->_weight._ch, 1, 1, fp_un);    //得到某个字符，写入文件
						cur = hft.GetRoot();
						if(--totalChar == 0)    // ****** 哈夫曼的根节点记录了所有字符出现的总字节数，若写完了，就返回
							goto RET;
					}
				}
			}

			RET:
			fclose(fp_conf);
			fclose(fp_un);
			fclose(fp_com);

#ifdef __TIME__
			finish = clock();
			duration = (double)(finish-start);
			cout<<"UnCompress used "<<duration / CLOCKS_PER_SEC <<" seconds"<<endl;
#endif  /* _TIME__ */

		}
		
	protected:
		// * 从fd中读一行到buf_line中，如果读到文件尾返回假，其他返回真
		bool ReadLine(FILE *fp, char *buf_line)
		{
			int sz = 0;
			char ch = '\0';
			string str = "";

			if((sz = fread(&ch, 1, 1, fp)) == 0)  
				return false;    //读到文件尾 EOF

			if(ch == '\n')
			{
				sprintf(buf_line, "%c", '\n');
				return true;
			}
			do
			{
				if(ch == '\n')
				{
					sprintf(buf_line, "%s", str.c_str());
					return true;	
				}
				str += ch;
			}while((sz = fread(&ch, 1, 1, fp)) > 0);

			return false;
		}


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


	protected:
		CharInfo _infos[256];
};
