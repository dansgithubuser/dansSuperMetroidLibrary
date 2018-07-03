
// This slows things down a lot but can be useful for debugging.
// It checks data integrity before and after each operation.
#define CHECK_INTEGRITY_btree_h 0


/****************************************************************************
 * btree.h
 * STL-compatible binary search tree class
 *
 * (c) Daniel Papenburg  2006
 *  
 * Tested on the following compilers:
 *	> Microsoft Visual C++ 6.0
 *  > Dev-Cpp version 4.9.5.0 
 *  > MinGW, g++ version 2.95.3-6
 *  > g++ 2.95.3-5
 *
 * Tested on the following operating systems:
 *  > Microsoft Windows 2000
 *  > Microsoft Windows XP
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Words from the author
 *
 * Hello, and thank you for expressing interest in my binary tree 
 * implementation. This file is the culmination of my best efforts over the
 * past few months to make a standardized binary tree template structure which
 * conforms to STL algorithms, procedures, and iterators. In STL terminology,
 * the class uses a forward mutable iterator, which may be initialized to the
 * beginning of the set, and incremented to pass over each element exactly
 * once. Development of a bidirectional iterator is being considered.
 *
 * This file is open source. You may use it and modify it to suit your needs.
 * I would appreciate a little credit if you use my code. Simply put my
 * name somewhere in your credits or documentation. And let me know so I
 * can put it on my resume. email dfpercush@gmail.com
 *
 * I started this project with several goals in mind. The first and foremost
 * was to re-invent the way binary trees were traditionally treated as the
 * textbook illustration of recursive algorithms. This library proves that
 * recursion is not necessary to create and manipulate a binary tree. 
 * Consequently, it may be used for manipulating extremely large databases
 * or lists without fear of stack overflow. Embedded designers may find this
 * feature particularly useful for system with low amounts of RAM. I admit,
 * C++ template code tends to produce larger code than a handwritten C 
 * algorithm. However, most of this template code can go into ROM, thus the 
 * savings for stack space in percent RAM consumed would be significant. 
 * If you feel that template code is too much, you may try to edit this source.
 * Do a text search and replace on this file, substituting a base type for 'T'.
 * (T is used for all template types, there is only one) thus resulting
 * in a non-template class for a particular type. The point is, you can 
 * achieve the speed of a binary tree without some of the memory requirements.
 * Three pointers are still reqired for each node, so this point is really
 * just academic. But it was fun to figure out how to make an iterator which
 * doesn't need an entire stack of state information. *shrug*
 *
 * The second goal was to make a storage class for extremely large data sets
 * which is fast, portable, reliable, and intuitive. Binary trees are well
 * known for their operational speed. Moreover, the red-black algorithms used
 * in this code ensure a roughly balanced tree, such that worst case scenario
 * is at most 2 times slower than the best possible case. The portability 
 * factor of course is achieved through the use of generic template code.
 * Reliability is an ongoing experiment. As of this writing, no bugs have been
 * discovered in the class. As far as intuition goes, anyone who can use STL
 * code should be able to use this class with ease.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Using class BTree:
 *
 * First let's examine an existing STL structure, the list.
 * The following sample code illustrates using the STL class <list>
 * which stores a set of rectangles 
 * (struct RECT {long left long top, long right, long bottom};)
 *
 * #include <list>
 * using namespace std;
 * ... int main() {
 * list<RECT> q;
 * list<RECT>::iterator it;
 * RECT r = {1,2,3,4};
 * q.push_back(r);
 * for (it = q.begin(); it != q.end(); it++) {
 *     it->right = it->left + 100; ... }
 * ...}
 *
 * We reference the data by using the iterator "it" as a pointer.
 * NOTE: An explicit cast to your type may be required.
 * Now let's examine using BTree for this same application
 * We will sort the tree using X coordinate (left)
 *
 * #include <btree.h>
 * ...
 * int rcmp(const RECT& r1, const RECT& r2) { return r1.left - r2.left; }
 * ... 
 * int main() {
 * BTree<RECT> t(rcmp);
 * BTree<RECT>::iterator it;
 * RECT r = {1,2,3,4};
 * ...
 * t.Add(r);
 * ...
 * // Make each rectangle 100 pixels wide
 * for (it = t.begin(); it!= t.end(); it++)
 *     it->right = it->left + 100;
 * }
 * ...}
 *
 * The first step is constructing the tree. You must provide a compare function
 * (a la qsort()) to tell the tree how to compare two elements. Read below for 
 * further explanation. You may add elements to the tree with Add() and remove
 * them with Remove(). Find() is what you will use to look up a particular 
 * entry. To make an algorithm pass over each element in the tree, list style,
 * you must use an iterator. Declare type BTree<T>::iterator. In your "for"
 * loop, initialize it to BTree<T>::begin(), terminate when it equals 
 * BTree<T>::end(), and as usual increment with ++ operator.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * FAQ
 *
 * Q: How can I balance the tree to achieve the best lookup performance?
 * A: This class uses red-black optimization to implement a self-balancing
 *    tree. You should not try to perform these operations yourself, or you
 *    will break the structure. The worst case scenario is O(2log2(n)) to 
 *    look up a value, where
 *    n is the number of entries in the structure. Functions to optimize,
 *    rotate, and balance the tree are private within BTree. 
 *    Each insertion or deletion uses a maximum of 3 rotations, thus the
 *    algorithm is acceptable for most real time applications. There is
 *    no garbage collection in this code, although system memory management
 *    from C++ "delete" operator is called.
 *    NOTE: The best case scenario is O(log2(n)), which is a factor of 2 
 *          difference from the worst case.
 *
 * Q: Doesn't a binary tree sort data? How does it know what's 
 *    greater than what? Do I need to overload operator > or < ?
 * A: No, you do not need to overload an operator. BTree operates like C's 
 *    qsort() function. You provide a callback function for the tree in its
 *    constructor. Declaration looks like this: 
 *      BTree<type> name(compare_function);
 *    If you do not specify a compare function, the code will purposely cause
 *    a segmentation fault to remind you (read address 0x00000000).
 *    The compare function should return a signed integer and take two [type]
 *    parameters by reference. compare_function would look like this:
 *      int compare_function(const type & arg1, const type & arg2);
 *    (compare_function(a, b) < 0)  means (a < b)
 *    (compare_function(a, b) > 0)  means (a > b)
 *  *** ADDENDUM: in some versions of this file, "const" is not necessary
 *                and may cause compile errors or warnings.
 *
 *    (compare_function(a, b) == 0) means (a == b)
 *    NOTE: IF you are storing pointers, you would use "type* const &s1"
 *          The const is applied to the reference and not the pointer.
 *    You can modify this source code to use overloaded inline operators
 *    if you want to. A text search for "cmp" in this file will reveal all
 *    places where comparisons between data are made. You should also modify the
 *    constructors for BTree if you attempt this.
 * 
 *
 * Q: When my tree structure goes out of scope or my program exits, it seems
 *    to freeze for a while, and the debugger says it's BTree code. Why is this? 
 *    How can I improve the performance of this operation?
 * A: If you have enough stack space available, you may call
 *    BTree::ResetRecursively(); This method resets the tree to its 
 *    initial state and quickly disposes of data and frees memory.
 *    Otherwise the destructor calls Remove() on all the elements, causing
 *    a max of O(2log2(n)) walk down the tree for each element. This also
 *    triggers re-balancing as the tree is being destroyed, which is really 
 *    not necessary. However this class was designed to never use recursion 
 *    unless you explicitly tell it to. The re-balancing involves a max of
 *    3 rotations per element so it's still O(log2(n)).
 *    NOTE: If you are storing classes inside the tree, their destructors
 *    may be what's causing the delay. You should profile your executable 
 *    to make sure. From my experience, it takes approximately 50% as long 
 *    to destroy a tree as it does to create it. And it takes about 15% as 
 *    long using recursion. (2.2GHz Pentium 4 PC, MSVC compiler)
 *
 * Q: But I like my recursive algorithms and i've already designed them. Will
 *    they be compatible with this code?
 * A: Yes. You can use BTree::Root(), BTNode::Left(), BTNode::Right(), and
 *    BTNode::Parent() to perform read-only operations on the tree. You may
 *    modify the data stored in the tree, but not the structure of the tree.
 *    Use BTNode::data to refer to your stored data.
 *    Other functions include BTNode::Sibling() and BTNode::Uncle()
 ****************************************************************************/

