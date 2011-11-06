#ifndef __STL_STRING_H__
#define __STL_STRING_H__

#include <types.h>


namespace std
{
  class string
  {
   private:
    char * m_cstr;
    int    m_size;

   public:
    string ();
    string (const char * cstr);
    //string (const string & chaine);
    ~string ();

    //int longueur (const char * chaine);
    //size_t copy  (char * s, size_t n, size_t pos = 0);

    //string operator = (const char * chaine);
    //string operator = (const string & chaine);
    //string operator + (const char * chaine);
    //string operator + (const string & chaine);

    //void resize (size_t n, char c = 0);

    const char * c_str ( ) const;

    size_t max_size() const;
    size_t length() const;

    char * getChaine ();

  };// class string

}// namespace std


#endif// __STL_STRING_H__
