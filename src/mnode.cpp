#include "../include/mnode.hpp"
Mnode::Mnode(){
}

void Mnode::addChild(Mnode * ch){
    child_nodes.push_back(ch);
}
void Mnode::setParent(Mnode * p){
    parent = p;
}
void Mnode::print(){
    std::cout << path << ": ";
    //if(type==MT_FILE)
        std::cout << hash;
    std::cout << "\n";
        
    for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        (*it)->print();
    }
}
//At first we'll asume all the child hashes are present and already calculated.
// Needs to be improved
/*Function to calculate and set the hash of a directoy*/
void Mnode::HashDir(){
    std::string value;
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    //For each child, we concatenate the hash
     for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        blake3_hasher_update(&hasher, (*it)->hash.c_str() , sizeof((*it)->hash.c_str()));
    }
    //we add the dir path
    blake3_hasher_update(&hasher, path.c_str(),sizeof(path.c_str()));

    uint8_t output[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
    std::ostringstream convert;
    for (int a = 0; a < BLAKE3_OUT_LEN; a++) {
        convert << std::uppercase << std::hex << (int)output[a];
    }
    this->hash=convert.str(); 
}
/* Function to calculate and set the hash of the file*/
void Mnode::HashFile(){
    std::ifstream in{path,std::ios::binary};
    blake3_hasher hasher;

    blake3_hasher_init(&hasher);

    char buf[65536];
    std::streamsize s;

    while(!in.eof()){
       in.read(buf, sizeof(buf));
       s=in.gcount();
       blake3_hasher_update(&hasher, buf, s);     
    }
    uint8_t output[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
    std::ostringstream convert;
    for (int i = 0; i < BLAKE3_OUT_LEN; i++) {
        convert << std::uppercase << std::hex << (int)output[i];
    }
    this->hash=convert.str();
}
/* Erase and free the specified child*/
void Mnode::deleteChild(Mnode * child){
    child_nodes.erase(std::find(child_nodes.begin(),child_nodes.end(),child));
    free(child);
}
/* Caller function to generate the hash of any node
 * Depending of the type, the correct function will be called. 
 */
void Mnode::genHash(){
    if(type == MT_FILE)
        HashFile();
        
    if(type == MT_DIR)
        HashDir();
}
