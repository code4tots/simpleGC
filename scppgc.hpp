#ifndef SCPPGC_HPP
#define SCPPGC_HPP

// Simple CPP Garbage Collector
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

template<class T>
void printSet(std::set<T> s);

namespace SCPPGC {

class Object;
class P;
class Map;
class Leaf;

class Object {
public:
    // returns whether the class is
    // a map type or a leaf type.
    virtual std::string getClassType()=0;
    virtual std::string toString() const {return "Object";}
    
    // for garbage collection
    bool color;
};


// All subclasses of Object should be allocated
// dynamically. DON'T create them on the stack.
// Just DON'T.
class Map : public Object {
public:
    static std::string type()   {return "map";}
    std::string getClassType()  {return type();}
    
    std::map<std::string,P> map;
};

class Leaf : public Object {
public:
    static std::string type()   {return "leaf";}
    std::string getClassType()  {return type();}
};

template<class T>
class WrappedLeaf : public Leaf {
public:
    WrappedLeaf()           {}
    WrappedLeaf(T t): v(t)  {}
    
    WrappedLeaf<T>& operator=(WrappedLeaf<T> other){
        v = other.v;
        return *this;
    }
    WrappedLeaf<T>& operator=(T t){
        v = t;
        return *this;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }
    
    T v;
private:
};

typedef WrappedLeaf<int> Integer;

/*
class Integer : public Leaf {
public:
    Integer()       {i=0;}
    Integer(int x)  {i=x;}
    
    std::string toString() const {
        std::stringstream ss;
        ss << i;
        return ss.str();
    }
private:
    int i;
};*/

class Double : public Leaf {
public:
    Double()           {d=0;}
    Double(double x)   {d=x;}
    
    std::string toString() const {
        std::stringstream ss;
        ss << d;
        return ss.str();
    }
private:
    int d;
};


// for printing --
std::ostream& operator<<(std::ostream& fout, const Object& obj){
    fout << obj.toString();
    return fout;
}


// Just for the record, P should never be 
// allocated dynamically. Just NO. Don't do it.

class P {
public:
    static void doGC(){
        // 
        std::cout << "GC started!\n";
        
        // first we need to figure out 
        // which pointers are root.
        std::set<P*> root;
        
        std::set<P*> nonroot;
        getNonRoot(nonroot);
        //printSet(nonroot);
        
        // Root should contain all P* elements that aren't in nonroot.
        std::set_difference(pset.begin(),pset.end(),nonroot.begin(),nonroot.end(),std::inserter(root,root.begin()));
        
        //printSet(root);
        
        std::cout << "GC 1!\n";
        // from here, it's just basic mark and sweep
        
        // color all P false.
        colorAll(false);
        
        std::cout << "GC 2\n";
        // now color all elements reachable from the root true.
        colorAllReachable(root,true);
        
        std::cout << "GC 2\n";
        // now color all objects false
        colorAllObjects(false);
        
        std::cout << "GC 2\n";
        // now color all objects with at least one P that points
        // to it that is reachable.
        colorAllReachableObjects(true);
        
        std::cout << "GC 2\n";
        // now all the unreachable objects are marked false.
        // kill them.
        delUnreachableObjects();
        
        // Yay!! we've just finished garbage collection!!
    }

    static std::set<P*>         pset;
    static std::set<Object*>    oset;
    
    P()             : p(NULL)   { init(); }
    P(const P& o)   : p(o.p)    { init(); }
    P(Object*  o)   : p(o)      { init(); }
    
    P& operator=(const P& o)    { p = o.p; init(); return *this; }
    P& operator=(Object*& o)    { p =  o;  init(); return *this; }
    
    ~P(){
        pset.erase(this);
    }
    
    
    P& operator[](std::string key){
        if (p->getClassType() != Map::type()){
            // error!!
        }
        Map* m = (Map*) p;
        return m->map[key];
    }
    
    
    Object* p;
    bool color;
    
    
    virtual std::string toString() const {
        return "Pointer P";
    }
private:


    void init(){
        pset.insert(this);
        if (p != NULL) oset.insert(p);
    }
    
    
    
    
    // helper methods for garbage collection ...
    
    static void getNonRoot(std::set<P*>& nonroot){
        // A pointer is non-root if it is owned by a Map object.
        std::set<Object*>::iterator it = oset.begin();
        std::map<std::string,P>::iterator pit;
        Map* m;
        
        for(;it!=oset.end();it++){
            if ( (*it)->getClassType() == Map::type() ){
                m = (Map*) (*it);
                pit = m->map.begin();
                
                for (;pit!=m->map.end();pit++){
                    // std::cout << "pit->first: " << pit->first << "\n";
                    // std::cout << "pit->second: " << &(pit->second) << "\n";
                    nonroot.insert( &(pit->second) );
                }
            }
        }
    }
    
    
    static void colorAll(bool c){
        std::set<P*>::iterator it = pset.begin();
        for(;it!=pset.end();it++){
            (*it)->color = c;
        }
    }
    
    static void colorAllReachable(const std::set<P*>& root, bool c){
        std::set<P*>::const_iterator it = root.begin();
        
        for (;it!=root.end();it++){
            colorAllReachable( (*it), c );
        }
    }
    static void colorAllReachable(P* root, bool c){
        // if it's the correct color, I've already visited this node.
        if (root->color == c) return;
        
        // otherwise, I should color it, and visit all its children, if it has any.
        root->color = c;
        
        if (root->p->getClassType() == Map::type()){
            Map* m = (Map*) root->p;
            
            std::map<std::string,P>::iterator it = m->map.begin();
            for (;it!=m->map.end();it++){
                colorAllReachable(&(it->second), c);
            }
        }
    }
    
    static void colorAllObjects(bool c){
        std::set<Object*>::iterator it = oset.begin();
        for (;it!=oset.end();it++){
            (*it)->color = c;
        }
    }
    
    static void colorAllReachableObjects(bool c){
        std::set<P*>::iterator it = pset.begin();
        for (;it!=pset.end();it++){
            if ( (*it)->color ){
                (*it)->p->color = c;
            }
        }
    }
    
    static void delUnreachableObjects(){
        std::vector< std::set<Object*>::iterator > toRemove;
        std::set<Object*>::iterator it = oset.begin();
        for (;it!=oset.end();it++){
            if ( ! ((*it)->color) ) {
                toRemove.push_back( it );
            }
        }
        // now remove all elements marked for removal.
        for (unsigned int i = 0; i < toRemove.size(); i++){
            std::cout << (*(*toRemove[i])) << "\n";
            
            delete (*toRemove[i]);
            oset.erase( toRemove[i] );
        }
    }
};

std::set<P*> P::pset;
std::set<Object*> P::oset;


/*

template <class T>
class TP : public P {
public:
    TP() {}
    TP(T t){
        v = t;
    }
    
    TP<T>& operator=(T t){ v = t; return *this;}
    TP<T>& operator=(TP<T> other){ v = other.v; return *this;}
    
    WrappedLeaf<T> v;
    
    std::string toString() const {
        return v.toString();
    }
};

std::ostream& operator<<(std::ostream& fout, const P& p){
    fout << p.toString();
    return fout;
}

*/

}
#endif