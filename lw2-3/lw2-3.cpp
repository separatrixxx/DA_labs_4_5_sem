#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

const int T = 100;
const unsigned MAX_KEY_LENGTH = 256;

struct TBPair {
    string word;
    unsigned long long number;
};

struct TBNode {
    std::vector<TBPair> keys;
    std::vector<TBNode*> childs;
    TBNode() : keys(1), childs(2) {}
};

class TBTree {

private:
    TBNode *root = nullptr;
    void SearchNode(TBNode *curNode, TBNode* &res, unsigned &position, TBPair &pair);
    void SaveNode(TBNode *node, ofstream &file);
    void Restruct(TBNode* &node, TBNode* &toPushNode);
    void InsertNode(TBNode* &node, TBNode* &toPushNode, TBPair &pair);
    unsigned BinarySearch(vector<TBPair> &v, TBPair element);
    void RemoveNode(TBNode* &node, TBPair &pair);

public:
    TBTree();
	~TBTree();
    void Insert(TBPair &pair);
    void Remove(TBPair &pair);
    void Search(TBNode* &res, unsigned &position, TBPair &pair) ;
    void Save(ofstream &file);
    void Load(ifstream &file);
};

TBTree::TBTree() {
	root = nullptr;
}

TBTree::~TBTree() {
	delete root;
}

void TBTree::Restruct(TBNode* &node, TBNode* &toPushNode) {
        TBNode *leftNode = new TBNode;
        leftNode->keys.resize(T - 1);
        leftNode->childs.resize(T);

        TBNode* rightNode = new TBNode;
        rightNode->keys.resize(T - 1);
        rightNode->childs.resize(T);

        for (unsigned i = 0; i < T - 1; ++i) {
            leftNode->keys[i] = node->keys[i];
            leftNode->childs[i] = node->childs[i];
            rightNode->keys[i] = node->keys[T + i];
            rightNode->childs[i] = node->childs[T + i];
        }

        leftNode->childs[T - 1] = node->childs[T - 1];
        rightNode->childs[T - 1] = node->childs[2 * T - 1];

        TBNode *middleNode = new TBNode;
        middleNode->keys[0] = node->keys[T - 1];
        middleNode->childs[0] = leftNode;
        middleNode->childs[1] = rightNode;
        toPushNode = middleNode;

        for (unsigned i = 0; i < 2 * T; ++i) {
            node->childs[i] = nullptr;
        }

        delete node;
    }

unsigned TBTree::BinarySearch(vector<TBPair> &v, TBPair pair) {
    unsigned left = -1;
    unsigned right = v.size();

    while (left + 1 < right) {
        unsigned mid = (left + right) / 2;

        int compare = v[mid].word.compare(pair.word);

        if (compare < 0) {
            left = mid;
        }
        else {
            right = mid;
        }
    }
    return right;
}

void TBTree::InsertNode(TBNode* &node, TBNode* &toPushNode, TBPair &pair) {
    unsigned index = BinarySearch(node->keys, pair);

    if (node->childs[index] == nullptr) {
        node->keys.insert(node->keys.begin() + index, pair);
        node->childs.insert(node->childs.begin() + index, nullptr);
    } else {
        TBNode *newToPushNode = nullptr;
        InsertNode(node->childs[index], newToPushNode, pair);
        if (newToPushNode != nullptr) {
            node->childs[index] = newToPushNode->childs[1];
            node->keys.insert(node->keys.begin() + index, newToPushNode->keys[0]);
            node->childs.insert(node->childs.begin() + index, newToPushNode->childs[0]);
            newToPushNode->childs[0] = nullptr;
            newToPushNode->childs[1] = nullptr;

            delete newToPushNode;
        }
    }
    if (node->keys.size() == 2 * T - 1) {
        Restruct(node, toPushNode);
    }
}

void TBTree::Insert(TBPair &pair) {   
    if (root == nullptr) {
        root = new TBNode;
        root->keys[0] = pair;
        root->childs[0] = nullptr;
        root->childs[1] = nullptr;
    } else {
        TBNode* toPushNode = nullptr;
        InsertNode(root, toPushNode, pair);
        if (toPushNode != nullptr) {
            root = toPushNode;
        }
    }
}

