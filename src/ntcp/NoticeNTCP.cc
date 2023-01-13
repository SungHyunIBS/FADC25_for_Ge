#include <sys/time.h>
#include <string.h>
#include <unistd.h>   //for sleep command
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "NoticeNTCP.h"

#ifdef NKROOT
ClassImp(NKNTCP)
#endif

  // #ifndef NKC
NKNTCP::NKNTCP()
{
    printf("NKNTCP Initialization \n") ;
}

NKNTCP::~NKNTCP()
{
    printf("Leaving NKNTCP now \n");
}
//#endif
int NKNTCP::tcpOpen(char *host, int port){
    
  struct sockaddr_in	serv_addr;
  int tcp_Handle;
  const int disable = 1;
  /*
   * Fill in the structure "serv_addr" with the address of the
   * server that we want to connect with.
   */
        
  //bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(host);
  serv_addr.sin_port        = htons(port);
        
  /*
   * Open a TCP socket (an Internet stream socket).
   */

  if ( (tcp_Handle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("client: can't open stream socket\n");
    return -1;
  }

  /* turning off TCP NAGLE algorithm : if not, there is a delay 
     problem (up to 200ms) when packet size is small */ 
  setsockopt(tcp_Handle, IPPROTO_TCP,TCP_NODELAY,(char *) &disable, sizeof(disable)); 

  /*
   * Connect to the server.
   */ 
  int conn = connect(tcp_Handle, (struct sockaddr *) &serv_addr,sizeof(serv_addr));

  if ( conn< 0) {
    printf("client: can't connect to server\n");
    printf("ip %s , port %d \n",host,port);
    printf("error number is %d \n", conn);

    return -2;
  } 
  return (tcp_Handle);
}


/* #ifdef NKC */
/* int tcpClose(int tcp_Handle)  */
/* #else */
int NKNTCP::tcpClose(int tcp_Handle) 
	   //#endif
{
  close(tcp_Handle);

  return 0;
}


/* #ifdef NKC */
/* int tcpWrite(int tcp_Handle, char *buf,int len) */
/* #else */
int NKNTCP::tcpWrite(int tcp_Handle, char *buf,int len)
	   //	   #endif
{
  int result, bytes_more, bytes_xferd;
  char *idxPtr;

  //	BOOL eoi_flag	= TRUE;

  bytes_more = len;
  idxPtr = buf;
  bytes_xferd = 0;
  while (1) {
    /* then write the rest of the block */
    idxPtr = buf + bytes_xferd;
    result=write (tcp_Handle, (char *) idxPtr, bytes_more);

    if (result<0) {
      printf("Could not write the rest of the block successfully, returned: %d\n",bytes_more);
	
      return -1;
    }
    
    bytes_xferd += result;
    bytes_more -= result;
    
    if (bytes_more <= 0)
      break;
  }

  return 0;

}


/* #ifdef NKC */
/* int tcpRead(int tcp_Handle, char *buf, int len) */
/* #else */
int NKNTCP::tcpRead(int tcp_Handle, char *buf, int len)
	   //#endif
{
  int result, accum, space_left, bytes_more, buf_count;
  //int i;
  char *idxPtr;

  fd_set rfds;
  // struct timeval tval;

  // tval.tv_sec = MAX_TCP_READ;
  // tval.tv_usec = 0;

  FD_ZERO(&rfds);
  FD_SET(tcp_Handle, &rfds);

  if (buf==NULL)
    return -1;

  //memset(buf, 0, len);

  idxPtr = buf;

  buf_count = 0;
  space_left = len;
  while (1) {
    /* block here until data is received of timeout expires */
    /*
    //      result = select((tcp_Handle+1), &rfds, NULL, NULL, &tval);
    if (result < 0) {
    printf("Read timeout\n");
    return -1;
    }

    printf("Passed Timeout  \n");
    */
    
    /* read the block */
    accum = 0;
    while (1) {
      idxPtr = buf + (buf_count + accum);
      bytes_more = space_left;
      
      if ((result = read(tcp_Handle, (char *) idxPtr, (bytes_more>2048)?2048:bytes_more)) < 0) {
	printf("Unable to receive data from the server.\n");
	return -1;
      }
      
      accum += result;
      if ((accum + buf_count) >= len)
                                break;
      /* in case data is smaller than wanted on */
      if(result<bytes_more) {
	printf("wanted %d got %d \n",bytes_more,result);
	return accum+buf_count;
      }
    }
    
    buf_count += accum;
    space_left -= accum;

    if (space_left <= 0)
      break;
  }

  return buf_count;
}


// write a byte to module
/* #ifdef NKC */
/* int NTCPwriteByte(int tcp_Handle, int addr, int data) */
/* #else */
int  NKNTCP::NTCPwriteByte(int tcp_Handle,int addr, int data)
	    //#endif
{
  char tcpBuf[4];
  int value;
	
  tcpBuf[0] = data & 0xFF;
  tcpBuf[1] = 0;
  tcpBuf[2] = addr & 0xFF;
  tcpBuf[3] = (addr >> 8) & 0x7F;

  tcpWrite (tcp_Handle, tcpBuf, 4);

  tcpRead (tcp_Handle, tcpBuf, 1);

  value = tcpBuf[0] & 0xFF;

  return value;
}


// write a short word(2byte) to module
/* #ifdef NKC */
/* int NTCPwriteShort(int tcp_Handle,int addr, int data) */
/* #else */
int  NKNTCP::NTCPwriteShort(int tcp_Handle,int addr, int data)
	    //#endif
{
  char tcpBuf[4];
  int i;
  int saddr;
  int value;
  int tmp;
	
  value = 0;

  for (i = 0; i < 2; i++) {

    saddr = addr + 1 - i;
	
    tcpBuf[0] = (data >> (8 - 8 * i)) & 0xFF;
    tcpBuf[1] = 0;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = (saddr >> 8) & 0x7F;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, 1);

    tmp = tcpBuf[0] & 0xFF;
    value = value + (tmp << (8 - 8 * i));
  }

  return value;
}


