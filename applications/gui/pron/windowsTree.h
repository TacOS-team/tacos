#ifndef __WINDOWS_TREE_H__
#define __WINDOWS_TREE_H__ 

/**
 * @file windowsTree.h
 * WindowsTree class definition.
 */

#include <queue>
#define END_OF_TREE NULL /**< Indicates that we have reached the end of the tree */

class Window;

using namespace std;

/**
 * @brief WindowsTree class definition. 
 * the windowsTree is the structure that pron uses to manage all the pronWindows
 */
class WindowsTree {
  public:
    /**
     * @brief IteratorBFS class definition. 
     * Iterates on the windowsTree performing a Breadth First Search algorithm
     */
    class IteratorBFS {
      public:
        /**
         * IteratorBFS constructor
         * @param win The window from where we start discovering the windowsTree
         */
        IteratorBFS(Window * win);

        /** 
         * Pre incrementation 
         * @return The new Iterator
         */
        IteratorBFS operator++(); 

        /**
         * Post incrementation
         * @return The same Iterator, which will be incremented
         */
        IteratorBFS operator++(int);

        /** 
         * Operator *
         * @return A reference to the current window
         */
        Window & operator*();

        /**
         * Operator ->
         * @return the value of the current window
         */
        Window * operator->();

        /**
         * Operator == 
         * performs a comparison between two IteratorBFS
         * @param it The iterator to be compared to this
         * @return the result of the operation
         */
        bool operator==(IteratorBFS it);

        /**
         * Operator != 
         * performs a comparison between two IteratorBFS
         * @param it The iterator to be compared to this
         * @return the result of the operation
         */
        bool operator!=(IteratorBFS it);

      private:
        Window * localRoot; /**< The window from where we start discovering the tree (by default : the root of the windowsTree) */
        Window * currentWindow; /**< The window that will be return by a call to *Iterator */
        queue<Window *> winQueue; /**< The queue of the windows not yet covered (actually we only save the first child of their parent) */
    };

    /**
     * @brief IteratorDFS class definition. 
     * Iterates on the windowsTree performing a Depth First Search algorithm
     */
    class IteratorDFS {
      public:
        /**
         * IteratorDFS constructor
         * @param win The window from where we start discovering the windowsTree
         */
        IteratorDFS(Window * win);

        /** 
         * Pre incrementation 
         * @return The new Iterator
         */
        IteratorDFS operator++(); 

        /**
         * Post incrementation
         * @return The same Iterator which will be incremented
         */
        IteratorDFS operator++(int);

        /** 
         * Operator *
         * @return A reference to the current window
         */
        Window & operator*();

        /**
         * Operator ->
         * @return the value of the current window
         */
        Window * operator->();

        /**
         * Operator == 
         * performs a comparison between two IteratorDFS
         * @param it The iterator to be compared to this
         * @return the result of the operation
         */
        bool operator==(IteratorDFS it);

        /**
         * Operator != 
         * performs a comparison between two IteratorDFS
         * @param it The iterator to be compared to this
         * @return the result of the operation
         */
        bool operator!=(IteratorDFS it);

      private:
        Window * localRoot; /**< The window from where we start discovering the tree (by default : the root of the windowsTree) */
        Window * currentWindow; /**< The window that will be return by a call to *Iterator */
    };

    /**
     * Constructor of a WindowsTree
     */
    WindowsTree ();

    /**
     * Get the root window of the tree
     * @return The root window of the tree
     */
    Window * getRoot();

    /**
     * Set a new root window
     * @param newRoot The new root window
     */
    void setRoot(Window * newRoot);

    /**
     * Get the beginning of the tree for a BFS iterator
     * @return the ItertorBFS corresponding to the begginning of the tree
     */
    IteratorBFS beginBFS();

    /**
     * Get the beginning of the tree for a BFS iterator that will start to discover the tree from root
     * @param root The window from where the windows tree will be discovered
     * @return the ItertorBFS corresponding to the begginning of the tree
     */ 
    IteratorBFS beginBFS(Window * root);

    /**
     * Get the end of the tree for a BFS iterator
     * @return the IteratorBFS corresponding to the end of the tree
     */
    IteratorBFS endBFS();

    /**
     * Get the beginning of the tree for a DFS iterator
     * @return the ItertorBFS corresponding to the begginning of the tree
     */
    IteratorDFS beginDFS();

    /**
     * Get the beginning of the tree for a DFS iterator that will start to discover the tree from root
     * @param root The window from where the windows tree will be discovered
     * @return the ItertorBFS corresponding to the begginning of the tree
     */ 
    IteratorDFS beginDFS(Window * root);

    /**
     * Get the end of the tree for a DFS iterator
     * @return the IteratorBFS corresponding to the end of the tree
     */
    IteratorDFS endDFS();

  private:
    Window * root; /**< The root window of the tree */
};

#endif
