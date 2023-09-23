#include "Spider.h"

//下载函数
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

	if (SOCKET_ERROR == send(Cnewurl.m_socket, info.c_str(), info.length(), 0))//sockfd：接收消息的套接字的文件描述符。buf：要发送的消息。len：要发送的字节数flags：flags参数表示额外参数为0个或多个。
		return false;

	FILE* fp = fopen(filename.c_str(), "wb");


	//接受数据
	char buffer[20] = { 0 };
	int nReacv = 0;
	while (recv(Cnewurl.m_socket, buffer, sizeof(char), 0))
	{
		fwrite(buffer,1,nReacv,fp);
	}
	//关闭套接字
	closesocket(Cnewurl.m_socket);
	//释放网络库
	WSACleanup();
	return true;


}

int main()
{
	std::cout << "**********************************************************" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                  C++爬虫学习                           *" << endl;
	cout << "*                                                        *" << endl;
	cout << "*                                                        *" << endl;
	cout << "**********************************************************" << endl;


	cout << "请输入URL" << endl;
	string startUrl;
	cin >> startUrl;
	StartCatch(startUrl);
	//创建文件夹 函数CreateDirectory表示((c格式)参数 lpPathName 表示路径参数 ,lpSecurityAttributes 表示安全属性(一般为NULL))
	CreateDirectory(L"./resource", NULL);//一级
	CreateDirectory(L"./resource/study_images", NULL);//二级
	CreateDirectory(L"./resource/study_videos", NULL);
	return 0;
}


//开始抓取
bool StartCatch(string url)
{
	queue<string> q;//url队列
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
		

		//匹配出所有的URL

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

		//遍历所有NEWURL
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
//解析域名
bool CHttp::AnalyseUrl(string url)
{
	 if (url.length() <= 7)
		return  false;
	//查找函数 ‘某个字符’或“某一个字符串”
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
//构造函数
CHttp::CHttp()
{
	//初始化网络
	WSADATA wd;//指向 WSADATA 数据结构的指针，该结构用于接收 Windows 套接字实现的详细信息。wsaData用来存储系统传回的关于WINSOCK的资料。
	WSAStartup(MAKEWORD(2, 2), &wd);//打开网络库，才能启动网络库函数；第一个参数为MAKEWORD (2,2)表示使用WINSOCK2版本.加载WINSOCK库


	//创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);//1.地址族2.形式3指定协议
}

//联网
bool CHttp::Connect()
{
	//域名转换IP  IP才是直接联机的方式
	hostent* host= gethostbyname(m_host.c_str());//name必须为C语言形式
	if (host == NULL)
		return false;
	//连接Web服务器
	sockaddr_in sa;//网络通讯地址
	sa.sin_family = AF_INET;//sa_family是地址家族，一般都是“AF_xxx”的形式。通常大多用的是都是AF_INET,代表TCP/IP协议族
	sa.sin_port = htons(80);//sin_port存储端口号（使用网络字节顺序）  htons将主机字节顺序转换为网络字节顺序
	memcpy(&sa.sin_addr, host->h_addr, 4);//C 库函数 void *memcpy(void *str1, const void *str2, size_t n) 从存储区 str2 复制 n 个字节到存储区 str1。IP地址为四个字节 host是h_addr_list中的第一地址
	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&sa, sizeof(sockaddr)))//connect连接函数
		return false;

	return true;
}



//获取网页数据
bool CHttp::GetHtml(string& html)
{
	//发送信息
	string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n\r\n";
	
	if (SOCKET_ERROR == send(m_socket, info.c_str(), info.length(), 0))//sockfd：接收消息的套接字的文件描述符。buf：要发送的消息。len：要发送的字节数flags：flags参数表示额外参数为0个或多个。
		return false;

	//接受数据
	char ch = 0;
	while(recv(m_socket,&ch,sizeof(char),0))
	{
		html = html + ch;
	}
	//关闭套接字
	closesocket(m_socket);
	//释放网络库
	WSACleanup();
	return true;
}