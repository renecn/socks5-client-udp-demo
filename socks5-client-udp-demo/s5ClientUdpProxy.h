#pragma once

#include <winsock2.h>	

#define FIELD_RSV			0x00
#define ATYP_IPV4			0x01

#pragma pack(push)
#pragma pack(1)
// udpͷ
struct sock5udpheader
{
	WORD	rsv;  // ���� 
	BYTE	frag; // �ֶα�־��������ֶ���Ϊ0 
	BYTE	type; // 
	ULONG  addr; // Ŀ���ַ
	WORD	port; // Ŀ��˿�
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

