#include <bits/stdc++.h>

#ifdef NDEBUG // Typically release mode defines NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M"\n", __FILE__, __LINE__, __VA_ARGS__)
#endif

#ifdef BST_RECURSION_MODE
#define BST_RECURSION_MODE 1
#else
#define BST_RECURSION_MODE 0
#endif

// Managing BSTree methods are in the following order because some function have dependencies on the other
// Insert + print (3 cases)
// Apply RAII idiom for recursively delete nodes in the tree so that no memory leakage is possible
// Traversal inorder is a must than pre-order to create a tree copy and sorting a tree
// for post order traversal and all traversal reference:
// https://www.enjoyalgorithms.com/blog/iterative-binary-tree-traversals-using-stack
// Search the key in the BSTree (3 cases)
// Find max and min key
// check if the BStree is bal
// convert to balanc tree in a simple way
// Cmake option/macro driven to switch between iterative and recursive implementation
// Find successor and predecessor (3 cases)
// Delete the node with the key

namespace BinaryTree
{

    struct TreeNode
    {
        int key;
        static int num_nodes;
        TreeNode* parent_node;
        TreeNode* left_node;
        TreeNode* right_node;
        TreeNode(int data_as_key): key(data_as_key), parent_node(nullptr), left_node(nullptr), right_node(nullptr)
        {
            num_nodes = num_nodes+1;
        }
        
    };
    int TreeNode::num_nodes = 0;
    static int count_idx; // index used during traversal
    static auto create_tree_node = [](auto data){  return new TreeNode(data); };

    struct BSTree
    {
        BSTree(int data) noexcept: m_node(new TreeNode(data))
        {
            debug("ctor called on %p with value %d\n", m_node, m_node->key);
        };
        void insert(int data_as_key);
        // void inorder_traversal(int* inorder_arr);
        void inorder_traversal(std::vector<int>&);
        void preorder_traversal(std::vector<int>&);
        void print_tree();
        bool search_key(int key);
        int find_max_value();
        int find_min_value();
        ~BSTree()
        {
            destroy_recursive(m_node);
        }
        private:
        int min_value_node(TreeNode*); // expect correct answers only on balanced BST
        int max_value_node(TreeNode*); // expect correct answers only on balanced BST
        TreeNode* search(int, TreeNode*);
        void preorder(TreeNode* node, std::vector<int>& preorder_arr);
        // void inorder(TreeNode* node, int* inorder_arr);
        void inorder(TreeNode* node, std::vector<int>& inorder_arr); // inorder_arr to get the result
        void destroy_recursive(TreeNode*);
        std::string print_tree_as_string(TreeNode*);
        TreeNode* insert_node(int data_as_key, TreeNode* node) noexcept;
        TreeNode* m_node;
    };

    // part of the destructor of the class
    void BSTree::destroy_recursive(TreeNode* node)
    {
        if (node)
        {
            destroy_recursive(node->left_node);
            destroy_recursive(node->right_node);
            debug("dtor called on %p with value %d\n", node, node->key); // this is actual place in the function call stack when nodes are destroyed
            delete node;
        }
    }

    void BSTree::print_tree()
    {
        auto bst_string = print_tree_as_string(m_node);
        debug("BSTree as string: %s\n", bst_string.c_str());
        // std::cout<< bst_string<<"\n";
    }

    std::string BSTree::print_tree_as_string(TreeNode* node)
    {
        std::string left_node_string = (node->left_node == nullptr) ? "{}" : print_tree_as_string(node->left_node);
        std::string right_node_string = (node->right_node == nullptr) ? "{}" : print_tree_as_string(node->right_node);
        std::string tree_string = "{" + std::to_string(node->key)  + ", " + left_node_string + ", " + right_node_string + "}" ;
        return tree_string;
    }

