#include "../include/mnode.hpp"

/*Default constructor, set the hash functions to the blake3 variant*/
Mnode::Mnode(){
     HashDir = std::bind(&Mnode::_blake3_HashDir, this);
     HashFile = std::bind(&Mnode::_blake3_HashFile, this);
}

Mnode::~Mnode(){
    for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        delete (*it);
    }
}
/* Constructor, set the hash function variant acording to the mode parameter*/
Mnode::Mnode(int mode){
    if (mode==_BLAKE3){
        HashDir = std::bind(&Mnode::_blake3_HashDir, this);
        HashFile = std::bind(&Mnode::_blake3_HashFile, this);       
    }
    if (mode==_SHA256){
        HashDir = std::bind(&Mnode::_sha256_HashDir, this);
        HashFile = std::bind(&Mnode::_sha256_HashFile, this);            
    }
}

void Mnode::addChild(Mnode * ch){
    child_nodes.push_back(ch);
}

void Mnode::setParent(Mnode * p){
    parent = p;
}

void Mnode::print(){
    std::cout << path << ": " << hash << " " << isLeaf << "\n";
        
    for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        (*it)->print();
    }
}

/*Function to calculate and set the hash of a directoy using blake3*/
void Mnode::_blake3_HashDir(){
    std::string value;
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    //For each child, we concatenate the hash
     for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        blake3_hasher_update(&hasher, (*it)->hash.c_str() , strlen((*it)->hash.c_str()));
    }
    //we add the dir path
    blake3_hasher_update(&hasher, path.c_str(),strlen(path.c_str()));

    uint8_t output[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
    CryptoPP::StringSource ss(output, sizeof(output), true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(this->hash)
        ) // HexEncoder
    ); // StringSource

}
/* Function to calculate and set the hash of the file using blake3*/
void Mnode::_blake3_HashFile(){
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

    CryptoPP::StringSource ss(output, sizeof(output), true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(this->hash)
        ) // HexEncoder
    ); // StringSource
}

/*Function to calculate and set the hash of a directoy with sha256*/
void Mnode::_sha256_HashDir(){
    using namespace CryptoPP;
    std::string predata="", value;
    //For each child, we concatenate the hash
     for (std::vector<Mnode *>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++){
        predata.append((*it)->hash);
    }
    //we add the dir path
    predata.append(path);
    SHA256 hash;
    StringSource fs( predata, true /* PumpAll */,
          new HashFilter( hash, 
            new HexEncoder( 
              new StringSink( value )
            ) // HexEncoder
          ) // HashFilter
        ); // FileSource
    this->hash=value;
}

/* Function to calculate and set the hash of the file with sha256*/
void Mnode::_sha256_HashFile(){
    std::ifstream in{path,std::ios::binary};
    using namespace CryptoPP;
    SHA256 hash;
    std::string value;
    try {
    FileSource fs( in, true /* PumpAll */,
          new HashFilter( hash, 
            new HexEncoder( 
              new StringSink( value )
            ) // HexEncoder
          ) // HashFilter
        ); // FileSource
    }
    catch (const CryptoPP::FileStore::ReadErr& e){
        std::cout << "FSError:" << path << "\n";
    }
    this->hash=value;
}

/* Erase and free the specified child*/
void Mnode::deleteChild(Mnode * child){
    child_nodes.erase(std::find(child_nodes.begin(),child_nodes.end(),child));
    delete child;
}
/* Caller function to generate the hash of any node
 * Depending of the type, the correct function will be called. 
 */
void Mnode::genHash(){
    if(type == MT_FILE){

        HashFile(this);
    }
    if(type == MT_DIR)
        HashDir(this);
}
