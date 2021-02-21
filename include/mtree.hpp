#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "../include/mnode.hpp"


class Mtree{
    public: 
        int n_nodes; //Total number of files and dirs
        Mnode *root_node; // Pointer of the top node
        std::string root_hash;

        Mtree();
        void populateTree(fs::path path);
        void print();

};
