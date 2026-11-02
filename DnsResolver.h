#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

const char *googleDnsServer = "8.8.8.8";
const int googleDnsPort = 53;
const int RecivePort = 10000;

typedef struct {
  unsigned short Id;
  // flags
  unsigned char QR : 1;
  unsigned char Opcode : 4;
  unsigned char AA : 1;
  unsigned char TC : 1;
  unsigned char RD : 1;
  unsigned char RA : 1;
  unsigned char Z : 3;
  unsigned char Rcode : 4;

  unsigned short Que_Count;
  unsigned short Ans_Count;
  unsigned short Auth_count;
  unsigned short Add_count;

} Dns_header_Req;

typedef struct {
  unsigned short Id;
  // flags
  unsigned char Rcode : 4;
  unsigned char Z : 3;
  unsigned char RA : 1;
  unsigned char RD : 1;
  unsigned char TC : 1;
  unsigned char AA : 1;
  unsigned char Opcode : 4;
  unsigned char QR : 1;

  unsigned short Que_Count;
  unsigned short Ans_Count;
  unsigned short Auth_count;
  unsigned short Add_count;

} Dns_header_Recv;

typedef struct {
  unsigned short QueryType;
  unsigned short QueryClass;
} QueryStruct;

// for DNS request
Dns_header_Req DnsReqHeader;
QueryStruct queryStruct;
char req_question[256];
int req_question_size;

// for recving DNS packet
Dns_header_Recv DnsHeaderRecv;
char recv_question[256];
QueryStruct RecvQueryStruct;
short recv_name;
short recv_type;
short recv_class;
int recv_ttl;
short recv_RDLength;
int recv_question_size;
int recv_ans[20]; // as ip address is 4 bytesssss and this can also caled rdata
int size_recv_ans;

char DnsPacket[512];
int PacketSize;

struct sockaddr_in sendingTo;

struct sockaddr_in ReciveTo;
