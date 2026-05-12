package src;

// -------------------------------------------------------------------------
// NETWORK TOPOLOGY: ServerSystem
// -------------------------------------------------------------------------
// This network represents the high-level architecture of the server system.
// It consists of multiple independent servers and a centralized repair
// facility that manages the staff.
network ServerSystem {
    parameters:
        int numServers = default(5);

    submodules:
        // Define an array of 5 server instances based on the Server module.
        // Each server runs its own independent attack generation cycle.
        server[numServers]: Server;
        
        // Define a single repair center instance.
        // This is a shared resource where all servers send repair requests.
        repairCenter: RepairCenter;

    connections:
        // Iterate through each server to establish bidirectional communication 
        // with the repair center.
        for i=0..numServers-1 {
            
            // Connection: Server to Repair Center.
            // Used to send 'RepairRequest' messages when a server fails.
            // The '++' operator automatically finds the next available gate index.
            server[i].out --> repairCenter.in++;
            
            // Connection: Repair Center back to Server.
            // Used to send confirmation messages once the repair is complete, 
            // signaling the server to resume its normal operation.
            server[i].in <-- repairCenter.out++;
        }
}
