#pragma once

#include <omnetpp.h>

// Use the standard OMNeT++ namespace to avoid prefixing everything with
// 'omnetpp::'
using namespace omnetpp;

/**
 * Server Module Class
 *
 * This class implements the logic for a single server that can be attacked.
 * It inherits from cSimpleModule, which is the base class for all C++
 * simulation components in OMNeT++.
 */
class Server : public cSimpleModule {
private:
  // ------------------------------------------------------------------------------------
  // PRIVATE MEMBERS (State Variables)
  // ------------------------------------------------------------------------------------

  // A self-message pointer used to schedule the next attack.
  // In OMNeT++, events are triggered by sending messages to 'self'.
  cMessage *attackTimer = nullptr;

  // Current operational status of the server.
  // true meants that server is up and running.
  // false means that erver is currently down and awaiting or undergoing repair.
  bool isWorking = true;

  // The unique index of this server (0 to 4 in our case).
  // Stored locally for easy access when creating repair requests.
  int serverId;

protected:
  // ------------------------------------------------------------------------------------
  // LIFECYCLE METHODS
  // ------------------------------------------------------------------------------------

  /**
   * Called once at the beginning of the simulation to initialize variables and
   * schedule the very first event.
   */
  virtual void initialize() override;

  /**
   * The heart of the module. This method is called whenever a message
   * (like an attack timer or a repair confirmation) arrives at this module.
   */
  virtual void handleMessage(cMessage *msg) override;
};
