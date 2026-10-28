#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

const char *googleDnsServer = "8.8.8.8";
const int googleDnsPort = 53;
const int RecivePort=10000;


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

} Dns_header_Que_query;

char question[256];
int question_size;

typedef struct {
  unsigned short QueryType;
  unsigned short QueryClass;
} QueryStruct;

char DnsPacket[512];
int PacketSize;

struct sockaddr_in sendingTo;

struct sockaddr_in ReciveTo;

