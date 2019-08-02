#include <stdio.h>
#include <stdlib.h>

typedef struct RBNode{
  struct RBNode *parent, *left, *right;
  char col;
  int val;
} RBNode; 

RBNode* init__RBNode(RBNode *nil){
  RBNode *node;
  node = (RBNode*) malloc(sizeof(RBNode)); 
  node->parent = nil;
  node->left = nil;
  node->right = nil;
  return node;
}

typedef struct RBTree{
  struct RBNode *root;
  struct RBNode *nil;
  int size;
}RBTree;

RBTree* init__RBTree(){
  RBNode *nil;
  nil = (RBNode*) malloc(sizeof(RBNode));
  nil->parent = nil;
  nil->left = nil;
  nil->right = nil;
  nil->col = 'B';
  
  RBTree *tree;
  tree = (RBTree*) malloc(sizeof(RBTree));
  tree->nil = nil;
  tree->root = nil;
  tree->size = 0;
  return tree;
}

RBNode* search(RBTree *tree, int val){
  RBNode *x = tree->root;
  while(x->val != val && x != tree->nil){
    if(x->val > val){
      x = x->left;
    }
    else {
      x = x->right;
    }
  }
  return x;
}

RBNode* treeMinimum(RBTree *tree, RBNode *node){
  if(node->left == tree->nil){
    return node;
  }
  return treeMinimum(tree, node->left);
}

void leftRotate(RBTree *tree, RBNode *x){
  RBNode *y = x->right;
  x->right = y->left; // exchange beta
  if(y->left != tree->nil){
    y->left->parent = x; // If y's left child is a node, set its parent to x.
  }
  y->parent = x->parent; // x's parent becomes y's parent.
  if(x->parent == tree->nil){
    tree->root = y; // X was root, now y is root
  }
  else if(x->parent->right == x){
    x->parent->right = y; // X was a right child, now y is that right child
  }
  else{
    x->parent->left = y; // X was a left child now y is that left child
  }
  y->left = x; // x becomes y's left child
  x->parent = y; // x's parent is y.
  
  
}

