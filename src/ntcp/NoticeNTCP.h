#ifndef NKNTCP_H
#define NKNTCP_H
//#define NKC

#define MAX_TCP_CONNECT         5       /* time in secs to get a connection */
#define MAX_TCP_READ            3       /* time in secs to wait for the DSO
                                           to respond to a read request */
#define BOOL                    int
#define TRUE                    1
#define FALSE                   0

class NKNTCP {
 public:
    NKNTCP();
    virtual ~NKNTCP();

    // open TCP socket
    int tcpOpen(char *host, int port);
    // close TCP socket
    int tcpClose(int tcp_Handle);
    // write to TCP socket
    int tcpWrite(int tcp_Handle, char *buf,int len);
    // read from TCP socket
    int tcpRead(int tcp_Handle, char *buf, int len);
    // write a byte to module
    int NTCPwriteByte(int tcp_Handle,int addr, int data);
    // write a short word(2byte) to module
    int NTCPwriteShort(int tcp_Handle,int addr, int data);
    // write a long word(4byte) to module
    int NTCPwriteLong(int tcp_Handle,int addr, int data);
    // read byte data from module(count = number of data)
    int NTCPreadByte(int tcp_Handle,int addr, int count, int *data);
    // read short word(2byte) data from module(count = number of data)
    int NTCPreadShort(int tcp_Handle,int addr, int count, int *data);
    // read long word(4byte) data from module(count = number of data)
    int NTCPreadLong(int tcp_Handle,int addr, int count, int *data);

#ifdef NKROOT
    ClassDef(NKNTCP,2)
#endif
	};
#endif
