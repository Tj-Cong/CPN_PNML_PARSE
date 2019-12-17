//
// Created by hecong on 19-12-10.
//

#include "CPN_RG.h"
static int COLORFLAG;
static bool VARFLAG;

void MarkingMetacopy(MarkingMeta &mm1,const MarkingMeta &mm2,type tid,SORTID sid)
{
    mm1.colorcount = mm2.colorcount;
    Tokens *q = mm2.tokenQ->next;
    Tokens *p,*ppre = mm1.tokenQ;
    while(q)
    {
        p = new Tokens;
        Tokenscopy(*p,*q,tid,sid);
        ppre->next = p;
        ppre = p;
        q=q->next;
    }
}
void arrayToString(string &str,COLORID *cid,int num)
{
    str="";
    for(int i=0;i<num;++i)
    {
        str += to_string(cid[i]);
    }
}
void MINUS(MarkingMeta &res,const MarkingMeta &mm1,const MarkingMeta &mm2)
{
    Tokens *resp,*pp1,*pp2;
    pp1 = mm1.tokenQ->next;
    pp2 = mm2.tokenQ->next;
    res.sid = mm1.sid;
    res.tid = mm1.tid;
    if(mm1.tid == dot)
    {
        resp = new Tokens;
        SHORTNUM num = pp1->tokencount - pp2->tokencount;
        resp->initiate(num,dot);
        res.insert(resp);
    }
    else if(mm1.tid == usersort)
    {
        COLORID cid1,cid2;
        while(pp2)
        {
            pp2->tokens->getColor(cid2);
            while(pp1)
            {
                pp1->tokens->getColor(cid1);
                if(cid1<cid2)
                {
                    resp = new Tokens;
                    Tokenscopy(*resp,*pp1,usersort);
                    res.insert(resp);
                    pp1=pp1->next;
                    continue;
                }
                else if(cid1==cid2)
                {
                    SHORTNUM num = pp1->tokencount - pp2->tokencount;
                    if(num>0)
                    {
                        resp = new Tokens;
                        resp->initiate(num,usersort);
                        resp->tokens->setColor(cid1);
                        res.insert(resp);
                    }
                    break;
                }
                else if(cid1 > cid2)
                {
                    cerr<<"[MINUS]ERROR:The minued is not greater than the subtractor."<<endl;
                    exit(-1);
                }
            }
            if(!pp1)
            {
                cerr<<"[MINUS]ERROR:The minued is not greater than the subtractor."<<endl;
                exit(-1);
            }
            else {
                pp1=pp1->next;
                pp2=pp2->next;
            }
        }
    }
    else if(mm1.tid == productsort)
    {
        int sortnum = sorttable.productsort[mm1.sid].sortnum;
        COLORID *cid1,*cid2;
        cid1 = new COLORID[sortnum];
        cid2 = new COLORID[sortnum];
        string str1,str2;
        while(pp2)
        {
            pp2->tokens->getColor(cid2,sortnum);
            arrayToString(str2,cid2,sortnum);
            while(pp1)
            {
                pp1->tokens->getColor(cid1,sortnum);
                arrayToString(str1,cid1,sortnum);
                if(str1<str2)
                {
                    resp = new Tokens;
                    Tokenscopy(*resp,*pp1,productsort,sortnum);
                    res.insert(resp);
                    pp1=pp1->next;
                    continue;
                }
                else if(str1 == str2)
                {
                    SHORTNUM num = pp1->tokencount-pp2->tokencount;
                    if(num>0)
                    {
                        resp = new Tokens;
                        resp->initiate(num,productsort,sortnum);
                        resp->tokens->setColor(cid1,sortnum);
                        res.insert(resp);
                    }
                    break;
                }
                else if(str1 > str2)
                {
                    cerr<<"[MINUS]ERROR:The minued is not greater than the subtractor."<<endl;
                    exit(-1);
                }
            }
            if(!pp1)
            {
                cerr<<"[MINUS]ERROR:The minued is not greater than the subtractor."<<endl;
                exit(-1);
            }
            else {
                pp1=pp1->next;
                pp2=pp2->next;
            }
        }
    }
}

