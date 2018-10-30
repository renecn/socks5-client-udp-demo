#include "s5ClientTcpChannel.h"
#include <Ws2tcpip.h>
#include <iostream>
#include <string.h>
#include "common.h"

s5ClientTcpChannel::s5ClientTcpChannel()
{
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (0 != WSAStartup(wVersion, &wsaData))
	{
		std::cout << "WSAStartup() error" << std::endl;
		return ;
	}

	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		std::cout << "s5ClientTcpChannel() error" << std::endl;
		return;
	}
}

BOOL s5ClientTcpChannel::QequestCertificate()
{
	sockaddr_in server;
	InetPton(AF_INET, SS5_SERVER_IP, &server.sin_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(SS5_SERVER_PORT);
	// 客户端连到服务器后, 然后就发送请求来协商版本和认证方法
	if (connect(sClient, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "connect() error" << std::endl;
		return FALSE;
	}

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bufferlen = 1023;
	int ret;
	sock5req1 *m_proxyreq1;
	m_proxyreq1 = (sock5req1 *)buffer;
	m_proxyreq1->Ver = SOCKS_VER;				// V 5.      [版本]
	m_proxyreq1->nMethods = 2;						// 2 种验证方式[方法的数目]
	m_proxyreq1->Methods[0] = METHOD_AUTH_NO;		// X'00'  不需要认证 [方法1]
	m_proxyreq1->Methods[1] = METHOD_AUTH;			// X'02'  用户名/密码[方法2]

	ret = send(sClient, buffer, 4, 0);
	if (ret == SOCKET_ERROR)
	{
		std::cout << "send() in QequestCertificate function" << std::endl;
		closesocket(sClient);
		return FALSE;
	}

	memset(buffer, 0, sizeof(buffer));
	ret = recv(sClient, buffer, bufferlen, 0);
	if (ret == SOCKET_ERROR)
	{
		std::cout << "recv() in QequestCertificate function" << std::endl;
		closesocket(sClient);
		return FALSE;
	}

	sock5ans1 *m_proxyans1;
	m_proxyans1 = (sock5ans1 *)buffer;
	
	//PrintBuffer(buffer, bufferlen);

	if (m_proxyans1->Ver != SOCKS_VER || (m_proxyans1->Method != METHOD_AUTH_NO && m_proxyans1->Method != METHOD_AUTH))
	{
		std::cout << "连接失败" << std::endl;
		closesocket(sClient);
		return FALSE;
	}

	if (m_proxyans1->Method == METHOD_AUTH)
	{
		const char *AuthName = "test1"; //ss5 server 用户名
		const char *AuthPasswd = "12345"; //ss5 server 密码

		socks5authreq *m_authreq;
		int iNameLen = strlen(AuthName);
		int iPasswd = strlen(AuthPasswd);
		m_authreq = (socks5authreq *)buffer;

		m_authreq->Ver = SOCKS_AUTH_VER;
		m_authreq->other[0] = iNameLen;
		strcpy_s(m_authreq->other + 1, strlen(AuthName)+1,AuthName);
		m_authreq->other[iNameLen + 1] = iPasswd;
		strcpy_s(m_authreq->other + iNameLen + 2,strlen(AuthPasswd)+1, AuthPasswd);

		ret = send(sClient, buffer, 3 + iNameLen + iPasswd, 0);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "send() 133" << std::endl;
			closesocket(sClient);
			return FALSE;
		}

		memset(buffer, 0, sizeof(buffer));
		ret = recv(sClient, buffer, bufferlen, 0);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "recv() 142" << std::endl;
			closesocket(sClient);
			return FALSE;
		}
		socks5authans *m_authans;
		m_authans = (socks5authans *)buffer;

		if (m_authans->Ver != SOCKS_AUTH_VER || m_authans->Status != SOCKS_AUTH_OK)
		{
			std::cout << "身份验证失败" << std::endl;
			closesocket(sClient);
			return FALSE;
		}
		else {
			std::cout << "身份验证成功" << std::endl;
		}
	}

	return true;
}

BOOL s5ClientTcpChannel::GetUdpProxyInfo(char *ip, WORD &wPort)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int bufferlen = 1023;
	int ret;

	sock5udpreq2 *m_proxyudpreq2;
	m_proxyudpreq2 = (sock5udpreq2 *)buffer;
	int len;

	m_proxyudpreq2 = (sock5udpreq2 *)buffer;
	m_proxyudpreq2->Ver = SOCKS_VER;
	m_proxyudpreq2->Cmd = CMD_UDP;
	m_proxyudpreq2->Rsv = FIELD_RSV;
	m_proxyudpreq2->Atyp = ATYP_IPV4;
	m_proxyudpreq2->dwReserve = 0/*inet_addr("202.70.165.82")*/;
	m_proxyudpreq2->wLocalPort = htons(TEST_UDP_PORT);
	len = 10;

	ret = send(sClient, buffer, len, 0);
	if (ret == SOCKET_ERROR)
	{
		closesocket(sClient);
		return FALSE;
	}

	memset(buffer, 0, sizeof(buffer));
	ret = recv(sClient, buffer, bufferlen, 0);
	if (ret == SOCKET_ERROR)
	{
		closesocket(sClient);
		return FALSE;
	}

	sock5udpans2 *m_proxyudpans2;
	m_proxyudpans2 = (sock5udpans2 *)buffer;

	if (m_proxyudpans2->Rep != REP_SUCCESS)
	{
		std::cout << "通过代理连接远端主机失败!" << std::endl;
		closesocket(sClient);
		return FALSE;
	}

	// 获取代理服务器返回IP
	struct in_addr in;
	in.S_un.S_addr = m_proxyudpans2->dwServerIp;
	//strcpy(ip, inet_ntoa(in));
	char UdpProxyIp[INET_ADDRSTRLEN];
	int ipLen = strlen(inet_ntop(AF_INET, &in.S_un.S_addr, UdpProxyIp, sizeof(UdpProxyIp)));
	strcpy_s(ip, ipLen + 1, inet_ntop(AF_INET, &in.S_un.S_addr, UdpProxyIp, sizeof(UdpProxyIp)));

	// 获取代理服务器返回端口
	wPort = ntohs(m_proxyudpans2->wServerPort);

	return true;
}

s5ClientTcpChannel::~s5ClientTcpChannel()
{
}
