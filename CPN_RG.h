//
// Created by hecong on 19-12-10.
//

#ifndef CPN_PNML_PARSE_CPN_RG_H
#define CPN_PNML_PARSE_CPN_RG_H

#include "CPN.h"
#include <cmath>
#include <ctime>
using namespace std;

#define H1FACTOR 13
#define CPNRGTABLE_SIZE 1048576
#define random(x) rand()%(x)

extern NUM_t placecount;
extern NUM_t transitioncount;
extern NUM_t varcount;
extern CPN *cpnet;
class MarkingMeta;
class CPN_RGNode;
class CPN_RG;

void MarkingMetacopy(MarkingMeta &mm1,const MarkingMeta &mm2,type tid,SORTID sid);
void arrayToString(string &str,COLORID *cid,int num);
void MINUS(MarkingMeta &res,const MarkingMeta &mm1,const MarkingMeta &mm2);
class MarkingMeta
{
private:
    Tokens *tokenQ;      //带有头结点的队列；
    SHORTNUM colorcount;
    type tid;
    SORTID sid;
public:
    index_t hashvalue;

    MarkingMeta(){tokenQ=new Tokens;colorcount=0;hashvalue=0;}
    ~MarkingMeta(){
        Tokens *p,*q;
        p=tokenQ;
        while(p)
        {
            q=p->next;
            delete p;
            p=q;
        }
    }
    void initiate(type t,SORTID s) {tid=t;sid=s;}
    void insert(Tokens *token);
    index_t Hash();
    bool operator>=(const MarkingMeta &mm);
    friend class CPN_RGNode;
    friend class CPN_RG;
    friend void MarkingMetacopy(MarkingMeta &mm1,const MarkingMeta &mm2,type tid,SORTID sid);
    friend void MINUS(MarkingMeta &res,const MarkingMeta &mm1,const MarkingMeta &mm2);
};

class CPN_RGNode
{
private:
    MarkingMeta *marking;
    CPN_RGNode *next;
public:
    CPN_RGNode(){
        marking=new MarkingMeta[placecount];
        for(int i=0;i<placecount;++i) {
            marking[i].initiate(cpnet->place[i].tid,cpnet->place[i].sid);
        }
        next=NULL;
    }
    ~CPN_RGNode();
    index_t Hash(SHORTNUM *weight);
    bool operator==(const CPN_RGNode &n1);
    friend class CPN_RG;
};

class Bindind
{
public:
    COLORID *varvec;
    Bindind *next;
    MarkingMeta *arcsMultiSet;

    Bindind() {
        next=NULL;
        arcsMultiSet = NULL;
        varvec=new COLORID[varcount];
        memset(varvec,MAXSHORT,sizeof(COLORID)*varcount);
    }
    ~Bindind() {delete [] varvec;}
};

class TranBindQueue
{
public:
    unsigned short tranid;
    Bindind *bindQ;
    Bindind *bindptr;
    TranBindQueue *next;

    TranBindQueue() {
        tranid=MAXSHORT;
        bindQ=NULL;
        next=NULL;
    }
    ~TranBindQueue() {
        Bindind *q,*p;
        p=bindQ;
        while(p)
        {
            q=p->next;
            delete p;
            p=q;
        }
    }
    void initiate() {bindptr = bindQ;}
    void insert(Bindind *bound);
    void getSize(int &size);
};

class FiTranQueue
{
public:
    TranBindQueue *fireQ;
    TranBindQueue *fireptr;

    FiTranQueue(){fireQ=NULL;}
    ~FiTranQueue() {
        TranBindQueue *p,*q;
        p=fireQ;
        while(p)
        {
            q=p->next;
            delete p;
            p=q;
        }
    }
    void initiate(){fireptr = fireQ;}
    void insert(TranBindQueue *TBQ);
    void getSize(int &size);
};

class CPN_RG
{
private:
    CPN_RGNode **markingtable;
    SHORTNUM *weight;
    CPN_RGNode *initnode;
    NUM_t nodecount;
    NUM_t hash_conflict_times;

public:
    CPN_RG();
    ~CPN_RG();
    void addRGNode(CPN_RGNode *mark);
    void gerFireableTranx(CPN_RGNode *curnode,FiTranQueue &fireableTs);
    void giveArcColor(multiset_node *expnode,CPN_RGNode *curnode,VARID *varvec,index_t placeidx,int psindex);
    int giveVarColor(multiset_node *expnode,const MarkingMeta &mm,VARID *varvec,int psindex=0);
    void computeArcEXP(const arc_expression &arcexp,MarkingMeta &mm,int psnum=0);
    void computeArcEXP(meta *expnode,MarkingMeta &mm,int psnum=0);
    void judgeGuard(CTN *node,COLORID *cid);
    void getTupleColor(meta *expnode,COLORID *cid,int ptr);
    CPN_RGNode *CPNRGinitialnode();
    CPN_RGNode *CPNRGcreatenode(CPN_RGNode *mark,int tranxnum,bool &exist);
    bool NodeExist(CPN_RGNode *mark);
};
#endif //CPN_PNML_PARSE_CPN_RG_H