void TBTree::RemoveNode(TBNode* &node, TBPair &pair) {
    TBNode *curNode = node;

    vector<TBNode*> path;
    vector<unsigned> indexes;

    unsigned index = BinarySearch(curNode->keys, pair);

    while (1) {
        path.push_back(curNode);
        indexes.push_back(index);

        if ((index < curNode->keys.size()) && curNode->keys[index].word == pair.word) {
            break;
        }
        curNode = curNode->childs[index];
        index = BinarySearch(curNode->keys, pair);
    }

    if (curNode->childs[index] != nullptr) {
        TBNode *curPathNode = curNode->childs[index];
        while (curPathNode->childs[curPathNode->childs.size() - 1] != nullptr) {
            curPathNode = curPathNode->childs[curPathNode->childs.size() - 1];
        }
        TBPair value = curPathNode->keys[curPathNode->keys.size() - 1];
        RemoveNode(node, value);

        TBNode *curSearchNode = nullptr;
        unsigned position = 0;

        SearchNode(node, curSearchNode, position, pair);
        curSearchNode->keys[position] = value;

        return;
    }

    while (path.size() > 1) {
        TBNode *curPathNode = path[path.size() - 1];
        path.pop_back();
        unsigned curPathIndex = indexes[indexes.size() - 1];
        indexes.pop_back();

        if (curPathNode->keys.size() == T - 1) {
            TBNode* curPathNodeParent = path[path.size() - 1];
            unsigned curPathIndexParent = indexes[indexes.size() - 1];
            TBNode *leftBrother = nullptr;

            if (curPathIndexParent > 0) {
                leftBrother = curPathNodeParent->childs[curPathIndexParent - 1];
            }

            if ((leftBrother != nullptr) && (leftBrother->keys.size() > T - 1)) {
                curPathNode->keys.erase(curPathNode->keys.begin() + curPathIndex);
                curPathNode->childs.erase(curPathNode->childs.begin() + curPathIndex);
                curPathNode->keys.insert(curPathNode->keys.begin(), curPathNodeParent->keys[curPathIndexParent - 1]);
                curPathNode->childs.insert(curPathNode->childs.begin(), leftBrother->childs[leftBrother->childs.size() - 1]);
                curPathNodeParent->keys[curPathIndexParent - 1] =  leftBrother->keys[leftBrother->keys.size() - 1];
                leftBrother->keys.pop_back();
                leftBrother->childs.pop_back();

                return;
            }

            TBNode *rightBrother = nullptr;

            if (curPathIndexParent < curPathNodeParent->childs.size() - 1) {
                rightBrother = curPathNodeParent->childs[curPathIndexParent + 1];	
            }

            if ((rightBrother != nullptr) && (rightBrother->keys.size() > T - 1)) {
                curPathNode->keys.erase(curPathNode->keys.begin() + curPathIndex);
                curPathNode->childs.erase(curPathNode->childs.begin() + curPathIndex);
                curPathNode->keys.push_back(curPathNodeParent->keys[curPathIndexParent]);
                curPathNode->childs.push_back(rightBrother->childs[0]);
                curPathNodeParent->keys[curPathIndexParent] = 
                rightBrother->keys[0];
                rightBrother->keys.erase(rightBrother->keys.begin());
                rightBrother->childs.erase(rightBrother->childs.begin());

                return;
            }

            curPathNode->keys.erase(curPathNode->keys.begin() + curPathIndex);
            curPathNode->childs.erase(curPathNode->childs.begin() + curPathIndex);

            TBNode *newNode = new TBNode;
            newNode->keys.resize(2 * T - 2);
            newNode->childs.resize(2 * T - 1);

            if (leftBrother != nullptr) {
                indexes.pop_back();
                indexes.push_back(curPathIndexParent - 1);

                for (unsigned i = 0; i < T - 1; ++i) {
                    newNode->keys[i] = leftBrother->keys[i];
                    newNode->childs[i] = leftBrother->childs[i];
                }

                newNode->keys[T - 1] =  curPathNodeParent->keys[curPathIndexParent - 1];
                newNode->childs[T - 1] = 
                leftBrother->childs[T - 1];

                for (unsigned i = 0; i < T - 2; ++i) {
                    newNode->keys[T + i] = curPathNode->keys[i];
                    newNode->childs[T + i] = 
                    curPathNode->childs[i];
                }

                newNode->childs[2 * T - 2] = curPathNode->childs[T - 2];

                for (unsigned i = 0; i < leftBrother->childs.size(); ++i) {
                    leftBrother->childs[i] = nullptr;
                }

                delete leftBrother;

                for (unsigned i = 0; i < curPathNode->childs.size(); ++i) {
                    curPathNode->childs[i] = nullptr;
                }

                delete curPathNode;

                curPathNodeParent->childs[curPathIndexParent] = newNode;
                curPathNodeParent->childs[curPathIndexParent - 1] = nullptr;
            } else if (rightBrother != nullptr) {
                for (unsigned i = 0; i < T - 2; ++i) {
                    newNode->keys[i] = curPathNode->keys[i];
                    newNode->childs[i] = curPathNode->childs[i];
                }

                newNode->keys[T - 2] = curPathNodeParent->keys[curPathIndexParent];
                newNode->childs[T - 2] = 
                curPathNode->childs[T - 2];

                for (unsigned i = 0; i < T - 1; ++i) {
                    newNode->keys[T - 1 + i] = rightBrother->keys[i];
                    newNode->childs[T - 1 + i] = 
                    rightBrother->childs[i];
                }

                newNode->childs[2 * T - 2] = 
                rightBrother->childs[T - 1];

                for (unsigned i = 0; i < curPathNode->childs.size(); ++i) {
                    curPathNode->childs[i] = nullptr;
                }

                delete curPathNode;

                for (unsigned i = 0; i < rightBrother->childs.size(); ++i) {
                    rightBrother->childs[i] = nullptr;
                }

                delete rightBrother;

                curPathNodeParent->childs[curPathIndexParent] = nullptr;
                curPathNodeParent->childs[curPathIndexParent + 1] = newNode;
            }
        } else {
            curPathNode->keys.erase(curPathNode->keys.begin() + curPathIndex);
            curPathNode->childs.erase(curPathNode->childs.begin() + curPathIndex);

            return;
        }
    }

    TBNode* curPathNode = path[path.size() - 1];
    path.pop_back();
    unsigned curPathIndex = indexes[indexes.size() - 1];
    indexes.pop_back();
    
    if (curPathNode->keys.size() > 1) {
        curPathNode->keys.erase(curPathNode->keys.begin() + curPathIndex);
        curPathNode->childs.erase(curPathNode->childs.begin() + curPathIndex);
    } else {
        if (curPathNode->childs[0] == nullptr) {
            node = curPathNode->childs[1];
            curPathNode->childs[1] = nullptr;
        } else if (curPathNode->childs[1] == nullptr) {
            node = curPathNode->childs[0];
            curPathNode->childs[0] = nullptr;
        }
        delete curPathNode;
    }
}

