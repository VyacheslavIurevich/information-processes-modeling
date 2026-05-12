#pragma once

#include <omnetpp.h>

// Use the standard OMNeT++ namespace to avoid prefixing everything with
// 'omnetpp::'
using namespace omnetpp;

/**
 * RepairCenter Module Class
 *
 * This module acts as a resource manager (Queue + Server system).
 * It manages a pool of programmers and handles incoming repair requests
 * from multiple servers using a FIFO queue.
 */
class RepairCenter : public cSimpleModule {
private:
  // --------------------------------------------------------------------------------------
  // RESOURCE AND STATE VARIABLES
  // --------------------------------------------------------------------------------------

  // Total capacity: how many programmers do work in the center (from .ini)
  int numProgrammers;

  // Current load: how many programmers are currently busy with a server
  int busyProgrammers;

  // Statistics tracker: total number of servers currently in the center
  // (either being repaired or waiting in the queue).
  int currentFailedServers;

  // The waiting line: A built-in OMNeT++ container that stores messages
  // (RepairRequests) when all programmers are busy. It automatically handles
  // FIFO logic.
  cQueue queue;

  // --------------------------------------------------------------------------------------
  // STATISTICS SIGNALS
  // --------------------------------------------------------------------------------------

  // A numeric handle for the signal defined in the .ned file.
  // Used to "publish" updates about the failed server count to the statistics
  // engine.
  simsignal_t failedServersSignal;

protected:
  // -------------------------------------------------------------------------------------
  // LIFECYCLE AND LOGIC METHODS
  // -------------------------------------------------------------------------------------

  /**
   * Initializes the center: sets programmers to idle, clears the queue,
   * and registers the statistics signal.
   */
  virtual void initialize() override;

  /**
   * Central dispatcher:
   * 1. If it's a new RepairRequest from a server -> starts repair or queues it.
   * 2. If it's a self-message -> finishes a repair and checks the queue for
   * next.
   */
  virtual void handleMessage(cMessage *msg) override;

  /**
   * Helper method to assign a programmer to a server and schedule
   * the repair completion time.
   */
  void startRepair(cMessage *reqMsg);
};
