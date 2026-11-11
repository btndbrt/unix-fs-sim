#include "link.h"
#include "directory.h"
#include <iostream>

// Default constructor: null target (dangling link)
Link::Link() : FileSystemElement(), target(nullptr) {}

// Constructor from name, parent pointer, and target directory
Link::Link(const String& name, Directory* parent, Directory* target) : FileSystemElement(name, parent), target(target) {}

// Destructor: does not delete target, the directory tree owns it
Link::~Link() {}

// Target getter
const Directory* Link::getTarget() const {
    return target;
}

// Prints the link name and its target's name, or "(null)" if dangling (testing)
void Link::print() {
    std::cout << "Link: " << getName().getCstr() << " -> ";
    if (target) {
        std::cout << target->getName().getCstr();
    } else {
        std::cout << "(null)";
    }
    std::cout << std::endl;
}

// Target setter, used during deserialization to fix up dangling links
void Link::setTarget(Directory* t) {
    target = t;
}

// Returning the target here makes links transparent during path traversal:
// any code that calls asDirectory() on a link automatically follows it.
Directory* Link::asDirectory() {
    return target;
}

const Directory* Link::asDirectory() const {
    return target;
}

// targetId -1 means the target is null (dangling link)
// On load such a link will be set wtih a null target
void Link::serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const {
    int targetId = -1;
    if (target) {
        for (int i = 0; i < dirs.getSize(); ++i) {
            if (dirs[i] == target) { targetId = i; break; }
        }
    }
    os << "L " << getName().getCstr() << " " << targetId << "\n";
}