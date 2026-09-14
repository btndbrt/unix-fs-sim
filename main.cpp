#include <iostream>
#include <fstream>
#include "directory.h"
#include "file.h"
#include "link.h"
#include "string.h"

// Returns the parent directory for a given path.
// Used by mkdir, touch, and mv to find where to place a new element.
Directory* parentOf(const String& path, Directory* cwd, Directory* root) {
    int slash = path.lastIndexOf('/');
    if (slash < 0) return cwd;
    String parentPath = (slash == 0) ? String("/") : path.substr(0, slash);
    FileSystemElement* e = cwd->resolve(parentPath, root);
    if (e) return e->asDirectory();
    return nullptr;
}

String nameOf(const String& path) {
    int slash = path.lastIndexOf('/');
    if (slash < 0) return path;
    return path.substr(slash + 1, path.getLen() - slash - 1);
}

int main() {
    Directory* root = nullptr;
    {
        std::ifstream in("filesystem.dat");
        if (in.good()) root = Directory::load(in);
    }
    if (!root) root = new Directory("root", nullptr);
    Directory* cwd = root;

    while (true) {
        std::cout << cwd->getPath().getCstr() << " $ ";
        if (!std::cin.good()) break;

        String line = String::readLine(std::cin);
        int pos = 0;
        String cmd = line.getWord(pos);
        if (cmd.getLen() == 0) continue;

        if (cmd == String("exit") || cmd == String("quit")) {
            break;

        } else if (cmd == String("ls")) {
            String arg = line.getWord(pos);
            Directory* target = cwd;
            if (arg.getLen() > 0) {
                FileSystemElement* e = cwd->resolve(arg, root);
                if (!e || !e->asDirectory()) {
                    std::cout << "ls: " << arg.getCstr() << ": No such directory" << std::endl;
                    continue;
                }
                target = e->asDirectory();
            }
            const Vector<FileSystemElement*>& elems = target->getElements();
            for (int i = 0; i < elems.getSize(); ++i)
                std::cout << elems[i]->getName().getCstr() << " " << elems[i]->typeLabel() << std::endl;

        } else if (cmd == String("cd")) {
            String arg = line.getWord(pos);
            if (arg.getLen() == 0) { std::cout << "Usage: cd <path>" << std::endl; continue; }
            FileSystemElement* e = cwd->resolve(arg, root);
            if (!e || !e->asDirectory()) {
                std::cout << "cd: " << arg.getCstr() << ": No such directory" << std::endl;
                continue;
            }
            // asDirectory() follows a link to its target if e is a Link,
            // so cwd is always set to a real Directory, never a Link object.
            cwd = e->asDirectory();

        } else if (cmd == String("mkdir")) {
            String arg = line.getWord(pos);
            if (arg.getLen() == 0) { std::cout << "Usage: mkdir <path>" << std::endl; continue; }
            Directory* parent = parentOf(arg, cwd, root);
            String name = nameOf(arg);
            if (!parent || name.getLen() == 0) {
                std::cout << "mkdir: cannot create directory '" << arg.getCstr() << "': No such file or directory" << std::endl;
                continue;
            }
            parent->addElement(new Directory(name, parent));

        } else if (cmd == String("touch")) {
            String arg = line.getWord(pos);
            if (arg.getLen() == 0) { std::cout << "Usage: touch <path>" << std::endl; continue; }
            Directory* parent = parentOf(arg, cwd, root);
            String name = nameOf(arg);
            if (!parent || name.getLen() == 0) {
                std::cout << "touch: cannot create file '" << arg.getCstr() << "': No such file or directory" << std::endl;
                continue;
            }
            parent->addElement(new File(name, parent));

        } else if (cmd == String("rm")) {
            String arg = line.getWord(pos);
            if (arg.getLen() == 0) { std::cout << "Usage: rm <path>" << std::endl; continue; }
            FileSystemElement* e = cwd->resolve(arg, root);
            if (!e) {
                std::cout << "rm: cannot remove '" << arg.getCstr() << "': No such file or directory" << std::endl;
                continue;
            }
            if (e == root || e == cwd) {
                std::cout << "rm: cannot remove '" << arg.getCstr() << "'" << std::endl;
                continue;
            }
            e->getParent()->removeElement(e->getName());

        } else if (cmd == String("ln")) {
            String targetArg = line.getWord(pos);
            String nameArg   = line.getWord(pos);
            if (targetArg.getLen() == 0 || nameArg.getLen() == 0) {
                std::cout << "Usage: ln <target> <name>" << std::endl;
                continue;
            }
            FileSystemElement* t = cwd->resolve(targetArg, root);
            if (!t || !t->asDirectory()) {
                std::cout << "ln: " << targetArg.getCstr() << ": No such directory" << std::endl;
                continue;
            }
            cwd->addElement(new Link(nameArg, cwd, t->asDirectory()));

        } else if (cmd == String("mv")) {
            String srcArg = line.getWord(pos);
            String dstArg = line.getWord(pos);
            if (srcArg.getLen() == 0 || dstArg.getLen() == 0) {
                std::cout << "Usage: mv <src> <dst>" << std::endl;
                continue;
            }
            FileSystemElement* src = cwd->resolve(srcArg, root);
            if (!src) {
                std::cout << "mv: cannot stat '" << srcArg.getCstr() << "': No such file or directory" << std::endl;
                continue;
            }
            if (src == root || src == cwd) {
                std::cout << "mv: cannot move '" << srcArg.getCstr() << "'" << std::endl;
                continue;
            }

            Directory* newParent = nullptr;
            String newName;

            FileSystemElement* dstElem = cwd->resolve(dstArg, root);
            if (dstElem && dstElem->asDirectory()) {
                newParent = dstElem->asDirectory();
                newName   = src->getName();
            } else {
                newParent = parentOf(dstArg, cwd, root);
                newName   = nameOf(dstArg);
                if (!newParent || newName.getLen() == 0) {
                    std::cout << "mv: target '" << dstArg.getCstr() << "': No such file or directory" << std::endl;
                    continue;
                }
            }

            FileSystemElement* detached = src->getParent()->detachElement(src->getName());
            detached->setName(newName);
            detached->setParent(newParent);
            newParent->addElement(detached);

        } else {
            std::cout << "Unknown command: " << cmd.getCstr() << std::endl;
        }
    }

    // Save the filesystem on every exit, including EOF (e.g. end of piped input).
    std::ofstream out("filesystem.dat");
    root->save(out);
    delete root;
    return 0;
}
