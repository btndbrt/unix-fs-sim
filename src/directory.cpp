#include "directory.h"
#include "file.h"
#include "link.h"
#include <iostream>

// Default constructor
Directory::Directory() : FileSystemElement() {}

// Constructor from name and parent pointer
Directory::Directory(const String& name, Directory* parent) : FileSystemElement(name, parent) {}

// Destructor: deletes all children
Directory::~Directory() {
    for (int i = 0; i < elements.getSize(); ++i) {
        delete elements[i];
    }
}

// Adds an element to this directory's children
void Directory::addElement(FileSystemElement* element) {
    elements.push_back(element);
}

// Removes and deletes the named child element
void Directory::removeElement(const String& name) {
    for (int i = 0; i < elements.getSize(); ++i) {
        if (elements[i]->getName() == name) {
            delete elements[i];
            // Swap with the last element before popping so we avoid shifting the whole array
            elements[i] = elements[elements.getSize() - 1];
            elements.pop_back();
            return;
        }
    }
}

// Same as removeElement, but returns the element instead of deleting it so the caller can move it somewhere else (used by mv).
FileSystemElement* Directory::detachElement(const String& name) {
    for (int i = 0; i < elements.getSize(); ++i) {
        if (elements[i]->getName() == name) {
            FileSystemElement* detached = elements[i];
            elements[i] = elements[elements.getSize() - 1];
            elements.pop_back();
            return detached;
        }
    }
    return nullptr;
}

// Children getter
const Vector<FileSystemElement*>& Directory::getElements() const {
    return elements;
}

// Prints the directory name and all its children recursively (for testing)
void Directory::print() {
    std::cout << "Directory: " << getName().getCstr() << std::endl;
    for (int i = 0; i < elements.getSize(); ++i) {
        std::cout << "  ";
        elements[i]->print();
    }
}

// Finds a direct child by name (compared against the first len characters of name)
FileSystemElement* Directory::findChild(const char* name, int len) const {
    for (int i = 0; i < elements.getSize(); ++i) {
        if (elements[i]->getName().getLen() == len) {
            const char* childName = elements[i]->getName().getCstr();
            bool match = true;
            for (int j = 0; j < len; ++j) {
                if (childName[j] != name[j]) { match = false; break; }
            }
            if (match) return elements[i];
        }
    }
    return nullptr;
}

// Returns this directory as a Directory pointer
Directory* Directory::asDirectory() {
    return this;
}

// Returns this directory as a Directory pointer, const version
const Directory* Directory::asDirectory() const {
    return this;
}

// Resolves a path string to a filesystem element.
// Absolute paths (starting with '/') are resolved from root,
// relative paths are resolved from cwd.
// Links are transparent: if a non-final segment is a link, traversal continues into the link's target directory automatically.
FileSystemElement* Directory::resolve(const String& path, Directory* root) {
    const char* p = path.getCstr();
    FileSystemElement* current;

    if (p && p[0] == '/') {
        current = root;
    } else {
        current = this;
    }

    if (p) while (*p == '/') ++p;

    while (p && *p != '\0') {
        const char* segEnd = p;
        while (*segEnd != '/' && *segEnd != '\0') ++segEnd;
        int segLen = segEnd - p;

        const char* next = segEnd;
        while (*next == '/') ++next;
        bool isLast = (*next == '\0');

        if (segLen == 1 && p[0] == '.') {
            // stay in current
        } else if (segLen == 2 && p[0] == '.' && p[1] == '.') {
            // Go up to parent. At root, stay at root.
            Directory* parent = current->getParent();
            if (parent != nullptr) current = parent;
        } else {
            FileSystemElement* child = current->findChild(p, segLen);
            if (!child) return nullptr;
            // Return the element itself only if this is the last segment.
            // That way the caller can distinguish a link from its target.
            if (isLast) return child;
            // For intermediate segments, follow links transparently.
            Directory* nextDir = child->asDirectory();
            if (!nextDir) return nullptr;
            current = nextDir;
        }

        p = next;
    }

    return current;
}

// Collects all directories in depth-first order into dirs.
// The order here must match the parse order in deserializeWithTag so that
// the index of each directory is the same at save and load time.
void Directory::collectDirs(Vector<FileSystemElement*>& dirs) {
    dirs.push_back(this);
    for (int i = 0; i < elements.getSize(); ++i)
        elements[i]->collectDirs(dirs);
}

// Writes this directory and all its children recursively
void Directory::serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const {
    int id = -1;
    for (int i = 0; i < dirs.getSize(); ++i) {
        if (dirs[i] == this) { id = i; break; }
    }
    os << "D " << id << " " << getName().getCstr() << "\n";
    for (int i = 0; i < elements.getSize(); ++i)
        elements[i]->serialize(os, dirs);
    os << "END\n";
}

// Serializes the entire tree rooted at root to the stream
void Directory::save(std::ostream& os) {
    Vector<FileSystemElement*> dirs;
    collectDirs(dirs);
    serialize(os, dirs);
}

// Parses one record (D, F, or L) from the stream and returns the new element.
// Directories are parsed recursively until their END tag is reached.
// Links are stored in unresolvedLinks/unresolvedTargets and fixed up after
// the whole tree is built, because the target directory may not exist yet.
static FileSystemElement* deserializeWithTag(
    char tag,
    std::istream& is,
    Directory* parent,
    Vector<FileSystemElement*>& dirs,
    Vector<Link*>& unresolvedLinks,
    Vector<int>& unresolvedTargets
) {
    if (tag == 'D') {
        int id;
        if (!(is >> id)) return nullptr;
        String name = String::readWord(is);
        if (name.getLen() == 0) return nullptr;
        Directory* dir = new Directory(name, parent);
        dirs.push_back(dir);
        while (is) {
            String childTag = String::readWord(is);
            if (childTag.getLen() == 0) break;
            if (childTag[0] == 'E') break; // END
            FileSystemElement* child = deserializeWithTag(
                childTag[0], is, dir, dirs, unresolvedLinks, unresolvedTargets);
            if (child) dir->addElement(child);
        }
        return dir;
    } else if (tag == 'F') {
        String name = String::readWord(is);
        if (name.getLen() == 0) return nullptr;
        return new File(name, parent);
    } else if (tag == 'L') {
        String name = String::readWord(is);
        int targetId;
        if (name.getLen() == 0 || !(is >> targetId)) return nullptr;
        Link* link = new Link(name, parent, nullptr);
        unresolvedLinks.push_back(link);
        unresolvedTargets.push_back(targetId);
        return link;
    }
    return nullptr;
}

// Deserializes a full filesystem tree from the stream and returns the root
Directory* Directory::load(std::istream& is) {
    Vector<FileSystemElement*> dirs;
    Vector<Link*> unresolvedLinks;
    Vector<int> unresolvedTargets;

    String tag = String::readWord(is);
    if (tag.getLen() == 0) return nullptr;

    FileSystemElement* root = deserializeWithTag(
        tag[0], is, nullptr, dirs, unresolvedLinks, unresolvedTargets);

    // Second pass: now that all directories exist, resolve link targets by index.
    // If targetId is out of range the link gets a null target (dangling link).
    for (int i = 0; i < unresolvedLinks.getSize(); ++i) {
        int targetId = unresolvedTargets[i];
        Directory* target = nullptr;
        if (targetId >= 0 && targetId < dirs.getSize())
            target = dirs[targetId]->asDirectory();
        unresolvedLinks[i]->setTarget(target);
    }

    return root->asDirectory();
}

