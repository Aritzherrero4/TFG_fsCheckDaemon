#include "../external/blake3/blake3.h"
#include "../external/blake3/blake3_impl.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>



#define MT_DIR 2
#define MT_DIR_EMPTY 1
#define MT_FILE 0

namespace fs = std::filesystem;

class Mnode {
    public:
        std::string hash; //Hash of the file/dir
        int type; // A node can be a file or a directory. MT_DIR or MT_FILE
        bool isLeaf;
        fs::path path;
        Mnode * parent;
        std::vector<Mnode *> child_nodes;

        Mnode();
        void addChild(Mnode * ch);
        void setParent(Mnode * p);
        void print();
        void genHash();
        void deleteChild(Mnode * child);
    private:
        void HashFile();
        void HashDir();
};