//
// Created by hecong on 19-11-23.
//

#ifndef CPN_PNML_PARSE_CPN_H
#define CPN_PNML_PARSE_CPN_H

#endif //CPN_PNML_PARSE_CPN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include "condition_tree.h"
using namespace std;

#define MAXSHORT 65535
typedef unsigned int index_t;    //索引数据类型
typedef unsigned int NUM_t;
typedef unsigned short SORTID;
typedef unsigned short COLORID;
typedef unsigned short VARID;
typedef unsigned short SHORTNUM;

enum type{dot,finiteintrange,productsort,usersort};
int stringToNum(const string& str);

class CPN;
class SortTable;
struct mapsort_info;
extern SortTable sorttable;
extern NUM_t placecount;
extern NUM_t transitioncount;
extern NUM_t varcount;
extern CPN *cpnet;
/*========================Sort==========================*/
class Sort
{
public:
    type mytype;
    virtual ~Sort(){};
};

class ProductSort:public Sort
{
public:
    string id;
    int sortnum;
    vector<string> sortname;
    vector<mapsort_info> sortid;
};

//UserSort is a user declared enumerable sort
class UserSort:public Sort
{
public:
    string id;
    SHORTNUM feconstnum;
    vector<string> cyclicenumeration;
    map<string,COLORID> mapValue;

    COLORID getColorIdx(string color) {
        map<string,COLORID>::iterator iter;
        iter = mapValue.find(color);
        if(iter!=mapValue.end()) {
            return iter->second;
        }
        else {
            cerr<<"Can not find color"<<color<<"in colorset"<<this->id<<endl;
            exit(-1);
        }
    }
};

class FiniteIntRange:public Sort
{
public:
    string id;
    int start;
    int end;
};

/*==========================SortValue=========================*/
/*==================Abstract Base Class==================*/
class SortValue
{
public:
    virtual ~SortValue(){};
    virtual void setColor(COLORID cid)=0;
    virtual void setColor(COLORID *cid,int size)=0;
    virtual void getColor(COLORID &cid)=0;
    virtual void getColor(COLORID *cid,int size)=0;
};

/*====================ProductSortValue================*/
class ProductSortValue:public SortValue
{
private:
    //this is a index;
    COLORID *valueindex;
public:
    ProductSortValue(int sortnum) {
        valueindex = new COLORID[sortnum];
    }
    ~ProductSortValue() {
        delete [] valueindex;
    }

    void setColor(COLORID cid){};
    void getColor(COLORID &cid){};
    void setColor(COLORID *cid,int size) {
        memcpy(valueindex,cid,sizeof(COLORID)*size);
    }
    void getColor(COLORID *cid,int size) {
        memcpy(cid,valueindex,sizeof(COLORID)*size);
    }
};

/*===================UserSortValue=================*/
class UserSortValue:public SortValue
{
private:
    //this is a index;
    COLORID colorindex;
public:
    void setColor(COLORID cid) {colorindex = cid;}
    void getColor(COLORID &cid) {cid = colorindex;}
    void setColor(COLORID *cid,int size){};
    void getColor(COLORID *cid,int size){};
    ~UserSortValue(){};
};

/*==================FIRSortValue==================*/
class FIRSortValue:public SortValue
{
public:
    int value;
    void setColor(COLORID cid){};
    void getColor(COLORID &cid){};
    void setColor(COLORID *cid,int size){};
    void getColor(COLORID *cid,int size){};
    ~FIRSortValue(){};
};

/*==================DotSortValue==================*/
//class DotSortValue:public SortValue
//{
//private:
//    COLORID value;
//public:
//    void setColor(COLORID cid){value = cid;};
//    void getColor(COLORID &cid){cid = value;};
//    virtual void setColor(COLORID *cid,int size){};
//    virtual void getColor(COLORID *cid,int size){};
//};

class Tokens
{
public:
    SortValue *tokens;
    SHORTNUM tokencount;
    Tokens *next;

    Tokens() {tokens = NULL,tokencount=0;next=NULL;}
    ~Tokens() {delete tokens;}
    void initiate(SHORTNUM tc,type sort,int PSnum=0);
};
/*===========================================================*/
typedef struct mapsort_info
{
    //its index in corresponding vector
    SORTID sid;
    type tid;
} MSI;

typedef struct mapcolor_info  //only used for cyclicenumeration
{
    SORTID sid;
    COLORID cid;
} MCI;

class SortTable
{
public:
    vector<ProductSort> productsort;
    vector<UserSort> usersort;
    vector<FiniteIntRange> finitintrange;
    bool hasdot;
public:
    SortTable(){hasdot = false;}
    //first string is the sortname,the second MSI is this sort's information;
    map<string,MSI> mapSort;
    map<string,MCI> mapColor;
    SORTID getPSid(string PSname);
    SORTID getUSid(string USname);
    SORTID getFIRid(string FIRname);
    friend class CPN;
};

struct Variable
{
    string id;
    SORTID sid;
    VARID vid;
    type tid;

    bool operator == (const Variable &var) const {
        if(this->id == var.id)
            return true;
        else
            return false;
    }
    bool operator < (const Variable &var) const {
        if(this->vid < var.vid)
            return true;
        else
            return false;
    }
};

/*========================Net Element========================*/
typedef struct CPN_Small_Arc
{
    index_t idx;
    arc_expression arc_exp;

} CSArc;

typedef struct CPN_Place
{
    string id;
    type tid;
    SORTID sid;
    Tokens *initMarking=NULL;
    SHORTNUM metacount=0;
    vector<CSArc>producer;
    vector<CSArc>consumer;

    void printTokens(string &str);
    ~CPN_Place(){
        if(initMarking!=NULL)
            delete [] initMarking;
    }
} CPlace;

typedef struct CPN_Transition
{
    string id;
    condition_tree guard;
    bool hasguard;
    vector<CSArc> producer;
    vector<CSArc> consumer;
    set<Variable> relvars;
    vector<Variable> relvararray;
} CTransition;

typedef struct CPN_Arc
{
    string id;
    bool isp2t;
    string source_id;
    string target_id;
    arc_expression arc_exp;
    ~CPN_Arc() {
        arc_exp.destructor(arc_exp.root);
    }
} CArc;

/*========================Color_Petri_Net=======================*/
class CPN
{
public:
    CPlace *place;
    CTransition *transition;
    CArc *arc;
    Variable *vartable;
    NUM_t placecount;
    NUM_t transitioncount;
    NUM_t arccount;
    NUM_t varcount;
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    map<string,VARID> mapVariable;

    CPN();
    void getSize(char *filename);
    void readPNML(char *filename);
    void getInitMarking(TiXmlElement *initMarking,CPlace &pp,int i);
    void getRelVars();
    void TraArcTreeforVAR(meta *expnode,set<Variable> &relvars);
    void printCPN();
    void printSort();
    void printVar();
    void printTransVar();
    void setGlobalVar();
    ~CPN();
private:
};

void Tokenscopy(Tokens &t1,const Tokens &t2,type tid,int PSnum=0);
