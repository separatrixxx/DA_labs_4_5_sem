#include <iostream>
#include <string>
#include <math.h>

using namespace std;

long long counter(long long n, long long m) {
    if (m > n) {
        return 0;
    }

    long long counter = 0;

    string newN = to_string(n);

    // long long lengthN = (long long)log10(n) + 1;
    long long lengthN = newN.length();
    
    for (long long i = (long long)log10(m); i < lengthN; i++) {
        long long startDiv = pow(10, i);

        long long minM = 0;
        long long maxM = 0;

        for (long long j = startDiv; j <= stoll(newN.substr(0, i + 1)); j++) {
            if (j % m == 0) {
                minM = j;
                
                break;
            }
        }

        for (long long j = stoll(newN.substr(0, i + 1)); j >= startDiv; j--) {
            //cout << "j = " << j << '\n';
            if (j % m == 0) {
                if (j == n) {
                    maxM = j - m;
                } else {
                    maxM = j;
                }
                
                break;
            }
        }

        // cout << "minM = " << minM << '\n';
        // cout << "maxM = " << maxM << '\n';

        if (minM != 0 || maxM != 0) {
            counter += (maxM - minM) / m + 1;
        }
    }

    return counter;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    long long n;
    long long m;

    while (cin >> n >> m){
        cout << counter(n, m) << '\n';
    }

    return 0;
}