#ifndef btree_h
#define btree_h


//#define DEBUG_1236O327304857
#include <windows.h>

// Forward declarations
// These two classes are friends of each other
template <class T> class BTree;
template <class T> class BTNode;


enum RBTREE_COLOR
{
	BLACK = 0,
	RED
};

template <class T> 
class BTNode
{
  friend class BTree<T>;

  public:
	BTNode() { parent = NULL; left = NULL; right = NULL; color = RED; }
	T data;
	operator T /*const*/ () { return data; }
	T& operator=(/*const*/ T& dat) { data = dat; return data; }

  //private:
	BTNode<T>* Brother();   
	BTNode<T>* Sibling()    { return Brother(); }
	BTNode<T>* Uncle();     
	BTNode<T>* Parent()     { if (!this) return NULL; return parent; }
	BTNode<T>* Left()       { if (!this) return NULL; return left; }
	BTNode<T>* Right()      { if (!this) return NULL; return right; }
	BTNode<T>* LeftChild()  { if (!this) return NULL; return left; }
	BTNode<T>* RightChild() { if (!this) return NULL; return right; }

  private: 
	BTNode<T>* parent;
	BTNode<T>* left;
	BTNode<T>* right;
	RBTREE_COLOR color;  // for a red/black implementation
	// The following function uses the definition that all leaves are black.
	RBTREE_COLOR Color() {if (!this) return BLACK; else return color; }
};
// END BTNode declaration


template <class T> 
class BTree
{
  friend class BTNode<T>;
  public:
	class iterator;
  friend class iterator;


	BTree(signed int (*compareFunction)(/*const*/ T&, /*const*/ T&)) 
	{
		initialized = false;
		Initialize(compareFunction);
	}
	BTree(BTree<T>& src) { operator=(src); }
	BTree<T>& operator=(/*const*/ BTree<T>& src);

#ifdef WIN32
	int checkIntegrity();
	int checkIntegrityHelper(BTNode<T>*);
#endif // WIN32
	BTNode<T>* Add(/*const*/ T& arg);              // a
	BTNode<T>* AddIfNotPresent(/*const*/ T& arg);  // a
	BTNode<T>* AddIfUnique(T& arg) { return AddIfNotPresent(arg); }
	int Count() { return count; }
	BTNode<T>* DropEqualTo(T& val); // can be more than one of this value
	BTNode<T>* DropGreaterThan(T& val);  // delete all x where x > val
	BTNode<T>* DropGreaterThanOrEqualTo(T& val) { DropGreaterThan(val); return DropEqualTo(val); }
	BTNode<T>* DropLessThan(T& val);     // delete all x where x < val
	BTNode<T>* DropLessThanOrEqualTo(T& val)    { DropLessThan(val);    return DropEqualTo(val); }
	BTNode<T>* Min();  // node of least value in the tree
	BTNode<T>* Max();  // node of greatest value in the tree
	BTNode<T>* Min(T& floor);   // returns x where x is minimum of all {x >= floor}
	BTNode<T>* Max(T& ceiling); // returns x where x is maximum of all {x <= ceiling}
	BTNode<T>* Root() { return root; }  // root node
	BTNode<T>* Find(/*const*/ T& val);  // returns the element x closest to the root where x = val; NOTE: there may be others equal to val which are not returned
	int find_in_range(T& min, T& max, BTNode<T>** yourArray, int yaSize);
	BTNode<T>* find_in_range(T& p_min, T& p_max);
	bool IsUnique(BTNode<T>* node); // returns true if there are no other elements in the tree equal to 'node'
	bool IsUnique(T& val);
	bool Remove(T& val);           // 
	bool Remove(BTNode<T>*);       // 
	void ResetRecursively(); // faster than destructor but uses stack space
	void Reset();
	void clear() { Reset(); }



	/*********************************************
	BTree<T> SubTree(BTNode<T>* node);
	BTree<T> SubTreeLessThan(BTNode<T>* node);
	BTree<T> SubTreeLessThan(T val);
	BTree<T> SubTreeGreaterThan(BTNode<T>* node);
	BTree<T> SubTreeGreaterThan(T val);
	BTree<T> SubTreeEqualTo(BTNode<T>* node);
	BTree<T> SubTreeEqualTo(T val);
	BTree<T> SubTreeLessThanOrEqualTo(BTNode<T>* node);
	BTree<T> SubTreeLessThanOrEqualTo(T val);
	BTree<T> SubTreeGreaterThanOrEqualTo(BTNode<T>* node);
	BTree<T> SubTreeGreaterThanOrEqualTo(T val);
	BTree<T> SubTreeInRangeInclusive(T& min, T& max);
	BTree<T> SubTreeInRangeExclusive(T& min, T& max);
	*********************************************/
	~BTree();


