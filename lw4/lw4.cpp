#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

void ToLower(char &ch) {
    if (ch >= 'A' && ch <= 'Z') {
        ch = 'a' + (ch - 'A');
    }
}

struct TTrieNode {
    unordered_map <string, TTrieNode *> next;
    bool isLeaf;
    TTrieNode *parent;
    TTrieNode *suffix;
    TTrieNode *exit;
    string patCH;
    vector <int> shift;

    TTrieNode() : isLeaf(false), parent(nullptr), suffix(nullptr), exit(nullptr) {}
    ~TTrieNode();
    void AddWord(vector <string> &word, int gShift, int pos);
    void LinkSuffix();
    void LinkS();
    void LinkExit();
    void LinkE();
};

void TTrieNode::AddWord(vector <string> &word, int gShift, int pos) {
    if (int(word.size()) == pos) {
        isLeaf = true;
        shift.push_back(word.size() + gShift);

        return;
    }
    for (int i = 0; i < int(word[pos].size()); i++) {
        ToLower(word[pos][i]);
    }
    if (next.find(word[pos]) == next.end()) {
        next[word[pos]] = new TTrieNode();
        next[word[pos]]->parent = this;
        next[word[pos]]->patCH = word[pos];
    }
    next[word[pos]]->AddWord(word, gShift, pos + 1);
}

void TTrieNode::LinkSuffix() {
    if (suffix == nullptr) {
        TTrieNode *newParent = this->parent;
        if (newParent->suffix == nullptr && newParent->parent != nullptr) {
            newParent->LinkSuffix();
        } else if (newParent->parent == nullptr) {
            suffix = newParent;

            return;
        }
        TTrieNode * suff = newParent->suffix;
        while (true) {
            if (suff->next.find(patCH) != suff->next.end()) {
                suffix = suff->next[patCH];

                return;
            }
            if (suff->suffix == nullptr && suff->parent != nullptr) {
                suff->LinkSuffix();
            } else if (suff->parent == nullptr) {
                suffix = suff;

                return;
            }
            suff = suff->suffix;
        }
    }
}

void TTrieNode::LinkS() {
    for (pair <string, TTrieNode *> word : next) {
        word.second->LinkS();
    }
    LinkSuffix();
}

void TTrieNode::LinkExit() {
    TTrieNode * suff = suffix;
    while (true) {
        if (suff->isLeaf == true) {
            exit = suff;

            return;
        } else if (suff->parent == nullptr) {
            return;
        }
        suff = suff->suffix;
    }
}

void TTrieNode::LinkE() {
    for (pair <string, TTrieNode *> word : next) {
        word.second->LinkE();
    }
    LinkExit();
}

TTrieNode::~TTrieNode() {
    for (pair <string, TTrieNode *> word : next) {
        delete word.second;
    }
}

struct TSymbol {
    int str;
    int col;
    int search;
    string symbol;

    TSymbol (string &symbol, int str, int col) {
        this->symbol = symbol;
        this->str = str;
        this->col = col;
        this->search = 0;
    }
};

class TTrie {
public:
    TTrie () : gShift(-1) {
        root = new TTrieNode();
    }
    ~TTrie ();
    void AddWord(vector <vector <string> > &vPattern);
    void LinkTrie();
    TTrieNode * root;
    int gShift;
    int wordCount;
};

TTrie::~TTrie () {
    delete root;
}

void TTrie::AddWord(vector <vector <string> > &vPattern) {
    for (vector <string> word : vPattern) {
        if (word[0] != "?") {
            int pos = 0;
            root->AddWord(word, gShift, pos);
            wordCount += 1;
        }
        gShift += word.size();
    }
}

void TTrie::LinkTrie() {
    for (pair <string, TTrieNode *> word : root->next) {
        word.second->LinkS();
        word.second->LinkE();
    }
}

class TText {
public:
    TText() : curLine(1), curColumn(1) {}
    ~TText();
    void AddLetter(string &symbol);
    void AddLine();
    void AddColumn();
    int Size();

    int curLine;
    int curColumn;
    vector <TSymbol *> symbols;
};

TText::~TText() {
    for (TSymbol * symbol : symbols) {
        delete symbol;
    }
}

void TText::AddLetter(string &symbol) {
    for (int i = 0; i < int(symbol.size()); i++) {
        ToLower(symbol[i]);
    }
    symbols.push_back(new TSymbol(symbol, curLine, curColumn));
    AddColumn();
}

void TText::AddLine() {
    curLine += 1;
    curColumn = 1;
}

void TText::AddColumn() {
    curColumn += 1;
}

