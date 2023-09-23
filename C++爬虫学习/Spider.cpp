#include "Spider.h"

//���غ���
bool Download(string url,string filename)
{
	CHttp Cnewurl;
	if (false == Cnewurl.AnalyseUrl(url))
		return false;
	if (false == Cnewurl.Connect())
		return false;

	string info;
	info = info + "GET " + Cnewurl.m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + Cnewurl.m_host + "\r\n";
	info = info + "Connection: Close\r\n\r\n";

	if (SOCKET_ERROR == send(Cnewurl.m_socket, info.c_str(), info.length(), 0))//sockfd��������Ϣ���׽��ֵ��ļ���������buf��Ҫ���͵���Ϣ��len��Ҫ���͵��ֽ���flags��flags������ʾ�������Ϊ0��������
		return false;

	FILE* fp = fopen(filename.c_str(), "wb");


	//��������
	char buffer[20] = { 0 };
	int nReacv = 0;
	while (recv(Cnewurl.m_socket, buffer, sizeof(char), 0))
	{
		fwrite(buffer,1,nReacv,fp);
	}
	//�ر��׽���
	closesocket(Cnewurl.m_socket);
	//�ͷ������
	WSACleanup();
	return true;


}

int main()
{
	std::cout << "**********************************************************" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                  C++����ѧϰ                           *" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                                                        *" << endl;
	cout << "**********************************************************" << endl;


	cout << "������URL" << endl;
	string startUrl;
	cin >> startUrl;
	StartCatch(startUrl);
	//�����ļ��� ����CreateDirectory��ʾ((c��ʽ)���� lpPathName ��ʾ·������ ,lpSecurityAttributes ��ʾ��ȫ����(һ��ΪNULL))
	CreateDirectory(L"./resource", NULL);//һ��
	CreateDirectory(L"./resource/study_images", NULL);//����
	CreateDirectory(L"./resource/study_videos", NULL);
	return 0;
}


//��ʼץȡ
bool StartCatch(string url)
{
	queue<string> q;//url����
	q.push(url);
	while(!q.empty())
	{
		string  currentUrl = q.front();
		q.pop();
		CHttp http;
		if(false==http.AnalyseUrl(currentUrl))
			return false;

		if(false==http.Connect())
			return false;
		string html;
		http.GetHtml(html);
		

		//ƥ������е�URL

		smatch mat;
		string::const_iterator start = html.begin();
		string::const_iterator end = html.end();
		regex gex("http://[^\\s'\"<>()]+");//
		vector<string>veUrl;
		while (regex_search(start, end, mat, gex) )
		{
			string newurl(mat[0].first, mat[0].second);
			veUrl.push_back(newurl);
			start = mat[0].second;
		}

		//��������NEWURL
		for(int i=0;i<veUrl.size();i++)
		{
			string imageUrl = veUrl[i];
			char filename[64];
			
			sprintf_s(filename, sizeof(filename), "./resource/study_images/%d.jpg ", i + 1);
			if(imageUrl.find(".jpg")!=string::npos)
			{
				Download(veUrl[i],filename);
			}

		}
	}
	return true;
}
//��������
bool CHttp::AnalyseUrl(string url)
{
	 if (url.length() <= 7)
		return  false;
	//���Һ��� ��ĳ���ַ�����ĳһ���ַ�����
     if(-1==url.find("http://"))
		 return false;

	 int pos = url.find('/', 7);
	 if (pos == -1)
	 {
		m_host = url.substr(7);
		m_object = '/';
	 }
	 else
	 {
		 m_host = url.substr(7, pos - 7);
		 m_object = url.substr(pos);
	 }


	return true;
}
//���캯��
CHttp::CHttp()
{
	//��ʼ������
	WSADATA wd;//ָ�� WSADATA ���ݽṹ��ָ�룬�ýṹ���ڽ��� Windows �׽���ʵ�ֵ���ϸ��Ϣ��wsaData�����洢ϵͳ���صĹ���WINSOCK�����ϡ�
	WSAStartup(MAKEWORD(2, 2), &wd);//������⣬������������⺯������һ������ΪMAKEWORD (2,2)��ʾʹ��WINSOCK2�汾.����WINSOCK��


	//�����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, 0);//1.��ַ��2.��ʽ3ָ��Э��
}

//����
bool CHttp::Connect()
{
	//����ת��IP  IP����ֱ�������ķ�ʽ
	hostent* host= gethostbyname(m_host.c_str());//name����ΪC������ʽ
	if (host == NULL)
		return false;
	//����Web������
	sockaddr_in sa;//����ͨѶ��ַ
	sa.sin_family = AF_INET;//sa_family�ǵ�ַ���壬һ�㶼�ǡ�AF_xxx������ʽ��ͨ������õ��Ƕ���AF_INET,����TCP/IPЭ����
	sa.sin_port = htons(80);//sin_port�洢�˿ںţ�ʹ�������ֽ�˳��  htons�������ֽ�˳��ת��Ϊ�����ֽ�˳��
	memcpy(&sa.sin_addr, host->h_addr, 4);//C �⺯�� void *memcpy(void *str1, const void *str2, size_t n) �Ӵ洢�� str2 ���� n ���ֽڵ��洢�� str1��IP��ַΪ�ĸ��ֽ� host��h_addr_list�еĵ�һ��ַ
	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&sa, sizeof(sockaddr)))//connect���Ӻ���
		return false;

	return true;
}



//��ȡ��ҳ����
bool CHttp::GetHtml(string& html)
{
	//������Ϣ
	string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n\r\n";
	
	if (SOCKET_ERROR == send(m_socket, info.c_str(), info.length(), 0))//sockfd��������Ϣ���׽��ֵ��ļ���������buf��Ҫ���͵���Ϣ��len��Ҫ���͵��ֽ���flags��flags������ʾ�������Ϊ0��������
		return false;

	//��������
	char ch = 0;
	while(recv(m_socket,&ch,sizeof(char),0))
	{
		html = html + ch;
	}
	//�ر��׽���
	closesocket(m_socket);
	//�ͷ������
	WSACleanup();
	return true;
}