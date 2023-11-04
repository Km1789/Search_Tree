#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <list>
using namespace std;

template <class T>
class Tree
{
    struct Node
    {
        T v;
        Node *par, *left, *right;
        size_t n_left = 0;

        Node()
        {
            v = T();
            par = left = right = nullptr;
        }

        Node(const T &x, Node *par = nullptr) : Node()
        {
            this->v = x;
            this->par = par;
        }

        bool IsLeft()
        {
            return par && par->left == this;
        }

        bool IsRight()
        {
            return par && par->right == this;
        }

        void IncLeft()
        {
            for (Node *n = this; n->par; n = n->par)
            {
                if (n->IsLeft())
                {
                    n->par->n_left++;
                }
            }
        }

        void DecLeft()
        {
            for (Node *n = this; n->par; n = n->par)
            {
                if (n->IsLeft())
                {
                    n->par->n_left--;
                }
            }
        }

        static Node *Nothing;

        Node *&PNext()
        {
            if (IsLeft())
            {
                return par->left;
            }
            else if (IsRight())
            {
                return par->right;
            }
            else
            {
                return Nothing;
            }
        }
    } *root;

    size_t N = 0;

public:
    Tree()
    {
        root = nullptr;
    }

    Tree(initializer_list<T> l) : Tree()
    {
        for (auto &x : l)
        {
            insert(x);
        }
    }

    template<class R> Tree(const R&l) : Tree()
    {
        for (const auto &x : l)
        {
            insert(x);
        }
    }

    ~Tree()
    {
        clear();
    }

    void clear()
    {
        while (!empty())
        {
            erase(root);
        }
    }

    size_t size()
    {
        return N;
    }

    void erase(Node *n)
    {
        if (n == nullptr)
        {
            return;
        }
        if (n->left && n->right)
        {
            Node *m = Min(n->right).n;
            erase(m);
            return;
        }
        else if (n->left == nullptr && n->right == nullptr)
        {
            if (n->par == nullptr)
            {
                root = nullptr;
            }
            else
            {
                n->PNext() = nullptr;
            }
        }
        else if (n->left)
        {
            if (n->par)
            {
                n->PNext() = n->left;
                n->left->par = n->par;
            }
            else
            {
                root = n->left;
                n->left->par = nullptr;
            }
        }
        else if (n->right)
        {
            if (n->par)
            {
                n->PNext() = n->right;
                n->right->par = n->par;
            }
            else
            {
                root = n->right;
                n->right->par = nullptr;
            }
        }
        N--;
        delete n;
    }

    void insert(const T &x)
    {
        if (root == nullptr)
        {
            N++;
            root = new Node(x);
        }
        else
        {
            Node *n = root;
            for (; n;)
            {
                if (x < n->v)
                {
                    if (n->left)
                    {
                        n = n->left;
                    }
                    else
                    {
                        N++;
                        n->left = new Node(x, n);
                        n->left->IncLeft();
                        return;
                    }
                }
                else if (n->v < x)
                {
                    if (n->right)
                    {
                        n = n->right;
                    }
                    else
                    {
                        N++;
                        n->right = new Node(x, n);
                        n->right->IncLeft();
                        return;
                    }
                }
                else
                {
                    return;
                }
            }
        }
    }

    bool empty()
    {
        return root == nullptr;
    }

    const T&operator[](size_t i)
    {
        for (Node * n = root; n;)
        {
            if (i == n->n_left)
            {
                return n->v;
            }
            else if (i < n->n_left)
            {
                n = n->left;
            }
            else
            {
                i -= n->n_left + 1;
                n = n->right;
            }
        }
        return root->v;
    }

    void split(const T&v, Tree&t0, Tree&t1)
    {
        auto it = find(v);
        if (it == end())
        {
            return;
        }
        t0.clear();
        t1.clear();
        t0.root = it.n->left;
        t1.root = it.n->right;
        for (Node * n = it.n; n->par; n = n->par)
        {
            if (n->IsRight())
            {
                n->par->right = t0.root;
                if (t0.root)
                {
                    t0.root->par = n->par;
                }
                t0.root = n->par;
            }
            if (n->IsLeft())
            {
                n->par->left = t1.root;
                if (t1.root)
                {
                    t1.root->par = n->par;
                }
                t1.root = n->par;
            }
        }
        t0.root->par = nullptr;
        t1.root->par = nullptr;
        delete it.n;
        root = nullptr;
    }
    //------------------
    struct iterator
    {
        Tree *t;
        Node *n;

        iterator(Tree *t = nullptr, Node *n = nullptr)
        {
            this->t = t;
            this->n = n;
        }

        bool operator!=(const iterator &b) const
        {
            return n != b.n;
        }

        void operator++()
        {
            n = t->next(n);
        }

        T &operator*()
        {
            return n->v;
        }

        operator bool()
        {
            return n != nullptr;
        }
    };

    iterator begin()
    {
        return Min();
    }

    iterator end()
    {
        return iterator(this, nullptr);
    }

    //------------------

    iterator Min(Node *n = nullptr)
    {
        if (n == nullptr)
        {
            n = root;
        }
        if (n == nullptr)
        {
            return iterator(this, nullptr);
        }
        while (n->left)
        {
            n = n->left;
        }
        return iterator(this, n);
    }

    Node *next(Node *n)
    {
        if (n->right)
        {
            return Min(n->right).n;
        }
        while (n->IsRight())
        {
            n = n->par;
        }
        return n->par;
    }

    iterator find(const T &x)
    {
        Node *n = root;
        while (n)
        {
            if (x < n->v)
            {
                n = n->left;
            }
            else if (n->v < x)
            {
                n = n->right;
            }
            else
            {
                break;
            }
        }
        return iterator(this, n);
    }
};

template <class T>
typename Tree<T>::Node *Tree<T>::Node::Nothing = nullptr;

int main()
{
    Tree<int> t = {8, 5, 15, 2, 7, 3, 10, 18, 9, 12, 11, 14, 16}, t0, t1;
    for (auto &x : t)
    {
        cout << x << " ";
    }
    cout << endl;
    t.split(12, t0, t1);
    for (auto &x : t0)
    {
        cout << x << " ";
    }
    cout << endl;
    for (auto &x : t1)
    {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}