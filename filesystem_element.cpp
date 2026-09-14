#include "filesystem_element.h"
#include "directory.h"

// Default constructor: empty name, no parent
FileSystemElement::FileSystemElement() : name(), parent(nullptr) {}

// Constructor from name and parent pointer
FileSystemElement::FileSystemElement(const String& name, Directory* parent) : name(name), parent(parent) {}

// Virtual destructor so derived-class destructors are called correctly
FileSystemElement::~FileSystemElement() {}

// Name getter
const String& FileSystemElement::getName() const {
    return name;
}

// Parent getter
Directory* FileSystemElement::getParent() {
    return parent;
}

// Parent getter, const version
const Directory* FileSystemElement::getParent() const {
    return parent;
}

// Name setter
void FileSystemElement::setName(const String& n) {
    name = n;
}

// Parent setter
void FileSystemElement::setParent(Directory* p) {
    parent = p;
}

// Builds the full path by walking up to the root recursively.
// If there is no parent (root directory), just return the name without a leading slash.
String FileSystemElement::getPath() const {
    if (parent == nullptr) return getName();
    return parent->getPath() + String("/") + getName();
}

