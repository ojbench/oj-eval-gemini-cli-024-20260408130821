#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>

namespace sjtu {

template<class T>
class deque {
private:
    static const size_t BLOCK_SIZE = 512;
    T** map;
    size_t map_capacity;
    size_t map_head;
    size_t head_pos;
    size_t sz;

    void reallocate_map() {
        size_t new_map_capacity = map_capacity == 0 ? 8 : map_capacity * 2;
        T** new_map = new T*[new_map_capacity];
        for (size_t i = 0; i < new_map_capacity; i++) new_map[i] = nullptr;

        if (sz > 0) {
            size_t first_block_elems = BLOCK_SIZE - head_pos;
            size_t num_blocks = 1;
            if (sz > first_block_elems) {
                num_blocks += (sz - first_block_elems + BLOCK_SIZE - 1) / BLOCK_SIZE;
            }
            for (size_t i = 0; i < num_blocks; i++) {
                new_map[i] = map[(map_head + i) % map_capacity];
            }
            map_head = 0;
        } else {
            map_head = 0;
        }
        delete[] map;
        map = new_map;
        map_capacity = new_map_capacity;
    }

public:
    class const_iterator;
    class iterator {
        friend class deque;
        friend class const_iterator;
    private:
        deque* deque_ptr;
        size_t index;
    public:
        iterator(deque* p = nullptr, size_t i = 0) : deque_ptr(p), index(i) {}
        
