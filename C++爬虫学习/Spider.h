#pragma once
#include<iostream>
#include <Windows.h>
#include <regex>//match����
#include <queue>//����
//ʹ�����羲̬��
#pragma comment(lib,"ws2_32.lib")//lib��׺��βΪ��̬��
using namespace std;
bool StartCatch(string url);
class CHttp
{
private:
	

public:
	CHttp();//��ʼ������
	bool AnalyseUrl(string url);//��������
	bool Connect();//����
	bool GetHtml(string &html);//��ȡ��ҳ����
	string m_host;//����
	string m_object;//��Դ��
	SOCKET m_socket;//�ͻ����׽���//https://zhuanlan.zhihu.com/p/230800627�׽������֪ʶ

};
