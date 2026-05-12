#include "RepairCenter.h"
#include "RepairMessage_m.h"

// Register the module with the OMNeT++ engine
Define_Module(RepairCenter);

// -------------------------------------------------------------------------------------------
// INITIALIZATION
// -------------------------------------------------------------------------------------------
void RepairCenter::initialize() {
    // Read the number of programmers from the .ini file configuration
    numProgrammers = par("numProgrammers");
    
    // Initial state: no one is working, no servers are broken
    busyProgrammers = 0;
    currentFailedServers = 0;

    // Register the statistics signal to track the number of failed servers over time.
    // 'registerSignal' returns a handle used for efficient 'emit' calls.
    failedServersSignal = registerSignal("failedServers");
    
    // Emit initial value (0) so the statistics start recording from simulation time 0.
    emit(failedServersSignal, currentFailedServers);
}

// -------------------------------------------------------------------------------------------
// MESSAGE HANDLING (Main Logic)
// -------------------------------------------------------------------------------------------
void RepairCenter::handleMessage(cMessage *msg) {
    
    // CASE A: SELF-MESSAGE (A previously scheduled repair is now finished)
    if (msg->isSelfMessage()) {
        
        // 1. Release the resource (one programmer becomes idle)
        busyProgrammers--;
        
        // 2. Retrieve the original RepairRequest from the context pointer.
        // We stored it earlier in startRepair() to remember which server was being fixed.
        RepairRequest *req = (RepairRequest *)msg->getContextPointer();
        
        // 3. Notify the Server that it is working again.
        // We send a response back through the 'out' gate at the index matching the server ID.
        cMessage *response = new cMessage("repairResp");
        send(response, "out", req->getServerId());
        
        // 4. Update and emit statistics
        currentFailedServers--;
        emit(failedServersSignal, currentFailedServers);
        
        // 5. Memory Cleanup: Delete both the request and the finished timer message.
        delete req;
        delete msg;

        // 6. Check the Queue: If other servers are waiting, take the next one immediately.
        if (!queue.isEmpty()) {
            // Pop the first message from the FIFO queue and start working on it.
            RepairRequest *nextReq = (RepairRequest *)queue.pop();
            startRepair(nextReq);
        }
    } 
    
    // CASE B: INCOMING MESSAGE (A new server has just failed)
    else {
        // 1. Update the total count of failed servers (including those in queue).
        currentFailedServers++;
        emit(failedServersSignal, currentFailedServers);
        
        // 2. Resource check: Do we have a free programmer?
        if (busyProgrammers < numProgrammers) {
            // Yes, start the repair process immediately.
            startRepair(msg);
        } else {
            // No, all programmers are busy. Put the request into the waiting line (FIFO).
            queue.insert(msg);
        }
    }
}

// -------------------------------------------------------------------------------------------
// HELPER: START REPAIR PROCESS
// -------------------------------------------------------------------------------------------
void RepairCenter::startRepair(cMessage *reqMsg) {
    // Occupy a programmer
    busyProgrammers++;
    
    // Safely cast the generic cMessage to our custom RepairRequest type
    RepairRequest *req = check_and_cast<RepairRequest*>(reqMsg);
    
    // Create a new self-message that will represent the "End of Repair" event.
    cMessage *doneMsg = new cMessage("repairDone");
    
    // Store the request pointer inside the 'doneMsg'. 
    // This allows us to "remember" the server ID and repair data when the timer expires.
    doneMsg->setContextPointer(req);
    
    // Schedule the completion of the repair in the future.
    // The repair duration was calculated by the server when it sent the request.
    scheduleAt(simTime() + req->getRepairDuration(), doneMsg);
}
