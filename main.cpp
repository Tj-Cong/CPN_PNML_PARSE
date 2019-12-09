#include "CPN.h"
using namespace std;

int main() {
    CPN cpnet;
    char filename[] = "model1.pnml";
    cpnet.getSize(filename);
    cpnet.printSort();
    cpnet.readPNML(filename);
    return 0;
}