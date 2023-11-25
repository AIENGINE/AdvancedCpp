#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;
#define ADDRS_DEBUG 0
// TODO NODE SIZE: node struct can have static counter which keeps track of the total nodes in the list 
typedef struct Node
{
    int data;
    Node* next_node_ptr;
    Node* prev_node_ptr;
}NodeList;

/* 1. Current ref. to head Node (from func param list) of double linked list is used to return changes from the push function
2. ref. head is also used to update the next and prev pointer of the newly allocated struct.
3. Data is always pushed to the front of double linked list, which means if the new node is pushed
the next of this node(current node under scope) should point the old node addrs(updating prev_ptr of the next node). 
And old node prev pointer should point to thenew allocated node addrs 
head_ref acts as an observable pointer that can be shifted to make changes to the links*/
void push_node(NodeList** head_ref, int new_data)
{
    NodeList* new_node = (NodeList*)malloc(sizeof(NodeList));
    new_node->data = new_data;
    new_node->next_node_ptr = *head_ref; // at the start head is pointing to the NULL 
    new_node->prev_node_ptr = NULL;
    // (*head_ref)->prev_node_ptr = new_node;

    if(*head_ref != NULL) //the case where there is already a Node element infront new_node, then the head is actually on the last node that was allocated 
    {
        (*head_ref)->prev_node_ptr = new_node;
   
    }

    
    *head_ref = new_node; //returned new address in the ptr handle, and now *headref is on newly added node


}
/* 
1. Append node at the right end of the DLL opposite to where push_node places the newly created node
Currently, append only append new node infront of the currently passed headref
so push_node shift the *headref to the new node and append keep the
2. In append_node function, after appending the node head points to the node where it was left off, 
i-e, in our case if push_node has been used before the head_ref would point to the begining of the dll by the end of 
append node routine
*/
int append_node(NodeList** head_ref, const int new_data)
{
    NodeList* new_node = (NodeList*)malloc(sizeof(NodeList));
    
    // NodeList* last_node = *head_ref;
    new_node->data = new_data;
    // new_node->next_node_ptr = NULL;


    // Case 0: when head_ref is pointing to the first node in the DLL
    if (*head_ref == NULL)
    {
        new_node->prev_node_ptr = NULL;
        new_node->next_node_ptr = NULL;
        *head_ref = new_node;
        printf("\naddrs original head_ref = %p\n ", head_ref);
        printf("\naddrs contained in head_ref = %p\n ", *head_ref);
        printf("\nvalue when empty = %d\n ", (*head_ref)->data);
        return 1; 
    }

    printf("\naddrs *head_ref = %p\n ", *head_ref);
    printf("\naddrs original head_ref = %p\n ", head_ref); // this addrs is typically on the stack

    NodeList* last_node = *head_ref;

    while(last_node->next_node_ptr != NULL)
    {
        last_node = last_node->next_node_ptr; // this will take us to other end see notes for explanation
    }

    last_node->next_node_ptr = new_node;
    new_node->prev_node_ptr = last_node;
    new_node->next_node_ptr = NULL;
    // in princple in append function head should always point the previous node so any new node can be 
    // appended after that
    // *head_ref = new_node; 

    printf("\nvalue contained in new_node = %d\n ", new_node->data);
    printf("\nvalue pointed to by (*head) = %d\n ", (*head_ref)->data);
    return 1;

}

/* 1. To insert a node after a given node: a new_node has to be allocated
2. given_node->next_node_ptr should point to the newly allocated node, new_node prev_node_ptr should point
to the given_node addrs. And given_node next node's previous node_ptr should point the new_node addrs. which 
was pointing to given node before the insertion */
int insert_after(NodeList* given_node, const int new_data)
{
    //Check that given_node should not point to NULL
    if(given_node == NULL)
    {
        fprintf(stderr, "given node can not be NULL");
        return 1;
    }
    NodeList* new_node = (NodeList*)malloc(sizeof(NodeList));
    new_node->data = new_data;
    
    new_node->next_node_ptr = (given_node)->next_node_ptr; //check the next_node_ptr should not be NULL
    (given_node)->next_node_ptr = new_node;
    new_node->prev_node_ptr = given_node;

    if(new_node->next_node_ptr != NULL)
    {
        new_node->next_node_ptr->prev_node_ptr = new_node;
    }


    return 1;

}

