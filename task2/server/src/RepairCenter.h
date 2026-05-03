#ifndef __REPAIRCENTER_H_
#define __REPAIRCENTER_H_

#include <omnetpp.h>

using namespace omnetpp;

class RepairCenter : public cSimpleModule {
  private:
    int numProgrammers;
    int busyProgrammers;
    int currentFailedServers;
    cQueue queue;
    simsignal_t failedServersSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startRepair(cMessage *reqMsg);
};

#endif
