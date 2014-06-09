/**
 * @file windowsTree.cpp
 * WindowsTree class implementation.
 */

#include <stdio.h>

#include <windowsTree.h>
#include <window.h>

/* WindowsTree methods */

WindowsTree::WindowsTree() {
  this->root = NULL;
}

Window * WindowsTree::getRoot() {
  return this->root;
}

void WindowsTree::setRoot(Window * newRoot) {
  this->root = newRoot;
}

WindowsTree::IteratorBFS WindowsTree::beginBFS() {
  WindowsTree::IteratorBFS it(this->root);
  return it;
}

WindowsTree::IteratorBFS WindowsTree::beginBFS(Window * root) {
  WindowsTree::IteratorBFS it(root);
  return it;
}

WindowsTree::IteratorBFS WindowsTree::endBFS() {
  Window * win = END_OF_TREE;
  WindowsTree::IteratorBFS it(win);
  return it;
}   

WindowsTree::IteratorDFS WindowsTree::beginDFS() {
  WindowsTree::IteratorDFS it(this->root);
  return it;
}

WindowsTree::IteratorDFS WindowsTree::beginDFS(Window * root) {
  WindowsTree::IteratorDFS it(root);
  return it;
}

WindowsTree::IteratorDFS WindowsTree::endDFS() {
  Window * win = END_OF_TREE;
  WindowsTree::IteratorDFS it(win);
  return it;
}  


/* WindowsTree::IteratorBFS methods */

WindowsTree::IteratorBFS::IteratorBFS(Window * localRoot) {
  this->currentWindow = this->localRoot = localRoot;
  if (this->localRoot != END_OF_TREE && this->localRoot->firstChild != NULL) {
    this->winQueue.push(this->localRoot->firstChild); 
  }
}

WindowsTree::IteratorBFS WindowsTree::IteratorBFS::operator++() {
    /* 
  Breadth First Search algorithm
  We only push the first child of a window because we can access all the other childs by using nextSibling.
  */
  // Go right while you can
   if (this->currentWindow != this->localRoot && this->currentWindow->nextSibling != NULL) { 
    this->currentWindow = this->currentWindow->nextSibling;
  } else {
    if (!this->winQueue.empty()) {
      this->currentWindow = this->winQueue.front();
      this->winQueue.pop();
    } else { 
      // here we're finished
      this->currentWindow = END_OF_TREE;
    }
  }
  // Pushing the first child
  if (this->currentWindow != END_OF_TREE && this->currentWindow->firstChild != NULL) {
      this->winQueue.push(this->currentWindow->firstChild);
  }
  return *this;
}

WindowsTree::IteratorBFS WindowsTree::IteratorBFS::operator++(int) {
  IteratorBFS tmp = *this;
  this->operator++();
  return tmp;
}

Window & WindowsTree::IteratorBFS::operator*() {
  return *(this->currentWindow);
}

Window * WindowsTree::IteratorBFS::operator->() {
  return (this->currentWindow);
}

bool WindowsTree::IteratorBFS::operator==(IteratorBFS it) {
  return (it.currentWindow == this->currentWindow);
}

bool WindowsTree::IteratorBFS::operator!=(IteratorBFS it) {
  return (it.currentWindow != this->currentWindow);
}




/* WindowsTree::IteratorDFS methods */

WindowsTree::IteratorDFS::IteratorDFS(Window * localRoot) {
  this->currentWindow = this->localRoot = localRoot;
}

WindowsTree::IteratorDFS WindowsTree::IteratorDFS::operator++() {
  /* Depth first Search algorithm */
  // go down the tree while you can
  if (this->currentWindow->firstChild != NULL) {
    this->currentWindow = this->currentWindow->firstChild;
  } 
  else if (this->currentWindow->nextSibling != NULL ) { // then try to go right
    this->currentWindow = this->currentWindow->nextSibling;
  } else { // else try to find the next sibling of one of your parent
    while (this->currentWindow != localRoot) {
      this->currentWindow = this->currentWindow->parent;
      if (this->currentWindow->nextSibling != NULL) {
        this->currentWindow= this->currentWindow->nextSibling;
        break;
      }
    }
    if (this->currentWindow == this->localRoot) {
      this->currentWindow = END_OF_TREE;
    }
  }
  return *this;
}

WindowsTree::IteratorDFS WindowsTree::IteratorDFS::operator++(int) {
  IteratorDFS tmp = *this;
  this->operator++();
  return tmp;
}

Window & WindowsTree::IteratorDFS::operator*() {
  return *(this->currentWindow);
}

Window * WindowsTree::IteratorDFS::operator->() {
  return (this->currentWindow);
}

bool WindowsTree::IteratorDFS::operator==(IteratorDFS it) {
  return (it.currentWindow == this->currentWindow);
}

bool WindowsTree::IteratorDFS::operator!=(IteratorDFS it) {
  return (it.currentWindow != this->currentWindow);
}