    // inorder traversal sorts the BST
    void BSTree::inorder_traversal(std::vector<int>& inorder_arr)
    {
        inorder(m_node, inorder_arr);
    }

    
    void BSTree::inorder(TreeNode* node, std::vector<int>& inorder_arr)
    {
        
        // // if root node is NULL which will not be the for current impl 
        // if (node == nullptr)
        // {
        //     debug("there is no BSTree for inorder traversal %p\n", node);
        //     return;
        // }
        
        // if (node->left_node)
        // {
        //     inorder(node->left_node, inorder_arr);
        // }
        // // *(inorder_arr + count_idx)  = node->key; //when using array to store back inorder
        // debug("node key: %d : %d \n", node->key, count_idx);
        // count_idx++;
        // inorder_arr.emplace_back(node->key);
        // if (node->right_node)
        // {
        //     inorder(node->right_node, inorder_arr);
        // }

        std::stack<TreeNode* >node_stack;
        TreeNode* current_node{nullptr}; // for node traversal by assignment during while loop routine
        current_node = node; // In BST we start with the rootnode
        while(current_node != nullptr || node_stack.empty() == false) // the while loop cover the right node traversal as well
        {

            if(current_node)
            {
                node_stack.push(current_node);
                current_node = current_node->left_node;
            }
            else
            {
                current_node = node_stack.top();
                node_stack.pop();
                debug("inorder node key: %d : %d \n", current_node->key, count_idx);
                count_idx++;
                inorder_arr.emplace_back(current_node->key);
                current_node = current_node->right_node;
            }

        }
        std::cout<< "inorder conditions completed\n";
        return;
    }

    // To get the exact sequence that is used to build the BST, in other words it can be used to copy the BST
    void BSTree::preorder_traversal(std::vector<int>& preorder_arr)
    {
        preorder(m_node, preorder_arr);
    }

    void BSTree::preorder(TreeNode* node, std::vector<int>& preorder_arr)
    {
        
        // debug("preorder node key: %d : %d \n", node->key, count_idx);
        // count_idx++;
        // preorder_arr.emplace_back(node->key);
        // if (node->left_node)
        // {
        //     preorder(node->left_node, preorder_arr);
        // }
        // if (node->right_node)
        // {
        //     preorder(node->right_node, preorder_arr);
        // }
        // return;
        
        std::stack<TreeNode* >node_stack;
        TreeNode* current_node{nullptr};

        current_node = node;
        while (current_node != nullptr || node_stack.empty() ==  false)
        {
            
            if(current_node)
            {
                preorder_arr.emplace_back(current_node->key);
                debug("preorder node key: %d : %d \n", current_node->key, count_idx);
                count_idx++;
                node_stack.push(current_node);
                current_node = current_node->left_node;
                
            }
            else
            {
                current_node = node_stack.top();
                node_stack.pop();
                current_node = current_node->right_node;
            }
        }
        std::cout<< "preorder conditions completed\n";
        return;
    }


    void BSTree::insert(int data_as_key)
    {
        insert_node(data_as_key, m_node); // always root node is passed because Tree develops chained links from root
    }

    TreeNode* BSTree::insert_node(int data_as_key, TreeNode* node) noexcept
    {
// #if BST_RECURSION_MODE         
        // if(node == nullptr)
        // {
        //     node = new TreeNode(data_as_key);
        //     debug("node %p with value %d created \n", node, node->key);
            
        // }
        // else
        // {
        //     if (data_as_key > node->key)
        //     {
        //         node->right_node = this->insert_node(data_as_key, node->right_node);
        //         node->right_node->parent_node = node;

        //     }
        //     if (data_as_key < node->key)
        //     {
        //         node->left_node = this->insert_node(data_as_key, node->left_node);
        //         node->left_node->parent_node = node;
        //     }
        //     if (data_as_key == node->key)
        //     {
        //         debug("Duplicate key is not allowed current key %d: insert key %d\n", node->key, data_as_key);
        //         return node;
        //     }
        // }
        // return node;
// #else
        // TreeNode* current_node{nullptr}; // use to traversing left and right child nodes
        // TreeNode* comp_node{nullptr}; // for understanding clearly other node* in the function parameter is also fine
        // bool right_place{false};
        // bool left_place{false};
        // if (node == nullptr) // assuming root node is already present due to ctor should never succeed in current impl
        // {
        //     debug("can't accept start null node %p ", current_node);
        //     return node;
        // }
        // else
        //     current_node = node;

        // while (current_node != nullptr)
        // {
        //     comp_node = current_node;

        //     if (data_as_key > comp_node->key)
        //     {
        //         current_node = comp_node->right_node;
        //         right_place = true;
        //     }
        //     if (data_as_key < comp_node->key )
        //     {
        //         current_node = comp_node->left_node;
        //         left_place = true;
        //     }

        //     if (current_node == nullptr)
        //     {

        //         if (right_place)
        //         {
        //             current_node = create_tree_node(data_as_key);
        //             comp_node->right_node = current_node;
        //             current_node->parent_node = comp_node; 
        //             right_place = false;
        //             debug("node %p with value %d created \n", current_node, current_node->key);
        //             return node;
        //         }
        //         if (left_place)
        //         {
        //             current_node = create_tree_node(data_as_key);
        //             comp_node->left_node = current_node;
        //             current_node->parent_node = comp_node;
        //             left_place = false;
        //             debug("node %p with value %d created \n", current_node, current_node->key);
        //             return node;

        //         }
        //     }

        //     if ( data_as_key == current_node->key )
        //     {
        //         debug("Duplicate key is not allowed current key %d: insert key %d\n", current_node->key, data_as_key);
        //         return node;
        //     }
        //     left_place = false;
        //     right_place = false;

        // }
        // debug("control block reaches null ptr value %p \n", node);
        // return node;

        TreeNode* current_node{nullptr}; // use to traversing left and right child nodes
        bool right_place{false};
        bool left_place{false};
        if (node == nullptr) // assuming root node is already present due to ctor this routine should never true in current impl
        {
            debug("can't accept start null node %p ", current_node);
            return node;
        }
        else
            current_node = node;

        while (current_node != nullptr)
        {
            node = current_node;

            if (data_as_key > node->key)
                current_node = (node->right_node == nullptr) ? (node->right_node = create_tree_node(data_as_key)) : (node->right_node);

            if (data_as_key < node->key )
                current_node = (node->left_node == nullptr) ? (node->left_node = create_tree_node(data_as_key)) : (node->left_node);

            if (current_node->parent_node == nullptr) // it means it is a newly created node, now where it came from? definitely parent is not set
            {
                current_node->parent_node = node;
                debug("node %p with value %d created \n", current_node, current_node->key);
                return node;
            }

            if ( data_as_key == current_node->key )
            {
                debug("Duplicate key is not allowed current key %d: insert key %d\n", current_node->key, data_as_key);
                return node;
            }

        }
        debug("control block reaches null ptr value %p \n", node);
        return node;                
    }