	signed int compare(T& a, T& b) { return cmp(a, b); }

  private:
	BTNode<T>* RotateLeft(BTNode<T>* node);
	BTNode<T>* RotateRight(BTNode<T>* node);
	void DestroyRecursiveHelper(BTNode<T>* n);
	void Initialize(signed int (*compareFunction)(/*const*/ T&, /*const*/ T&));

	// Data
	BTNode<T>* root;
	int count;
	int mod;  // Change log number - this is changed every time the tree 
              // is modified. It is used to invalidate iterators if the 
			  // structure changes while they are in the middle of traversal.
	bool initialized;
	signed int (*cmp)(/*const*/ T&, /*const*/ T&);  // compare function


	void BalancePostAdd(BTNode<T>* node);   // 
	void DeleteCase1(BTNode<T>* n);
	void DeleteCase2(BTNode<T>* n);
	void DeleteCase3(BTNode<T>* n);
	void DeleteCase4(BTNode<T>* n);
	void DeleteCase5(BTNode<T>* n);
	void DeleteCase6(BTNode<T>* n);

  public:
	// If this /*const*/ructor is invoked by default...
	//BTree() { *(int*)0 = *(int*)0; }  
	BTree() 
	{
		initialized = false;
		cmp = NULL;
		root = NULL;
		count = 0;
		mod = 0;
	}
	// crash on purpose - must provide a compare function                                  
	// However in the case of embedded systems, don't
	// overwrite address 0... that could be bad.
	void InitComparator(signed int (*compareFunction)(/*const*/ T&, /*const*/ T&))
	{
		Initialize(compareFunction);
	}

	//-------------------------------------------------------------------------------
	class iterator
	{
        #if defined(_MSC_VER)
		//friend void TestTree(BTree<T>* t);
		friend class BTree<T>;
		#elif defined(__GNUC__)
		//friend void TestTree<T>(BTree<T>* t);
		friend class BTree<T>;
		#else
		friend class BTree<T>;
		#endif
	  private:
	  //public:
		BTNode<T> *current;
		enum { UP, DOWN } direction;
		bool past_root;
		int init_mod; 
		BTree<T>* tptr; // pointer to the host tree structure

	  public:
		//iterator() { current = NULL; past_root = false; init_mod = 0; tptr = NULL; }
		iterator()
		{
			current = NULL;
			direction = UP;
			past_root = false;
			init_mod = 0;
			tptr = NULL;
		}
		iterator(iterator& other)
		{
			current = other.current;
			direction = other.direction;
			past_root = other.past_root;
			init_mod = other.init_mod;
			tptr = other.tptr;
		}
		//~iterator() { current = NULL; past_root = false; init_mod = 0; tptr = NULL; }
/************************************************************************
		// We can't initialize init_mod with this function
		// Update: BTree::begin() is responsible for setting init_mod
		BTNode<T>* operator=(BTNode<T> *ptr)
		{
#ifdef DEBUG_1236O327304857
MessageBox(NULL, "iterator::operator=(BTNode*)", NULL, 0);
#endif
			current = ptr;
			direction = DOWN;
			if (current->Parent() == NULL)
			{
				past_root = true;
			}
			else
			{
				past_root = false;
			}
			// TODO:
			//init_mod = ...
			return ptr;
		}
**************************************************************************/
		
		iterator& operator=(iterator& other)
		{
#ifdef DEBUG_1236O327304857
MessageBoxA(NULL, "iterator::operator=(iterator&)", NULL, 0);
#endif
			current = other.current;
			direction = other.direction;
			past_root = other.past_root;
			init_mod = other.init_mod;
			tptr = other.tptr;
			return (*this);
		}
		
		// The next 2 functions are very similar; they provide pointers to the data. 
		// The first provides support for statements such as
		// int* A = iterator;
		operator T*()
		{
			if (init_mod == tptr->mod)
				return &(current->data);
			else 
			{
				current = NULL;
				return NULL;
			}
		}
		// The second provides support for statements such as
		// if (*iterator == 0) 
		T* operator*()
		{
			if (init_mod == tptr->mod)
				return &(current->data);
			else 
			{
				current = NULL;
				return NULL;
			}
		}
		//The third provides support for statements such as
		// iterator->member = ...;
		#ifdef _MSC_VER
		#pragma warning(disable:4284)
		// Warning 4284 is due to the fact that this function would try to
		// define -> for base data types such as int. Of course this
		// operation is only valid for union/struct/class types.
		#endif //_MSC_VER
		T* operator->()
		{
			if (init_mod == tptr->mod)
				return &(current->data);
			else 
			{
				current = NULL;
				return NULL;
			}
		}
		#ifdef _MSC_VER
		#pragma warning(default:4284)
		#endif
		//BTNode<T>* operator*()
		//{
		//	return current;
		//}
		BTNode<T>* node()
		{
			if (init_mod == tptr->mod)
				return current;
			else 
			{
				current = NULL;
				return NULL;
			}
		}
		operator BTNode<T>*()
		{
			return node();
		}
		bool operator==(/*const*/ BTNode<T>* ptr)
		{
			return current == ptr;
		}
		bool operator==(/*const*/ iterator& other)
		{
			return (current == other.current);
		}
		bool operator!=(/*const*/ BTNode<T>* ptr)
		{
			return current != ptr;
		}
		bool operator!=(/*const*/ iterator& other)
		{
			return (current != other.current);
		}
		//BTNode<T>* operator++(int)
		// TODO: Change return type from iterator* to iterator&
		iterator& operator++(int)
		{
			return operator++();
		}

