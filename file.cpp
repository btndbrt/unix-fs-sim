#include "file.h"
#include <iostream>

// Default constructor
File::File() : FileSystemElement() {}

// Constructor from name and parent pointer
File::File(const String& name, Directory* parent) : FileSystemElement(name, parent) {}

// Destructor
File::~File() {}

// Prints the file name to stdout (for testing)
void File::print() {
    std::cout << "File: " << getName().getCstr() << std::endl;
}

// Serializes the file as a single "F <name>" line
void File::serialize(std::ostream& os, const Vector<FileSystemElement*>&) const {
    os << "F " << getName().getCstr() << "\n";
}