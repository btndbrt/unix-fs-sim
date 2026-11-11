#ifndef FILE_H
#define FILE_H

#include "filesystem_element.h"

class File : public FileSystemElement {
public:
    File();
    File(const String& name, Directory* parent);
    ~File();
    const char* typeLabel() const override { return "<FILE>"; }
    void print() override;
    void serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const override;
};

#endif