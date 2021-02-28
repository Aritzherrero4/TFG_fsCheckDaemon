#include "../include/mtree.hpp"
using namespace std;

Mtree::Mtree()
{
  n_nodes = 0;
  root_node = new Mnode();
}

// Function to test the output
//To be removed
// void getFsDirList(fs::path path){
//   for(auto& p: fs::recursive_directory_iterator(path)){
//     fs::file_status s = fs::status(p);
//     std::cout << p << "\n";
//   }
// }

int calculateChilds(fs::path path, Mnode *tmp_node)
{
  tmp_node->isLeaf = 0;
  tmp_node->type = MT_DIR;
  int n = 1;
  tmp_node->path = path;

  for (auto &p : fs::directory_iterator(path))
  {
    //We dont want to follow simlinks, can derive in infinite loops
    fs::file_status s = fs::symlink_status(p);
    //fs::file_status ss = fs::status(p);
    //cout << "actual_path: " << p << "\n";

    if (fs::is_directory(s))
    {
      Mnode *d_node = new Mnode();
      n += calculateChilds(p, d_node);
      d_node->setParent(tmp_node);
      d_node->isLeaf = 0;
      d_node->type = MT_DIR;
      tmp_node->addChild(d_node);
      d_node->genHash(); //Add the actual hash;
    }

    if (fs::is_regular_file(s))
    {
      n++;
      Mnode *f_node = new Mnode();
      f_node->setParent(tmp_node);
      f_node->isLeaf = 1;
      f_node->type = MT_FILE;
      f_node->path = p;
      f_node->genHash();
      tmp_node->addChild(f_node);
    }
  }
  return n;
}

void Mtree::populateTree(fs::path path)
{
  this->n_nodes = calculateChilds(path, this->root_node);
  this->root_node->genHash();
  this->root_hash = this->root_node->hash;
}

Mnode * getNodeFromPath(fs::path path, Mnode * root){
        Mnode * tmp_node;
        if (root->path == path)
          return root;
        if (root->type==MT_DIR){
          for (std::vector<Mnode *>::iterator it = root->child_nodes.begin(); it != root->child_nodes.end(); it++){
              tmp_node = getNodeFromPath(path, (*it));
              if(tmp_node != NULL)
                return tmp_node;
          }
        }
        return NULL;
}

void Mtree::addNode(fs::path path ){
  Mnode *new_node = new Mnode();

  fs::file_status s = fs::symlink_status(path);
  if(fs::is_regular_file(s)){
    new_node->type=MT_FILE;
  } else if (fs::is_directory(s)){
    new_node->type=MT_DIR;
  }else{ //Another type of file like links
      return;
  }
  n_nodes++;
  new_node->path=path;
  new_node->setParent(getNodeFromPath(path.parent_path(), root_node));
  new_node->isLeaf=1;
  new_node->genHash();
  new_node->parent->addChild(new_node);

  Mnode * tmp_node = new_node ->parent;
  while(tmp_node != root_node){
    tmp_node->genHash();
    tmp_node=tmp_node->parent;
  }
  tmp_node->genHash(); //update root hash
  root_hash = tmp_node->hash;

}

void Mtree::nodeChanged(fs::path path, int change){
  Mnode * tmp_node = getNodeFromPath(path, root_node);
  Mnode * parent;
  switch(change){
    case 1: //File edited
      tmp_node->genHash();
      break;

    case 2: //File deleted
      n_nodes--;
      parent = tmp_node->parent;
      parent->deleteChild(tmp_node);
      tmp_node = parent;
      break;
  }
  while(tmp_node != root_node){
    tmp_node->genHash();
    tmp_node=tmp_node->parent;
  }
  tmp_node->genHash(); //update root hash
  root_hash = tmp_node->hash;
}

void Mtree::print()
{
  cout << "Number of nodes in the tree:" << this->n_nodes << "\n";
  cout << "root hash: " << this->root_hash << "\n";
  cout << "Tree structure:\n";

  this->root_node->print();
}