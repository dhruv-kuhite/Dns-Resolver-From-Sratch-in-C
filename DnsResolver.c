#include "DnsResolver.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

char DnsString[256] = "www.google.com";

Dns_header_Que_query DnsReqHeader;
QueryStruct queryStruct;

void setDnsReqHeader() {

  memset(&DnsReqHeader, 0, sizeof(Dns_header_Que_query));
  DnsReqHeader.Id = htons(50);
  DnsReqHeader.RD = htons(1);
  DnsReqHeader.Que_Count = htons(1);
}

void FormatDnsLink() {

  int count = 0;
  int Qindex = 0;
  int Sindex = 0;

  for (int i = 0; i < (strlen(DnsString) + 1); i++) {

    if (i > 0 && DnsString[i - 1] == '.') {
      Sindex = i;
    }

    if (DnsString[i] == '.' || DnsString[i] == '\0') {
      question[Qindex] = count;
      Qindex++;
      memcpy(&question[Qindex], &DnsString[Sindex], count);
      Qindex += count;
      count = 0;
      continue;
    }
    count++;
  }
  printf("\n");
  question[Qindex] = '\0';

  question_size = Qindex;

}

void SetQueryStruct() {

  queryStruct.QueryClass = htons(1);
  queryStruct.QueryType = htons(1);
}

void CombineToMakeDnsReq() {

  setDnsReqHeader();
  FormatDnsLink();
  SetQueryStruct();
  PacketSize = 0;
  memset(DnsPacket, 0, sizeof(DnsPacket));
  memcpy(&DnsPacket[PacketSize], &DnsReqHeader, sizeof(Dns_header_Que_query));

  PacketSize += sizeof(Dns_header_Que_query);
  memcpy(&DnsPacket[PacketSize], question, strlen(question));
  PacketSize += strlen(question) + 1; // end letter  should be 00 for it to work
                                      // so +1 to memcopy with null being there
  memcpy(&DnsPacket[PacketSize], &queryStruct, sizeof(QueryStruct));
  PacketSize += sizeof(QueryStruct);


}

void setReciveToAndBind(int socketFd) {

  ReciveTo.sin_addr.s_addr = INADDR_ANY;
  ReciveTo.sin_port = htons(RecivePort);
  ReciveTo.sin_family = AF_INET;

  int check = bind(socketFd, (struct sockaddr *)&ReciveTo, sizeof(ReciveTo));

  if (check != 0) {
    printf("Bind Failed");
  }
}

void SendDnsPacket(int socketFd) {
  sendingTo.sin_family = AF_INET;
  sendingTo.sin_port = htons(googleDnsPort);

  inet_pton(AF_INET, googleDnsServer, &sendingTo.sin_addr.s_addr);

  int check = sendto(socketFd, DnsPacket, PacketSize, 0,
                     (struct sockaddr *)&sendingTo, sizeof(sendingTo));

  if (check == -1) {
    printf("Error Sending Data\n");
  }
}

void RecivingDnsPacket(int socketFd) {
  char buffer[1024];
  int sizeReciveTo = sizeof(ReciveTo);
  int sizebuff = recvfrom(socketFd, buffer, sizeof(buffer), 0,
                          (struct sockaddr *)&ReciveTo, &sizeReciveTo);

  for (int i = 0; i < sizebuff; i++) {
    printf("%d ", buffer[i]);
  }
}

int main() {

  int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  CombineToMakeDnsReq();
  setReciveToAndBind(socketFd);
  SendDnsPacket(socketFd);
  RecivingDnsPacket(socketFd);
  return 0;
}