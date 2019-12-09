#include "CPN.h"
using namespace std;

int main() {
    CPN cpnet;
    char filename[] = "model3.pnml";
    cpnet.getSize(filename);
    cpnet.printSort();
    cpnet.readPNML(filename);
    cpnet.printCPN();
    return 0;
}