/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "face.hpp"
#include "core/logger.hpp"

namespace nfd {

Face::Face(const FaceUri& remoteUri, const FaceUri& localUri, bool isLocal)
  : m_id(INVALID_FACEID)
  , m_isLocal(isLocal)
  , m_remoteUri(remoteUri)
  , m_localUri(localUri)
  , m_isOnDemand(false)
  , m_isFailed(false)
  , m_metric(0)
{
  onReceiveInterest += [this](const ndn::Interest&) { ++m_counters.getNInInterests(); };
  onReceiveData     += [this](const ndn::Data&) {     ++m_counters.getNInDatas(); };
  onSendInterest    += [this](const ndn::Interest&) { ++m_counters.getNOutInterests(); };
  onSendData        += [this](const ndn::Data&) {     ++m_counters.getNOutDatas();  /*substractBytes(1024);*/ 
                                                      substractUtilization(1024);
                                                };
  
  setTotalBytes(1024);
  m_bytes = 0;
  setUtilization(0);
}

Face::~Face()
{
}

FaceId
Face::getId() const
{
  return m_id;
}

// this method is private and should be used only by the FaceTable
void
Face::setId(FaceId faceId)
{
  m_id = faceId;
}

void
Face::setDescription(const std::string& description)
{
  m_description = description;
}

const std::string&
Face::getDescription() const
{
  return m_description;
}

bool
Face::isMultiAccess() const
{
  return false;
}

bool
Face::isUp() const
{
  return true;
}

long long int
Face::getBytes()
{
  return m_bytes;
}

void
Face::addBytes(long long int bs)
{
  m_bytes = m_bytes + bs;
  // std::cout << getId() << "add" << std::endl;
}

void
Face::substractBytes(long long int bs)
{
  m_bytes = m_bytes - bs;
  // std::cout << getId() << "sub" << std::endl;
}

void
Face::addTotalBytes(long long int bs)
{
  m_totalBytes = m_totalBytes + bs;
}

long long int
Face::getTotalBytes()
{
  return m_totalBytes;
}

void
Face::setTotalBytes(long long int bs)
{
  m_totalBytes = bs;
}

void
Face::addUtilization(long long int bs)
{
  m_utilization += bs;
}

void
Face::substractUtilization(long long int bs)
{
  m_utilization -= bs;
}

long long int
Face::getUtilization()
{
  return m_utilization;
}

void
Face::setUtilization(long long int bs)
{
  m_utilization = bs;
}

bool
Face::decodeAndDispatchInput(const Block& element)
{
  try {
    /// \todo Ensure lazy field decoding process

    if (element.type() == tlv::Interest)
      {
        shared_ptr<Interest> i = make_shared<Interest>();
        i->wireDecode(element);
        this->onReceiveInterest(*i);
      }
    else if (element.type() == tlv::Data)
      {
        shared_ptr<Data> d = make_shared<Data>();
        d->wireDecode(element);
        this->onReceiveData(*d);
      }
    else
      return false;

    return true;
  }
  catch (tlv::Error&) {
    return false;
  }
}

void
Face::fail(const std::string& reason)
{
  if (m_isFailed) {
    return;
  }

  m_isFailed = true;
  this->onFail(reason);

  this->onFail.clear();
}

template<typename FaceTraits>
void
Face::copyStatusTo(FaceTraits& traits) const
{
  traits.setFaceId(getId())
    .setRemoteUri(getRemoteUri().toString())
    .setLocalUri(getLocalUri().toString());

  if (isLocal()) {
    traits.setFaceScope(ndn::nfd::FACE_SCOPE_LOCAL);
  }
  else {
    traits.setFaceScope(ndn::nfd::FACE_SCOPE_NON_LOCAL);
  }

  if (isOnDemand()) {
    traits.setFacePersistency(ndn::nfd::FACE_PERSISTENCY_ON_DEMAND);
  }
  else {
    traits.setFacePersistency(ndn::nfd::FACE_PERSISTENCY_PERSISTENT);
  }
}

template void
Face::copyStatusTo<ndn::nfd::FaceStatus>(ndn::nfd::FaceStatus&) const;

template void
Face::copyStatusTo<ndn::nfd::FaceEventNotification>(ndn::nfd::FaceEventNotification&) const;

ndn::nfd::FaceStatus
Face::getFaceStatus() const
{
  ndn::nfd::FaceStatus status;
  copyStatusTo(status);

  this->getCounters().copyTo(status);

  return status;
}

} //namespace nfd