		/**********************************************************************
		 * BTree::iterator::operator++
		 *
		 * moves the pointer to the next element in tree in ascending order
		 **********************************************************************/
		//BTNode<T>* operator++()
		//iterator& operator++();
		iterator& operator++()
		{
			BTNode<T> *tmp;
			if (current != NULL)
			{
				if (init_mod != tptr->mod)
				{
					current = NULL;
					return (*this);
				}
				if (direction == UP)
				{
					tmp = current;
					while (tmp->Parent())
					{
						if (tmp->Parent()->Right() == tmp)
						{
							// already been to this node
							tmp = tmp->Parent();
						}
						else
						{
							current = tmp->Parent();
							if (current->Right())
							{
								direction = DOWN;
							}
							//return current;
							return (*this);
						}
					}
					if (!tmp->Parent())
					{
						// at root
						direction = DOWN;
						if (past_root)
						{
							current = NULL;
							//return NULL;
							return (*this);
						}
						past_root = true;
						return operator++();
					}
				}
				else if (direction == DOWN)
				{
					// Select the minimum of the right subtree.
					// Unless there is no right subtree in which case
					// we need to start over going up.
					tmp = current->Right();
					if (tmp == NULL)
					{
						if (current->Parent() == NULL)
						{
							// Current node is root with no right child
							current = NULL;
							//return NULL;
							return (*this);
						}
						else
						{
							direction = UP;
							operator++();
						}
					}
					else
					{
						while (tmp->Left()) tmp = tmp->Left();
						current = tmp;
						if (tmp->Right()) direction = DOWN;
						else direction = UP;
					}
				}
				else
				{
					#if defined(stderr)
					fprintf(stderr, "Invalid direction specifier in BTree<T>::iterator line %d %s\n", __LINE__, __FILE__);
					#endif 
				}
			}
			//return current;
			return (*this);
		}
		// END operator++()

		/**********************************************************************
		 * BTree::iterator::operator++
		 *
		 * moves the pointer to the next element in tree in ascending order
		 **********************************************************************/
		iterator& operator--(int)
		{
			return operator--();
		}
		iterator& operator--()
		{
			perror("BTree<T>::iterator::operator--() is not implemented yet. Causing segfault.\n");
			// cause a segfault because this function is not implemented
			*(int*)0 = *(int*)0;
			return (*this);
		}
	};
	//---------------------------------------------------------------------------------------------
	// END class iterator (BTree::iterator)

	// Iterator support functions
	//BTNode<T>* begin() { return Min(); }
	//BTNode<T>* end() { return NULL; }

	#if defined(_MSC_VER)
	class BTree<T>::iterator begin()
	{
	#elif defined(__GNUC__)
	iterator begin()
	{
		//static iterator it;
	#endif
		it.current = Min();
		it.direction = iterator::DOWN;
		it.past_root = false;
		it.init_mod = mod;
		it.tptr = this;
		return it;
	}
	#if defined(_MSC_VER)
	class BTree<T>::iterator end()
	{
		BTree<T>::iterator it;
	#elif defined(__GNUC__)
	class iterator end()
	{
		iterator it;
	#endif
		//it = (BTNode<T>*)NULL;
		it.current = NULL;
		it.direction = iterator::UP;
		it.init_mod = mod;
		it.tptr = this;
		return it;
	}

