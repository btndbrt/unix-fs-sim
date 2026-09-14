#ifndef LINK_H
#define LINK_H

#include "filesystem_element.h"

class Link : public FileSystemElement {
    Directory* target; // the directory this link points to; nullptr if dangling
public:
    Link();
    Link(const String& name, Directory* parent, Directory* target);
    ~Link();
    const char* typeLabel() const override { return "<LINK>"; }
    const Directory* getTarget() const;
    void setTarget(Directory* t);
    Directory* asDirectory() override;
    const Directory* asDirectory() const override;
    void print() override;
    void serialize(std::ostream& os, const Vector<FileSystemElement*>& dirs) const override;
};

#endif