#include "CPN.h"
using namespace std;

NUM_t placecount;
NUM_t transitioncount;
NUM_t varcount;
CPN *cpnet;

int main() {
    CPN cpnet;
    char filename[] = "model4.pnml";
    cpnet.getSize(filename);
    cpnet.printSort();
    cpnet.readPNML(filename);
    cpnet.printVar();
    cpnet.printCPN();
    return 0;
}