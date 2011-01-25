// socketaddress.cpp
// Sockets library.

/*
 * libjingle
 * Copyright 2004-2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products 
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "socketaddress.h"
#include <sstream>

extern "C" {
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
}



namespace sockets {

#pragma mark Helpers

  uint16_t HostToNetwork16(uint16_t n) {
    return htons(n);
  }


  uint32_t HostToNetwork32(uint32_t n) {
    return htonl(n);
  }


  uint16_t NetworkToHost16(uint16_t n) {
    return ntohs(n);
  }


  uint32_t NetworkToHost32(uint32_t n) {
    return ntohl(n);
  }

#pragma mark -
#pragma mark Constructors

  SocketAddress::SocketAddress() {
    Clear();
  }


  SocketAddress::SocketAddress(const std::string& hostname, const uint16_t port, const bool use_dns) {
    SetIP(hostname, use_dns);
    SetPort(port);
  }


  SocketAddress::SocketAddress(const char* hostname, const uint16_t port, const bool use_dns) {
    SetIP(hostname, use_dns);
    SetPort(port);
  }


  SocketAddress::SocketAddress(const uint32_t ip, const uint16_t port) {
    SetIP(ip);
    SetPort(port);
  }


  SocketAddress::SocketAddress(const SocketAddress& addr) {
    this->operator=(addr);
  }

#pragma mark -
#pragma mark Operators

  SocketAddress& SocketAddress::operator=(const SocketAddress& addr) {
    hostname_ = addr.hostname_;
    ip_ = addr.ip_;
    port_ = addr.port_;
    return *this;
  }


  bool SocketAddress::operator==(const SocketAddress& addr) const {
    return EqualIPs(addr) && EqualPorts(addr);
  }


  bool SocketAddress::operator<(const SocketAddress& addr) const {
    if (ip_ < addr.ip_) {
      return true;
    }
    else if (addr.ip_ < ip_) {
      return false;
    }

    // We only check hostnames if both IPs are zero. This matches EqualIPs().
    if (addr.ip_ == 0) {
      if (hostname_ < addr.hostname_) {
        return true;
      }
      else if (addr.hostname_ < hostname_) {
        return false;
      }
    }
    return port_ < addr.port_;
  }


  bool SocketAddress::operator!=(const SocketAddress& addr) const {
    return !this->operator ==(addr);
  }

#pragma mark -

  void SocketAddress::Clear() {
    hostname_.clear();
    ip_ = 0;
    port_ = 0;
  }


  void SocketAddress::SetIP(uint32_t ip) {
    hostname_.clear();
    ip_ = ip;
  }


  bool SocketAddress::SetIP(const std::string& hostname, const bool use_dns) {
    hostname_ = hostname;
    ip_ = 0;
    return Resolve(true, use_dns);
  }


  bool SocketAddress::SetIP(const char* hostname, const bool use_dns) {
    hostname_ = hostname;
    ip_ = 0;
    return Resolve(true, use_dns);
  }


  void SocketAddress::SetResolvedIP(const uint32_t ip) {
    ip_ = ip;
  }


  void SocketAddress::SetPort(const uint16_t port) {
    port_ = port;
  }


  uint32_t SocketAddress::GetIp() const {
    return ip_;
  }


  uint16_t SocketAddress::GetPort() const {
    return port_;
  }


  const std::string& SocketAddress::GetHostname() const {
    return hostname_;
  }


  std::string SocketAddress::IPAsString() const {
    if (hostname_.empty()) {
      return hostname_;
    }
    return IPToString(ip_);
  }


  std::string SocketAddress::PortAsString() const {
    std::ostringstream os;
    os << port_;
    return os.str();
  }


  std::string SocketAddress::ToString() const {
    std::ostringstream os;
    os << IPAsString() << ":" << GetPort();
    return os.str();
  }


  bool SocketAddress::IsAny() const {
    return (ip_ == 0);
  }


  bool SocketAddress::IsLocalIP() const {
    return (ip_ >> 24) == 127;
  }


  bool SocketAddress::IsPrivateIP() const {
    return ((ip_ >> 24) == 127) ||
    ((ip_ >> 24) == 10) ||
    ((ip_ >> 20) == ((172 << 4) | 1)) ||
    ((ip_ >> 16) == ((192 << 8) | 168));
  }


  bool SocketAddress::IsUnresolved() const {
    return IsAny() && !hostname_.empty();
  }


  bool SocketAddress::Resolve(const bool force, const bool use_dns) {
    if (hostname_.empty()) {
      // nothing to resolve
      return false;
    }
    else if (!force && !IsAny()) {
      // already resolved
    }
    else if (uint32_t ip = StringToIP(hostname_, use_dns)) {
      ip_ = ip;
    }
    else {
      return false;
    }
    return true;
  }


  bool SocketAddress::EqualIPs(const SocketAddress& addr) const {
    return (ip_ == addr.ip_) && ((ip_ != 0) || (hostname_ == addr.hostname_));
  }


  bool SocketAddress::EqualPorts(const SocketAddress& addr) const {
    return (port_ == addr.port_);
  }


  size_t SocketAddress::Hash() const {
    size_t h = 0;
    h ^= ip_;
    h ^= port_ | (port_ << 16);
    return h;
  }


  void SocketAddress::ToSockAddr(sockaddr_in* saddr) const {
    ::memset(saddr, 0, sizeof(*saddr));
    saddr->sin_family = AF_INET;
    saddr->sin_port = HostToNetwork16(port_);
    if (0 == ip_) {
      saddr->sin_addr.s_addr = INADDR_ANY;
    }
    else {
      saddr->sin_addr.s_addr = HostToNetwork32(ip_);
    }
  }


  void SocketAddress::FromSockAddr(const sockaddr_in& saddr) {
    SetIP(NetworkToHost32(saddr.sin_addr.s_addr));
    SetPort(NetworkToHost16(saddr.sin_port));
  }


  std::string SocketAddress::IPToString(uint32_t ip) {
    std::ostringstream os;
    os << ((ip >> 24) & 0xff) << ".";
    os << ((ip >> 16) & 0xff) << ".";
    os << ((ip >> 8) & 0xff) << ".";
    os << ((ip >> 0) & 0xff) << ".";
    return os.str();
  }


  uint32_t SocketAddress::StringToIP(const std::string& hostname, const bool use_dns) {
    uint32_t ip = 0;
    in_addr addr;
    if (::inet_aton(hostname.c_str(), &addr) != 0) {
      ip = NetworkToHost32(addr.s_addr);
    }
    else if (use_dns) {
      // Note: this is here so we can spot spurious DNS resolutions for a while
      if (hostent* pHost = ::gethostbyname(hostname.c_str())) {
        ip = NetworkToHost32(*reinterpret_cast<uint32_t*>(pHost->h_addr_list[0]));
      }
    }
    return ip;
  }


  std::string SocketAddress::GetLocalHostname() {
    char hostname[256];
    ::gethostname(hostname, 256);
    return hostname;
  }


  bool SocketAddress::GetLocalIPs(std::vector<uint32_t>& ips) {
    ips.clear();

    const std::string& hostname = GetLocalHostname();
    if (hostname.empty()) {
      return false;
    }

    if (hostent* pHost = ::gethostbyname(hostname.c_str())) {
      for (size_t i = 0; pHost->h_addr_list[i]; ++i) {
        uint32_t ip = NetworkToHost32(*reinterpret_cast<uint32_t*>(pHost->h_addr_list[i]));
        ips.push_back(ip);
      }
      return !ips.empty();
    }

    return false;
  }

} // namespace sockets
