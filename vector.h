#ifndef VECTOR_H
#define VECTOR_H

template <typename T>
class Vector {
    T* data;
    int size;     // number of elements currently stored
    int capacity; // allocated slots in data
public:
    Vector();
    Vector(int n);
    Vector(const Vector& other);
    ~Vector();
    Vector& operator=(const Vector& other);
    void push_back(const T& value);
    void pop_back();
    int getSize() const;
    int getCapacity() const;
    T& operator[](int idx);
    const T& operator[](int idx) const;
};

#endif