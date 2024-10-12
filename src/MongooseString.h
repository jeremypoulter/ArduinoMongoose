#ifndef MongooseString_h
#define MongooseString_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "mongoose.h"

#define MG_NULL_STR \
  { nullptr, 0 }

class MongooseString
{
  private:
    mg_str _string;

    // use a function pointer to allow for "if (s)" without the
    // complications of an operator bool(). for more information, see:
    // http://www.artima.com/cppsource/safebool.html
    typedef void (MongooseString::*StringIfHelperType)() const;
    void StringIfHelper() const {
    }

  public:
    MongooseString() : 
      _string(MG_NULL_STR)
    {
    }
    MongooseString(const mg_str *string) :
      _string(MG_NULL_STR)
    {
      if(string) {
        _string = *string; 
      }
    }
    MongooseString(const mg_str string) :
      _string(string) {
    }
    MongooseString(const char *string) :
      _string(mg_str_s(string)) {
    }
    MongooseString(const char *string, size_t len) :
      _string(mg_str_n(string, len)) {
    }
#ifdef ARDUINO
    MongooseString(String &str)
    {
      _string.buf = str.c_str();
      _string.len = str.length();
    }
#endif

// The 'fix' in mongoose.h for mg_str breaks this
#undef mg_str
    operator struct mg_str ()
    {
      return _string;
    }

    operator struct mg_str *()
    {
      return &_string;
    }
#define mg_str(s) mg_str_s(s)

    operator const char *() const
    {
      return _string.buf;
    }

    // use a function pointer to allow for "if (s)" without the
    // complications of an operator bool(). for more information, see:
    // http://www.artima.com/cppsource/safebool.html
    operator StringIfHelperType() const
    {
      return nullptr != _string.buf ? &MongooseString::StringIfHelper : 0;
    }
    
#ifdef ARDUINO
    operator String()
    {
      return toString();
    }
#endif

    MongooseString & operator = (const char *cstr) {
      _string = mg_str_s(cstr);
      return *this;
    }
    MongooseString & operator = (const mg_str *rhs) {
      _string.buf = rhs ? rhs->buf : NULL;
      _string.len = rhs ? rhs->len : 0;
      return *this;
    }
    MongooseString & operator = (const mg_str &rhs) {
      _string = rhs;
      return *this;
    }
//    MongooseString & operator = (const String &rhs);
//    MongooseString & operator = (const __FlashStringHelper *str);

    void get(const char * &p, size_t &len) {
      p = _string.buf;
      len = _string.len;
    }

    size_t length() {
      return _string.len;
    }

    const char *c_str()
    {
      return _string.buf;
    }

    int compareTo(const mg_str &str) const {
      return mg_strcmp(_string, str);
    }
    int compareTo(const MongooseString &str) const {
      return mg_strcmp(_string, str._string);
    }
    int compareTo(const char *str) const {
      mg_str mgStr = mg_str_s(str);
      return mg_strcmp(_string, mgStr);
    }

    unsigned char equals(const mg_str &str) const {
      return 0 == compareTo(str);
    }
    unsigned char equals(const MongooseString &str) const {
      return 0 == compareTo(str);
    }
    unsigned char equals(const char *str) const {
      return 0 == compareTo(str);
    }

    int compareToIgnoreCase(const mg_str &str) const
    {
      return mg_strcasecmp(_string, str);
    }
    int compareToIgnoreCase(const MongooseString &str) const {
      return compareToIgnoreCase(str._string);
    }
    int compareToIgnoreCase(const char *str) const {
      mg_str mgStr = mg_str_s(str);
      return compareToIgnoreCase(mgStr);
    }

    unsigned char equalsIgnoreCase(const mg_str &str) const {
      return 0 == compareToIgnoreCase(str);
    }
    unsigned char equalsIgnoreCase(const MongooseString &str) const {
      return 0 == compareToIgnoreCase(str);
    }
    unsigned char equalsIgnoreCase(const char *str) const {
      return 0 == compareToIgnoreCase(str);
    }

    unsigned char operator ==(const mg_str &str) const {
      return equals(str);
    }
    unsigned char operator ==(const MongooseString &str) const {
      return equals(str);
    }
    unsigned char operator ==(const char *str) const {
      return equals(str);
    }

    unsigned char operator !=(const mg_str &str) const {
      return !equals(str);
    }
    unsigned char operator !=(const MongooseString &str) const {
      return !equals(str);
    }
    unsigned char operator !=(const char *str) const {
      return !equals(str);
    }

    mg_str toMgStr() {
      return _string;
    }

#ifdef ARDUINO
    int compareTo(const String &str) const {
      mg_str mgStr = mg_str_n(str.c_str(), str.length());
      return mg_strcmp(_string, mgStr);
    }

    unsigned char equals(const String &str) const {
      return 0 == compareTo(str);
    }

    unsigned char operator ==(const String &str) const {
      return equals(str);
    }

    unsigned char operator !=(const String &str) const {
      return !equals(str);
    }

    String toString() {
      if(NULL == _string.buf) {
        return String("");
      }
      mg_str copy = mg_strdup_nul(_string);
      String ret = String(copy.buf);
      mg_strfree(&copy);

      return ret;
    }
#endif // ARDUINO
};

#endif // MongooseString_h