/***********************************************************************/
index_t MarkingMeta::Hash() {
    index_t hv = 0;
    if(tid == usersort)
    {
        hv = colorcount*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;
        for(p;p!=NULL;p=p->next)
        {
            COLORID cid;
            p->tokens->getColor(cid);
            hv += p->tokencount*H1FACTOR*H1FACTOR+(cid+1)*H1FACTOR;
        }
    }
    else if(tid == productsort)
    {
        hv = colorcount*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p;
        for(p=tokenQ->next;p!=NULL;p=p->next)
        {
            int sortnum = sorttable.productsort[sid].sortnum;
            COLORID *cid = new COLORID[sortnum];
            p->tokens->getColor(cid,sortnum);
            hv += p->tokencount*H1FACTOR*H1FACTOR;
            for(int j=0;j<sortnum;++j)
            {
                hv += (cid[j]+1)*H1FACTOR;
            }
            delete [] cid;
        }
    }
    else if(tid == dot)
    {
        hv = colorcount*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;
        hv += p->tokencount*H1FACTOR*H1FACTOR;
    }
    else if(tid == finiteintrange)
    {
        cerr<<"Sorry, we don't support finteintrange now."<<endl;
        exit(-1);
    }
    hashvalue = hv;
    return hv;
}
/*1.找到要插入的位置，分为三种：
 * i.末尾
 * ii.中间
 * iii.开头
 * 2.插入到该插入的位置，并设置colorcount++；
 * (3).如果是相同的color，仅更改tokencount；
 * */
void MarkingMeta::insert(Tokens *t) {
    if(tid == usersort)
    {
        Tokens *q=tokenQ,*p = tokenQ->next;
        COLORID pcid,tcid;
        t->tokens->getColor(tcid);
        while(p!=NULL)
        {
            p->tokens->getColor(pcid);
            if(tcid<=pcid)
                break;
            q=p;
            p=p->next;
        }
        if(p == NULL)
        {
            q->next = t;
            colorcount++;
            return;
        }
        if(tcid == pcid)
        {
            p->tokencount+=t->tokencount;
        }
        else if(tcid<pcid)
        {
            t->next = p;
            q->next = t;
            colorcount++;
        }
    }
    else if(tid == productsort)
    {
        Tokens *q=tokenQ,*p = tokenQ->next;
        COLORID *pcid,*tcid;
        string pstr,tstr;
        int sortnum = sorttable.productsort[sid].sortnum;
        pcid = new COLORID[sortnum];
        tcid = new COLORID[sortnum];
        t->tokens->getColor(tcid,sortnum);
        arrayToString(tstr,tcid,sortnum);
        while(p!=NULL)
        {
            p->tokens->getColor(pcid,sortnum);
            arrayToString(pstr,pcid,sortnum);
            if(tstr<=pstr)
                break;
            q=p;
            p=p->next;
        }
        if(p == NULL)
        {
            q->next = t;
            colorcount++;
            return;
        }
        if(tstr == pstr)
        {
            p->tokencount += t->tokencount;
        }
        else if(tstr < pstr)
        {
            t->next = p;
            q->next = t;
            colorcount++;
        }
        delete [] pcid;
        delete [] tcid;
    }
    else if(tid == dot)
    {
        tokenQ->next->tokencount+=t->tokencount;
    }
}

