#ifndef __STL_STRING_H__
#define __STL_STRING_H__

#include <sys/types.h>

namespace std {
  class string {
   private:
    char *m_cstr;
    size_t m_size;

   public:
    /**
     * Default constructor creates an empty string.
     */
    string();

    /**
     * Constructs string as copy of a C string.
     * @param cstr Source C string.
     */
    string(const char *cstr);

    /**
     * Constructs string with copy of value of @a str.
     * @param str Source string.
     */
    string(const string &str);

    /**
     * Destroys the string instance.
     */
    ~string();

    /**
     * Appends a C substring.
     * @param s The C string to append.
     * @param n The number of characters to append.
     * @return Reference to this string.
     */
    string& append(const char *s, size_t n);

    /**
     * Appends a C string.
     * @param s The C string to append.
     * @return Reference to this string.
     */
    string& append(const char *s);

    /**
     * Appends a string to this string.
     * @param str The string to append.
     * @return Reference to this string.
     */
    string& append(const string &str);

    /**
     * @brief Appends a substring.
     * @param str The string to append.
     * @param pos Index of the first character of str to append.
     * @param n The number of characters to append.
     * @return Reference to this string.
     *
     * This function appends @a n characters from @a str
     * starting at @a pos to this string. If @a n is is larger
     * than the number of available characters in @a str, the
     * remainder of @a str is appended.
     */
    string& append(const string &str, size_t pos, size_t n);

    /**
     * @brief Removes characters.
     * @param pos Index of first character to remove (default 0).
     * @param n Number of characters to remove (default remainder).
     * @return Reference to this string.
     *
     * Removes @a n characters from this string starting at @a
     * pos. The length of the string is reduced by @a n. If
     * there are < @a n characters to remove, the remainder of
     * the string is truncated.
     */
    string& erase(size_t pos = 0, size_t n = size_t(-1));

    /**
     * Returns the total number of characters that the %string can hold
     * before needing to allocate more memory.
     * @return The capacity.
     */
    size_t capacity() const;

    /**
     * @brief Attempts to preallocate enough memory for specified number of
     * characters.
     * @param res_arg Number of characters required.
     *
     * This function attempts to reserve enough memory for the
     * %string to hold the specified number of characters.
     */
    void reserve(size_t res_arg = 0);

    /**
     * Concatenate this string and a C string.
     * @param chaine C string
     * @return New string with @a this followed by @a chaine.
     */
    string operator+(const char *chaine);

    /**
     * Concatenate this string and another string.
     * @param chaine String
     * @return New string with value of @a this followed by @a chaine.
     */
    string operator+(const string &chaine);

    /**
     * Returns const pointer to null-terminated contents.
     * @return Const pointer to null-terminated contents.
     */
    const char* c_str() const;

    /**
     * Returns the size() of the largest possible %string.
     * @return The maximum size.
     */
    size_t max_size() const;

    /**
     * Returns the number of characters in the string, not including any
     * null-termination.
     * @return The size of the string.
     */
    size_t length() const;

    // TODO: not implemented
    //size_t copy(char *s, size_t n, size_t pos = 0);
    //string operator=(const char *chaine);
    //string operator=(const string &chaine);
    //void resize(size_t n, char c = 0);

  };// class string

}// namespace std

#endif // __STL_STRING_H__
