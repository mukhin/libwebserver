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

#ifndef SOCKETS_SOCKETADDRESS_H__
#define SOCKETS_SOCKETADDRESS_H__

#include <base/basicmacros.h>
#include <inttypes.h>
#include <string>
#include <vector>

extern "C" {
#include <arpa/inet.h>
#include <netinet/in.h> // sockaddr_in
}


namespace sockets {

  // @interface

  // Records an IP address and port, which are 32 and 16 bit integers, respectively, both in host byte-order.
  class SocketAddress {
  public:
#pragma mark Constructors
    // Creates a missing / unknown address.
    SocketAddress();

    // Creates the address with the given host and port.  If use_dns is true,
    // the hostname will be immediately resolved to an IP (which may block for
    // several seconds if DNS is not available).  Alternately, set use_dns to
    // false, and then call Resolve() to complete resolution later, or use
    // SetResolvedIP to set the IP explictly.
    SocketAddress(const std::string& hostname, const uint16_t port = 0, bool use_dns = true);
    SocketAddress(const char* hostname, const uint16_t port = 0, bool use_dns = true);

    // Creates the address with the given IP and port.
    SocketAddress(const uint32_t ip, const uint16_t port);

    // Creates a copy of the given address.
    SocketAddress(const SocketAddress& addr);

#pragma mark -
#pragma mark Operators

    // Replaces our address with the given one.
    inline SocketAddress& operator=(const SocketAddress& addr);

    // Determines whether this address is identical to the given one.
    bool operator==(const SocketAddress& addr) const;

    bool operator!=(const SocketAddress& addr) const;

    // Compares based on IP and then port.
    bool operator<(const SocketAddress& addr) const;

#pragma mark -

    // Resets to missing / unknown address.
    inline void Clear();

    // Changes the IP of this address to the given one, and clears the hostname.
    void SetIP(const uint32_t ip);

    // Changes the hostname of this address to the given one.
    // Calls Resolve and returns the result.
    bool SetIP(const std::string& hostname, const bool use_dns = true);
    bool SetIP(const char* hostname, const bool use_dns = true);

    // Sets the IP address while retaining the hostname.  Useful for bypassing
    // DNS for a pre-resolved IP.
    void SetResolvedIP(const uint32_t ip);

    // Changes the port of this address to the given one.
    void SetPort(const uint16_t port);

    // Returns the IP address.
    uint32_t GetIp() const;

    // Returns the port part of this address.
    uint16_t GetPort() const;

    // Returns the hostname
    const std::string& GetHostname() const;

    // Returns the IP address in dotted form.
    std::string IPAsString() const;

    // Returns the port as a string
    std::string PortAsString() const;

    // Returns a display version of the IP/port.
    std::string ToString() const;

    // Determines whether this represents a missing / any address.
    bool IsAny() const;

    // Determines whether the IP address refers to the local host, i.e. within
    // the range 127.0.0.0/8.
    bool IsLocalIP() const;

    // Determines whether the IP address is in one of the private ranges:
    // 127.0.0.0/8 10.0.0.0/8 192.168.0.0/16 172.16.0.0/12.
    bool IsPrivateIP() const;

    // Determines whether the hostname has been resolved to an IP
    bool IsUnresolved() const;

    // Attempt to resolve a hostname to IP address.
    // Returns false if resolution is required but failed.
    // 'force' will cause re-resolution of hostname.
    bool Resolve(const bool force = false, const bool use_dns = true);

    // Determines whether this address has the same IP as the one given.
    bool EqualIPs(const SocketAddress& addr) const;

    // Deteremines whether this address has the same port as the one given.
    bool EqualPorts(const SocketAddress& addr) const;

    // Hashes this address into a small number.
    size_t Hash() const;

    // Convert to and from sockaddr_in
    void ToSockAddr(sockaddr_in* saddr) const;
    void FromSockAddr(const sockaddr_in& saddr);

    // Converts the IP address given in compact form into dotted form.
    static std::string IPToString(const uint32_t ip);

    // Converts the IP address given in dotted form into compact form.
    // Without 'use_dns', only dotted names (A.B.C.D) are resolved.
    static uint32_t StringToIP(const std::string& str, const bool use_dns = true);

    // Get local machine's hostname
    static std::string GetLocalHostname();

    // Get a list of the local machine's ip addresses
    static bool GetLocalIPs(std::vector<uint32_t>& ips);

  private:
    std::string hostname_;
    uint32_t ip_;
    uint16_t port_;
  };

} // namespace sockets

#endif // SOCKETS_SOCKETADDRESS_H__
