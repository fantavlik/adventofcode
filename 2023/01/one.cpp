#include <iostream>
#include <fstream>
#include <map>
using namespace std;

int main()
{
    // Map string digits to ints
    std::map<string, int> digs;
    digs["0"] = 0;
    digs["1"] = 1;
    digs["2"] = 2;
    digs["3"] = 3;
    digs["4"] = 4;
    digs["5"] = 5;
    digs["6"] = 6;
    digs["7"] = 7;
    digs["8"] = 8;
    digs["9"] = 9;

    std::ifstream file("input.txt");
    if (file.is_open()) {
        std::string line;
        int sum = 0;
        while (std::getline(file, line)) {
            // Process line
            int first = -1;
            int last = -1;
            for (int i = 0; i < line.length(); i++) {
                string ch;
                ch = line[i];
                if (digs.find(ch) == digs.end()) {
                    continue; // not in map
                }
                int val = digs.at(ch);
                if (first == -1) {
                    first = val;
                }
                last = val;
            }
            sum += 10*first + last;
            printf("\n%s -> %d%d, new sum = %d", line.c_str(), first, last, sum);
        }
        file.close();
    }
    return 0;
}