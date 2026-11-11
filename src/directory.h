#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "filesystem_element.h"
#include "vector.h"

class Directory : public FileSystemElement {
    Vector<FileSystemElement*> elements; // direct children of this directory
public:
    Directory();
    Directory(const String& name, Directory* parent);
    ~Directory();
    void addElement(FileSystemElement* element);
    void removeElement(const String& name);
    FileSystemElement* detachElement(const String& name);
    const Vector<FileSystemElement*>& getElements() const;
    const char* typeLabel() const override { return "<DIR>"; }
    void collectDirs(Vector<FileSystemElement*>& dirs) override;
    FileSystemElement* findChild(const char* name, int len) const override;
    Directory* asDirectory() override;
    const Directory* asDirectory() const override;
    void print() override;
    void serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const override;
    void save(std::ostream& os);
    static Directory* load(std::istream& is);
    FileSystemElement* resolve(const String& path, Directory* root);
};

#endif