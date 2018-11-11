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

class SegmentTree {

    vector<VALUE_TYPE> nodes;

    VALUE_TYPE traversal(int index, int cur_l, int cur_r, int l, int r);
    void inc_traversal(int index, int  cur_l, int cur_r, int position);
    size_t get_n();

public:

    SegmentTree(size_t size): nodes( pow(2, ceil(log2(size)) + 1) ) {};
    SegmentTree(const SegmentTree& rhs): nodes(rhs.nodes) {}

    void increment(int index);
    VALUE_TYPE count(int left, int right);
};

void SegmentTree::increment(int position) {
    inc_traversal(1, 0, get_n(), position);
}

VALUE_TYPE SegmentTree::count(int left, int right){
    return traversal(1, 0, get_n(), left, right);;
}

VALUE_TYPE SegmentTree::traversal(int index, int cur_l, int cur_r, int l, int r) {
    if (l > r)
        return 0;

    if (cur_l == l && cur_r == r) {
        return nodes[index].count;
    }
    int middle = (cur_l + cur_r) / 2;
    return traversal(index * 2, cur_l, middle, l, std::min(r, middle))
                        + traversal(index * 2 + 1, middle + 1, cur_r, std::max(l, middle + 1), r);
}

void SegmentTree::inc_traversal(int index, int  cur_l, int cur_r, int position) {
    if (cur_l == cur_r) {
        nodes[index].count++;
        return;
    }

    int middle = (cur_l + cur_r) / 2;
    if (position <= middle) {
        inc_traversal(index * 2, cur_l, middle, position);
    } else {
        inc_traversal(index * 2 + 1, middle + 1, cur_r, position);
    }

    nodes[index].count = nodes[index * 2].count + nodes[index * 2 + 1].count;
}

size_t SegmentTree::get_n() {
    return nodes.size() / 2 - 1;
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
        items[i].second = i;
    }

    sort(items.begin(), items.end(), [](auto &left, auto &right) {
        return left.first < right.first;
    });

    vector<pair<int, SegmentTree*>> trees;

    int current = -1;
    for (auto &i : items) {
        if (current != i.first) {
            current = i.first;
            SegmentTree *tree = new SegmentTree(items.size());
            tree->increment(i.second);
            trees.push_back(make_pair(current, tree));
        } else {
            SegmentTree *tree = trees.end()->second;
            tree->increment(i.second);
        }
    }

    for (int j = 0; j < q; ++j) {
        int l, r, x, y;
        cin >> l >> r >> x >> y;

        auto up = upper_bound(trees.begin(), trees.end(), y, [](auto &left, auto &right) {
            return left.first < right.first;
        });

        auto low = lower_bound(trees.begin(), trees.end(), y, [](auto &left, auto &right) {
            return left.first < right.first;
        });

        cout << up->second->count(l, r) - low->second->count(l, r) << endl;
    }


    return 0;
}
