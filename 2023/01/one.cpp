#include <iostream>
#include <fstream>
#include <map>

int main()
{
    // Map string digits to ints
    std::map<std::string, int> DIGS {
        {"1", 1}, {"one",   1},
        {"2", 2}, {"two",   2},
        {"3", 3}, {"three", 3},
        {"4", 4}, {"four",  4},
        {"5", 5}, {"five",  5},
        {"6", 6}, {"six",   6},
        {"7", 7}, {"seven", 7},
        {"8", 8}, {"eight", 8},
        {"9", 9}, {"nine",  9}
    };
    static const size_t N_KEYS = 18;
    // List of all string keys to map above, it would be great if std::map
    // granted access to the underlying pointer to the keys but that doesn't
    // seem to be supported
    std::string dig_keys[N_KEYS] = {};
    int d = 0;
    for (std::pair<const std::string, int> keypair: DIGS) {
        dig_keys[d++] = keypair.first;
    }

    // We could pre-process suffixes into trie, so that we can
    // check all possible digits in linear time. For this dictionary
    // it's overkill though - instead we'll just elminate possible
    // dictionary keys as we go and keep around a list of "contenders"
    // to prevent the worst n^2 case

    std::ifstream file("input.txt");
    if (file.is_open()) {
        std::string line;
        // Vector of remaining viables possible matches
        std::vector<std::string> contenders;
        int sum = 0;
        while (std::getline(file, line)) {
            // Process line
            int first = -1;
            int last = -1;
            // Start at first character and move right
            for (int i = 0; i < line.length(); i++) {
                std::string substr = "";
                int di = 0;
                // Reset contenders
                contenders.clear();
                contenders.insert(contenders.end(), &dig_keys[0], &dig_keys[N_KEYS]);
                do {
                    // Gather current substring, e.g. i=5
                    // 1 t w o  n [e] 5  (di=0)
                    // 1 t w o [n  e] 5  (di=-1)
                    char new_char = line[i + di];
                    substr = new_char + substr;
                    // Check easy case first, we have a match in
                    // our map to our current substr, we can stop
                    if (DIGS.find(substr) != DIGS.end()) {
                        int val = DIGS.at(substr);
                        if (first == -1) {
                            first = val;
                        }
                        last = val;
                        // Because our unique list of words doesn't
                        // have cases where matches are contained
                        // within one another we are done when the first
                        // match is found; this wouldn't be the case
                        // if we were dealing with 'six' vs 'twentysix'
                        continue;
                    }
                    // Iterate over remaining contenders that we know won't
                    // match, eliminate any non-contenders and continue
                    // if any survive
                    std::vector<std::string> newContenders;
                    while (contenders.size()) {
                        std::string dig = contenders.back();
                        contenders.pop_back();
                        int indx = dig.size() - 1 + di;
                        // Eliminate if the latest char doesn't match
                        // the next char in this contender or if the
                        // contender has fewer chars then our substr
                        if ((indx < 0) || (dig[indx] != new_char)) {
                            continue;
                        }
                        newContenders.push_back(dig);
                    }
                    contenders = newContenders;
                    // Walk backwards to begining of line
                    di--;
                } while ((i + di >= 0) && contenders.size());
            }
            sum += 10*first + last;
            printf("\n%s -> %d%d, new sum = %d", line.c_str(), first, last, sum);
        }
        file.close();
    }
    return 0;
}