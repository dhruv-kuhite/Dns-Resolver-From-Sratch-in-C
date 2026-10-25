#include "DnsResolver.h"
#include <stdio.h>
#include <string.h>

char DnsString[256] = "www.google.com";

Dns_header_Que_query DnsReqHeader;

void setDnsReqHeader() {

  memset(&DnsReqHeader, 0, sizeof(Dns_header_Que_query));
  DnsReqHeader.Id = htons(50);
  DnsReqHeader.RD = 1;
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
      printf("%d ", DnsString[i]);
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

  for (int i = 0; i <= Qindex; i++) {
    printf("%d ", question[i]);
  }
}

void SetQuery() {}

int main() {

  FormatDnsLink();



  return 0;
}