#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "s5ClientTcpChannel.h"
#include "s5ClientUdpProxy.h"

int main()
{
	s5ClientTcpChannel s5ClientTcpSocket;
	if (!s5ClientTcpSocket.QequestCertificate())
	{
		std::cout << "s5ClientTcpSocket.QequestCertificate() ie error!" << std::endl;
		return -1;
	}

	char szProxyIP[16]; //ss5 server 生成的udp代理ip
	memset(szProxyIP, 0, sizeof(szProxyIP));
	WORD wProxyPort;  //ss5 server生成的udp代理端口
	if (!s5ClientTcpSocket.GetUdpProxyInfo(szProxyIP, wProxyPort))
	{
		std::cout << "GetUdpProxyInfo error! " << std::endl;
	}
	else {
		std::cout << szProxyIP << std::endl;
		std::cout << wProxyPort << std::endl;
	}

	s5ClientUdpProxy s5ClientUdp(szProxyIP, wProxyPort);
	s5ClientUdp.sendUdpPackage();

	system("pause");
}