// Used by delete_node_pos
int delete_node(NodeList** head_ref, NodeList* node_tobe_deleted)
{
    /* base case */
    if (*head_ref == NULL || node_tobe_deleted == NULL)
    {
        printf("\ninvalid node deletion parameter\n");
        return 1;
    }
 
    /* If node to be deleted is head node */
    if (*head_ref == node_tobe_deleted)
        *head_ref = node_tobe_deleted->next_node_ptr;
 
    /* Change next only if node to be deleted is NOT
       the last node */
    if (node_tobe_deleted->next_node_ptr != NULL)
        node_tobe_deleted->next_node_ptr->prev_node_ptr = node_tobe_deleted->prev_node_ptr;
 
    /* Change prev only if node to be deleted is NOT
       the first node */
    if (node_tobe_deleted->prev_node_ptr != NULL)
        node_tobe_deleted->prev_node_ptr->next_node_ptr = node_tobe_deleted->next_node_ptr;
 
    /* Finally, free the memory occupied by node_tobe_deleted*/
    free(node_tobe_deleted);
    return 1;
}


//Delete node in DLL: delete the node at the specific pos or with a specific data(implment a search in DLL function)
int delete_node_pos(NodeList** head_ref, int node_pos)
{
    // if list is null or invalid node pos is given 
    if(*head_ref == NULL || node_pos < 0)
    {
        printf("invalid selection for node deletion");
        return 1;
    }

    //if the node to be deleted is the first node
    if (*head_ref != NULL && (*head_ref)->next_node_ptr == NULL && (*head_ref)->prev_node_ptr == NULL)
    {
        free(*head_ref);
        *head_ref = nullptr;
        return 1;
    }
    // TODO: track the added and appended node position so that checks can be placed
    NodeList* current_node = *head_ref; //just to make code readable
    for (int i = 0; current_node != NULL && i < node_pos; i++) 
    {
        current_node = current_node->next_node_ptr;
    }
     
    //TODO NODE SIZE: Node size should be known before hand so that out of bound case doesn't cause problems  it will make checking easy
    //if node_pos specified is greater then the number of nodes existed in DLL
    if (current_node == NULL) //then current_node will definitely go out of bound and will point to NULL
    {
        printf("\nnode doesn't exist that matches the node_pos parameter\n");
        return 1; 
    }

    delete_node(head_ref, current_node);
    return 1;
}



/* 1. If the current head node addrs. does not equal to NULL then current data is printed and current 
addrs is assigned to the temporary ptr(last_ptr). Node ptr is then moved to next addrs that it contains. The 
loop continues.
2.   */
void print_list(NodeList* node)
{
    NodeList* last_node_ptr;
    printf("\nTraversal in the forward direciton\n");
    while(node != NULL)
    {
        printf("\naddrs of node before change = %p\n ", node);
        printf("\ndata = %d\n", node->data);
        last_node_ptr = node;
        node = node->next_node_ptr;
        printf("\naddrs of node after change = %p\n ", node);
    }

    printf("\nTraversal in the backward direciton\n");
    while(last_node_ptr != NULL)
    {
        printf("\ndata = %d\n", last_node_ptr->data);
        last_node_ptr = last_node_ptr->prev_node_ptr;
    }


}

void free_all_nodes(NodeList* node)
{
    // mix of print and delete node logic

}

int main(int argc, char** argv)
{

    NodeList* head = nullptr;
    printf("\naddrs original head ptr = %p\n ", &head); // this address will be on the stack
    std::cout << "addrs in head = "<< static_cast<void*>(head) <<std::endl;
    push_node(&head, 5); // from this point onwards notice the change in the address space it is now heap
    std::cout << "addrs in head = "<< static_cast<void*>(head) <<std::endl;
    std::cout << "pointing value = "<< head->data <<std::endl;
    push_node(&head, 4);
    std::cout << "addrs in head = "<< static_cast<void*>(head) <<std::endl;
    std::cout << "pointing value = "<< head->data <<std::endl;
    
    append_node(&head, 6); // will be appended after 5
    append_node(&head, 7);

    push_node(&head, 3);
    std::cout << "addrs in head = "<< static_cast<void*>(head) <<std::endl;
    std::cout << "pointing value = "<< head->data <<std::endl;
    push_node(&head, 2);
    std::cout << "addrs in head = "<< static_cast<void*>(head) <<std::endl;
    std::cout << "pointing value = "<< head->data <<std::endl;

    append_node(&head, 8); // will be appended after 7 and current *head_ref will remain at node which has 2 or the last pushed node
    append_node(&head, 9);

    print_list(head);


    // // print_list(head); //check the node addrs if it has been changed by the function
    // push_node(&head, 1);
    // push_node(&head, 2);
    // printf("\naddrs of head = %p\n ", head);
    // // append_node(&head,4);
    // // append_node(&head,5);
    // insert_after(head->next_node_ptr, 9);
    // print_list(head); //check the node addrs if it has been changed by the function
    // delete_node_pos(&head, 2);
    // printf("\naddrs of head = %p and Data =  %d\n ", head, head->data);
    // print_list(head);
    return 1;
}