int TText::Size() {
    return symbols.size();
}

class TAhoKorasik : TText, TTrie {

private:
    TText text;
    TTrie trie;
public:
    void InsertTrie(vector <vector <string> > &vPattern);
    void InsertText(vector <vector <string> > &vPattern);
    void Search();

};

void TAhoKorasik::InsertTrie(vector <vector <string> > &vPattern) {
    trie.AddWord(vPattern);
    trie.LinkTrie();
}

void TAhoKorasik::InsertText(vector <vector <string> > &vPattern) {
    for (vector <string> str : vPattern) {
        for (string symbol : str) {
            text.AddLetter(symbol);
        }
        text.AddLine();
    }
}

void TAhoKorasik::Search() {
    if (text.Size() != 0) {
        int curPos = 0;

        TTrieNode *curNode = trie.root;
        TTrieNode *ptr = nullptr;

        for (TSymbol *curLetter : text.symbols) {
            while (true) {
                if (curNode->next.find(curLetter->symbol) != curNode->next.end()) {
                    curNode = curNode->next[curLetter->symbol];
                    break;
                } else {
                    if (curNode->suffix != nullptr) {
                        curNode = curNode->suffix;
                    } else {
                        break;
                    }
                }
            }

            if (trie.gShift != -1) {
                if (curPos - trie.gShift >= 0) {
                    text.symbols[curPos - trie.gShift]->search += 1;
                    if (text.symbols[curPos - trie.gShift]->search == trie.wordCount + 1) {
                        cout << text.symbols[curPos - trie.gShift]->str << ", " << text.symbols[curPos - trie.gShift]->col << "\n";
                    }
                }
            }

            if (curNode->isLeaf == true) {
                for (int i : curNode->shift) {
                    if (curPos - i >= 0) {
                        text.symbols[curPos - i]->search += 1;
                        if (text.symbols[curPos - i]->search == trie.wordCount + 1) {
                            cout << text.symbols[curPos - i]->str << ", " << text.symbols[curPos - i]->col << "\n";
                        }
                    }
                }
            }

            if (curNode->exit != nullptr) {
                ptr = curNode->exit;
                while (ptr != nullptr) {
                    if (ptr->isLeaf == true) {
                        for (int i : ptr->shift) {
                            if (curPos - i >= 0) {
                                text.symbols[curPos - i]->search += 1;
                                if (text.symbols[curPos - i]->search == trie.wordCount + 1) {
                                    cout << text.symbols[curPos - i]->str << ", " << text.symbols[curPos - i]->col << "\n";
                                }
                            }
                        }
                    }

                    ptr = ptr->exit;
                }
            }

            ++curPos;
        }
    }
}

void Parser(string &str, vector<string> &words) {
    string splitWords;
    str += ' ';

    bool flag = true;

    for (int i = 0; i < str.length(); i++) {
        if (str[i] != ' ') {
            splitWords += str[i];
            flag = false;
        } else if (str[i] == ' ' && !flag) {
            words.push_back(splitWords);
            splitWords.clear();
            flag = true;
        } else {
            flag = true;
        }
    }
}

void Clear(vector <string> &pattern, vector <vector <string> > &vPattern, vector <string> &words, bool flag) {
    if (flag) {
        pattern.clear();
        vPattern.clear();
        words.clear();
    } else {
        pattern.clear();
        words.clear();
    }
}

TAhoKorasik ak;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    string str;
    vector <string> pattern;
    vector <vector <string> > vPattern;

    int flag = 0;
    vector <string> words;

    getline(cin, str);
    Parser(str, words);

    for (int i = 0; i < words.size(); i++) {
        if (words[i] == "?") {
            if (flag == 0) {
                flag = 2;
            } else if (flag == 1) {
                vPattern.push_back(pattern);
                pattern.clear();
                flag = 2;
            }
            pattern.push_back(words[i]);
        } else {
            if (flag == 0) {
                flag = 1;
            } else if (flag == 2) {
                vPattern.push_back(pattern);
                pattern.clear();
                flag = 1;
            }
            pattern.push_back(words[i]);
        }
    }
    vPattern.push_back(pattern);

    ak.InsertTrie(vPattern);
    Clear(pattern, vPattern, words, true);

    while (getline(cin, str)) {
        Parser(str, words);

        for (int i = 0; i < words.size(); i++) {
            pattern.push_back(words[i]);
        }

        vPattern.push_back(pattern);
        Clear(pattern, vPattern, words, false);
    }

    ak.InsertText(vPattern);
    ak.Search();

    return 0;
}