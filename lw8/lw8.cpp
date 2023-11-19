#include <iostream>
#include <string>

using namespace std;


int finder(int n, int* nums, int counter[3]) {
    int ans = 0;

    for (int i = 0; i < counter[0]; i++) {
        if (nums[i] == 2) {
            for (int j = counter[0]; j < n; j++) {
				if (nums[j] == 1) {
					nums[i] = 1;
					nums[j] = 2;
					ans++;

					break;
				}
			}
        } else if (nums[i] == 3) {
            bool flag = false;

            for (int j = counter[0] + counter[1]; j < n; j++) {
				if (nums[j] == 1) {
					nums[i] = 1;
					nums[j] = 3;
					ans++;
                    flag = true;

					break;
				}
			}

            if (!flag) {
                for (int j = counter[0]; j < counter[0] + counter[1]; j++) {
                    if (nums[j] == 1) {
                        nums[i] = 1;
                        nums[j] = 3;
                        ans++;

                        break;
                    }
                }
            }
        }
    }

    for (int i = counter[0]; i < counter[0] + counter[1]; i++) {
        if (nums[i] == 3) {
            ans++;
        }
    }

    return ans;
}


int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    int n;

    while (cin >> n) {
        int* nums = new int[n];
    
        int counter[3] = {0, 0, 0};

        for (int i = 0; i < n; i++) {
            cin >> nums[i];

            counter[nums[i] - 1]++;
        }

        int ans = finder(n, nums, counter);
        

        cout << ans << '\n';

        delete[] nums;
    }

    return 0;
}
