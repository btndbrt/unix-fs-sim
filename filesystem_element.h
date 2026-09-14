#ifndef FILESYSTEM_ELEMENT_H
#define FILESYSTEM_ELEMENT_H

#include <iostream>
#include "string.h"
#include "vector.h"

class Directory;

class FileSystemElement {
    String name;
    Directory* parent; // nullptr only for the root directory
public:
    FileSystemElement();
    FileSystemElement(const String& name, Directory* parent);
    virtual ~FileSystemElement();
    const String& getName() const;
    Directory* getParent();
    const Directory* getParent() const;
    void setName(const String& n);
    void setParent(Directory* p);
    String getPath() const;
    virtual const char* typeLabel() const = 0;
    virtual void collectDirs(Vector<FileSystemElement*>& dirs) {}
    virtual FileSystemElement* findChild(const char* name, int len) const { return nullptr; }
    virtual Directory* asDirectory() { return nullptr; }
    virtual const Directory* asDirectory() const { return nullptr; }
    virtual void print() = 0;
    virtual void serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const = 0;
};

#endif