void TBTree::Remove(TBPair &pair) {
    RemoveNode(root, pair);
}

void TBTree::SearchNode(TBNode *curNode, TBNode* &res, unsigned &position, TBPair &pair) {
    TBNode *node = curNode;

    while (node != nullptr) {
        unsigned index = BinarySearch(node->keys, pair);

        if ((index < node->keys.size()) && pair.word == node->keys[index].word) {
            res = node;
            position = index;

            return;
        } else {
            node = node->childs[index];
        }
    }

    res = nullptr;
}

void TBTree::Search(TBNode* &res, unsigned &position, TBPair &pair) {
    SearchNode(root, res, position, pair);
}

void TBTree::SaveNode(TBNode *curNode, ofstream &file) {
    if (curNode == nullptr) {
		unsigned long long size = 0;
        file.write(reinterpret_cast<char *>(&size), sizeof(unsigned long long));
	} else {
		unsigned long long size = curNode->keys.size();

        file.write(reinterpret_cast<char *>(&size), sizeof(unsigned long long));

		for (unsigned i = 0; i < curNode->keys.size(); ++i) {
            unsigned wordSize = curNode->keys[i].word.size();
            file.write(reinterpret_cast<char *>(&wordSize), sizeof(unsigned));

            for (unsigned j = 0; j < curNode->keys[i].word.size(); ++j) {
                file.write(reinterpret_cast<char *>(&curNode->keys[i].word[j]), sizeof(char));
            }

            file.write(reinterpret_cast<char *>(&curNode->keys[i].number), sizeof(unsigned long long));
		}

		for (unsigned short i = 0; i < curNode->childs.size(); ++i) {
			SaveNode(curNode->childs[i], file);
		}
	}
}

