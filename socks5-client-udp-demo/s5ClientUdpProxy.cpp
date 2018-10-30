#include "s5ClientUdpProxy.h"
#include "common.h"
#include <Ws2tcpip.h>
#include <iostream>

s5ClientUdpProxy::s5ClientUdpProxy(char *szProxyIP, WORD &wProxyPort)
{
	udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	// 绑定套接字
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(TEST_UDP_PORT);//10888
	::bind(udpSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR_IN));
	// 获取目标服务器IP
	//ULONG ulDestAddr = inet_addr(SS5_SERVER_IP);//10.211.55.12
	ULONG ulDestAddr;
	InetPton(AF_INET, SS5_SERVER_IP, &ulDestAddr);

	unsigned short pt = htons(TEST_UDP_PORT);//10888
	// UDP请求头
	udpHdr.addr = ulDestAddr;
	udpHdr.frag = 0;
	udpHdr.port = pt;
	udpHdr.rsv = FIELD_RSV;
	udpHdr.type = ATYP_IPV4;

	// 代理服务器地址
	ProxyServerAddr.sin_family = AF_INET;
	ProxyServerAddr.sin_port = htons(wProxyPort); //ss5为本次udp创建的Ip和端口
	//ProxyServerAddr.sin_addr.s_addr = inet_addr(szProxyIP);
	InetPton(AF_INET, szProxyIP, &ProxyServerAddr.sin_addr);
}

void s5ClientUdpProxy::sendUdpPackage()
{
	for (int i = 0; i < 2; i++)
	{
		char pMsg[MAX_PATH];
		sprintf_s(pMsg, "%d\t%s", i, "test\r\n");

		int n = strlen(pMsg);

		int  nSendLen = 10 + strlen(pMsg) + 1;
		char *p = new char[nSendLen];

		memcpy(p, (char*)&udpHdr, 10);
		memcpy(p + 10, pMsg, strlen(pMsg) + 1);
		int addrlen = sizeof(ProxyServerAddr);
		int nRet = sendto(udpSocket, p, nSendLen, 0, (SOCKADDR*)&ProxyServerAddr, sizeof(SOCKADDR));
		memset(p, 0, nSendLen);
		//int rRet = recvfrom(udpSocket, p, nSendLen, 0, (SOCKADDR*)&ProxyServerAddr, &addrlen);

		//GetDlgItem(IDC_EDIT4)->SetWindowText(CString(p));
		delete[]p;

		Sleep(1000);
	}
	closesocket(udpSocket);
}

s5ClientUdpProxy::~s5ClientUdpProxy()
{
}