    bool BSTree::search_key(int key)
    {
       auto node = search(key, m_node);
       if (node)
       {
            return true;
       }
       else
       {
            return false;
       }
    }

    TreeNode* BSTree::search(int key, TreeNode* node)
    {
        // called with no node
        if (node == nullptr)
        {
            debug("key not found or no BST child found %p\n", node);
            return node;
        }
        
        if (key == node->key)
        {
            return node;
        }

        if (key < node->key)
        {
            return search(key, node->left_node);
        }
        return search(key, node->right_node); // search the right BST anyway
    }

    // assumption is the valid BSTree starts with a some value given to the default ctor also it is valid BST
    int BSTree::find_min_value()
    {
        auto min_value = min_value_node(m_node);
        return min_value;    
    }

    int BSTree::min_value_node(TreeNode* node)
    {
        // // recursion case when node under
        // if (node == nullptr)
        // {
        //     debug("no BST left for search %p", node);    
        //     return node->key;
        // }

        if(node->left_node == nullptr)
            return node->key;
        else
           return min_value_node(node->left_node);
    }

    int BSTree::find_max_value()
    {
        auto max_value = max_value_node(m_node);
        return max_value;    
    }

    int BSTree::max_value_node(TreeNode* node)
    {

        if(node->right_node == nullptr)
            return node->key;
        else
           return max_value_node(node->right_node);
    }

    

}

// {4,5,1,7,6} unbalanced
// {10, 6, 4, 8, 14, 12, 16}

int main()
{
    BinaryTree::BSTree bstree{10};
    bstree.insert(6);
    bstree.insert(4);
    bstree.insert(8);
    bstree.insert(14);
    bstree.insert(12);
    bstree.insert(16);
    bstree.print_tree();
    std::cout<< "number of nodes: "<< BinaryTree::TreeNode::num_nodes<<"\n";
    std::vector<int> result_vec{};
    // int* result_arr = new int[BinaryTree::TreeNode::num_nodes];
    // bstree.inorder_traversal(result_arr);
    // std::cout<< result_arr[4]<<"\n";
    bstree.inorder_traversal(result_vec);
    std::copy(result_vec.begin(), result_vec.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout<<std::endl;
    result_vec.clear();
    BinaryTree::count_idx = 0;
    bstree.preorder_traversal(result_vec);
    std::copy(result_vec.begin(), result_vec.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout<<std::endl;

    auto search_key{7};
    if(bstree.search_key(search_key))
        debug("key found %d ", search_key);
    else
        debug("key not found %d ", search_key);

    auto minval = bstree.find_min_value();
    std::cout<< "min value: "<<  minval<<"\n";

    auto maxval = bstree.find_max_value();
    std::cout<< "max value: "<<  maxval<<"\n";



   
    return 0;

}
