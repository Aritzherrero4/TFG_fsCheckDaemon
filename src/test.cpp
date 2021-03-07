#include "../include/mtree.hpp"
#include <fstream>
#include <iostream>
/*Dummy file to test tree and nodes
* Will be replaced with the daemon
*/
int main(){
    fs::path p = "/home/aritz/test/root/";
    Mtree * tree = new Mtree();
    //getFsDirList("/");
    tree->populateTree(p);  
    tree->print();
    std::cout<< "\n\n ADD NEW FILE:\n";
    //Create a file inside the working dir and update the tree:
    std::ofstream outfile ("/home/aritz/test/root/sub2/sub2_1/test.txt");
    outfile << "my text here!" << std::endl;
    outfile.close();
    fs::path nf = "/home/aritz/test/root/sub2/sub2_1/test.txt";
    tree->addNode(nf);
    tree->print();

    //Edit node
    std::cout<< "\n\n EDIT FILE:\n";

    std::ofstream outfile2 ("/home/aritz/test/root/sub2/sub2_1/test.txt");
    outfile << "edited text" << std::endl;
    outfile.close();
    tree->nodeChanged(nf, 1);
    tree->print();
    
    //Delete node
    std::cout<< "\n\n DELETE FILE:\n";
    std::filesystem::remove(nf);
    tree->nodeChanged(nf, 2);
    tree->print();
}