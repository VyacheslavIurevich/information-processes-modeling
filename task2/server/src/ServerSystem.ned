package src;

network ServerSystem {
    submodules:
        server[5]: Server;
        repairCenter: RepairCenter;
    connections:
        for i=0..4 {
            server[i].out --> repairCenter.in++;
            server[i].in <-- repairCenter.out++;
        }
}