void TBTree::Save(ofstream &file) {
    SaveNode(root, file);
}

TBNode* LoadNode(ifstream &file) {
	unsigned long long size = 0;
    file.read(reinterpret_cast<char *>(&size), sizeof(unsigned long long));
	if (size == 0) {
		return nullptr;
	} else {
		TBNode* curNode = new TBNode;
		curNode->keys.resize(size);
		curNode->childs.resize(size + 1);
		for (unsigned i = 0; i < size; ++i) {
            unsigned wordSize;
            file.read(reinterpret_cast<char *>(&wordSize), sizeof(unsigned));
            curNode->keys[i].word.resize(wordSize);

            for (unsigned j = 0; j < wordSize; ++j) {
                file.read(reinterpret_cast<char *>(&curNode->keys[i].word[j]), sizeof(char));
            }

            file.read(reinterpret_cast<char *>(&curNode->keys[i].number), sizeof(unsigned long long));
		}
		for (unsigned i = 0; i < size + 1; ++i) {
			curNode->childs[i] = LoadNode(file);
		}
		return curNode;
	}
}

void TBTree::Load(ifstream &file) {
    delete root;
	root = nullptr;
	root = LoadNode(file);
}

void ToLower(string &word) {
    for (char &i: word) {
        i = tolower(i);
    } 
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    string action;
    string word;
    unsigned long long number;

    TBTree tree;
    TBPair pair;

    while (cin >> action) {
        if (action == "+") {
            cin >> word >> number;

            ToLower(word);

            pair.word = word;
            pair.number = number;

            TBNode *curSearchNode = nullptr;
            unsigned curSearchPosition = 0;

            tree.Search(curSearchNode, curSearchPosition, pair);

            if (curSearchNode != nullptr) {
                cout << "Exist\n";
            } else {
                tree.Insert(pair);

                cout << "OK\n";
            }
        } else if (action == "-") {
            cin >> word;

            ToLower(word);

            pair.word = word;

            TBNode *curSearchNode = nullptr;
            unsigned curSearchPosition = 0;

            tree.Search(curSearchNode, curSearchPosition, pair);

            if (curSearchNode != nullptr) {
                tree.Remove(pair);
                cout << "OK\n";
            }
            else {
                cout << "NoSuchWord\n";
            }
        } else if (action == "!") {
            string command;

            cin >> command;

            if (command == "Save") {
                string fPath;

                cin >> fPath;

				ofstream file(fPath, ios::binary);

				tree.Save(file);
                file.close();

                cout << "OK\n";
            } else {
                string fPath;

                cin >> fPath;

				ifstream file(fPath, ios::binary);
				
                tree.Load(file);
                file.close();

                cout << "OK\n";
            }
        } else {
            ToLower(action);

            pair.word = action;

            TBNode *curSearchNode = nullptr;
            unsigned curSearchPosition = 0;

            tree.Search(curSearchNode, curSearchPosition, pair);

            if (curSearchNode != nullptr) {
                cout << "OK: " << curSearchNode->keys[curSearchPosition].number << "\n";
            } else {
                cout << "NoSuchWord\n";
            }
        }
    }
    
    return 0;
}