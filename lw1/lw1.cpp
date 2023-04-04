#include <iostream>
using namespace std;

int THREE_MAX = 3;

template <typename T>
class TVec {
    private:
        T* array;
        unsigned int capacity;
        unsigned int size;
    public:
        TVec(TVec<T> &other) {
            capacity = other.capacity;
            size = other.size;
            array = new T[size];
            for (unsigned int i = 0; i < other.size; ++i) {
                array[i] = other.array[i];
            }
        }
        
        TVec() {
            array = new T[1000000];
            capacity = 1000000;
            size = 0;
        }

        TVec(unsigned int size) {
            array = new T[size];
            capacity = size;
            this->size = size;
        }

        ~TVec() {
            delete [] array;
            capacity = 0;
            size = 0;
        }

        T& operator[](unsigned int pos) {
            return Get(pos);
        }

        void Push(T data) {
            if (size >= capacity) {
                T* temp = new T[2 * capacity];
                for (unsigned int i = 0; i < capacity; i++) {
                    temp[i] = array[i];
                }
                delete[] array;
                capacity *= 2;
                array = temp;
            }
            array[size] = data;
            size++;
        }

        void Push(T data, int index) {
            array[index] = data;
        }
        
        T& Get(int index) {
            return array[index];
        }

        void Pop() {
            size--;
        }

        int Size() { 
            return size; 
        }
};

struct KV {
    KV() = default;
    KV(unsigned long long key, unsigned long long value, int flag): key(key), value(value), flag(flag) {}
    unsigned long long key;
    unsigned long long value;
    int flag;
};

void CountingSort(TVec<KV> &input, unsigned long long maxNum, 
                  long a, long b, TVec<KV> &result)
{
    TVec<size_t> B(maxNum + 1);

    for(unsigned long long i = 0; i <= maxNum; i++){
        B[i] = 0;
    }

    for (unsigned int i = 0; i < input.Size(); i++) {
        B[input[i].key % a / b] += 1;
    }

    for (size_t i = 1; i < B.Size(); ++i) {
        B[i] += B[i - 1];
    }

    for (int32_t i = input.Size() - 1; i >= 0; --i) {
        size_t index = B[input[i].key % a / b] - 1;
        result[index] = input[i];
        --B[input[i].key % a / b];
    }
}

void RadixSort(TVec<KV> &elems, size_t maxLength, TVec<KV> &result) {
    TVec<KV> newElems = elems;

    long a = 10;
    long b = 1;

    for (int i = maxLength - 1; i >= 0; i--) {
        unsigned long long maxNum = 0;

        for (unsigned int j = 0; j < result.Size(); j++) {
            maxNum = max(maxNum, elems[j].key % a / b);
        }

        CountingSort(newElems, maxNum, a, b, result);

        a *= 10;
        b *= 10;

        for (unsigned int i = 0; i < result.Size(); i++) {
            newElems[i] = result[i];
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    TVec<KV> elems;
    std::string key;
    unsigned long long value;

    size_t maxLength = 0;
    
    while (cin >> key >> value) {
        string a;
        int flag = 0;
        bool check = true;

        for (int i = 0; i <= key.length() - 1; i++) {
            if (key[i] != '+' && key[i] != '-') {
                a += key[i];

                if (key[i] == '0' && check) {
                    flag++;
                } else {
                    check = false;
                }
            }
        }

        unsigned long long newKey = stol(a);

        elems.Push( KV(newKey, value, flag) );
        maxLength = max(maxLength, key.length());
    }

    if (elems.Size() == 0) {
        return 0;
    }

    TVec<KV> result(elems.Size());
    RadixSort(elems, maxLength - THREE_MAX, result);

    for (unsigned int i = 0; i < result.Size(); i++) {
        string str = to_string(result[i].key);

        for (int j = 0; j < result[i].flag; j++) {
            str = '0' + str;
        }

        int len = str.length();

        cout << '+' 
        << str.substr(0, len - 10) 
        << '-' 
        << str.substr(len - 10, 3) 
        << '-' 
        << str.substr(len - 7, 7)
        << '\t' 
        << result[i].value 
        << '\n';
    }

    return 0;
}