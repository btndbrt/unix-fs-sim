#ifndef STRING_H
#define STRING_H

#include <iostream>

class String {
    char* pData;
    int len; // The number of stored characters
public:
    String();
    String(const char* s);
    String(const String& str);
    ~String();
    String& operator=(const String& rhs_str);
    bool operator==(const String& rhs_str) const;
    const char* getCstr() const;
    const int getLen() const;
    char& operator[](int idx);
    char operator[](int idx) const;
    String operator+(const String& rhs) const;
    String operator+(const char* rhs) const;
    void append(char c);
    int lastIndexOf(char c) const;
    String substr(int start, int len) const;
    String getWord(int& pos) const;
    static String readLine(std::istream& is);
    static String readWord(std::istream& is);
};

#endif