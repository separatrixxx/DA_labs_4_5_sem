#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class TTrie {
    struct TNode {
        TNode() = default;
        TNode(int begin, int end): begin(begin), end(end), suffLink(nullptr) {}
        map<char, TNode*> to;
        int begin, end;
        TNode *suffLink;
    };

public:
    TTrie() : activeEdgeSize(0), remainder(0) {}
    void BuildTrie(const string& str);
    void Search(const string& pattern, int count);

private:
    string text;
    TNode *root;
    TNode *lastAdded, *activeNode;
    int activeEdgeSize;
    int activeEdge;
    int remainder;

    void InsertChar(int addingCharIdx);
    void SearchNode(TNode *node, vector<int> &result, int patternPos);
    void SplitEdge(TNode* node, int posIdx);
};

void TTrie::BuildTrie(const string& str) {
    text = str;
    root = new TNode(text.size(), text.size());
    lastAdded = root;
    activeNode = root;
    activeEdge = 0;
    root->suffLink = root;

    for (int i = 0; i < text.size(); i++) {
        InsertChar(i);
    }
}

void TTrie::InsertChar(int addingCharIdx) {
    lastAdded = root;
    remainder++;

    while (remainder) {
        if (activeEdgeSize == 0) {
            activeEdge = addingCharIdx;
        }

        auto textPos = activeNode->to.find(text[activeEdge]);
        TNode *newNode;

        if (textPos == activeNode->to.end()) {
            TNode *leaf = new TNode(addingCharIdx, text.size());
            activeNode->to[text[activeEdge]] = leaf;

            if (lastAdded != root) {
                lastAdded->suffLink = activeNode;
            }

            lastAdded = activeNode;
        } else {
            newNode = textPos->second;
            int newEdgeSize = min(addingCharIdx + 1, newNode->end) - newNode->begin;

            if (activeEdgeSize >= newEdgeSize) {
                activeEdge += newEdgeSize;
                activeEdgeSize -= newEdgeSize;
                activeNode = newNode;

                continue;
            }

            if (text[newNode->begin + activeEdgeSize] == text[addingCharIdx]) {
                activeEdgeSize++;

                if (lastAdded != root) {
                    lastAdded->suffLink = activeNode;
                }

                lastAdded = activeNode;

                break;
            }

            SplitEdge(newNode, addingCharIdx);
        }

        remainder--;

        if (activeNode == root && activeEdgeSize) {
            activeEdgeSize--;
            activeEdge = addingCharIdx - remainder + 1;
        } else {
            activeNode = activeNode->suffLink;
        }
    }
}

void TTrie::SplitEdge(TNode* node, int posIdx) {
    TNode *split = new TNode(node->begin, node->begin + activeEdgeSize);
    TNode *leaf = new TNode(posIdx, text.size());
    activeNode->to[text[activeEdge]] = split;

    split->to[text[posIdx]] = leaf;
    node->begin += activeEdgeSize;
    split->to[text[node->begin]] = node;

    if (lastAdded != root) {
        lastAdded->suffLink = split;
    }

    lastAdded = split;
}

void TTrie::SearchNode(TNode *node, vector<int> &result, int patternPos) {
    if (node->end == text.size()) {
        result.push_back(text.size() - patternPos + 1);
    } else {
        int depPlus;
        for (const auto& [key, child] : node->to) {
            depPlus = child->end - child->begin;
            SearchNode(child, result, patternPos + depPlus);
        }
    }
}

void PrintAnswer(vector<int> ans, int count) {
    if (ans.size() != 0) {
        cout << count << ": ";
            
        for (int i = 0; i < ans.size(); i++) {
            cout << ans[i];

            if (i != ans.size() - 1) {
                cout << ", ";
            } else {
                cout << "\n";
            }
        }
    }
}

void TTrie::Search(const string& pattern, int count) {
    vector<int> result;
    bool flag = false;

    int patternPos = 0;
    TNode *node = root;

    int i = 0;

    while (i < pattern.size()) {
        auto pathTo = node->to.find(pattern[i]);

        if (pathTo == node->to.end()) {
            flag = true;

            break;
        }

        node = pathTo->second;
        patternPos += node->end - node->begin;

        int j;

        for (j = 0; i + j < pattern.size() && node->begin + j < node->end; ++j) {
            if (text[node->begin + j] != pattern[i + j]) {
                flag = true;
                
                break;
            }
        }

        i += j;

        if (i == pattern.size()) {
            break;
        }
    }

    if (pattern.size() < text.size() && !flag) {
        SearchNode(node, result, patternPos);
    }

    sort(result.begin(), result.end());

    PrintAnswer(result, count);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    TTrie trie;
    string text;
    cin >> text;
    text += "$";

    trie.BuildTrie(text);

    int count = 1;
    string pattern;

    while (cin >> pattern) {
        trie.Search(pattern, count);
        count++;
    }

    return 0;
}
