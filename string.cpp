#include "string.h"
#include "memtrace.h"

// Default constructor: pData is nullptr, so getCstr() returns nullptr for an empty string
String::String() : pData(nullptr), len(0) {}

// Constructor from a C String
String::String(const char* s) {
    len = 0;
    const char* s_ither = s;
    while (*s_ither != '\0') {
        len++;
        s_ither++;
    }
    pData = new char[len+1];
    for (int i = 0; i < len; i++)
        pData[i] = s[i];
    pData[len] = '\0';
}

// Copy constructor
String::String(const String& str) {
    len = str.len;
    pData = new char[len + 1];
    for (int i = 0; i < len; i++) 
        pData[i] = str.pData[i];
    pData[len] = '\0';
}

// Destructor
String::~String() {
    delete[] pData;
}

// Assign operator
String& String::operator=(const String& rhs_str) {
    // Guard against self-assignment by comparing the data pointers.
    if (this->pData == rhs_str.pData) return *this;
    delete[] pData;
    len = rhs_str.len;
    pData = new char[len + 1];
    for (int i = 0; i < len; i++)
        pData[i] = rhs_str.pData[i];
    pData[len] = '\0';
    return *this;
}

// Compare operator
bool String::operator==(const String& rhs_str) const {
        if (len != rhs_str.len) return false;
        for (int i = 0; i < len; i++) {
            if (pData[i] != rhs_str.pData[i]) return false;
        }
        return true;
    }

// CString getter, returns a pointer to the first char in the string
const char* String::getCstr() const {
    return pData;
}

// Length getter, returns the length of the string
const int String::getLen() const {
    return len;
}

// Index operator
char& String::operator[](int idx) {
    return pData[idx];
}

// Index operator, const version
char String::operator[](int idx) const {
    return pData[idx];
}

// Add operator, concatenates the strings
String String::operator+(const String& rhs) const {
    int newLen = len + rhs.len;
    char* temp = new char[newLen + 1];
    for (int i = 0; i < len; i++) temp[i] = pData[i];
    for (int i = 0; i < rhs.len; i++) temp[len + i] = rhs.pData[i];
    temp[newLen] = '\0';
    String result(temp);
    delete[] temp;
    return result;
}

// Add operator const C string version
String String::operator+(const char* rhs) const {
    return *this + String(rhs);
}

// Append function, appends a character to the end of the string
void String::append(char c) {
    char* newData = new char[len + 2];
    for (int i = 0; i < len; ++i) newData[i] = pData[i];
    newData[len]     = c;
    newData[len + 1] = '\0';
    delete[] pData;
    pData = newData;
    ++len;
}

// A function that finds the last index of a character in the string
// Used for finding / characters in path strings
int String::lastIndexOf(char c) const {
    for (int i = len - 1; i >= 0; --i) {
        if (pData[i] == c) return i;
    }
    return -1;
}

// A function that returns a substring from a given index with a given length
String String::substr(int start, int length) const {
    String result;
    for (int i = start; i < start + length && i < len; ++i)
        result.append(pData[i]);
    return result;
}

// Reads the next whitespace-delimited word starting at pos and sets pos past it
// This can be called repeatedly to read all words in a line
String String::getWord(int& pos) const {
    while (pos < len && (pData[pos] == ' ' || pData[pos] == '\t')) ++pos;
    String word;
    while (pos < len && pData[pos] != ' ' && pData[pos] != '\t')
        word.append(pData[pos++]);
    return word;
}

// Reads one line from the stream
String String::readLine(std::istream& is) {
    String line;
    char c;
    while (is.get(c) && c != '\n' && c != '\r')
        line.append(c);
    return line;
}

// Reads the next whitespace-delimited token from the stream
String String::readWord(std::istream& is) {
    String word;
    char c;
    while (is.get(c) && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {}
    if (is) {
        word.append(c);
        while (is.get(c) && c != ' ' && c != '\t' && c != '\n' && c != '\r')
            word.append(c);
    }
    return word;
}