bool MarkingMeta::operator>=(const MarkingMeta &mm) {
    bool greaterorequ = true;
    Tokens *p1,*p2;
    p1 = this->tokenQ->next;
    p2 = mm.tokenQ->next;
    if(this->tid == dot)
    {
        if(p1->tokencount>=p2->tokencount)
            greaterorequ = true;
        else
            greaterorequ = false;
    }
    else if(this->tid == usersort)
    {
        COLORID cid1,cid2;
        while(p2)
        {
            //p2中的元素，p1中必须都有；遍历p2
            p2->tokens->getColor(cid2);
            while(p1)
            {
                p1->tokens->getColor(cid1);
                if(cid1<cid2)
                {
                    p1=p1->next;
                    continue;
                }
                else if(cid1==cid2)
                {
                    if(p1->tokencount>=p2->tokencount)
                    {
                        break;
                    }
                    else
                    {
                        greaterorequ = false;
                        break;
                    }

                }
                else if(cid1>cid2)
                {
                    greaterorequ = false;
                    break;
                }
            }
            if(p1 == NULL)
                greaterorequ = false;
            if(!greaterorequ)
                break;
            else{
                p2=p2->next;
            }
        }
    }
    else if(this->tid == productsort)
    {
        int sortnum = sorttable.productsort[sid].sortnum;
        COLORID *cid1,*cid2;
        cid1 = new COLORID[sortnum];
        cid2 = new COLORID[sortnum];
        string str1,str2;
        while(p2)
        {
            p2->tokens->getColor(cid2,sortnum);
            arrayToString(str2,cid2,sortnum);
            while(p1)
            {
                p1->tokens->getColor(cid1,sortnum);
                arrayToString(str1,cid1,sortnum);
                if(str1<str2){
                    p1=p1->next;
                    continue;
                }
                else if(str1 == str2)
                {
                    if(p1->tokencount>=p2->tokencount)
                    {
                        break;
                    }
                    else
                    {
                        greaterorequ = false;
                        break;
                    }
                }
                else if(str1>str2)
                {
                    greaterorequ = false;
                    break;
                }
            }
            if(p1 == NULL)
                greaterorequ = false;
            if(!greaterorequ)
                break;
            else {
                p2=p2->next;
            }
        }
    }
    return greaterorequ;
}

/*****************************************************************/
CPN_RGNode::~CPN_RGNode() {
    delete [] marking;
}

index_t CPN_RGNode::Hash(SHORTNUM *weight) {
    index_t hashvalue = 0;
    for(int i=0;i<placecount;++i)
    {
        hashvalue += weight[i]*marking[i].hashvalue;
    }
}

bool CPN_RGNode::operator==(const CPN_RGNode &n1) {
    bool equal = true;
    for(int i=0;i<placecount;++i)
    {
        //检查库所i的tokenmetacount是否一样
        SHORTNUM tmc = this->marking[i].colorcount;
        if(tmc != n1.marking[i].colorcount)
        {
            equal = false;
            break;
        }

        //检查每一个tokenmeta是否一样
        Tokens *t1=this->marking[i].tokenQ->next;
        Tokens *t2=n1.marking[i].tokenQ->next;
        for(t1,t2; t1!=NULL && t2!=NULL; t1=t1->next,t2=t2->next)
        {
            //先检查tokencount
            if(t1->tokencount!=t2->tokencount)
            {
                equal = false;
                break;
            }

            //检查color
            type tid = cpnet->place[i].tid;
            if(tid == dot) {
                continue;
            }
            else if(tid == usersort){
                COLORID cid1,cid2;
                t1->tokens->getColor(cid1);
                t2->tokens->getColor(cid2);
                if(cid1!=cid2) {
                    equal = false;
                    break;
                }
            }
            else if(tid == productsort){
                COLORID *cid1,*cid2;
                SORTID sid = cpnet->place[i].sid;
                int sortnum = sorttable.productsort[sid].sortnum;
                cid1 = new COLORID[sortnum];
                cid2 = new COLORID[sortnum];
                t1->tokens->getColor(cid1,sortnum);
                t2->tokens->getColor(cid2,sortnum);
                for(int k=0;k<sortnum;++k)
                {
                    if(cid1[k]!=cid2[k])
                    {
                        equal = false;
                        delete [] cid1;
                        delete [] cid2;
                        break;
                    }
                }
                if(equal==false)
                    break;
                delete [] cid1;
                delete [] cid2;
            }
            else if(tid == finiteintrange){
                cerr<<"[CPN_RG\\line110]FiniteIntRange ERROR.";
                exit(-1);
            }
        }
        if(equal == false)
            break;
    }
    return equal;
}

