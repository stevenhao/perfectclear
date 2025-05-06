#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

int main() {
    ifstream inFile("book_100k.txt");
    ofstream outFile("public/data/book_compressed.txt");
    
    if (!inFile || !outFile) {
        cerr << "Error opening files" << endl;
        return 1;
    }
    
    string line;
    int lineCount = 0;
    
    while (getline(inFile, line) && lineCount < 100000) { // Limit to 100,000 lines
        outFile << line << "\n";
        lineCount++;
    }
    
    cout << "Compressed book file created with " << lineCount << " lines." << endl;
    return 0;
}
