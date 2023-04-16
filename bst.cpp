#include <iostream>
#include <string>
#include <memory>


struct TreeNode
{
    uint64_t key;
    TreeNode* parent_node; // to find successor or predecessor use to fulfill the requirements of delete node in BSTree
    std::unique_ptr<TreeNode> left_node;
    std::unique_ptr<TreeNode> right_node;
    TreeNode(uint64_t data_as_key): key(data_as_key), left_node(nullptr), right_node(nullptr)
    {}
};

/*
TODO
0. Reject any duplicate key
1. Bst can be created empty
2. Insert: if the tree is empty then create the TreeNode with left and right nodes empty
3. if there is already a node present compare the key with the already present node object's key in the BSTree
meaning current object underconsideration (object pointed to by the this pointer). The idea here is to find out
nullptr either on the left or right childs of already present nodes based on the key comparison. The key comaprison
is essentially fulfiling the BST property and placing/creating the nodes at the right places.
To implement right placement node traversing the BST can be done recursively. 
*/

class BSTree
{
    public:
    BSTree(): tree_node(nullptr) {};
    ~BSTree() = default;
    void insert_key(uint64_t key);
    void print_nodes();
    void inorder_traversal();
    void delete_key(uint64_t key_to_be_deleted);
    private:
    void delete_node(std::unique_ptr<TreeNode>& node, uint64_t key);
    std::unique_ptr<TreeNode> find_min(std::unique_ptr<TreeNode>& node); // with BSTree property min val will lie on the left subtree always useful to find successor in the right subtree as well
    void inorder(std::unique_ptr<TreeNode>& node);
    std::string tree_nodes_as_string(std::unique_ptr<TreeNode>& node);
    void insert(uint64_t key, std::unique_ptr<TreeNode>& node);
    std::unique_ptr<TreeNode> tree_node; // every TreeNode is a unique resource

};

void BSTree::insert_key(uint64_t key)
{
    this->insert(key, this->tree_node);

}

void BSTree::insert(uint64_t key, std::unique_ptr<TreeNode>& node)
{
    // The case where there is no node from the start or when node place is found as child of the root underconsideration
    // in all these above cases node default allocation is nullptr
    if (node == nullptr)
    {
        node = std::make_unique<TreeNode>(key);
 
    }
    else
    {
        // case when key < current node's key so left side of the root node is underconsideration
        if (key < node->key)
        {
            this->insert(key, node->left_node);
        }
        // case when key > current node's key so right side of root node is underconsideration
        if (key > node->key)
        {
            this->insert(key, node->right_node);
        }
        if (key == node->key)
        {
            std::cout<< "duplicate keys are not allowed"<<std::endl;
            return;
        }


    }

}

std::string BSTree::tree_nodes_as_string(std::unique_ptr<TreeNode>& node)
{
    std::string left_node_string = (node->left_node == nullptr) ? "{}": tree_nodes_as_string(node->left_node);
    std::string right_node_string = (node->right_node == nullptr) ? "{}": tree_nodes_as_string(node->right_node);
    std::string tree_string = "{" + std::to_string(node->key) + ", "+ left_node_string + ", "+ right_node_string + "}";
    return tree_string;

}

void BSTree::print_nodes()
{
    if (this->tree_node == nullptr)
    {
        std::cout<< "{}"<<std::endl;
    }
    else
    {
        std::cout<< this->tree_nodes_as_string(this->tree_node) << std::endl;

    }

}

void BSTree::inorder(std::unique_ptr<TreeNode>& node)
{
    // recursion base case
    if (node == NULL)
        return;
    inorder(node->left_node);
    std::cout<< node->key << " ";
    inorder(node->right_node);
    
}
// sorting the BSTree in ascending order, for descending order visit right subtree first
// Can be used to delete a node that has two children(left and right subtrees)
void BSTree::inorder_traversal()
{
    // exception case can't call when tree is empty
    if (this->tree_node == nullptr)
    {
        std::cout<< "can't traverse empty tree"<<std::endl;
        return;
    }
    else
    {

        this->inorder(this->tree_node);
        std::cout<< std::endl;
    }
}

// traversing complete left
std::unique_ptr<TreeNode> BSTree::find_min(std::unique_ptr<TreeNode>& node)
{
    // covers the exception case and findmin can be used independently of other class methods
    if (node == nullptr) // start node is NULL

        return nullptr;
    if (node->left_node == nullptr) // we have traverse left depth
    {
        return std::move(node); // the node key now contains
    }    
    else
    {
        return find_min(node->left_node);

    }

}
// TODO: Current implementation does not handle slightly or completely skewed BSTree cases, 
// requires successor and predecessor with parent tracking
void BSTree::delete_node(std::unique_ptr<TreeNode>& node, uint64_t key)
{
    if (node == nullptr)
    {
        std::cout<< "couldnt find the key"<<std::endl;
        return;
    }

    else //this will run in any of the above cases
    {
        
        if (node->key == key )
        {
            // node to be deleted has no child it is lead node case
            if (node->left_node == nullptr && node->right_node == nullptr)
            {
                std::cout<< "node: "<< node.get() << " with key: "<<node->key << "deleted "<<std::endl;
                node.release();
                return;
            }    

            // node only has right child
            if (node->left_node == nullptr && node->right_node != nullptr)
            {
                node = std::move(node->right_node);
                return;
            }
            
            // node only has left child
            if (node->right_node == nullptr && node->left_node != nullptr)
            {
                node = std::move(node->left_node);
                return;
            }

            if(node->left_node != nullptr && node->right_node != nullptr)
            {
                std::cout<< "inorder_successor node case: \n" << " node addr: " << node.get() << " with key: "<<node->key << " replaced ";
                auto inorder_successor = this->find_min(node->right_node);
                node->key = inorder_successor->key;
                std::cout<< " with: "<< inorder_successor->key <<std::endl;
                inorder_successor.release();
                return;

            }
            // shouldn't run 
            else
            {
                std::cout<< "invalid case has not been handled"<<std::endl;
                return;
            }


        }
        if (node->key > key)
        {
            this->delete_node(node->left_node, key);
        }

        if (node->key < key )
        {
            this->delete_node(node->right_node, key);
        }


    }

}

void BSTree::delete_key(uint64_t key_to_be_deleted)
{
    
    if (this->tree_node == nullptr)
    {
        std::cout<< "can't delete empty tree"<<std::endl;
        return;
    }
    this->delete_node(this->tree_node, key_to_be_deleted);
    // del node has no sub trees 
    
    // del node has only one child then delete the current node with key and replace it with a child

    // del node has left and right subtrees in that case get the inorder successor 
}

int main()
{
    
    BSTree t_bstree{};
    t_bstree.print_nodes();
    t_bstree.insert_key(10);
    t_bstree.insert_key(12);
    t_bstree.insert_key(8);
    t_bstree.insert_key(6);
    t_bstree.insert_key(7);
    t_bstree.insert_key(14);
    t_bstree.insert_key(11);
    t_bstree.insert_key(13);
    t_bstree.print_nodes();
    t_bstree.inorder_traversal();
    t_bstree.delete_key(10);
    t_bstree.print_nodes();
    


    return 0;
}