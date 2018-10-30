#pragma once

#include <winsock2.h>	
#define SOCKS_VER		    0x05

#define METHOD_AUTH_NO		0x00
#define METHOD_AUTH			0x02
#define METHOD_GSSAPI		0x01
#define METHOD_NO_ACCEPT	0xFF

#define SOCKS_AUTH_VER      0x01
#define SOCKS_AUTH_OK       0x00

#define CMD_CONNECT			0x01
#define CMD_BIND			0x02
#define CMD_UDP				0x03

#define FIELD_RSV			0x00

#define ATYP_IPV4			0x01
#define ATYP_DOMAINNAME		0x03
#define ATYP_IPV6			0x04

#define REP_SUCCESS			0x00
#define REP_FAIL_GENERAL	0x01
#define REP_CONNECT_DENY	0x02
#define REP_UNREACHABLE		0x03
#define REP_HOST_NOTFIND	0x04
#define REP_REFUSED			0x05
#define	REP_TTL_EXPIRED		0x06
#define REP_NOT_SUPPORT		0x07
#define REP_ATYP_NOTSUPPORT 0x08

#pragma pack(push)
#pragma pack(1)
struct sock5req1
{
	char Ver;
	char nMethods;
	char Methods[255];
};
struct sock5ans1
{
	char Ver;
	char Method;
};
struct socks5authreq
{
	char Ver;
	char other[1];
};
struct socks5authans
{
	char Ver;
	char Status;
};
// udp«Î«Û
struct sock5udpreq2
{
	char	Ver;
	char	Cmd;
	char	Rsv;
	char	Atyp;
	DWORD	dwReserve;
	WORD	wLocalPort;
};
// udpªÿ∏¥
struct sock5udpans2
{
	char	Ver;
	char	Rep;
	char	Rsv;
	char	Atyp;
	DWORD	dwServerIp;
	WORD	wServerPort;
};
#pragma pack(pop)

class s5ClientTcpChannel
{
public:
	s5ClientTcpChannel();
	BOOL QequestCertificate();
	BOOL GetUdpProxyInfo(char *ip, WORD &wPort);
	~s5ClientTcpChannel();
	SOCKET & getTcpSocket()
	{
		return sClient;
	}
private:
	SOCKET sClient;
};