	#if defined(_MSC_VER)
	class BTree<T>::iterator make_iterator(BTNode<T>* ptr)
	{
		BTree<T>::iterator it;
	#elif defined(__GNUC__)
	iterator make_iterator(BTNode<T>* ptr)
	{
		iterator it;
	#endif
		/****************
		BTNode<T> *current;
		enum { UP, DOWN } direction;
		bool past_root;
		int init_mod; 
		BTree<T>* tptr; // pointer to the host tree structure
		****************/
		it.tptr = this;
		it.init_mod = mod;
		it.current = ptr;
		it.direction = iterator::DOWN;
		int res = 0;
		while(ptr->parent)
		{
			if (ptr->parent->left == ptr) res = 1;
			else res = 2;
			ptr = ptr->parent;
		}
		ptr = it.current;
		it.past_root = res == 2 ? true : false;
		
		return it;
	}
	private:
		// TODO: Thread safe protect
	iterator it; //BTree<T>::iterator it;


};
// END class BTree


//#########################################################################
//                  BEGIN  FUNCTION  CODE
//#########################################################################


/*
template <class T>
BTree<T>::iterator& BTree<T>::iterator::operator++()
{
	return (*this);
}
*/


template <class T>
void BTree<T>::Initialize(signed int (*compareFunction)(/*const*/ T&, /*const*/ T&))
{
#ifdef DEBUG_1236O327304857
MessageBoxA(NULL, "BTree::Initialize", NULL, 0);
#endif
	if (!initialized)
	{
		cmp = compareFunction;
		root = NULL;
		count = 0;
		mod = 0;
		initialized = true;
	}
	else
	{
		// Crash if it tries to initialize an
		// already-initialized instance.
		*(int*)0 = *(int*)0;
	}
}


template <class T>
BTree<T>& BTree<T>::operator=(/*const*/ BTree<T>& src)
{
	#if defined(_MSC_VER)
	BTree<T>::iterator it;
	#elif defined(__GNUC__)
	iterator it;
	#endif
	initialized = false;
	Initialize(src.cmp);
	for (it = src.begin(); it != src.end(); it++)
	{
		this->Add(*(T*)it);
	}
	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif
	return *this;
}


/**********************************************************************
 * BTree:~BTree
 *
 * Standard destructor, clean up all resources and free memory.
 * In order to preserve the convention in BTree of not using any
 * recursive function calls, we call Remove on the root until the
 * entire tree disappears. This is slower than a recursive
 * implementation because the red/black algorithms will perform color
 * changes and rotations on the tree after each Remove. The algorithm
 * also has to search a maximum of O(log2(n)) nodes for each Remove.
 **********************************************************************/
template<class T>
BTree<T>::~BTree()
{
	while (Root())
	{
		Remove(Root());
	}
}

/********************************************************************
 * BTNode::Brother
 *
 * Returns the other child of the same parent as 'this' node
 * The function will return NULL if the sibling node is a leaf.
 ********************************************************************/

template <class T>
BTNode<T>* BTNode<T>::Brother()
{
	if (this == NULL) return NULL;
	if (parent == NULL) return NULL;
	return (parent->left == this) ? (parent->right) : (parent->left);
}

/********************************************************************
 * BTNode::Uncle
 *
 * Just what it says. See BTNode::Brother. Can return NULL.
 ********************************************************************/

template <class T>
BTNode<T>* BTNode<T>::Uncle()
{
	//if (this == NULL) return NULL;
	//if (parent == NULL) return NULL;
	//if (parent->parent == NULL) return NULL;
	//return (parent->parent->left == parent) ? (parent->parent->right) : (parent->parent->left);
	
	return (          this == NULL ? NULL : 
	       (        parent == NULL ? NULL :
	       (parent->parent == NULL ? NULL :
	       (parent->parent->left == parent ? parent->parent->right : parent->parent->left
    ))));
}


/*******************************************************************
 * BTNode::RotateRight
 *
 * Performs a right rotation about 'this' node.
 * This code (and the letters) are based upon the images from 
 * http://en.wikipedia.org/wiki/Tree_rotation
 *******************************************************************/


template <class T>
//BTNode<T>* BTNode<T>::RotateRight()
BTNode<T>* BTree<T>::RotateRight(BTNode<T>* node)
{
	BTNode<T> *a, *b, *c, *d, *e, *p;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	mod++;

	d = node;
	e = node->Right();
	b = node->Left();
	a = b->Left();
	c = b->Right();
	p = node->Parent();

	if (p)
	{
		if (p->left == node)
		{
			p->left = b;
		}
		else if (p->right == node)
		{
			p->right = b;
		}
		else
		{
			fprintf(stderr, "Pointer error in BTNode::RotateRight, line %d %s\n", __LINE__, __FILE__);
		}
	}
	else
	{
		// TODO: We're rotating the root, so we need to change the tree's root pointer.
		root = b;
	}
	if (b)
	{
		b->parent = p;
		b->right = d;
	}
	if (c)
	{
		c->parent = d; //c->parent = b;
	}
	if (d)
	{
		d->left = c;
		d->parent = b;
	}
	mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif
	
	return b;
}
// END BTNode<T>::RotateRight()


/*******************************************************************
 * BTNode::RotateLeft
 *
 * Performs a left rotation about 'this' node.
 *******************************************************************/
template <class T>
//BTNode<T>* BTNode<T>::RotateLeft()
BTNode<T>* BTree<T>::RotateLeft(BTNode<T>* node)
{
    BTNode<T> *a, *b, *c, *d, *e, *p;  // p is the parent of 'this' node

    mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

    a = node->Left();
    b = node;
    c = node->Right()->Left();
    d = node->Right();
    e = node->Right()->Right();
    p = node->Parent();
    
    if (p)
    {
    	if (p->left == node)
    	{
    		p->left = d;
    	}
    	else if (p->right == node)
    	{
    		p->right = d;
    	}
		else 
		{
			fprintf(stderr, "Pointer error in BTNode::RotateLeft, line %d %s\n", __LINE__, __FILE__);
		}
    }
	else
	{
		// TODO: We're rotating the root, so we need to change the corresponding tree's root pointer.
		root = d;
	}
    if (b)
    {
    	b->right = c;
    	b->parent = d;
    }
    if (c)
    {
    	c->parent = b;
    }
    if (d)
    {
    	d->parent = p;
    	d->left = b;
    }

    mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	return d;    
}
// END BTNode<T>::RotateLeft()







//######################################################################
//#  END   BTNode functions; 
//#
//#  BEGIN BTree  functions
//######################################################################



/*********************************************************************
 *
 * IsUnique
 * 
 * Determines whether the data value in a specific node is unique.
 * i.e. There are no other elements in the tree which have the same
 *      (keyed/sortable) data value.
 *********************************************************************/

template <class T> 
bool BTree<T>::IsUnique(BTNode<T>* node)
{
	if (node->Left())
	{
		if (cmp(node->Left()->data,  node->data) == 0) return false;
	}
	if (node->Right())
	{
		if (cmp(node->Right()->data, node->data) == 0) return false;
	}
	if (node->Parent())
	{
		if (cmp(node->Parent()->data, node->data) == 0) return false;
	}
	return true;
}


template <class T> 
bool BTree<T>::IsUnique(T& val)
{
	return IsUnique(Find(val));
}


/********************************************************************
 * Add
 *
 * Adds the argument to the tree under a new node. Does not check for
 * repetition. To ensure elements remain unique, use AddIfNotPresent.
 *
 * Returns the newly created node with the new data.
 *********************************************************************/

template <class T>
BTNode<T>* BTree<T>::Add(/*const*/ T& arg)
{
	BTNode<T>* current = root;
	BTNode<T>* return_value;

	mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	if (root == NULL)
	{
		root = new BTNode<T>;
		root->data = arg;
		root->color = BLACK;
		count++;

		#if CHECK_INTEGRITY_btree_h
		if (checkIntegrity()) *(int*)0 = 0;
		#endif

		return root;
	}

	while (current != NULL)
	{
		//if (current > arg)
		if (cmp(arg, current->data) > 0)
		{
			if (current->right != NULL)
			{
				current = current->right;
			}
			else
			{
				current->right = new BTNode<T>;
				current->right->left = NULL;
				current->right->right = NULL;
				current->right->parent = current;
				current->right->color = RED;  //BTREE_COLOR_RED;
				current->right->data = arg;
				return_value = current->right;
				BalancePostAdd(current->right);
				count++;
				mod++;

				#if CHECK_INTEGRITY_btree_h
				if (checkIntegrity()) *(int*)0 = 0;
				#endif
				
				return return_value;
			}
		}
		else
		{
		//---------------------------------
			if (current->left != NULL)
			{
				current = current->left;
			}
			else
			{
				current->left = new BTNode<T>;
				current->left->left = NULL;
				current->left->right = NULL;
				current->left->parent = current;
				current->left->color = RED; //BTREE_COLOR_RED;
				current->left->data = arg;
				return_value = current->left;
				BalancePostAdd(current->left);
				count++;
				mod++;

				#if CHECK_INTEGRITY_btree_h
				if (checkIntegrity()) *(int*)0 = 0;
				#endif

				return return_value;
			}
		}
	}
	mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	return NULL;
}



/********************************************************************
 * AddIfNotPresent
 *
 * Adds an entry to the tree if it would be unique, otherwise performs
 * no action on the tree. 
 *
 * Returns the node containing the new data, or its equivalent which
 * was already in the tree.
 ********************************************************************/

template <class T>
BTNode<T>* BTree<T>::AddIfNotPresent(/*const*/ T& arg)
{
	BTNode<T>* current = root;
	BTNode<T>* return_value;
	int c;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif


	if (root == NULL)
	{
		root = new BTNode<T>;
		root->data = arg;
		root->color = BLACK;
		count++;
		mod++;

		#if CHECK_INTEGRITY_btree_h
		if (checkIntegrity()) *(int*)0 = 0;
		#endif

		return root;
	}

	while (current != NULL)
	{
		//if (current > arg)
		c = cmp(arg, current->data);
		if (c > 0)
		{
			if (current->right != NULL)
			{
				current = current->right;
			}
			else
			{
				current->right = new BTNode<T>;
				current->right->left = NULL;
				current->right->right = NULL;
				current->right->parent = current;
				current->right->color = RED; //BTREE_COLOR_RED;
				current->right->data = arg;
				return_value = current->right;
				BalancePostAdd(current->right);
				count++;
				mod++;

				#if CHECK_INTEGRITY_btree_h
				if (checkIntegrity()) *(int*)0 = 0;
				#endif

				return return_value;
			}
		}
		else if (c < 0)
		{
		//---------------------------------
			if (current->left != NULL)
			{
				current = current->left;
			}
			else
			{
				current->left = new BTNode<T>;
				current->left->left = NULL;
				current->left->right = NULL;
				current->left->parent = current;
				current->left->color = RED; //BTREE_COLOR_RED;
				current->left->data = arg;
				return_value = current->left;
				BalancePostAdd(current->left);
				count++;
				mod++;

				#if CHECK_INTEGRITY_btree_h
				if (checkIntegrity()) *(int*)0 = 0;
				#endif

				return return_value;
			}
		}
		else if (c == 0)
		{
			// equal

			#if CHECK_INTEGRITY_btree_h
			if (checkIntegrity()) *(int*)0 = 0;
			#endif

			return current;
		}
		else
		{
			#if CHECK_INTEGRITY_btree_h
			if (checkIntegrity()) *(int*)0 = 0;
			#endif

			return NULL;
		}
	}

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	return NULL;
}

/********************************************************************
 * Find
 *
 * The all-important function of the binary search tree.
 * This function returns the top-most node containing the requested 
 * data. If not found, the function returns NULL.
 ********************************************************************/

template <class T>
BTNode<T>* BTree<T>::Find(/*const*/ T& val)
{
	BTNode<T> *current = root;

	while ((current != NULL) && (cmp(val, current->data) != 0))  //(val != current))
	{
		//if (val > current)
		if (cmp(val, current->data) > 0)
		//if (cmp(current->data, val) > 0)
		{
			current = current->right;
		}
		else
		{
			current = current->left;
		}
	}
	return current;
}


/********************************************************************
 * Min
 * 
 * Returns the node of the minimum element in the tree.
 *
 *
 * Min(floor)
 *
 * Returns the minimum element that is greater than floor.
 ********************************************************************/

template <class T>
BTNode<T>* BTree<T>::Min()
{
	BTNode<T>* current = root;

	if (current == NULL) return NULL;
	while (current->left)
	{
		current = current->left;
	}
	return current;
}

template <class T>
BTNode<T>* BTree<T>::Min(T& floor)
{
	BTNode<T>* current = root;
	BTNode<T>* closest = root;
	int c;

	if (current == NULL) return NULL;

	// This is basically the same as Find(). The difference is we don't need an
	//exact match. The smallest E for which (E >= floor) is what we want.
	while (current)
	{
		c = cmp(current->data, floor);
		if (c > 0) // current > floor
		{
			closest = current;
			current = current->left;
		}
		else if (c < 0) // current < floor
		{
			current = current->right;
		}
		else // current == floor
		{
			closest = current;
			break;
		}
	}
	current = closest;
	if (current)
	{
		while (current->left)
		{
			// Go to the first of a set of equal elements
			if (compare(current->left->data, current->data) == 0)
			{
				current = current->left;
			}
			else
			{
				break;
			}
		}
	}
	return current;
}



/********************************************************************
 * Max
 *
 * Returns the node of the maximum element in the tree.
 ********************************************************************/

template <class T>
BTNode<T>* BTree<T>::Max()
{
	BTNode<T>* current = root;
	if (current == NULL) return NULL;

	while (current->right)
	{
		current = current->right;
	}
	return current;
}

template <class T>
BTNode<T>* BTree<T>::Max(T& ceiling)
{
	BTNode<T>* current = root;
	BTNode<T>* closest = root;
	int c;

	if (current == NULL) return NULL;
	// This is basically the same as Find(). The difference is we don't need an
	//exact match. The largest E for which (E <= ceiling) is what we want.
	while (current)
	{
		c = cmp(current->data, floor);
		if (c < 0) // current < ceiling
		{
			closest = current;
			current = current->right;
		}
		else if (c > 0) // current > ceiling
		{
			current = current->left;
		}
		else // current == floor
		{
			closest = current;
			break;
		}
	}
	current = closest;
	if (current)
	{
		while (current->right)
		{
			// Go to the last of a set of equal elements
			if (compare(current->right->data, current->data) == 0)
			{
				current = current->right;
			}
			else
			{
				break;
			}
		}
	}
	return current;

}


/*************************************************************
 * BalancePostAdd
 * 
 * Internal private function of BTree class
 * This function optimizes the tree after an insertion.
 * It conforms to the red/black algorithm detailed at
 * http://en.wikipedia.org/wiki/Red_black_tree
 *************************************************************/
template <class T>
void BTree<T>::BalancePostAdd(BTNode<T>* node)
{
	mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	if (node == NULL) return;

	// Case 1: New node is the root.
	if (node->Parent() == NULL) 
	{
		node->color = BLACK;
	}
	// Case 2: New node's parent is black. No action necessary
	else if (node->Parent()->Color() == BLACK)
	{
	}
	// Case 3: New node, its parent, and its uncle are all red
	else if (node->Parent()->Color() == RED && node->Uncle()->Color() == RED)
	{
		// Can not get here if node's parent is root node, because 
		// root is always black. Therefore no check for validity of
		// the grandparent pointer should be becessary. We also know
		// that the uncle node exists because it was colored red.
		node->Parent()->Parent()->color = RED;
		node->Parent()->color = BLACK;
		node->Uncle()->color = BLACK;
		BalancePostAdd(node->Parent()->Parent());
	}
	// Case 4: The parent P is red but the uncle U is black;
	// also, the new node N is the right child of P, and P in turn
	// is the left child of its parent G. 
	else if (node->Parent()->Color() == RED && node->Uncle()->Color() == BLACK
		&& node == node->Parent()->Right() && node->Parent() == node->Parent()->Parent()->Left())
	{
		// Perform a left rotation about the parent
		RotateLeft(node->Parent());
		node = node->Left();
		// The following call will induce case 5 recursively.
		BalancePostAdd(node);
	}
	// 4b: The symmetric relationship from case 4.
	else if (node->Parent()->Color() == RED && node->Uncle()->Color() == BLACK
		&& node == node->Parent()->Left() && node->Parent() == node->Parent()->Parent()->Right())
	{
		RotateRight(node->Parent());
		node = node->Right();
		// The following call will induce case 5 recursively.
		BalancePostAdd(node);
	}
	// Case 5: New node and its parent are red, uncle is black.
	// Node is left child of parent. Parent is left child of grapdparent.
	else if (node->Parent()->Color() == RED && node->Uncle()->Color() == BLACK
		&& node == node->Parent()->Left() && node->Parent() == node->Parent()->Parent()->Left())
	{
		node->Parent()->color = BLACK;
		node->Parent()->Parent()->color = RED;
		RotateRight(node->Parent()->Parent());
	}
	// 5b: symmetric operation to case 5
	else if (node->Parent()->Color() == RED && node->Uncle()->Color() == BLACK
		&& node == node->Parent()->Right() && node->Parent() == node->Parent()->Parent()->Right())
	{
		node->Parent()->color = BLACK;
		node->Parent()->Parent()->color = RED;
		RotateLeft(node->Parent()->Parent());
	}
	else
	{
		fprintf(stderr, "Unhandled case in BTree::BalancePostAdd() line %d %s\n", __LINE__, __FILE__);
	}
	mod++;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif
}


/****************************************************************************
 * Remove
 *
 * Removes (or deletes) the specified node from the tree.
 * Preserves red-black properties
 * Returns true if success, false if failed
 ****************************************************************************/
template <class T>
bool BTree<T>::Remove(BTNode<T>* node)
{

	// The first step is to copy the max node of the left subtree into the
	// deleted node's position and recolor it;
	
	BTNode<T>* node_parent;
	BTNode<T>* node_left;
	BTNode<T>* node_right;
	BTNode<T>* replacement_node_parent;
	BTNode<T>* replacement_node_child;
	BTNode<T>* replacement_node;
	RBTREE_COLOR node_color, replacement_node_color;

	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity())
	{
		*(int*)0 = 0;
	}
	#endif

