package src;

// -------------------------------------------------------------------------
// Server
// -------------------------------------------------------------------------
// This module represents a physical server in the computing system.
// It independently generates attack events and manages its own state.
simple Server {
    
    parameters:
        // -----------------------------------------------------------------
        // OPERATIONAL PARAMETERS
        // -----------------------------------------------------------------
        
        // The average time between incoming attacks.
        // We use the '@unit(h)' metadata to specify that this value is 
        // measured in hours. OMNeT++ will automatically handle time 
        // conversions if other units are used in the .ini file.
        double attackMean @unit(h);

    gates:
        // -----------------------------------------------------------------
        // COMMUNICATION PORTS (GATES)
        // -----------------------------------------------------------------
        
        // Input gate to receive messages. 
        // In this model, it receives "Repair Finished" notifications 
        // from the RepairCenter.
        input in;
        
        // Output gate to send messages.
        // Used to transmit "RepairRequest" messages to the RepairCenter 
        // when an attack successfully disables the server.
        output out;
}
