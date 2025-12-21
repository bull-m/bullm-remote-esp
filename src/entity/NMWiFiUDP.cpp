/*
  重写WiFiUDP类
*/

#include "entity/NMWiFiUDP.h"
#include <new>  //std::nothrow
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <errno.h>
#include "main.h"

#undef write
#undef read


NMWiFiUDP::NMWiFiUDP()
: udp_server(-1)
, server_port(0)
, remote_port(0)
, tx_buffer(0)
, tx_buffer_len(0)
, rx_buffer(0)
{}

NMWiFiUDP::~NMWiFiUDP(){
   stop();
}

uint8_t NMWiFiUDP::begin(IPAddress address, uint16_t port){
  stop();

  server_port = port;

  tx_buffer = (char *)malloc(UDP_MAX_PACKET_SIZE);
  if(!tx_buffer){
    log_e("could not create tx buffer: %d", errno);
    return 0;
  }

  if ((udp_server=socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    log_e("could not create socket: %d", errno);
    return 0;
  }

  int yes = 1;
  if (setsockopt(udp_server,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
      log_e("could not set socket option: %d", errno);
      stop();
      return 0;
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(server_port);
  addr.sin_addr.s_addr = (in_addr_t)address;
  if(bind(udp_server , (struct sockaddr*)&addr, sizeof(addr)) == -1){
    log_e("could not bind socket: %d", errno);
    stop();
    return 0;
  }
  fcntl(udp_server, F_SETFL, O_NONBLOCK);
  return 1;
}

uint8_t NMWiFiUDP::begin(uint16_t p){
  return begin(IPAddress(INADDR_ANY), p);
}

uint8_t NMWiFiUDP::beginMulticast(IPAddress a, uint16_t p){
  if(begin(IPAddress(INADDR_ANY), p)){
    if(a != 0){
      struct ip_mreq mreq;
      mreq.imr_multiaddr.s_addr = (in_addr_t)a;
      mreq.imr_interface.s_addr = INADDR_ANY;
      if (setsockopt(udp_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
          log_e("could not join igmp: %d", errno);
          stop();
          return 0;
      }
      multicast_ip = a;
    }
    return 1;
  }
  return 0;
}

void NMWiFiUDP::stop(){
  if(tx_buffer){
    free(tx_buffer);
    tx_buffer = NULL;
  }
  tx_buffer_len = 0;
  if(rx_buffer){
    cbuf *b = rx_buffer;
    rx_buffer = NULL;
    delete b;
  }
  if(udp_server == -1)
    return;
  if(multicast_ip != 0){
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (in_addr_t)multicast_ip;
    mreq.imr_interface.s_addr = (in_addr_t)0;
    setsockopt(udp_server, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    multicast_ip = IPAddress(INADDR_ANY);
  }
  close(udp_server);
  udp_server = -1;
}

int NMWiFiUDP::beginMulticastPacket(){
  if(!server_port || multicast_ip == IPAddress(INADDR_ANY))
    return 0;
  remote_ip = multicast_ip;
  remote_port = server_port;
  return beginPacket();
}

int NMWiFiUDP::beginPacket(){
  if(!remote_port)
    return 0;

  // allocate tx_buffer if is necessary
  if(!tx_buffer){
    tx_buffer = (char *)malloc(UDP_MAX_PACKET_SIZE);
    if(!tx_buffer){
      log_e("could not create tx buffer: %d", errno);
      return 0;
    }
  }

  tx_buffer_len = 0;

  // check whereas socket is already open
  if (udp_server != -1)
    return 1;

  if ((udp_server=socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    log_e("could not create socket: %d", errno);
    return 0;
  }

  fcntl(udp_server, F_SETFL, O_NONBLOCK);

  return 1;
}

int NMWiFiUDP::beginPacket(IPAddress ip, uint16_t port){
  remote_ip = ip;
  remote_port = port;
  return beginPacket();
}

int NMWiFiUDP::beginPacket(const char *host, uint16_t port){
  struct hostent *server;
  server = gethostbyname(host);
  if (server == NULL){
    log_e("could not get host from dns: %d", errno);
    return 0;
  }
  return beginPacket(IPAddress((const uint8_t *)(server->h_addr_list[0])), port);
}

int NMWiFiUDP::endPacket(){
  struct sockaddr_in recipient;
  recipient.sin_addr.s_addr = (uint32_t)remote_ip;
  recipient.sin_family = AF_INET;
  recipient.sin_port = htons(remote_port);
  int sent = sendto(udp_server, tx_buffer, tx_buffer_len, 0, (struct sockaddr*) &recipient, sizeof(recipient));
  if(sent < 0){
    // log_e("could not send data: %d", errno);
    return 0;
  }
  return 1;
}

size_t NMWiFiUDP::write(uint8_t data){
  size_t ret = 1;
  if(tx_buffer_len == UDP_MAX_PACKET_SIZE){
    ret = endPacket();
    tx_buffer_len = 0;
  }
  tx_buffer[tx_buffer_len++] = data;
  return ret;
}

size_t NMWiFiUDP::write(const uint8_t *buffer, size_t size){
  size_t i;
  for(i=0;i<size;i++)
    write(buffer[i]);
  return i;
}

int NMWiFiUDP::parsePacket(){
  if(rx_buffer)
    return 0;
  struct sockaddr_in si_other;
  int slen = sizeof(si_other) , len;
  char *buf = (char *)malloc(UDP_MAX_PACKET_SIZE);
  if(!buf) {
    return 0;
  }
  if ((len = recvfrom(udp_server, buf, UDP_MAX_PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *) &si_other, (socklen_t *)&slen)) == -1){
    free(buf);
    if(errno == EWOULDBLOCK){
      return 0;
    }
    log_e("could not receive data: %d", errno);
    return 0;
  }
  remote_ip = IPAddress(si_other.sin_addr.s_addr);
  remote_port = ntohs(si_other.sin_port);
  if (len > 0) {
    rx_buffer = new(std::nothrow) cbuf(len);
    rx_buffer->write(buf, len);
  }
  free(buf);
  return len;
}

int NMWiFiUDP::available(){
  if(!rx_buffer) return 0;
  return rx_buffer->available();
}

int NMWiFiUDP::read(){
  if(!rx_buffer) return -1;
  int out = rx_buffer->read();
  if(!rx_buffer->available()){
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
  }
  return out;
}

int NMWiFiUDP::read(unsigned char* buffer, size_t len){
  return read((char *)buffer, len);
}

int NMWiFiUDP::read(char* buffer, size_t len){
  if(!rx_buffer) return 0;
  int out = rx_buffer->read(buffer, len);
  if(!rx_buffer->available()){
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
  }
  return out;
}

int NMWiFiUDP::peek(){
  if(!rx_buffer) return -1;
  return rx_buffer->peek();
}

void NMWiFiUDP::flush(){
  if(!rx_buffer) return;
  cbuf *b = rx_buffer;
  rx_buffer = 0;
  delete b;
}

IPAddress NMWiFiUDP::remoteIP(){
  return remote_ip;
}

uint16_t NMWiFiUDP::remotePort(){
  return remote_port;
}
