#include <fstream>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;

enum Side {right=0, left=1};

struct input_node
{
    int key;
    int children[2];

    input_node(int key, int left, int right) : key(key)
    {
        children[0] = left;
        children[1] = right;
    }
};

struct node
{
    int key;
    int height;
    int inp_num;
    node* children[2];

    node(int key, node * left, node * right, int num) : key(key), height(1), inp_num(num)
    {
        children[0] = left;
        children[1] = right;
    }

    node(input_node * node, int num) : height(1), key(node->key), inp_num(num)
    {
        children[0] = children[1] = nullptr;
    }
};

struct avl_tree
{
    vector<input_node *> input_tree;
    vector<input_node *> output_tree;
    node * root = nullptr;

    void make_tree()
    {
        root = new node(input_tree[0], 0);
        dfs_make(root);
    }

    void dfs_make(node * v)
    {
        for (int i = 0; i < 2; i++)
        {
            if (input_tree[v->inp_num]->children[i] == -1)
                v->children[i] = nullptr;
            else
            {
                v->children[i] =
                        new node(input_tree[input_tree[v->inp_num]->children[i]], input_tree[v->inp_num]->children[i]);
                dfs_make(v->children[i]);
            }
        }
    }

    int count_balance(node * v)
    {
        return (v->children[1] != nullptr ? v->children[1]->height : 0) -
               (v->children[0] != nullptr ? v->children[0]->height : 0);
    }

    void fix_height(node * v)
    {
        v->height = max(height_left(v), height_right(v)) + 1;
    }

    int height_right(node * v)
    {
        return v->children[1] == nullptr ? 0 : v->children[1]->height;
    }

    int height_left(node * v)
    {
        return v->children[0] == nullptr ? 0 : v->children[0]->height;
    }

    void count_height()
    {
        if (input_tree.empty())
            return ;
        dfs(root);
    }

    void dfs(node * v)
    {
        for (auto child : v->children)
        {
            if (child != nullptr)
                dfs(child);
        }
        fix_height(v);
    }

    node * rotate(node * v, int side)
    {
        node * u = v->children[side == Side::left ? 1 : 0];
        v->children[side == Side::left ? 1 : 0] = u->children[side == Side::left ? 0 : 1];
        u->children[side == Side::left ? 0 : 1] = v;
        fix_height(v);
        fix_height(u);
        return u;
    }

    node * balance(node * v)
    {
        fix_height(v);
        if (count_balance(v) > 1)
        {

            if (count_balance(v->children[1]) < 0)
                v->children[1] = rotate(v->children[1], Side::right);
            return rotate(v, Side::left);
        }
        if (count_balance(v) < -1)
        {
            if (count_balance(v->children[0]) > 0)
                v->children[0] = rotate(v->children[0], Side::left);
            return rotate(v, Side::right);
        }
        return v;
    }

    void make_output_tree()
    {
        queue<node *> queue;
        queue.push(root);
        int num = 1;
        while (!queue.empty())
        {
            node * v = queue.front();
            queue.pop();
            int children_num[2] = {0, 0};
            for (int i = 0; i < 2; i++)
            {
                if (v->children[i] != nullptr)
                {
                    queue.push(v->children[i]);
                    children_num[i] = ++num;
                }
            }
            output_tree.push_back(new input_node(v->key, children_num[0], children_num[1]));
        }
    }

    node * insert(node * root, int key)
    {
        if (root == nullptr)
            return new node(key, nullptr, nullptr, static_cast<int>(input_tree.size() + 1));
        root->children[key < root->key ? 0 : 1] = insert(root->children[key < root->key ? 0 : 1], key);
        return balance(root);
    }
};

int main()
{
    ifstream in("addition.in");
    ofstream out("addition.out");

    size_t n;
    in >> n;
    auto avl = new avl_tree();

    for (int i = 0; i < n; i++)
    {
        int key;
        int left, right;
        in >> key >> left >> right;
        avl->input_tree.push_back(new input_node(key, left - 1, right - 1));
    }

    int key;
    in >> key;

    if (n == 0)
    {
        out << 1 << "\n" << key << " " << 0 << " " << 0 << endl;
        return 0;
    }

    avl->make_tree();
    avl->count_height();
    avl->root = avl->insert(avl->root, key);
    avl->make_output_tree();

    out << n + 1 << endl;
    for (auto v : avl->output_tree)
        out << v->key << " " << v->children[0] << " " << v->children[1] << endl;
}