	// avoid crashes
	if (!node) return false;

	node_parent = node->Parent();
	node_left = node->Left();
	node_right = node->Right();
	node_color = node->Color();

	// find the max of the left subtree
	replacement_node = node->Left();
	if (replacement_node)
	{
		while (replacement_node->Right())
		{
			replacement_node = replacement_node->Right();
		}
	}
	else
	{
		// or the min of the right subtree
		replacement_node = node->Right();
		while(replacement_node->Left())
		{
			replacement_node = replacement_node->Left();
		}
	}

	if (!replacement_node)
	{
		if (!node->Parent())
		{
			// In this case there is only one node: the root.
			if (root != node) fprintf(stderr, "BTree.h logic fault on line %d.\n", __LINE__);
			delete node;
			count--;
			mod++;
			root = NULL;

			#if CHECK_INTEGRITY_btree_h
			if (checkIntegrity()) *(int*)0 = 0;
			#endif

			return true;
		}
		else
		{
			replacement_node = node;
			/************************************************
			if (node->Parent()->Left() == node)
			{
				node->Parent()->left = NULL;
			}
			else if (node->Parent()->Right() == node)
			{
				node->Parent()->right = NULL;
			}
			else
			{
				fprintf(stderr, "BTree template code line %d: corrupt tree was found.\n", __LINE__);
			}
			*************************************************/
		}
	}
	else
	{
		mod++;
		node->data = replacement_node->data;
	}

