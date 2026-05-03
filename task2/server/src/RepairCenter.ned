package src;

simple RepairCenter {
    parameters:
        int numProgrammers;
        @signal[failedServers](type="long");
        @statistic[failedServers](title="Failed Servers Count"; record=timeavg,vector; interpolationmode=sample-hold);
    gates:
        input in[];
        output out[];
}