// write a long word(4byte) to module
/* #ifdef NKC */
/* int NTCPwriteLong(int tcp_Handle,int addr, int data) */
/* #else */
int  NKNTCP::NTCPwriteLong(int tcp_Handle,int addr, int data)
	    //#endif
{
  char tcpBuf[4];
  int i;
  int saddr;
  int value;
  int tmp;
	
  value = 0;

  for (i = 0; i < 4; i++) {

    saddr = addr + 3 - i;
	
    tcpBuf[0] = (data >> (24 - 8 * i)) & 0xFF;
    tcpBuf[1] = 0;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = (saddr >> 8) & 0x7F;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, 1);

    tmp = tcpBuf[0] & 0xFF;
    value = value + (tmp << (24 - 8 * i));
  }

  return value;
}


// read byte data from module(count = number of data)
/* #ifdef NKC */
/* int NTCPreadByte(int tcp_Handle,int addr, int count, int *data) */
/* #else */
int NKNTCP::NTCPreadByte(int tcp_Handle,int addr, int count, int *data)
	   //#endif
{
  char tcpBuf[1024];
  int chunk;
  int rem;
  int i, j;
  int saddr;

  chunk = count / 1024;
  rem = count % 1024;
  
  saddr = addr;

  // repeat for # of chunk
  for (i = 0; i < chunk; i++) {
    tcpBuf[0] = 0;
    tcpBuf[1] = 4;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, 1024);
	
    for (j = 0; j < 1024; j++)
      data[1024 * i + j] = tcpBuf[j] & 0xFF;

    saddr = saddr + 1024;
  }

  // do it for remainings
  if (rem) {
    tcpBuf[0] = rem & 0xFF;
    tcpBuf[1] = (rem >> 8) & 0xFF;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, rem);
	
    for (j = 0; j < rem; j++)
      data[1024 * chunk + j] = tcpBuf[j] & 0xFF;
  }

  return 0;
}


