#ifndef __SERVER_H_
#define __SERVER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Server : public cSimpleModule {
  private:
    cMessage *attackTimer = nullptr;
    bool isWorking = true;
    int serverId;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