/*************************TranBindQueue****************************/
void TranBindQueue::insert(Bindind *bound) {
    bound->next = bindQ;
    bindQ = bound;
}

void TranBindQueue::getSize(int &size) {
    size = 0;
    Bindind *p = bindQ;
    for(p;p!=NULL;++p,++size);
}

void FiTranQueue::insert(TranBindQueue *TBQ) {
    TBQ->next = fireQ;
    fireQ = TBQ;
}

void FiTranQueue::getSize(int &size) {
    size = 0;
    TranBindQueue *p;
    for(p=fireQ;p!=NULL;++p,++size);
}

/*****************************************************************/
CPN_RG::CPN_RG() {
    markingtable = new CPN_RGNode*[CPNRGTABLE_SIZE];
    for(int i=0;i<CPNRGTABLE_SIZE;++i) {
        markingtable[i] = NULL;
    }
    nodecount = 0;
    hash_conflict_times = 0;

    weight = new SHORTNUM[placecount];
    srand((int)time(NULL));
    for(int j=0;j<placecount;++j)
    {
        weight[j] = random(11)+1;
    }
}

CPN_RG::~CPN_RG() {
    for(int i=0;i<CPNRGTABLE_SIZE;++i)
    {
        if(markingtable[i]!=NULL)
        {
            CPN_RGNode *p=markingtable[i];
            CPN_RGNode *q;
            while(p)
            {
                q=p->next;
                delete p;
                p=q;
            }
        }
    }
    delete [] markingtable;
}

void CPN_RG::addRGNode(CPN_RGNode *mark) {
    index_t hashvalue = mark->Hash(weight);
    index_t size = CPNRGTABLE_SIZE-1;
    hashvalue = hashvalue & size;

    if(markingtable[hashvalue]!=NULL)
        hash_conflict_times++;

    mark->next = markingtable[hashvalue];
    markingtable[hashvalue] = mark;
    nodecount++;
}

CPN_RGNode *CPN_RG::CPNRGinitialnode() {
    initnode = new CPN_RGNode;
    for(int i=0;i<placecount;++i)
    {
        CPlace &pp = cpnet->place[i];
        MarkingMeta &mm = initnode->marking[i];

        for(int j=0;j<pp.metacount;++j)
        {
            if(pp.tid!=productsort)
            {
                Tokens *q=new Tokens;
                Tokenscopy(*q,pp.initMarking[j],pp.tid);
                mm.insert(q);
            }
            else
            {
                int sortnum = sorttable.productsort[pp.sid].sortnum;
                Tokens *q=new Tokens;
                Tokenscopy(*q,pp.initMarking[j],pp.tid,sortnum);
                mm.insert(q);
            }
        }
        mm.Hash();
    }
    addRGNode(initnode);
}

CPN_RGNode *CPN_RG::CPNRGcreatenode(CPN_RGNode *mark, int tranxnum, bool &exist) {

    return nullptr;
}

bool CPN_RG::NodeExist(CPN_RGNode *mark) {
    index_t hashvalue = mark->Hash(weight);
    index_t size = CPNRGTABLE_SIZE-1;
    hashvalue = hashvalue & size;

    bool exist = false;
    CPN_RGNode *p = markingtable[hashvalue];
    while(p)
    {
        if(*p == *mark)
        {
            exist = true;
            break;
        }
    }
    return exist;
}

