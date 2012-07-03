#include "scppgc.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <set>
using namespace std;
using namespace SCPPGC;

template<class T>
void printSet(set<T> s){
    typename set<T>::iterator it;
    cout << "set(";
    string sep = "";
    
    for (it = s.begin(); it != s.end(); it++){
        cout << sep << (*it)->p->toString() << ' ' << (*it)->color;
        sep = ",";
    }
    cout << ")\n";
}


void printOSet(set<Object*> s){
    set<Object*>::iterator it;
    cout << "oset(";
    string sep = "";
    
    for (it = s.begin(); it != s.end(); it++){
        cout << sep << (*it)->toString(); // << ' ' << (*it)->color;
        sep = ",";
    }
    cout << ")\n";
}

void f(){
    P p(new Map());
    ((Map*)p.p)->map["y"] = new Integer(166);
    ((Map*)p.p)->map["m"] = new Map();
}

int m(){
    P p(new Integer(66));
    P q(new Map());
    for (int i = 0; i < 5; i++) f();
    //((Map*)q.p)->map["x"] = new Integer(12);
    q["x"] = new Integer(12);
    P x  = ((Map*)q.p)->map["x"];
    
    printOSet(P::oset);
    P::doGC();
    //printSet(P::pset);
    printOSet(P::oset);
    
    
    return 0;
}

int main(int argc, char** argv){
    m();
    P::doGC();
    printOSet(P::oset);
    return 0;
}