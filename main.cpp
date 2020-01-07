#include <sys/time.h>
#include "CPN_RG.h"
using namespace std;

NUM_t placecount;
NUM_t transitioncount;
NUM_t varcount;
CPN *cpnet;

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

int main() {
    double starttime, endtime;
    starttime = get_time();
    CPN *cpn = new CPN;
    char filename[] = "model1.pnml";
    cpn->getSize(filename);
    cpn->printSort();
    cpn->readPNML(filename);
    cpn->getRelVars();
    cpn->printTransVar();
    cpn->printVar();
    cpn->printCPN();
    cpn->setGlobalVar();
    cpnet = cpn;

    CTransition &t = cpn->transition[2];

    CPN_RG *crg = new CPN_RG;
    CPN_RGNode *initnode = crg->CPNRGinitialnode();
    initnode->printMarking();
    crg->Generate(initnode);

    delete cpn;
    delete crg;
    endtime = get_time();
    cout<<"RUNTIME:"<<endtime-starttime<<endl;
    return 0;
}