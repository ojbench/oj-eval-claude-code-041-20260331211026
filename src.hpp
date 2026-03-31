#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include <stdexcept>
#include "utility.hpp"

namespace sjtu {

template<class Key, class T, class Compare = std::less<Key>>
class map {
public:
    typedef pair<const Key, T> value_type;

private:
    enum Color { RED, BLACK };

    struct Node {
        value_type *data;
        Node *left;
        Node *right;
        Node *parent;
        Color color;

        Node(const value_type &val, Node *p = nullptr)
            : data(new value_type(val)), left(nullptr), right(nullptr), parent(p), color(RED) {}

        Node(value_type &&val, Node *p = nullptr)
            : data(new value_type(std::move(val))), left(nullptr), right(nullptr), parent(p), color(RED) {}

        // Constructor for sentinel node
        Node() : data(nullptr), left(nullptr), right(nullptr), parent(nullptr), color(BLACK) {}

        ~Node() { delete data; }
    };

    Node *root;
    Node *header; // sentinel node, header->left points to leftmost, header->right points to rightmost
    size_t sz;
    Compare comp;

    void rotateLeft(Node *x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node *x) {
        Node *y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insertFixup(Node *z) {
        while (z->parent && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node *y = z->parent->parent->right;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else {
                Node *y = z->parent->parent->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void transplant(Node *u, Node *v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    Node* minimum(Node *x) const {
        while (x->left) x = x->left;
        return x;
    }

    Node* maximum(Node *x) const {
        while (x->right) x = x->right;
        return x;
    }

    void eraseFixup(Node *x, Node *xParent) {
        while (x != root && (!x || x->color == BLACK)) {
            if (x == xParent->left) {
                Node *w = xParent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rotateLeft(xParent);
                    w = xParent->right;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if (w->left) w->left->color = BLACK;
                        w->color = RED;
                        rotateRight(w);
                        w = xParent->right;
                    }
                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->right) w->right->color = BLACK;
                    rotateLeft(xParent);
                    x = root;
                    break;
                }
            } else {
                Node *w = xParent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rotateRight(xParent);
                    w = xParent->left;
                }
                if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if (w->right) w->right->color = BLACK;
                        w->color = RED;
                        rotateLeft(w);
                        w = xParent->left;
                    }
                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->left) w->left->color = BLACK;
                    rotateRight(xParent);
                    x = root;
                    break;
                }
            }
        }
        if (x) x->color = BLACK;
    }

    void eraseNode(Node *z) {
        Node *y = z;
        Node *x, *xParent;
        Color yOriginalColor = y->color;

        if (!z->left) {
            x = z->right;
            xParent = z->parent;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            xParent = z->parent;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                xParent = y;
            } else {
                xParent = y->parent;
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        // Update header pointers
        if (header->left == z) {
            if (z->right) header->left = minimum(z->right);
            else header->left = z->parent;
        }
        if (header->right == z) {
            if (z->left) header->right = maximum(z->left);
            else header->right = z->parent;
        }

        delete z;

        if (yOriginalColor == BLACK && root) {
            eraseFixup(x, xParent);
        }
    }

    void destroyTree(Node *node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    Node* copyTree(Node *node, Node *parent) {
        if (!node) return nullptr;
        Node *newNode = new Node(*node->data, parent);
        newNode->color = node->color;
        newNode->left = copyTree(node->left, newNode);
        newNode->right = copyTree(node->right, newNode);
        return newNode;
    }

public:
    class const_iterator;
    class iterator {
        friend class map;
        friend class const_iterator;
    private:
        Node *node;
        const map *container;

        iterator(Node *n, const map *c) : node(n), container(c) {}

    public:
        iterator() : node(nullptr), container(nullptr) {}
        iterator(const iterator &other) : node(other.node), container(other.container) {}

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator++() {
            if (node == container->header) return *this;
            if (node->right) {
                node = container->minimum(node->right);
            } else {
                Node *p = node->parent;
                while (p && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                node = p ? p : container->header;
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        iterator& operator--() {
            if (node == container->header) {
                node = container->header->right;
            } else if (node->left) {
                node = container->maximum(node->left);
            } else {
                Node *p = node->parent;
                while (p && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        value_type& operator*() const { return *(node->data); }
        value_type* operator->() const { return node->data; }
        bool operator==(const iterator &rhs) const { return node == rhs.node; }
        bool operator==(const const_iterator &rhs) const { return node == rhs.node; }
        bool operator!=(const iterator &rhs) const { return node != rhs.node; }
        bool operator!=(const const_iterator &rhs) const { return node != rhs.node; }
    };

    class const_iterator {
        friend class map;
        friend class iterator;
    private:
        Node *node;
        const map *container;

        const_iterator(Node *n, const map *c) : node(n), container(c) {}

    public:
        const_iterator() : node(nullptr), container(nullptr) {}
        const_iterator(const const_iterator &other) : node(other.node), container(other.container) {}
        const_iterator(const iterator &other) : node(other.node), container(other.container) {}

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator& operator++() {
            if (node == container->header) return *this;
            if (node->right) {
                node = container->minimum(node->right);
            } else {
                Node *p = node->parent;
                while (p && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                node = p ? p : container->header;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator& operator--() {
            if (node == container->header) {
                node = container->header->right;
            } else if (node->left) {
                node = container->maximum(node->left);
            } else {
                Node *p = node->parent;
                while (p && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        const value_type& operator*() const { return *(node->data); }
        const value_type* operator->() const { return node->data; }
        bool operator==(const iterator &rhs) const { return node == rhs.node; }
        bool operator==(const const_iterator &rhs) const { return node == rhs.node; }
        bool operator!=(const iterator &rhs) const { return node != rhs.node; }
        bool operator!=(const const_iterator &rhs) const { return node != rhs.node; }
    };

    map() : root(nullptr), header(new Node()), sz(0) {
    }

    map(const map &other) : root(nullptr), header(new Node()), sz(other.sz), comp(other.comp) {
        root = copyTree(other.root, nullptr);
        if (root) {
            header->left = minimum(root);
            header->right = maximum(root);
        }
    }

    map& operator=(const map &other) {
        if (this != &other) {
            clear();
            sz = other.sz;
            comp = other.comp;
            root = copyTree(other.root, nullptr);
            if (root) {
                header->left = minimum(root);
                header->right = maximum(root);
            }
        }
        return *this;
    }

    ~map() {
        clear();
        delete header;
    }

    T& at(const Key &key) {
        Node *node = root;
        while (node) {
            if (comp(key, node->data->first)) node = node->left;
            else if (comp(node->data->first, key)) node = node->right;
            else return node->data->second;
        }
        throw std::out_of_range("map::at");
    }

    const T& at(const Key &key) const {
        Node *node = root;
        while (node) {
            if (comp(key, node->data->first)) node = node->left;
            else if (comp(node->data->first, key)) node = node->right;
            else return node->data->second;
        }
        throw std::out_of_range("map::at");
    }

    T& operator[](const Key &key) {
        Node *node = root;
        Node *parent = nullptr;
        bool isLeft = false;

        while (node) {
            if (comp(key, node->data->first)) {
                parent = node;
                node = node->left;
                isLeft = true;
            } else if (comp(node->data->first, key)) {
                parent = node;
                node = node->right;
                isLeft = false;
            } else {
                return node->data->second;
            }
        }

        // Insert new node
        Node *newNode = new Node(value_type(key, T()), parent);
        if (!parent) {
            root = newNode;
            header->left = header->right = newNode;
        } else if (isLeft) {
            parent->left = newNode;
            if (parent == header->left) header->left = newNode;
        } else {
            parent->right = newNode;
            if (parent == header->right) header->right = newNode;
        }

        sz++;
        insertFixup(newNode);
        return newNode->data->second;
    }

    const T& operator[](const Key &key) const {
        return at(key);
    }

    iterator begin() { return sz ? iterator(header->left, this) : iterator(header, this); }
    const_iterator cbegin() const { return sz ? const_iterator(header->left, this) : const_iterator(header, this); }
    iterator end() { return iterator(header, this); }
    const_iterator cend() const { return const_iterator(header, this); }

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    void clear() {
        destroyTree(root);
        root = nullptr;
        header->left = header->right = nullptr;
        sz = 0;
    }

    pair<iterator, bool> insert(const value_type &value) {
        Node *node = root;
        Node *parent = nullptr;
        bool isLeft = false;

        while (node) {
            if (comp(value.first, node->data->first)) {
                parent = node;
                node = node->left;
                isLeft = true;
            } else if (comp(node->data->first, value.first)) {
                parent = node;
                node = node->right;
                isLeft = false;
            } else {
                return pair<iterator, bool>(iterator(node, this), false);
            }
        }

        Node *newNode = new Node(value, parent);
        if (!parent) {
            root = newNode;
            header->left = header->right = newNode;
        } else if (isLeft) {
            parent->left = newNode;
            if (parent == header->left) header->left = newNode;
        } else {
            parent->right = newNode;
            if (parent == header->right) header->right = newNode;
        }

        sz++;
        insertFixup(newNode);
        return pair<iterator, bool>(iterator(newNode, this), true);
    }

    void erase(iterator pos) {
        if (pos.node == header) return;
        sz--;
        eraseNode(pos.node);
        if (sz == 0) {
            header->left = header->right = nullptr;
        }
    }

    size_t count(const Key &key) const {
        return find(key) != cend() ? 1 : 0;
    }

    iterator find(const Key &key) {
        Node *node = root;
        while (node) {
            if (comp(key, node->data->first)) node = node->left;
            else if (comp(node->data->first, key)) node = node->right;
            else return iterator(node, this);
        }
        return end();
    }

    const_iterator find(const Key &key) const {
        Node *node = root;
        while (node) {
            if (comp(key, node->data->first)) node = node->left;
            else if (comp(node->data->first, key)) node = node->right;
            else return const_iterator(node, this);
        }
        return cend();
    }
};

}

#endif
