#include "Server.h"
#include "RepairMessage_m.h"

// -------------------------------------------------------------------------
// CONSTANTS & SIMULATION PARAMETERS
// -------------------------------------------------------------------------
const int SECONDS_IN_HOUR = 3600;

// Probabilities
const double PROB_ATTACK_SUCCESS = 0.5; // 50% of attacks are successful
const double PROB_PARTIAL_TOTAL = 0.2;  // 20% of attacks are partial

const double P_PARTIAL_ADJUSTED =
    PROB_PARTIAL_TOTAL / (1.0 - PROB_ATTACK_SUCCESS);

// Repair durations (in hours)
const double PARTIAL_REPAIR_MIN = 12.0;
const double PARTIAL_REPAIR_MAX = 20.0;
const double FULL_REPAIR_MIN = 24.0;
const double FULL_REPAIR_MAX = 40.0;

// -------------------------------------------------------------------------
// MODULE REGISTRATION
// -------------------------------------------------------------------------
// This macro registers the C++ class with the OMNeT++ simulation engine.
// Without this, the engine won't be able to instantiate the 'Server'
// module defined in the .ned file.
Define_Module(Server);

// -------------------------------------------------------------------------
// INITIALIZATION PHASE
// -------------------------------------------------------------------------
void Server::initialize() {
  // getIndex() retrieves the array index of this specific module instance
  // For server[0], serverId = 0; for server[1], serverId = 1, etc.
  serverId = getIndex();

  // Create the self-message that will act as a recurring alarm clock
  // for cyber attacks.
  attackTimer = new cMessage("attackTimer");

  // Retrieve the mean time between attacks from the module parameters.
  double mean = par("attackMean").doubleValue();

  // Schedule the very first attack.
  // The exponential() function provided by OMNeT++ is used for random
  // distribution. Since 'mean' is in hours, we multiply by SECONDS_IN_HOUR to
  // convert it to seconds (the native simulation time unit).
  scheduleAt(simTime() + exponential(mean * SECONDS_IN_HOUR), attackTimer);
}

// -------------------------------------------------------------------------
// EVENT PROCESSING LOOP
// -------------------------------------------------------------------------
void Server::handleMessage(cMessage *msg) {

  // =====================================================================
  // EVENT TYPE 1: AN ATTACK HAS OCCURRED
  // =====================================================================
  if (msg == attackTimer) {

    // Maintain the continuous cycle of attacks
    // As soon as one attack hits, we calculate and schedule the next one.
    double mean = par("attackMean").doubleValue();
    scheduleAt(simTime() + exponential(mean * SECONDS_IN_HOUR), attackTimer);

    // Process the attack effects
    // The server can only be damaged if it is currently working.
    if (isWorking) {

      // Generate a random number between 0.0 and 1.0 to simulate probability
      double p = uniform(0, 1);

      if (p >= PROB_ATTACK_SUCCESS) {

        // Server is successfully broken
        isWorking = false;

        // Create a repair request message to send to the RepairCenter
        RepairRequest *req = new RepairRequest("repairReq");
        req->setServerId(serverId);

        // Determine damage severity
        if (uniform(0, 1) < P_PARTIAL_ADJUSTED) {
          // Partial failure branch
          // uniform() generates a random duration between PARTIAL_REPAIR_MIN
          // and PARTIAL_REPAIR_MAX hours.
          req->setRepairDuration(
              uniform(PARTIAL_REPAIR_MIN, PARTIAL_REPAIR_MAX) *
              SECONDS_IN_HOUR);
        } else {
          // Full failure branch (Remaining 60% of successful attacks)
          // uniform() generates a random duration between FULL_REPAIR_MIN and
          // FULL_REPAIR_MAX hours.
          req->setRepairDuration(uniform(FULL_REPAIR_MIN, FULL_REPAIR_MAX) *
                                 SECONDS_IN_HOUR);
        }

        // Transmit the request to the programming staff
        send(req, "out");
      }
    }
  }
  // =====================================================================
  // EVENT TYPE 2: REPAIR IS COMPLETED
  // =====================================================================
  else {
    // We received a message from the RepairCenter indicating the job is done.
    isWorking = true; // Bring the server back online

    // CRITICAL: We must delete the message object to prevent memory leaks!
    // OMNeT++ does not have automatic garbage collection for messages.
    delete msg;
  }
}
