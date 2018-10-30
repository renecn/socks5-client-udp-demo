#pragma once

#include <winsock2.h>	

#define FIELD_RSV			0x00
#define ATYP_IPV4			0x01

#pragma pack(push)
#pragma pack(1)
// udp头
struct sock5udpheader
{
	WORD	rsv;  // 保留 
	BYTE	frag; // 分段标志，如果不分段则为0 
	BYTE	type; // 
	ULONG  addr; // 目标地址
	WORD	port; // 目标端口
};
#pragma pack(pop)

class s5ClientUdpProxy
{
public:
	s5ClientUdpProxy(char *ip, WORD &wPort);
	void sendUdpPackage();
	~s5ClientUdpProxy();
private:
	SOCKET udpSocket; 
	sockaddr_in ProxyServerAddr;
	sock5udpheader   udpHdr;
};

