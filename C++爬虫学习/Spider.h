#pragma once
#include<iostream>
#include <Windows.h>
#include <regex>//match函数
#include <queue>//队列
//使用网络静态库
#pragma comment(lib,"ws2_32.lib")//lib后缀结尾为静态库
using namespace std;
bool StartCatch(string url);
class CHttp
{
private:
	

public:
	CHttp();//初始化函数
	bool AnalyseUrl(string url);//解析域名
	bool Connect();//联网
	bool GetHtml(string &html);//获取网页数据
	string m_host;//域名
	string m_object;//资源名
	SOCKET m_socket;//客户端套接字//https://zhuanlan.zhihu.com/p/230800627套接字相关知识

};
