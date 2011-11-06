#include <stl_string.h>

#include <stdlib>
#include <cstring>

using namespace std;

string::string()
{
  m_cstr = (char*) NULL;
  m_size = 0;

}// string()


string::string (const char * cstr) {
  m_cstr = strdup(cstr);
  m_size = strlen(m_cstr) + 1;

}// string::string()


size_t string::length() const {
  if(m_cstr == NULL) {
    return 0;
  }
  return strlen(m_cstr);

}// string::resize()


const char * string::c_str ( ) const {
  return m_cstr;

}// c_str()



size_t string::max_size() const {
  return size_t(-1);

}// string::max_size()


string::~string() {
  if(m_cstr != NULL) {
    delete[] m_cstr;
  }

}// ~string()
