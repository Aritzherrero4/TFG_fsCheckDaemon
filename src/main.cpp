#include "../include/mtree.hpp"
/*Dummy file to test tree and nodes
* Will be replaced with the daemon
*/
int main(){
    fs::path p = "/home/aritz/test/";
    Mtree * tree = new Mtree();
    //getFsDirList("/");
    tree->populateTree(p);  
    tree->print();
}