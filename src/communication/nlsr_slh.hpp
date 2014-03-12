#ifndef NLSR_SLH_HPP
#define NLSR_SLH_HPP

#include <iostream>

#include <ndn-cpp-dev/face.hpp>
#include <nsync/sync-socket.h>
#include <ndn-cpp-dev/security/validator-null.hpp>
#include <ndn-cpp-dev/util/scheduler.hpp>

#include "nlsr_sm.hpp"

class KeyManager;

extern "C" {
#include <unistd.h>
}

using namespace ndn;
using namespace Sync;
using namespace std;

class interestManager;
class ConfParameter;

namespace nlsr
{
  class SyncLogicHandler
  {
  public:
    SyncLogicHandler(ndn::shared_ptr<boost::asio::io_service> ioService)
      : validator(new ndn::ValidatorNull())
      , syncFace(new ndn::Face(ioService))
    {}


    void createSyncSocket(Nlsr& pnlsr);
    void nsyncUpdateCallBack(const vector<MissingDataInfo> &v,
                             SyncSocket *socket, Nlsr& pnlsr );
    void nsyncRemoveCallBack(const string& prefix, Nlsr &pnlsr);
    void removeRouterFromSyncing(string& routerPrefix);
    void publishRoutingUpdate(SequencingManager& sm, string updatePrefix);
    void publishKeyUpdate(KeyManager& km);
    void publishIdentityUpdate(string identityName);
    void setSyncPrefix(string sp)
    {
      syncPrefix.clear();
      syncPrefix.set(sp);
    }
  private:
    void processUpdateFromSync(std::string updateName, uint64_t seqNo,
                               Nlsr& pnlsr);
    void processRoutingUpdateFromSync(std::string routerName, uint64_t seqNo,
                                      Nlsr& pnlsr);
    void processKeysUpdateFromSync(std::string certName, uint64_t seqNo,
                                   Nlsr& pnlsr);
    void publishSyncUpdate(string updatePrefix, uint64_t seqNo);
  private:
    ndn::shared_ptr<ndn::ValidatorNull> validator;
    ndn::shared_ptr<ndn::Face> syncFace;
    ndn::shared_ptr<SyncSocket> syncSocket;
    ndn::Name syncPrefix;
  };
}
#endif
