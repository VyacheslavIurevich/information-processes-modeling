package src;

// -------------------------------------------------------------------------
// RepairCenter
// -------------------------------------------------------------------------
// This module acts as a task dispatcher and resource manager. 
// It tracks the number of busy programmers, manages a queue of broken 
// servers, and records system-wide statistics.
simple RepairCenter {
    
    parameters:
        // -------------------------------------------------------------------------------
        // RESOURCE PARAMETERS
        // -------------------------------------------------------------------------------
        
        // Total number of system programmers available for work.
        // Each programmer can handle only one server at a time.
        int numProgrammers;

        // -------------------------------------------------------------------------------
        // STATISTICS AND SIGNALS
        // -------------------------------------------------------------------------------
        
        // A 'signal' is an internal event trigger. We emit a value to this 
        // signal every time the number of failed servers changes.
        @signal[failedServers](type="long");

        // The 'statistic' defines how the recorded signal values are processed.
        // - title: Label used in result analysis tools.
        // - record=timeavg: Calculates the time-weighted average (crucial for our goal).
        // - record=vector: Saves every change over time to plot a graph later.
        // - interpolationmode=sample-hold: Needed for plotting.
        @statistic[failedServers](title="Failed Servers Count"; record=timeavg,vector; interpolationmode=sample-hold);

    gates:
        // -------------------------------------------------------------------------------
        // COMMUNICATION PORTS (DYNAMIC ARRAYS)
        // -------------------------------------------------------------------------------
        
        // Input gate array. The empty brackets '[]' mean the size is 
        // determined at runtime based on the number of connections.
        input in[];
        
        // Output gate array to send 'repair completed' messages back to servers.
        output out[];
}