/*得到当前状态CPN_RGNode *curnode下所有的可发生变迁，
 * FiTranQueue:可发生变迁队列，FiTranQueue{{TranBindQueue}->{TranBindQueue}}
 * TranBindQueue：可发生变迁，里面还有所有的能使其发生的绑定队列，{{Bindind}->{Bindind}}
 * Bindind:变量的绑定，一个变量向量，如<v1=c1,v2=c2,v3=c3,...>
 *
 * persudo:
 * FOR Trans in cpnet->transition
 *  CREATE a TranBindQueue for Trans;
 *  initiate all Trans.producer CSArc;
 *  //finding all possible bindings
 *  WHILE(...)
 *      //find a binding
 *      CREATE a Binding bb
 *      FOR every arc of Trans' producer
 *          Give arc a color;
 *      END FOR
 *      IF successfully find bb
 *          COMPUTE ALL arc's express;
 *          Check if Place.token > arc_express;
 *          Check if guard_express;
 *          IF(satisfy both)
 *
 *          END IF
 *      END
 *  END WHILE
 * END FOR
 *
 * !!!!!!有的变迁根本不需要绑定
 * */
void CPN_RG::gerFireableTranx(CPN_RGNode *curnode,FiTranQueue &fireableTs){
    //遍历每一个变迁，看其能否发生
    for(int i=0;i<transitioncount;++i)
    {
        VARFLAG = false;
        TranBindQueue *tbq = new TranBindQueue;
        tbq->tranid = i;

        CTransition &tran = cpnet->transition[i];
        //在寻找该变迁的所有绑定之前需要对该变迁的所有前继弧初始化
        vector<CSArc>::iterator csiter;
        for(csiter=tran.producer.begin();csiter!=tran.producer.end();++csiter)
        {
            csiter->arc_exp.initiate(csiter->arc_exp.root);
        }

        //对于变迁tran,寻找他的每一种绑定，放在tbq中
        while(true)
        {
            //在寻找一种绑定时，需要遍历该变迁的全部前继库所和弧
            Bindind *bb = new Bindind;
            bool success = true;   //success表示是否着色成功
            vector<CSArc>::iterator iter;
            for(iter=tran.producer.begin();iter!=tran.producer.end();++iter)
            {
                COLORFLAG = 0;
                giveArcColor(iter->arc_exp.root,curnode,bb->varvec,iter->idx,0);
                if(COLORFLAG = -1)
                {
                    success = false;
                    break;
                }
            }

            if(success) //着色成功后，判断该着色是否合法
            {
                //计算弧的颜色多重集
                bool firebound = true;

                //将每一个弧表达式转化为一个多重集，多重集用MarkingMeta表示
                //申请一个MarkingMeta数组，mm[i]就表示变迁tran第i个前继弧的multi-set
                MarkingMeta *mm = new MarkingMeta[tran.producer.size()];
                vector<CSArc>::iterator it;
                int i;
                for(i=0,it=tran.producer.begin();it!=tran.producer.end();++i,++it)
                {
                    //计算每一个前继弧的多重集，并判断个前继库所的包含关系
                    type ptid = cpnet->place[it->idx].tid;
                    SORTID psid = cpnet->place[it->idx].sid;
                    mm[i].initiate(ptid,psid);

                    int psnum;
                    if(ptid == productsort){
                        psnum = sorttable.productsort[psid].sortnum;
                    }
                    else
                        psnum = 0;
                    computeArcEXP(it->arc_exp,mm[i],psnum);

                    //判断前继库所中的token和弧上表达式的关系
                    if(curnode->marking[it->idx] >= mm[i])
                        continue;
                    else
                    {
                        firebound = false;
                        break;
                    }
                }
                if(!firebound)
                {
                    delete bb;
                    delete [] mm;
                    if(!VARFLAG)   //没有变量
                        break;
                    else
                        continue;
                }

                if(!tran.hasguard) {
                    continue;
                }

                //判断guard函数
                judgeGuard(tran.guard.root->left,bb->varvec);
                if(tran.guard.root->left->mytruth)
                {
                    //变迁真的能发生！
                    bb->arcsMultiSet = mm;
                    tbq->insert(bb);
                }
                else
                {
                    delete bb;
                    delete [] mm;
                }

                //如果这个变迁不需要绑定，则只要进行一次
                if(!VARFLAG)
                    break;
            }
            else
                break;
        }

        //看看该变迁是否有可使能的
        if(tbq->bindQ == NULL)
        {
            delete tbq;
        }
        else {
            tbq->initiate();
            fireableTs.insert(tbq);
        }
    }
}

