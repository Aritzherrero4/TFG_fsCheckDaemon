#include <cryptopp/sha.h> 
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>

#include "../external/blake3/blake3.h"
#include "../external/blake3/blake3_impl.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include <functional>

#define MT_DIR 2
#define MT_DIR_EMPTY 1
#define MT_FILE 0

#define _BLAKE3 0
#define _SHA256 1

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
        Mnode(int mode);
        void addChild(Mnode * ch);
        void setParent(Mnode * p);
        void print();
        void genHash();
        void deleteChild(Mnode * child);
    private:
        void _blake3_HashFile();
        void _blake3_HashDir();
        void _sha256_HashFile();
        void _sha256_HashDir();
        std::function<void(Mnode *)> HashDir;
        std::function<void(Mnode *)> HashFile;
};