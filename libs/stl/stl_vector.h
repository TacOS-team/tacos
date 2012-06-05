#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include <iterator>
#include <sys/types.h>

namespace std {
  template <class T>
  class vector {
   public:
    typedef T *iterator; /**< Iterator. */
    typedef _reverse_iterator<iterator, T> reverse_iterator; /**< Reverse iterator. */
    
   protected:
    T *     m_data; /**< Elements of the %vector. */
    size_t  m_size; /**< Number of elements in the %vector. */
    size_t  m_reserve; /**< Number of elements that the %vector can contain. */

   public:
    /**
     * Default constructor creates no elements.
     */
    vector();
      
    /**
     * @brief Creates a %vector with copies of an exemplar element.
     * @param size The number of elements to initially create.
     * @param value An element to copy.
     *
     * This constructor fills the %vector with @a size copies of @a value.
     */
    vector(size_t size, const T & value = T());
    
    /**
     * %Vector copy constructor.
     * @param v A %vector of identical element and allocator types.
     */
    vector(const vector<T> & v);

    /**
     * The dtor only erases the elements, and note that if the
     * elements themselves are pointers, the pointed-to memory is
     * not touched in any way.
     */
    ~vector();
    
    /**
     * %Vector assignment operator.
     * @param v A %vector of identical element and allocator types.
     * @return A new %vector containing the same elements as @a v
     */
    vector<T> & operator = (const vector<T> & v);
    
    /**
     * Subscript access to the data contained in the %vector.
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read-only (constant) reference to data.
     */
     
    const T & operator [] (size_t index) const;
    
    /**
     * Provides access to the data contained in the %vector.
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read-only (constant) reference to data.
     */
    const T & at (size_t index) const;
    
    /**
     * Subscript access to the data contained in the %vector.
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read/write reference to data.
     */
    T & operator [] (size_t index);

    /**
     * Provides access to the data contained in the %vector.
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read/write reference to data.
     */
    T & at (size_t index);

    /**
     * Returns a read-only (constant) reference to the data at the first
     * element of the %vector.
     * @return Read-only reference to the first element
     */
    const T & front () const;
    
    /**
     * Returns a read/write reference to the data at the first
     * element of the %vector.
     * @return Read-write reference to the first element
     */
    T & front ();

    /**
     * Returns a read-only (constant) reference to the data at the
     * last element of the %vector.
     * @return Read-only reference to the last element
     */
    const T & back () const;
    
    /**
     * Returns a read/write reference to the data at the last
     * element of the %vector.
     * @return Read-write reference to the last element
     */
    T & back ();
    
    /**
     * Returns a read-only (constant) iterator that points to the
     * first element in the %vector. Iteration is done in ordinary
     * element order.
     * @return Read-only iterator pointing on the first element
     */
    /*const*/ iterator begin();

    /**
     * Returns a read-only (constant) reverse iterator that points
     * to the last element in the %vector. Iteration is done in
     * reverse element order.
     * @return Read-only reverse iterator pointing on the last element
     */
    const reverse_iterator rbegin();

    /**
     * Returns a read-only (constant) iterator that points one past
     * the last element in the %vector. Iteration is done in
     * ordinary element order.
     * @return Read-only iterator pointing after the last element
     */
    /*const*/ iterator end();

    /**
     * Returns a read-only (constant) reverse iterator that points
     * to one before the first element in the %vector. Iteration
     * is done in reverse element order.
     * @return Read-only reverse iterator pointing before the first element
     */
    const reverse_iterator rend();
    
    /**
     * Adds data to the end of the %vector.
     * @param x Data to be added.
     */
    void push_back (const T & x);
     
    /**
     * Removes the last element of the %vector.
     */
    void pop_back ();

    /**
     * @brief Inserts given value into %vector before specified iterator.
     * @param position An iterator into the %vector.
     * @param x Data to be inserted.
     * @return An iterator that points to the inserted data.
     *
     * This function will insert a copy of the given value before the
     * specified location.
     */
    void insert (const iterator position, const T & x);
    
    /**
     * @brief Inserts a number of copies of given data into the %vector.
     * @param position An iterator into the %vector.
     * @param n Number of elements to be inserted.
     * @param x Data to be inserted.
     *
     * This function will insert a specified number of copies of
     * the given data before the location specified by @a position.
     */
    void insert (const iterator position, size_t n, const T & x);
    
    /**
     * @brief Inserts a range into the %vector.
     * @param position An iterator into the %vector.
     * @param first An input iterator.
     * @param last An input iterator.
     *
     * This function will insert copies of the data in the range
     * [first,last) into the %vector before the location specified
     * by @a position.
     */
    void insert (const iterator position, const iterator first, const iterator last);
    
    /**
     * Erases all the elements.
     */
    void clear();

    /**
     * @brief Removes element at given position.
     * @param position Iterator pointing to element to be erased.
     * @return An iterator pointing to the next element (or end()).
     *
     * This function will erase the element at the given position and thus
     * shorten the %vector by one.
     */
    iterator erase (const iterator position );
      
    /**
     * @brief Removes a range of elements.
     * @param first Iterator pointing to the first element to be erased.
     * @param last Iterator pointing to one past the last element to be erased.
     * @return An iterator pointing to the element pointed to by @a last
     * prior to erasing (or end()).
     *
     * This function will erase the elements in the range
     * [first,last) and shorten the %vector accordingly.
     */
    iterator erase (const iterator first, const iterator last );

    /**
     * Returns the number of elements in the %vector.
     * @return The number of elements
     */
    size_t size() const;

    /**
     * Returns the size() of the largest possible %vector.
     * @return The maximum number of elements
     */
    size_t max_size() const;

    /**
     * Returns true if the %vector is empty.
     * @return true if the %vector is empty
     */
    bool empty() const;

    /**
     * @brief Attempt to preallocate enough memory for specified
     * number of elements.
     * @param reserve Number of elements required.
     *
     * This function attempts to reserve enough memory for the
     * %vector to hold the specified number of elements.  If the
     * number requested is more than max_size(), length_error is
     * thrown.
     */
    void reserve(size_t reserve);
        
    /**
     * Swaps data with another %vector.
     * @param v A %vector of the same element and allocator types.
     */
    void swap (vector<T> & v);

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif // __STL_VECTOR_H__
