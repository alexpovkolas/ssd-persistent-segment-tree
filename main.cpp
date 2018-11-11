//
// Created by Aliaksandr Paukalas on 10/17/18.
//
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <cstdio>
#include <fstream>

//#define __PROFILE__

using namespace std;

typedef unsigned int VALUE_TYPE;

class PersistentSegmentTree {

    struct Node {
        VALUE_TYPE value;
        size_t left;
        size_t right;

        Node(VALUE_TYPE value = 0, size_t left = (size_t)-1, size_t right = (size_t)-1): value(value), left(left), right(right) {}
    };

    vector<Node> nodes;
    vector<size_t> roots;
    size_t element_count;
    size_t values_first_index;

    void build(size_t current, size_t current_depth_count);
    VALUE_TYPE traversal(size_t from, size_t to, size_t current, size_t left, size_t right);
    void inc_traversal(size_t position, size_t previous, size_t current_depth_count, size_t left, size_t right);

public:

    PersistentSegmentTree(size_t size);
    PersistentSegmentTree(const PersistentSegmentTree& rhs): nodes(rhs.nodes) {}

    size_t increment(size_t index);
    VALUE_TYPE count(size_t left, size_t right, size_t version);
};


PersistentSegmentTree::PersistentSegmentTree(size_t size): element_count(size), values_first_index((size_t)-1), roots(1, 0) {
    build(0, 1);
}

void PersistentSegmentTree::build(size_t current, size_t current_depth_count) {
    nodes.resize(max(nodes.size(), current + 1));

    if (current_depth_count >= element_count)
    {
        if (values_first_index == (size_t)-1)
        {
            values_first_index = current;
        }

        nodes[current] = Node();
    } else {
        size_t left = 2 * current + 1;
        size_t right = 2 * current + 2;

        build(left, current_depth_count * 2);
        build(right, current_depth_count * 2);

        nodes[current] = Node(nodes[left].value + nodes[right].value, left, right);
    }
}

VALUE_TYPE PersistentSegmentTree::count(size_t left, size_t right, size_t version){
    return traversal(left, right, roots[version], 0, values_first_index);
}

VALUE_TYPE PersistentSegmentTree::traversal(size_t from, size_t to, size_t current, size_t left, size_t right) {
    if (from > to) {
        return 0;
    }
    if (from == left && to == right) {
        return nodes[current].value;
    }
    size_t middle = (left + right) / 2;
    return traversal(from, min(to, middle), nodes[current].left, left, middle) +
                       traversal(max(middle + 1, from), to, nodes[current].right, middle + 1, right);
}

size_t PersistentSegmentTree::increment(size_t position) {
    size_t current = *roots.end();
    roots.push_back(nodes.size());
    inc_traversal(position, current, 1, 0, values_first_index);
    return roots.size();
}

void PersistentSegmentTree::inc_traversal(size_t position, size_t previous, size_t current_depth_count, size_t left, size_t right) {
    size_t new_node = nodes.size();
    nodes.push_back(Node());

    if (current_depth_count >= element_count)
    {
        nodes[new_node].value++;
    } else {
        size_t middle = (left + right) / 2;
        if (position <= middle)
        {
            nodes[new_node].left = nodes.size();
            nodes[new_node].right = nodes[previous].right;
            inc_traversal(position, nodes[previous].left, 2 * current_depth_count, left, middle);
        } else {
            nodes[new_node].left = nodes[previous].left;
            nodes[new_node].right = nodes.size();
            inc_traversal(position, nodes[previous].right, 2 * current_depth_count, middle + 1, right);
        }

        nodes[new_node].value = nodes[nodes[new_node].left].value + nodes[nodes[new_node].right].value;
    }
}


int main() {
#ifdef __PROFILE__
    ifstream in("input");
    cin.rdbuf(in.rdbuf());
#endif

    int n = 0;
    int q = 0;

    cin >> n >> q;
    vector<pair<int, int>> items(n);

    for (int i = 0; i < n; ++i) {
        cin >> items[i].first;
        items[i].second = i + 1;
    }

    sort(items.begin(), items.end(), [](auto &left, auto &right) {
        return left.first < right.first;
    });

    vector<pair<int, size_t >> trees;


    int current = -1;
    PersistentSegmentTree tree(items.size());
    for (auto &i : items) {
        size_t version = tree.increment(i.second);
        if (current != i.first) {
            current = i.first;
            trees.push_back(make_pair(i.first, version));
        } else {
            trees.back().second = version;
        }
    }

    for (int j = 0; j < q; ++j) {
        int l, r, x, y;
        cin >> l >> r >> x >> y;

        auto up = upper_bound(trees.begin(), trees.end(), make_pair(y, NULL), [](auto &left, auto &right) {
            return left.first < right.first;
        });

        auto low = lower_bound(trees.begin(), trees.end(), make_pair(x, NULL), [](auto &left, auto &right) {
            return left.first < right.first;
        });

        if (up != trees.begin()) {
            --up;
        }

        VALUE_TYPE up_count = tree.count(l, r, up->second);

        VALUE_TYPE low_count = 0;
        if (low != trees.begin()) {
            low--;
            low_count = tree.count(l, r, low->second);
        }

        cout << up_count - low_count << endl;
    }


    return 0;
}