void rightRotate(RBTree *tree, RBNode *y){
  RBNode *x = y->left;
  if(x != NULL){
    y->left = x->right;
    if(x->right != tree->nil){
      x->right->parent = y;
    }
    x->parent = y->parent;
    if(y->parent == tree->nil){
      tree->root = x;
    }
    else if(y->parent->right == y){
      y->parent->right = x;
    }
    else {
      y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
  }
   
}

void RBInsertFixup(RBTree *tree, RBNode *node){
  /* Cleans the tree after insertion. */
  while(node->parent->col == 'R'){ // While parent node is red
    if(node->parent == node->parent->parent->left){
      RBNode *y = node->parent->parent->right;
      if (y->col == 'R'){ // CASE 1: Node has a uncle which is not nill and is not black
        node->parent->col = 'B';
        y->col = 'B';
        node->parent->parent->col = 'R';
        node = node->parent->parent;
      }
      else {
        if(node == node->parent->right){ // Case 2: Nodes uncle is black and node is a right child
          node = node->parent; // Change node to the parent
          leftRotate(tree, node); // Rotate node til be the child of its child, node is now a leaf.
        }
        // Case 3:
        node->parent->col = 'B'; // Parents color is set to black
        node->parent->parent->col = 'R'; // grandparents color is set to red
        rightRotate(tree, node->parent->parent); // Move the grandparent down so it becomes the child of the parent. Makes the previous coloring follow the rules.
      }
    }
    else {
      RBNode *y = node->parent->parent->left;
      if (y->col == 'R'){ // Case 1:
        node->parent->col = 'B';
        y->col = 'B';
        node->parent->parent->col = 'R';
        node = node->parent->parent;
      }
      else{
        if(node == node->parent->left){ // Case 2:
          node = node->parent;
          rightRotate(tree, node);
        }
        // Case 3:
        node->parent->col = 'B';
        node->parent->parent->col = 'R';
        leftRotate(tree, node->parent->parent);
      }
    }
    tree->nil->col = 'B';
  }
  tree->root->col = 'B';
}

void RBInsert(RBTree *tree, int val){
  /* Insert new node into the tree with value val.*/
  
  RBNode *node = init__RBNode(tree->nil);
  node->val = val;
  RBNode *y = tree->nil; // lacking node
  RBNode *x = tree->root; // Front node
  while(x != tree->nil){ // Search until the end of the tree.
    y = x; // Update lacking node before updating front node.
    if(x->val > val){ // If val is less than nodes val, go left in the tree
      x = x->left;
    }
    else{ // Else go right in the tree
      x = x->right;
    }
  } // Y is the last node with max one child
  node->parent = y; // t becomes the new node's parent.
  if (y == tree->nil){ 
    tree->root = node; // No nodes in the tree, new node becomes the first one, thus the root.
    node->col = 'B';
  }
  else if(y->val > val){
    y->left = node; // Node is a left child
  }
  else{
    y->right = node; // Node is a right child
  }
  if (node != tree->root){
    node->col = 'R'; // Set color of new node to red.
    RBInsertFixup(tree, node); // Only fix if the new node is not the root.
  }  
  tree->size = tree->size + 1;
}

void RBTransplant(RBTree *tree, RBNode *node1, RBNode *node2){
  //Exchanges node1 with node2. Node1 has to be the child of node2.
  if (node1->parent == tree->nil){
    tree->root = node2;
  }
  else if (node1 == node1->parent->left){
    node1->parent->left = node2;
  }
  else{
    node1->parent->right = node2;
  }
  node2->parent = node1->parent;
}

void RBDeleteFixup(RBTree *tree, RBNode *node){
  /*Cleans the tree after deletion. */
  
  while(node != tree->nil && node->col == 'B'){
    if (node == node->parent->left){ // Node is a left child
      RBNode *sibling = node->parent->right;
      if(sibling->col == 'R'){ //Case 1, nodes sibling is red.
        sibling->col = 'B'; // Set sibling to black, becomes new root
        node->parent->col = 'R'; // Set parent to red
        leftRotate(tree, node->parent); // Rotate so sibling becomes the parent of nodes parent.
      }
      if(sibling->left->col == 'B' && sibling->right->col == 'B'){ // Case 2, sibling is black, and both of its children are black
        sibling->col = 'R'; // Set the color of the sibling to red.
        node = node->parent; // Update node to its parent.
      }
      else{ // Case 3, one of the siblings children are red.
        if (sibling->right->col == 'B'){ // Right child of sibling is red
          sibling->left->col = 'B'; // set Left child color to black
          sibling->col = 'R'; // set the siblings color to red
          rightRotate(tree, sibling); // Left child of sibling becomes the parent of sibling
          sibling = node->parent->right; // Set sibling equal to nodes new sibling.
        }
        // Case 4
        sibling->col = node->parent->col;
        node->parent->col = 'B'; // Set the parent(root) color to black
        sibling->right->col = 'B';
        leftRotate(tree, node->parent); // nodes parent becomes the child of its right child
        node = tree->root; //Set node to the tree root.
      } 
    }
    else{
      RBNode *sibling = node->parent->left;
      if(sibling->col == 'R'){
        sibling->col = 'B';
        node->parent->col = 'R';
        leftRotate(tree, node->parent);
      }
      if(sibling->right->col == 'B' && sibling->left->col == 'B'){
        sibling->col = 'R';
        node = node->parent;
      }
      else{
        if (sibling->left->col == 'B'){
          sibling->right->col = 'B';
          sibling->col = 'R';
          leftRotate(tree, sibling);
          sibling = node->parent->left;
        }
        sibling->col = node->parent->col;
        node->parent->col = 'B';
        sibling->left->col = 'B';
        rightRotate(tree, node->parent);
        node = tree->root;
      } 
    }
  }
  node->col = 'B';
}

void RBDelete(RBTree *tree, int val){
  /* Delets the node with the value val.*/
  RBNode *node = search(tree, val);
  RBNode* y = node;
  RBNode *x;
  char yOrigCol = y->col;
  if(node->left == tree->nil){ // Exchange node with its right child, even if it is nil (Nil get reset later)
    x = node->right;
    RBTransplant(tree, node, node->right);
  }
  else if (node->right == tree->nil){ // Nodes only got a left child, exchange node with its left child.
    x = node->left;
    RBTransplant(tree, node, node->left); // 
  }
  else{ // Nodes got two children
    y = treeMinimum(tree, node->right); // Find the node which value is the first greater then node.
    yOrigCol = y->col;
    x = y->right;
    if (y->parent != node) { // Y is child of node
      RBTransplant(tree, y, y->right); // Transplant y's right child with y
      y->right = node->right; // Set up nodes's child to be y's child
      y->right->parent = y; // Set the child to point to y as its parent istead of node.
    }
    RBTransplant(tree, node, y); // Transplant y with node.
    y->left = node->left; //Setting the left child of node to y.
    y->left->parent = y; // Updating y's child
    y->col = node->col;
  }
  if (yOrigCol == 'B'){
    RBDeleteFixup(tree, x);
  }
  tree->size = tree->size - 1;
  // Reset tree nil:
  tree->nil->parent = tree->nil;
  tree->nil->left = tree->nil;
  tree->nil->right = tree->nil;
  free(node);
}

void RBGetList(RBTree *tree, RBNode *root, int* list, int *index){
  /*
  Variables: 
    tree - Pointer to a tree struct
    root - Pointer to the root of the tree
    list - Array of integers, size 4 * tree->size.
    index - Pointer to the current index in the array. 
  Populates the list vairable with the sorted values of the nodes in the RB-tree.
  */
  if(root->left != tree->nil){
    RBGetList(tree, root->left, list, index);
  }
  if(root != tree->nil){
    list[*index] = root->val;
    *index = *index + 1; // Update value of index pointer
  }
  if(root->right != tree->nil){
    RBGetList(tree, root->right, list, index);
  }
}

int main(){
  RBTree *tree = init__RBTree();
  
  // Testing insertion:
  int arr[] = {-1, 5, -56, 59, 23,10, 11,12,13,14,15, 16, 17};
  for(int i = 0; i < sizeof(arr)/sizeof(int); i++){
    RBInsert(tree, arr[i]);
  }
  
  // Testing Deletion
  RBDelete(tree, 10);  
  
  // Printing the tree as a sorted array
  int *list = (int*) malloc(tree->size*4);
  int index = 0;
  RBGetList(tree, tree->root, list, &index);
  for(int i = 0; i < tree->size; i++){
    printf("%d ", list[i]);
  }
  
  return 0;
}

// Search tree Rules:
// 1. All left childs are less than their parent
// 2. all right childs are greater than their parent.

// RB Tree rules:
// 1. Every node is either red or black
// 2. The root is black
// 3. Every leaf is black
// 4. If a node is red, then both cildren are black
// 5. For each node, all simple paths from the ndoe to descendant leaves contain the same number of black nodes.

