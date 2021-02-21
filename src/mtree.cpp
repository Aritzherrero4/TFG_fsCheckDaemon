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

void Mtree::print()
{
  cout << "Number of nodes in the tree:" << this->n_nodes << "\n";
  cout << "root hash: " << this->root_hash << "\n";
  cout << "Tree structure:\n";

  this->root_node->print();
}