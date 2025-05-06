#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;
typedef long long ll;

int main() {
    ifstream inFile("book_100k.txt");
    ofstream outFile("public/data/book_compressed.txt");
    
    if (!inFile || !outFile) {
        cerr << "Error opening files" << endl;
        return 1;
    }
    
    int turn, msk, cnt;
    ll board;
    int entries = 0;
    
    while (inFile >> turn >> board >> msk >> cnt) {
        outFile << turn << " " << board << " " << msk << " " << cnt << "\n";
        entries++;
    }
    
    cout << "Compressed book file created with " << entries << " entries." << endl;
    return 0;
}