	mod++;

	replacement_node_parent = replacement_node->Parent();
	replacement_node_child = replacement_node->Left() ? replacement_node->Left() : replacement_node->Right();
	replacement_node_color = replacement_node->Color();

	if (replacement_node_child->Color() == RED || replacement_node->Color() == RED)
	{
		if (replacement_node->Parent()->Left() == replacement_node)
		{
			replacement_node->Parent()->left = replacement_node_child;
		}
		else if (replacement_node->Parent()->Right() == replacement_node)
		{
			replacement_node->Parent()->right = replacement_node_child;
		}
		else
		{
			//fprintf(stderr, "BTree.h line %d: corrupt tree structure.\n", __LINE__);
			root = replacement_node_child;
		}
		if (replacement_node_child)
		{
			replacement_node_child->parent = replacement_node->Parent();
			if (replacement_node_child->Color() == RED)
			{
				replacement_node_child->color = BLACK;
			}
		}
		delete replacement_node;
		count--;

		#if CHECK_INTEGRITY_btree_h
		if (checkIntegrity()) *(int*)0 = 0;
		#endif

		return true;
	}
	else if (replacement_node_color == BLACK && replacement_node_child->Color() == BLACK)
	{
		// This is the fun one
		if (replacement_node_child != NULL)
		{
			// re-link around replacement_node
			if (replacement_node_parent->Left() == replacement_node)
				replacement_node_parent->left = replacement_node_child;
			else if (replacement_node_parent->Right() == replacement_node)
				replacement_node_parent->right = replacement_node_child;
			else
				fprintf(stderr, "Pointer errors in BTree.h line %d\n", __LINE__);

			if (replacement_node_child)
				replacement_node_child->parent = replacement_node_parent;
			delete replacement_node;
			count--;
			DeleteCase1(replacement_node_child);

			#if CHECK_INTEGRITY_btree_h
			if (checkIntegrity()) *(int*)0 = 0;
			#endif

			return true;
		}
		else  // replacement_node_child is NULL
		{
			// Quote from Wikipedia
			//Note: If N is a null leaf and we don't want to represent null 
			//leaves as actual node objects, we can modify the algorithm by
			//first calling delete_case1() on its parent (the node that we 
			//delete, n in the code above) and deleting it afterwards. We can
			//do this because the parent is black, so it behaves in the same 
			//way as a null leaf (and is sometimes called a 'phantom' leaf).0
			//And we can safely delete it at the end as n will remain a leaf 
			//after all operations, as shown above.
			DeleteCase1(replacement_node);
			if (replacement_node->Parent()->Left() == replacement_node)
				replacement_node->Parent()->left = NULL;
			else if (replacement_node->Parent()->Right() == replacement_node)
				replacement_node->Parent()->right = NULL;
			delete replacement_node;
			count--;

			#if CHECK_INTEGRITY_btree_h
			if (checkIntegrity()) *(int*)0 = 0;
			#endif

			return true;
		}
	}
	
	#if CHECK_INTEGRITY_btree_h
	if (checkIntegrity()) *(int*)0 = 0;
	#endif

