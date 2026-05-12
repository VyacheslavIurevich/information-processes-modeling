// -----------------------------------------------------------------------------
// MESSAGE DEFINITION: RepairRequest
// -----------------------------------------------------------------------------
// This file defines the custom message structure used in the simulation.
// OMNeT++ will automatically generate a C++ class (RepairMessage_m.h/cc) 
// from this definition, providing methods like getServerId() and setServerId().

message RepairRequest {
    
    // Identifies which server is currently under repair.
    // This allows the RepairCenter to know exactly where to send the 
    // "repair complete" notification once the work is finished.
    int serverId;

    // Stores the calculated duration for this specific repair event.
    // We use 'simtime_t', which is the native OMNeT++ type for simulation 
    // time, ensuring high precision and compatibility with the 
    // simulation engine's clock.
    simtime_t repairDuration;
}
