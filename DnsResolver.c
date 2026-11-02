#include "DnsResolver.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

char DnsString[256] = "www.google.com";

void setDnsReqHeader() {

  memset(&DnsReqHeader, 0, sizeof(Dns_header_Req));
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
      req_question[Qindex] = count;
      Qindex++;
      memcpy(&req_question[Qindex], &DnsString[Sindex], count);
      Qindex += count;
      count = 0;
      continue;
    }
    count++;
  }
  printf("\n");
  req_question[Qindex] = '\0';

  req_question_size = Qindex;
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
  memcpy(&DnsPacket[PacketSize], &DnsReqHeader, sizeof(Dns_header_Req));

  PacketSize += sizeof(Dns_header_Req);
  memcpy(&DnsPacket[PacketSize], req_question, strlen(req_question));
  PacketSize +=
      strlen(req_question) + 1; // end letter  should be 00 for it to work
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

  int n = 0;

  memcpy(&DnsHeaderRecv, &buffer[n], sizeof(DnsHeaderRecv));
  n += sizeof(DnsHeaderRecv);

  memcpy(recv_question, &buffer[n], strlen(req_question));
  n += strlen(req_question)+1; // to start from after null

  memcpy(&RecvQueryStruct, &buffer[n], sizeof(RecvQueryStruct));
  n += sizeof(RecvQueryStruct);

    memcpy(&recv_name, &buffer[n], sizeof(recv_name));
  n += sizeof(recv_name);

      memcpy(&recv_type, &buffer[n], sizeof(recv_type));
  n += sizeof(recv_type);

        memcpy(&recv_class, &buffer[n], sizeof(recv_class));
  n += sizeof(recv_class);

  memcpy(&recv_ttl, &buffer[n], sizeof(recv_ttl));
  n += sizeof(recv_ttl);

  memcpy(&recv_RDLength, &buffer[n], sizeof(recv_RDLength));
  n += sizeof(recv_RDLength);

  // convert Ans Count to little endian as we are on
  // little endian machine
  DnsHeaderRecv.Ans_Count = ntohs(DnsHeaderRecv.Ans_Count);

  memcpy(&recv_ans, &buffer[n], sizeof(int));

  size_recv_ans = DnsHeaderRecv.Ans_Count;

  DnsHeaderRecv.Id = ntohs(DnsHeaderRecv.Id);
}

int CheckForSuccess() {

  switch (DnsHeaderRecv.Rcode) {

  case 0:
    return 0;
    break;

  case 1:
    printf("Format Error");
    return -1;
    break;

  case 2:
    printf("Server Failure");
    return -1;
    break;

  case 3:
    printf("Non-Existent Domain");
    return -1;
    break;

  case 4:
    printf("Not Implemented");
    return -1;
    break;

  case 5:
    printf("Refused");
    return -1;
    break;
  }

  return 0;
}

void printTheIpAddress() {

  printf("Dns Lookup Succcessss : \n");

  for (int i = 0; i < size_recv_ans; i++) {
    char buffer[16];

    inet_ntop(AF_INET, &recv_ans[i], buffer, sizeof(buffer));
    printf("%d)Ip  : %s\n", i + 1, buffer);
  }
}

int main() {

  int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  CombineToMakeDnsReq();
  setReciveToAndBind(socketFd);
  SendDnsPacket(socketFd);
  RecivingDnsPacket(socketFd);
  if (CheckForSuccess() == -1) {
    return -1;
  }
  printTheIpAddress();
  return 0;
}