// read short word(2byte) data from module(count = number of data)
/* #ifdef NKC */
/* int NTCPreadShort(int tcp_Handle,int addr, int count, int *data) */
/* #else */
int NKNTCP::NTCPreadShort(int tcp_Handle,int addr, int count, int *data)
	   //#endif
{
  char tcpBuf[1024];
  int chunk;
  int rem;
  int i, j;
  int saddr;
  int tmp;

  chunk = count / 512;
  rem = count % 512;
  
  saddr = addr;

  // repeat for # of chunk
  for (i = 0; i < chunk; i++) {
    tcpBuf[0] = 0;
    tcpBuf[1] = 4;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, 1024);
	
    for (j = 0; j < 512; j++) {
      tmp = tcpBuf[2 * j + 1] & 0xFF;
      data[512 * i + j] = tmp << 8;
      tmp = tcpBuf[2 * j] & 0xFF;
      data[512 * i + j] = data[512 * i + j] + tmp;
    }

    saddr = saddr + 1024;
  }

  // do it for remainings
  if (rem) {
    tcpBuf[0] = (rem << 1) & 0xFF;
    tcpBuf[1] = (rem >> 7) & 0xFF;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, rem * 2);
	
    for (j = 0; j < rem; j++) {
      tmp = tcpBuf[2 * j + 1] & 0xFF;
      data[512 * chunk + j] = tmp << 8;
      tmp = tcpBuf[2 * j] & 0xFF;
      data[512 * chunk + j] = data[512 * chunk + j] + tmp;
    }
  }

  return 0;
}


// read long word(4byte) data from module(count = number of data)
/* #ifdef NKC */
/* int NTCPreadLong(int tcp_Handle,int addr, int count, int *data) */
/* #else */
int NKNTCP::NTCPreadLong(int tcp_Handle,int addr, int count, int *data)
	   //#endif
{
  char tcpBuf[1024];
  int chunk;
  int rem;
  int i, j;
  int saddr;
  int tmp;

  chunk = count / 256;
  rem = count % 256;
  
  saddr = addr;

  // repeat for # of chunk
  for (i = 0; i < chunk; i++) {
    tcpBuf[0] = 0;
    tcpBuf[1] = 4;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, 1024);
	
    for (j = 0; j < 256; j++) {
      tmp = tcpBuf[4 * j + 3] & 0xFF;
      data[256 * i + j] = tmp << 24;
      tmp = tcpBuf[4 * j + 2] & 0xFF;
      data[256 * i + j] = data[256 * i + j] + (tmp << 16);
      tmp = tcpBuf[4 * j + 1] & 0xFF;
      data[256 * i + j] = data[256 * i + j] + (tmp << 8);
      tmp = tcpBuf[4 * j] & 0xFF;
      data[256 * i + j] = data[256 * i + j] + tmp;
    }

    saddr = saddr + 1024;
  }

  // do it for remainings
  if (rem) {
    tcpBuf[0] = (rem << 2) & 0xFF;
    tcpBuf[1] = (rem >> 6) & 0xFF;
    tcpBuf[2] = saddr & 0xFF;
    tcpBuf[3] = ((saddr >> 8) & 0x7F) | 0x80;

    tcpWrite (tcp_Handle, tcpBuf, 4);

    tcpRead (tcp_Handle, tcpBuf, rem * 4);
	
    for (j = 0; j < rem; j++) {
      tmp = tcpBuf[4 * j + 3] & 0xFF;
      data[256 * chunk + j] = tmp << 24;
      tmp = tcpBuf[4 * j + 2] & 0xFF;
      data[256 * chunk + j] = data[256 * chunk + j] + (tmp << 16);
      tmp = tcpBuf[4 * j + 1] & 0xFF;
      data[256 * chunk + j] = data[256 * chunk + j] + (tmp << 8);
      tmp = tcpBuf[4 * j] & 0xFF;
      data[256 * chunk + j] = data[256 * chunk + j] + tmp;
    }
  }

  return 0;
}