	return false;

}


template <class T>
void BTree<T>::DeleteCase1(BTNode<T>* n)
{
	// n is the new root
	if (n->Parent() == NULL) return;
	else DeleteCase2(n);
}

template <class T>
void BTree<T>::DeleteCase2(BTNode<T>* n)
{
	// sibling is red
	if (n->Sibling()->Color() == RED)
	{
		n->parent->color = RED;
		n->Sibling()->color = BLACK;
		if (n = n->parent->left)
		{
			RotateLeft(n->parent);
		}
		else if (n = n->parent->right)
		{
			RotateRight(n->parent);
		}
		else
		{
			fprintf(stderr, "BTree line %d: corrupt tree structure was detected.\n", __LINE__);
		}
	}
	DeleteCase3(n);
}


template <class T>
void BTree<T>::DeleteCase3(BTNode<T>* n)
{
	// P, S, and S's children are all black
	if (n->Parent()->Color() == BLACK &&
		n->Sibling()->Color() == BLACK &&
		n->Sibling()->Left()->Color() == BLACK &&
		n->Sibling()->Right()->Color() == BLACK)
	{
		n->color = RED;
	}
	else
	{
		DeleteCase4(n);
	}
}


template <class T>
void BTree<T>::DeleteCase4(BTNode<T>* n)
{
	// similar to case 3:
	// S and S's children aer black, but P is red
	if (n->Parent()->Color() == RED &&
		n->Sibling()->Color() == BLACK &&
		n->Sibling()->Left()->Color() == BLACK &&
		n->Sibling()->Right()->Color() == BLACK)
	{
		if (n->Sibling()) n->Sibling()->color = RED;
		n->parent->color = BLACK;
	}
	else
	{
		DeleteCase5(n);
	}
}


template <class T>
void BTree<T>::DeleteCase5(BTNode<T>* n)
{
	// S is black, S's left child is red, S's right child is black,
	// and N is the left child of its parent.
	// In this case we rotate right at S, so that S's left child becomes 
	// S's parent and N's new sibling. We then exchange the colors of S 
	// and its new parent.

	if (n == n->parent->left &&
		n->Sibling()->Color() == BLACK &&
		n->Sibling()->Left()->Color() == RED &&
		n->Sibling()->Right()->Color() == BLACK)
	{
		n->Sibling()->color = RED;
		n->Sibling()->left->color = BLACK;
		RotateRight(n->Sibling());
	}
	else if (n == n->parent->right &&
			 n->Sibling()->Color() == BLACK &&
			 n->Sibling()->Right()->Color() == RED &&
			 n->Sibling()->Left()->Color() == BLACK)
	{
		n->Sibling()->color = RED;
		n->Sibling()->right->color = BLACK;
		RotateLeft(n->Sibling());
	}
	DeleteCase6(n);
}


template <class T>
void BTree<T>::DeleteCase6(BTNode<T>* n)
{
	//S is black, S's right child is red, and N is the left child of its
	// parent P. In this case we rotate left at P, so that S becomes the
	// parent of P and S's right child. We then exchange the colors of P 
	// and S, and make S's right child black.

	if (n->Sibling())
	{
		n->Sibling()->color = n->Parent()->Color();
		n->Parent()->color = BLACK;
		if (n == n->parent->left)
		{
			// n->Sibling()->right->color == RED
			if (n->Sibling()->Right()) n->Sibling()->right->color = BLACK;
			RotateLeft(n->parent);
		}
		else if (n = n->parent->right)
		{
			// n->Sibling()->left->color == RED
			if (n->Sibling()->Left()) n->Sibling()->left->color = BLACK;
			RotateRight(n->parent);
		}
		else
		{
			fprintf(stderr, "BTree.h line %d: corrupt tree structure was detected.\n", __LINE__);
		}
	}
	// This is the end of the last possible case, 
	// if the tree remains intact and there are no bugs.
}

template <class T>
bool BTree<T>::Remove(T& val)
{
	return Remove(Find(val));
}
// END BTree::Remove

template <class T>
void BTree<T>::DestroyRecursiveHelper(BTNode<T>* n)
{
	mod++;
	if (n)
	{
		DestroyRecursiveHelper(n->Left());
		DestroyRecursiveHelper(n->Right());
		delete n;
		count--;
	}
	mod++;
}

template <class T>
void BTree<T>::ResetRecursively()
{
	DestroyRecursiveHelper(Root());
	initialized = false;
	Initialize(this->cmp);
}

template <class T>
void BTree<T>::Reset()
{
	mod++;
	while (root)
	{
		Remove(root);
	}
	initialized = false;
	Initialize(this->cmp);  
}

template<class T> int BTree<T>::find_in_range(T& p_min, T& p_max, BTNode<T>** yourArray, int yaSize)
{

	int yaCurrent = 0;
	iterator it;
	it = chunks.make_iterator(Min(p_min));
	while (compare(it.node()->data, p_max) <= 0 && it != end()) 
	{
		if (yaCurrent < yaSize) yourArray[yaCurrent] = it.node();
		yaCurrent++;
		it++;
	}
	return yaCurrent;
}


template<class T> BTNode<T>* BTree<T>::find_in_range(T& min, T& max)
{
	BTNode<T>* node;
	int cmin, cmax; // comparisons
	node = Root();
	while (node)
	{
		cmin = compare(node->data, min);
		cmax = compare(node->data, max);
		if (cmin < 0)
		{
			node = node->Right();
		}
		else if (cmax > 0)
		{
			node = node->Left();
		}
		else if (cmin >= 0 && cmax <= 0)
		{
			return node;
		}
		else
		{
			// Should never happen
			node = NULL;
		}
	}
	return NULL;
}

#ifdef WIN32

template <class T> int BTree<T>::checkIntegrity()
{
	// Since this is a debugging tool only, we can use recursion
	int ret;
	ret = checkIntegrityHelper(Root());
	if (ret)
	{
		*(int*)0=0; // breakpoint
	}
	return ret;
}

template <class T> int BTree<T>::checkIntegrityHelper(BTNode<T>* node)
{
	// Note: For the linux version, just read the address. If it's going to 
	// fail, at east make it noticeable so it can be caught. Perhaps
	// that should be a debug only option. See GLW_DEBUG_LEVEL
#if CHECK_INTEGRITY_btree_h == 0
	return 0;
#endif
	if (!this) return 1;
	if (!node) return 0;
	if (node != NULL && IsBadReadPtr(node, sizeof(BTNode<T>))) return 1;
	if (node->left != NULL)
	{
		if (IsBadReadPtr(node->left, sizeof(BTNode<T>))) return 1;
		if (checkIntegrityHelper(node->left)) return 1;
		if (cmp(node->data, node->left->data) < 0) return 1;
	}
	if (node->right != NULL)
	{
		if (IsBadReadPtr(node->right, sizeof(BTNode<T>))) return 1;
		if (checkIntegrityHelper(node->right)) return 1;
		if (cmp(node->data, node->right->data) > 0) return 1;
	}
	return 0;
}

#endif // WIN32



#endif // btree_h