/*功能说明：
 * 给弧表达式（弧的抽象语法树）上的变量（变量节点）赋值，并反映在变量向量中。
 * */
void CPN_RG::giveArcColor(multiset_node *expnode,CPN_RGNode *curnode,VARID *varvec,index_t placeidx,int psindex) {
    if(COLORFLAG == -1)
        return;
    if(expnode->mytype == var)
    {
        VARFLAG = true;
        int res;
        res = giveVarColor(expnode,curnode->marking[placeidx],varvec,psindex);
        if(res == -1)
            COLORFLAG = -1;
    }
    else if(expnode->myname == "tuple")
    {
        giveArcColor(expnode->leftnode,curnode,varvec,placeidx,psindex);
        if(COLORFLAG == -1)
            return;
        giveArcColor(expnode->rightnode,curnode,varvec,placeidx,psindex+1);
    }
    else
    {
        if(expnode->leftnode != NULL)
        {
            giveArcColor(expnode->leftnode,curnode,varvec,placeidx,psindex);
            if(COLORFLAG == -1)
                return;
        }
        if(expnode->rightnode != NULL)
        {
            giveArcColor(expnode->rightnode,curnode,varvec,placeidx,psindex);
        }
    }
}

/*功能说明：
 * 给弧表达式（弧的抽象语法树）上的变量（变量节点）赋值，并反映在变量向量中。
 * 参数说明：
 * expnode:当前需要赋值的变量节点，需要该节点的name来索取该变量在数组中的索引位置
 * const MarkingMeta &mm：该变量赋值所参考的库所的token；
 * VARID *varvec：变量值的向量，<v1=c1,v2=c2,v3=c3,...>，赋值后需要体现在该向量中；
 * int psindex：如果是productsort类型，需要知道该变量在交类型的第几个
 * */
int CPN_RG::giveVarColor(multiset_node *expnode,const MarkingMeta &mm,VARID *varvec,int psindex) {
    if(mm.tid == productsort)
    {
        //检查是否已经绑定过
        map<string,VARID>::iterator viter;
        viter = cpnet->mapVariable.find(expnode->myname);
        VARID vid = viter->second;
        if(varvec[vid] != MAXSHORT)
        {
            //已经绑定过
            expnode->number = varvec[vid];
            return 0;
        }

        //绑定一个值
        int &bindptr = expnode->bindptr;
        //检查是否还有未绑定的值
        if(bindptr < mm.colorcount){
            //取出一个值
            SORTID sid = mm.sid;
            int sortnum = sorttable.productsort[sid].sortnum;
            COLORID *cid = new COLORID[sortnum];

            Tokens *p = mm.tokenQ->next;
            for(int i=0;i<bindptr;++i,p=p->next)
            p->tokens->getColor(cid,sortnum);
            //绑定值
            expnode->number = cid[psindex];
            varvec[vid] = cid[psindex];

            bindptr++;
            return 0;
        }
        else{
            //已经全部绑定完了
            return -1;
        }
    }
    else if(mm.tid == usersort)
    {
        //检查是否已经绑定过
        map<string,VARID>::iterator viter;
        viter = cpnet->mapVariable.find(expnode->myname);
        VARID vid = viter->second;
        if(varvec[vid] != MAXSHORT)
        {
            //已经绑定过
            expnode->number = varvec[vid];
            return 0;
        }

        //绑定一个值
        int &bindptr = expnode->bindptr;
        //检查是否还有未绑定的值
        if(bindptr < mm.colorcount){
            //取出一个值
            COLORID cid;
            Tokens *p=mm.tokenQ->next;
            for(int i=0;i<bindptr;++i,p=p->next);
            p->tokens->getColor(cid);
            //绑定值
            expnode->number = cid;
            varvec[vid] = cid;
            bindptr++;
            return 0;
        }
        else{
            //已经全部绑定完了
            return -1;
        }
    }
    else if(mm.tid == dot)
    {
        //DOT类型不需要绑定
        return 0;
    }
}

