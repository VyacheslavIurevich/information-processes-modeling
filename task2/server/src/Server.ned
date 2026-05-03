package src;

simple Server {
    parameters:
        double attackMean @unit(h);
    gates:
        input in;
        output out;
}