#include <stl_string.h>

#include <stdlib>
#include <cstring>
#include <algorithm>

#include <cstdio>

using namespace std;

string::string() {
  m_cstr = (char*) NULL;
  m_size = 0;

}// string()


string::string (const char * cstr) {
  m_cstr = strdup(cstr);
  m_size = strlen(m_cstr) + 1;

}// string::string()


string::string (const string & str) {
  m_cstr = strdup(str.c_str());
  m_size = strlen(m_cstr) + 1;

}// string()


size_t string::length() const {
  if(m_cstr == NULL) {
    return 0;
  }
  return strlen(m_cstr);

}// string::resize()


string & string::append ( const char * s, size_t n ) {
  size_t oldLength = this->length();
  if(oldLength + n > this->capacity()) {
    char * oldCStr = m_cstr;
    m_size = oldLength + n + 1;
    m_cstr = new char [m_size];
    strcpy(m_cstr, oldCStr);
    delete [] oldCStr;
  }
  memcpy(m_cstr + oldLength, s, n);
  m_cstr[oldLength + n] = '\0';

  return *this;

}// append()


string & string::append (const char * s) {
  return this->append(s, strlen(s));

}// append()


string & string::append (const string & str) {
  return this->append(str.c_str());

}// append()


string & string::append (const string & str, size_t pos, size_t n) {
  return this->append(str.c_str() + pos, n);

}// append()


string & string::erase (size_t pos /* = 0 */, size_t n /* = size_t(-1) */) {
  pos = min(pos, this->length());
  size_t oldLength = this->length();

  if(n >= oldLength - pos) {
    m_cstr[pos] = '\0';
  } else {
    // i <= oldLength pour également copier le '\0'
    for (size_t i = pos; i <= oldLength; ++i) {
      m_cstr[i]   = m_cstr[i+n];
    }
  }

  return *this;

}// erase()


size_t string::capacity ( ) const {
  return min(m_size - 1, (size_t) 0);

}// capacity()


void string::reserve (size_t res_arg /* = 0 */) {
  if (res_arg != 0) {
    char * old = m_cstr;
    m_cstr     = new char [res_arg + 1];
    memcpy(m_cstr, old, min(m_size, res_arg));
    delete [] m_cstr;
    m_size = res_arg + 1;
    m_cstr[m_size-1] = '\0';

  } else if (m_cstr != NULL) {
    delete [] m_cstr;
    m_cstr = (char*) NULL;
    m_size = 0;
  }

}// reserve()


const char * string::c_str ( ) const {
  return m_cstr;

}// c_str()


size_t string::max_size() const {
  return size_t(-1);

}// string::max_size()


string string::operator + (const char * chaine) {
  return string(*this).append(chaine);
}


string string::operator + (const string & chaine) {
  return string(*this).append(chaine);
}


string::~string() {
  if(m_cstr != NULL) {
    delete[] m_cstr;
  }

}// ~string()