void CPN_RG::computeArcEXP(const arc_expression &arcexp,MarkingMeta &mm,int psnum) {
    computeArcEXP(arcexp.root->leftnode,mm,psnum);
}

void CPN_RG::computeArcEXP(meta *expnode, MarkingMeta &mm,int psnum) {
    if(expnode->myname == "add")
    {
        computeArcEXP(expnode->leftnode,mm);
        computeArcEXP(expnode->rightnode,mm);
    }
    else if(expnode->myname == "numberof")
    {
        /*numberof节点等同于多重集合中的一个元，如1'a+2'b中的1'a
         * 处理分两步：
         * 1.取出左边节点的数,如1'a中的1;
         * 2.取出有边节点的color，如1'a中的a；
         * */

        //doing the first job;
        SHORTNUM num = expnode->leftnode->number;
        //doing the second job;
        meta *color = expnode->rightnode;
        if(color->myname == "tuple")
        {
            //tuple取出来的是一个数组cid
            COLORID *cid = new COLORID[psnum];
            getTupleColor(color,cid,0);
            //创建一个colortoken，插入到mm中
            Tokens *t = new Tokens;
            t->initiate(num,productsort,psnum);
            t->tokens->setColor(cid,psnum);
            mm.insert(t);
            delete [] cid;
        }
        else if(color->mytype == delsort)
        {
            Tokens *p = new Tokens;
            if(color->myname == "dotconstant")
            {
                p->initiate(num,dot);
                mm.insert(p);
            }

            COLORID cid;
            map<string,MCI>::iterator citer;
            citer = sorttable.mapColor.find(color->myname);
            cid = citer->second.cid;

            p->initiate(num,usersort);
            p->tokens->setColor(cid);
            mm.insert(p);
        }
        else if(color->mytype == var)
        {
            COLORID cid = color->number;
            Tokens *p = new Tokens;
            p->initiate(num,usersort);
            p->tokens->setColor(cid);
            mm.insert(p);
        }
    }
    else{
        cerr<<"[CPN_RG::computeArcEXP] ERROR:Unexpected arc_expression node"<<expnode->myname<<endl;
        exit(-1);
    }
}