        iterator operator+(const int &n) const {
            return iterator(deque_ptr, index + n);
        }
        iterator operator-(const int &n) const {
            return iterator(deque_ptr, index - n);
        }
        int operator-(const iterator &rhs) const {
            if (deque_ptr != rhs.deque_ptr) throw invalid_iterator();
            return static_cast<int>(index) - static_cast<int>(rhs.index);
        }
        iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            index++;
            return tmp;
        }
        iterator& operator++() {
            index++;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            index--;
            return tmp;
        }
        iterator& operator--() {
            index--;
            return *this;
        }
        T& operator*() const {
            if (!deque_ptr || index >= deque_ptr->sz) throw invalid_iterator();
            return deque_ptr->at(index);
        }
        T* operator->() const noexcept {
            size_t first_block_elems = deque_ptr->BLOCK_SIZE - deque_ptr->head_pos;
            if (index < first_block_elems) {
                return &deque_ptr->map[deque_ptr->map_head][deque_ptr->head_pos + index];
            } else {
                size_t pos = index - first_block_elems;
                size_t b = (deque_ptr->map_head + 1 + pos / deque_ptr->BLOCK_SIZE) % deque_ptr->map_capacity;
                size_t p = pos % deque_ptr->BLOCK_SIZE;
                return &deque_ptr->map[b][p];
            }
        }
        bool operator==(const iterator &rhs) const {
            return deque_ptr == rhs.deque_ptr && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return deque_ptr == rhs.deque_ptr && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    
    class const_iterator {
        friend class deque;
        friend class iterator;
    private:
        const deque* deque_ptr;
        size_t index;
    public:
        const_iterator(const deque* p = nullptr, size_t i = 0) : deque_ptr(p), index(i) {}
        const_iterator(const iterator &other) : deque_ptr(other.deque_ptr), index(other.index) {}
        
        const_iterator operator+(const int &n) const {
            return const_iterator(deque_ptr, index + n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(deque_ptr, index - n);
        }
        int operator-(const const_iterator &rhs) const {
            if (deque_ptr != rhs.deque_ptr) throw invalid_iterator();
            return static_cast<int>(index) - static_cast<int>(rhs.index);
        }
        const_iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        const_iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            index++;
            return tmp;
        }
        const_iterator& operator++() {
            index++;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            index--;
            return tmp;
        }
        const_iterator& operator--() {
            index--;
            return *this;
        }
        const T& operator*() const {
            if (!deque_ptr || index >= deque_ptr->sz) throw invalid_iterator();
            return deque_ptr->at(index);
        }
        const T* operator->() const noexcept {
            size_t first_block_elems = deque_ptr->BLOCK_SIZE - deque_ptr->head_pos;
            if (index < first_block_elems) {
                return &deque_ptr->map[deque_ptr->map_head][deque_ptr->head_pos + index];
            } else {
                size_t pos = index - first_block_elems;
                size_t b = (deque_ptr->map_head + 1 + pos / deque_ptr->BLOCK_SIZE) % deque_ptr->map_capacity;
                size_t p = pos % deque_ptr->BLOCK_SIZE;
                return &deque_ptr->map[b][p];
            }
        }
        bool operator==(const iterator &rhs) const {
            return deque_ptr == rhs.deque_ptr && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return deque_ptr == rhs.deque_ptr && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    deque() : map(nullptr), map_capacity(0), map_head(0), head_pos(0), sz(0) {}
    
    deque(const deque &other) : map(nullptr), map_capacity(0), map_head(0), head_pos(0), sz(0) {
        for (size_t i = 0; i < other.sz; i++) {
            push_back(other.at(i));
        }
    }
    
    ~deque() {
        clear();
        delete[] map;
    }
    
    deque &operator=(const deque &other) {
        if (this == &other) return *this;
        clear();
        for (size_t i = 0; i < other.sz; i++) {
            push_back(other.at(i));
        }
        return *this;
    }
    
    T & at(const size_t &pos) {
        if (pos >= sz) throw index_out_of_bound();
        size_t first_block_elems = BLOCK_SIZE - head_pos;
        if (pos < first_block_elems) {
            return map[map_head][head_pos + pos];
        } else {
            size_t p2 = pos - first_block_elems;
            size_t b = (map_head + 1 + p2 / BLOCK_SIZE) % map_capacity;
            size_t p = p2 % BLOCK_SIZE;
            return map[b][p];
        }
    }
    
    const T & at(const size_t &pos) const {
        if (pos >= sz) throw index_out_of_bound();
        size_t first_block_elems = BLOCK_SIZE - head_pos;
        if (pos < first_block_elems) {
            return map[map_head][head_pos + pos];
        } else {
            size_t p2 = pos - first_block_elems;
            size_t b = (map_head + 1 + p2 / BLOCK_SIZE) % map_capacity;
            size_t p = p2 % BLOCK_SIZE;
            return map[b][p];
        }
    }
    
    T & operator[](const size_t &pos) {
        return at(pos);
    }
    
    const T & operator[](const size_t &pos) const {
        return at(pos);
    }
    
    const T & front() const {
        if (sz == 0) throw container_is_empty();
        return at(0);
    }
    
    const T & back() const {
        if (sz == 0) throw container_is_empty();
        return at(sz - 1);
    }
    
    iterator begin() { return iterator(this, 0); }
    const_iterator cbegin() const { return const_iterator(this, 0); }
    iterator end() { return iterator(this, sz); }
    const_iterator cend() const { return const_iterator(this, sz); }
    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }
    
    void clear() {
        while (sz > 0) pop_back();
    }
    
    iterator insert(iterator pos, const T &value) {
        if (pos.deque_ptr != this) throw invalid_iterator();
        size_t index = pos.index;
        if (index > sz) throw invalid_iterator();
        if (index == 0) {
            push_front(value);
            return iterator(this, 0);
        }
        if (index == sz) {
            push_back(value);
            return iterator(this, sz - 1);
        }
        if (index < sz / 2) {
            push_front(at(0));
            for (size_t i = 1; i < index; i++) {
                at(i) = at(i + 1);
            }
            at(index) = value;
            return iterator(this, index);
        } else {
            push_back(at(sz - 1));
            for (size_t i = sz - 2; i > index; i--) {
                at(i) = at(i - 1);
            }
            at(index) = value;
            return iterator(this, index);
        }
    }
    
    iterator erase(iterator pos) {
        if (pos.deque_ptr != this) throw invalid_iterator();
        size_t index = pos.index;
        if (index >= sz) throw invalid_iterator();
        if (index == 0) {
            pop_front();
            return iterator(this, 0);
        }
        if (index == sz - 1) {
            pop_back();
            return iterator(this, sz);
        }
        if (index < sz / 2) {
            for (size_t i = index; i > 0; i--) {
                at(i) = at(i - 1);
            }
            pop_front();
            return iterator(this, index);
        } else {
            for (size_t i = index; i < sz - 1; i++) {
                at(i) = at(i + 1);
            }
            pop_back();
            return iterator(this, index);
        }
    }
    
    void push_back(const T &value) {
        if (sz == 0) {
            if (map_capacity == 0) reallocate_map();
            if (map[map_head] == nullptr) {
                map[map_head] = reinterpret_cast<T*>(operator new(BLOCK_SIZE * sizeof(T)));
            }
            new (&map[map_head][0]) T(value);
            head_pos = 0;
            sz = 1;
            return;
        }
        size_t first_block_elems = BLOCK_SIZE - head_pos;
        size_t b, p;
        if (sz < first_block_elems) {
            b = map_head;
            p = head_pos + sz;
        } else {
            size_t tail_pos_logical = sz - first_block_elems;
            b = (map_head + 1 + tail_pos_logical / BLOCK_SIZE) % map_capacity;
            p = tail_pos_logical % BLOCK_SIZE;
            if (p == 0 && b == map_head) {
                reallocate_map();
                b = (map_head + 1 + tail_pos_logical / BLOCK_SIZE) % map_capacity;
            }
        }
        if (map[b] == nullptr) {
            map[b] = reinterpret_cast<T*>(operator new(BLOCK_SIZE * sizeof(T)));
        }
        new (&map[b][p]) T(value);
        sz++;
    }
    
    void pop_back() {
        if (sz == 0) throw container_is_empty();
        size_t first_block_elems = BLOCK_SIZE - head_pos;
        size_t b, p;
        if (sz <= first_block_elems) {
            b = map_head;
            p = head_pos + sz - 1;
        } else {
            size_t tail_pos_logical = sz - 1 - first_block_elems;
            b = (map_head + 1 + tail_pos_logical / BLOCK_SIZE) % map_capacity;
            p = tail_pos_logical % BLOCK_SIZE;
        }
        map[b][p].~T();
        sz--;
        if (sz == 0) {
            operator delete(map[map_head]);
            map[map_head] = nullptr;
            map_head = 0;
            head_pos = 0;
        } else {
            if (p == 0) {
                operator delete(map[b]);
                map[b] = nullptr;
            }
        }
    }
    
    void push_front(const T &value) {
        if (sz == 0) {
            if (map_capacity == 0) reallocate_map();
            if (map[map_head] == nullptr) {
                map[map_head] = reinterpret_cast<T*>(operator new(BLOCK_SIZE * sizeof(T)));
            }
            new (&map[map_head][BLOCK_SIZE - 1]) T(value);
            head_pos = BLOCK_SIZE - 1;
            sz = 1;
            return;
        }
        size_t b, p;
        if (head_pos > 0) {
            b = map_head;
            p = head_pos - 1;
        } else {
            size_t first_block_elems = BLOCK_SIZE - head_pos;
            size_t last_b = map_head;
            if (sz > first_block_elems) {
                last_b = (map_head + 1 + (sz - first_block_elems - 1) / BLOCK_SIZE) % map_capacity;
            }
            b = (map_head + map_capacity - 1) % map_capacity;
            if (b == last_b) {
                reallocate_map();
                b = (map_head + map_capacity - 1) % map_capacity;
            }
            p = BLOCK_SIZE - 1;
        }
        if (map[b] == nullptr) {
            map[b] = reinterpret_cast<T*>(operator new(BLOCK_SIZE * sizeof(T)));
        }
        new (&map[b][p]) T(value);
        map_head = b;
        head_pos = p;
        sz++;
    }
    
    void pop_front() {
        if (sz == 0) throw container_is_empty();
        map[map_head][head_pos].~T();
        sz--;
        if (sz == 0) {
            operator delete(map[map_head]);
            map[map_head] = nullptr;
            map_head = 0;
            head_pos = 0;
        } else {
            head_pos++;
            if (head_pos == BLOCK_SIZE) {
                operator delete(map[map_head]);
                map[map_head] = nullptr;
                map_head = (map_head + 1) % map_capacity;
                head_pos = 0;
            }
        }
    }
};

}

#endif
