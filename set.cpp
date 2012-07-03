#include <iostream>
#include <set>
using namespace std;

template<class T>
void printSet(set<T> s){
    typename set<T>::iterator it;
}

int main(int argc, char** argv){
    set<int> s;
    printSet<int>(s);
    return 0;
}