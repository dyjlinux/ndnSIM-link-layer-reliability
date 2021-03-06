/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ndn-header.hpp"

#include <iosfwd>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

namespace io = boost::iostreams;

namespace ns3 {
namespace ndn {

template<>
ns3::TypeId
PacketHeader<Interest>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::Interest")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<Interest>>()
    ;

  return tid;
}

template<>
ns3::TypeId
PacketHeader<Data>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::Data")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<Data>>()
    ;
  return tid;
}

template<>
ns3::TypeId
PacketHeader<LinkAck>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::LinkAck")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<LinkAck>>()
    ;
  return tid;
}

template<>
ns3::TypeId
PacketHeader<LinkEcho>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::LinkEcho")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<LinkEcho>>()
    ;
  return tid;
}

template<>
ns3::TypeId
PacketHeader<LinkReply>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::LinkReply")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<LinkReply>>()
    ;
  return tid;
}

template<>
ns3::TypeId
PacketHeader<LinkPbAck>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::LinkPbAck")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<LinkPbAck>>()
    ;
  return tid;
}

template<>
ns3::TypeId
PacketHeader<RepeatRequest>::GetTypeId()
{
  static ns3::TypeId tid =
    ns3::TypeId("ns3::ndn::RepeatRequest")
    .SetGroupName("Ndn")
    .SetParent<Header>()
    .AddConstructor<PacketHeader<RepeatRequest>>()
    ;
  return tid;
}

template<class Pkt>
TypeId
PacketHeader<Pkt>::GetInstanceTypeId(void) const
{
  return GetTypeId();
}

template<class Pkt>
PacketHeader<Pkt>::PacketHeader()
{
}

template<class Pkt>
PacketHeader<Pkt>::PacketHeader(const Pkt& packet)
  : m_packet(packet.shared_from_this())
{
}

template<class Pkt>
uint32_t
PacketHeader<Pkt>::GetSerializedSize(void) const
{
  return m_packet->wireEncode().size();
}

template<class Pkt>
void
PacketHeader<Pkt>::Serialize(ns3::Buffer::Iterator start) const
{
  start.Write(m_packet->wireEncode().wire(), m_packet->wireEncode().size());
}

class Ns3BufferIteratorSource : public io::source {
public:
  Ns3BufferIteratorSource(ns3::Buffer::Iterator& is)
    : m_is(is)
  {
  }

  std::streamsize
  read(char* buf, std::streamsize nMaxRead)
  {
    std::streamsize i = 0;
    for (; i < nMaxRead && !m_is.IsEnd(); ++i) {
      buf[i] = m_is.ReadU8();
    }
    if (i == 0) {
      return -1;
    }
    else {
      return i;
    }
  }

private:
  ns3::Buffer::Iterator& m_is;
};

template<class Pkt>
uint32_t
PacketHeader<Pkt>::Deserialize(ns3::Buffer::Iterator start)
{
  auto packet = make_shared<Pkt>();
  io::stream<Ns3BufferIteratorSource> is(start);
  packet->wireDecode(::ndn::Block::fromStream(is));
  m_packet = packet;
  return packet->wireEncode().size();
}

template<>
void
PacketHeader<Interest>::Print(std::ostream& os) const
{
  os << "I: " << *m_packet;
}

template<>
void
PacketHeader<Data>::Print(std::ostream& os) const
{
  os << "D: " << *m_packet;
}

template<>
void
PacketHeader<LinkAck>::Print(std::ostream& os) const
{
  os << "A: " << *m_packet;
}

template<>
void
PacketHeader<LinkEcho>::Print(std::ostream& os) const
{
  os << "E: " << *m_packet;
}

template<>
void
PacketHeader<LinkReply>::Print(std::ostream& os) const
{
  os << "R: " << *m_packet;
}

template<>
void
PacketHeader<LinkPbAck>::Print(std::ostream& os) const
{
  os << "P: " << *m_packet;
}

template<>
void
PacketHeader<RepeatRequest>::Print(std::ostream& os) const
{
  os << "R: " << *m_packet;
}

template<class Pkt>
shared_ptr<const Pkt>
PacketHeader<Pkt>::getPacket()
{
  return m_packet;
}

typedef PacketHeader<Interest> InterestHeader;
typedef PacketHeader<Data> DataHeader;
typedef PacketHeader<LinkAck> LinkAckHeader;
typedef PacketHeader<LinkEcho> LinkEchoHeader;
typedef PacketHeader<LinkReply> LinkReplyHeader;
typedef PacketHeader<LinkPbAck> LinkPbAckHeader;
typedef PacketHeader<RepeatRequest> RepeatRequestHeader;


NS_OBJECT_ENSURE_REGISTERED(InterestHeader);
NS_OBJECT_ENSURE_REGISTERED(DataHeader);
NS_OBJECT_ENSURE_REGISTERED(LinkAckHeader);
NS_OBJECT_ENSURE_REGISTERED(LinkEchoHeader);
NS_OBJECT_ENSURE_REGISTERED(LinkReplyHeader);
NS_OBJECT_ENSURE_REGISTERED(LinkPbAckHeader);
NS_OBJECT_ENSURE_REGISTERED(RepeatRequestHeader);

template class PacketHeader<Interest>;
template class PacketHeader<Data>;
template class PacketHeader<LinkAck>;
template class PacketHeader<LinkEcho>;
template class PacketHeader<LinkReply>;
template class PacketHeader<LinkPbAck>;
template class PacketHeader<RepeatRequest>;

} // namespace ndn
} // namespace ns3