void CPN_RG::getTupleColor(meta *expnode,COLORID *cid,int ptr) {
    if(expnode->mytype == var)
    {
        //1.首先检查该变量是否已经绑定
        //2.取出color，放在数组cid[ptr]；
        if(expnode->number == -1)
        {
            cerr<<"[CPN_RG::getTupleColor] ERROR:Variable "<<expnode->myname<<" unbounded!"<<endl;
            exit(-1);
        }
        cid[ptr] = expnode->number;
    }
    else if(expnode->mytype == delsort)
    {
        //根据颜色的名字索引颜色的索引值
        map<string,MCI>::iterator citer;
        citer = sorttable.mapColor.find(expnode->myname);
        cid[ptr] = citer->second.cid;
    }
    else if(expnode->myname == "tuple")
    {
        getTupleColor(expnode->leftnode,cid,ptr);
        getTupleColor(expnode->rightnode,cid,ptr+1);
    }
    else if(expnode->myname == "successor")
    {
        SHORTNUM feconstnum;
        if(expnode->leftnode->mytype == var)
        {
            map<string,VARID>::iterator viter;
            viter = cpnet->mapVariable.find(expnode->leftnode->myname);
            SORTID sid = cpnet->vartable[viter->second].sid;
            feconstnum = sorttable.usersort[sid].feconstnum;
        }
        else if(expnode->leftnode->mytype == delsort)
        {
            map<string,MCI>::iterator citer;
            citer = sorttable.mapColor.find(expnode->myname);
            feconstnum = sorttable.usersort[citer->second.sid].feconstnum;
        }
        cid[ptr] = ((expnode->leftnode->number)+1)%feconstnum;
    }
    else if(expnode->myname == "predecessor")
    {
        SHORTNUM feconstnum;
        if(expnode->leftnode->mytype == var)
        {
            map<string,VARID>::iterator viter;
            viter = cpnet->mapVariable.find(expnode->leftnode->myname);
            SORTID sid = cpnet->vartable[viter->second].sid;
            feconstnum = sorttable.usersort[sid].feconstnum;
        }
        else if(expnode->leftnode->mytype == delsort)
        {
            map<string,MCI>::iterator citer;
            citer = sorttable.mapColor.find(expnode->myname);
            feconstnum = sorttable.usersort[citer->second.sid].feconstnum;
        }
        if(expnode->leftnode->number == 0)
            cid[ptr] = feconstnum-1;
        else
            cid[ptr] = (expnode->leftnode->number)-1;
    }
}

void CPN_RG::judgeGuard(CTN *node,COLORID *cid) {
    switch(node->mytype)
    {
        case Boolean:{
            if(node->myname == "and")
            {
                judgeGuard(node->left,cid);
                judgeGuard(node->right,cid);
                if(!node->left->mytruth)
                    node->mytruth = false;
                else
                    node->mytruth = node->right->mytruth;
            }
            else if(node->myname == "or")
            {
                judgeGuard(node->left,cid);
                judgeGuard(node->right,cid);
                if(node->left->mytruth)
                    node->mytruth = true;
                else
                    node->mytruth = node->right->mytruth;
            }
            else if(node->myname == "imply")
            {
                judgeGuard(node->left,cid);
                judgeGuard(node->right,cid);
                if(!node->left->mytruth)
                    node->mytruth = true;
                else if(node->left->mytruth && node->right->mytruth)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "not")
            {
                judgeGuard(node->left,cid);
                if(node->left->mytruth)
                    node->mytruth = false;
                else
                    node->mytruth = true;
            }
            break;
        }
        case Relation:{
            if(node->left->mytype!=variable && node->left->mytype!=useroperator)
                cerr<<"[condition_tree::judgeGuard]ERROR:Relation node's leftnode is not a variable or color."<<endl;
            if(node->right->mytype!=variable && node->right->mytype!=useroperator)
                cerr<<"[condition_tree::judgeGuard]ERROR:Relation node's rightnode is not a variable or color."<<endl;
            judgeGuard(node->left,cid);
            judgeGuard(node->right,cid);
            if(node->myname == "equality")
            {
                if(node->left->cid == node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "inequality")
            {
                if(node->left->cid != node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "lessthan")
            {
                if(node->left->cid < node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "lessthanorequal")
            {
                if(node->left->cid <= node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "greaterthan")
            {
                if(node->left->cid > node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            else if(node->myname == "greaterthanorequal")
            {
                if(node->left->cid >= node->right->cid)
                    node->mytruth = true;
                else
                    node->mytruth = false;
            }
            break;
        }
        case variable:{
            map<string,VARID>::iterator viter;
            viter=cpnet->mapVariable.find(node->myname);
            if(cid[viter->second] == MAXSHORT)
            {
                cerr<<"[condition_tree::judgeGuard]ERROR:Variable "<<node->myname<<" is not bounded."<<endl;
                exit(-1);
            }
            node->cid = cid[viter->second];
            break;
        }
        case useroperator: {
            map<string,mapcolor_info>::iterator citer;
            citer = sorttable.mapColor.find(node->myname);
            node->cid = citer->second.cid;
            break;
        }
    }
}
