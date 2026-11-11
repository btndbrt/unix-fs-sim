#include "vector.h"
#include "filesystem_element.h"
#include "link.h"

#include "memtrace.h"

// The template implementation is in a .cpp file so we need to explicitly tell
// the compiler which types to generate. Without these lines the linker fails.
template class Vector<FileSystemElement*>;
template class Vector<int>;
template class Vector<Link*>;

// Default constructor: empty vector with no allocation
template <typename T>
Vector<T>::Vector() : data(nullptr), size(0), capacity(0) {}

// Constructor that allocates n slots and sets size to n
template <typename T>
Vector<T>::Vector(int n) : size(n), capacity(n) {
    data = new T[capacity];
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& other) : size(other.size), capacity(other.capacity) {
    data = new T[capacity];
    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
}

// Destructor
template <typename T>
Vector<T>::~Vector() {
    delete[] data;
}

// Assign operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if (this == &other) return *this;
    delete[] data;
    size = other.size;
    capacity = other.capacity;
    data = new T[capacity];
    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
    return *this;
}

template <typename T>
void Vector<T>::push_back(const T& value) {
    if (size == capacity) {
        // Double the capacity if it runs out of space
        // Starting from 1 avoids multiplying zero when the vector is empty.
        capacity = (capacity == 0) ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    data[size++] = value;
}

// Removes the last element by decrementing size (no deallocation)
template <typename T>
void Vector<T>::pop_back() {
    if (size > 0) {
        size--;
    }
}

// Size getter, returns the number of stored elements
template <typename T>
int Vector<T>::getSize() const {
    return size;
}

// Capacity getter, returns the number of allocated slots
template <typename T>
int Vector<T>::getCapacity() const {
    return capacity;
}

// Index operator
template <typename T>
T& Vector<T>::operator[](int idx) {
    return data[idx];
}

// Index operator, const version
template <typename T>
const T& Vector<T>::operator[](int idx) const {
    return data[idx];
}