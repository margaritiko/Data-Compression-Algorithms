// Konnova Margarita <BSE184>
#include <iostream>
#include "experiments/Experimenter.cpp"


int main() {
    Experimenter* experimenter = new Experimenter();

    /// Measuring characters' frequencies for each file
//    experimenter->measureFilesPrintResult("experiments/output/frequency.csv");

    /// Measuring entropy for each file, timings and size comparison for each algorithm
    experimenter->overallTesting("experiments/output/overall.csv");


    return 0;
}