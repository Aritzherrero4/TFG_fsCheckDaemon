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
    void Mnode::HashDir(){
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
    void Mnode::HashFile(){
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

    void Mnode::deleteChild(Mnode * child){
        child_nodes.erase(find(child_nodes.begin(),child_nodes.end(),child));
        free(child);
    }
    void Mnode::genHash(){
        if(type == MT_FILE)
            HashFile();
 
        if(type == MT_DIR)
            HashDir();

    }
