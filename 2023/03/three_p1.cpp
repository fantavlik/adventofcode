#include <iostream>
#include <fstream>

bool is_special(char c) {
    return (c != '.' && !isdigit(c));
}

bool special_adj(size_t c, std::string prev_line, std::string next_line) {
    bool special = false;
    if (c < prev_line.size()) {
        special = is_special(prev_line[c]);
    }
    if (!special && c < next_line.size()) {
        return is_special(next_line[c]);
    }
    return special;
}

void finish(bool& keep, std::string& num, std::vector<uint>& all_nums) {
    if (keep && num.size()) {
        std::cout << num << " ";
        all_nums.push_back(atoi(num.c_str()));
    }
    num = "";
    keep = false;
}

int main()
{
    std::ifstream file("input.txt");
    u_long sum = 0;
    if (file.is_open()) {
        std::string line;
        std::string prev_line;
        std::string next_line;
        // This can be done in place - read ahead one line to compare
        // prev and next lines to current (if applicable)
        std::vector<uint> all_nums = {};
        std::getline(file, line);
        size_t lineno = 0;
        while (line.size()) {
            std::cout << "\nLine " << ++lineno << " numbers found: ";
            if (file.eof()) {
                next_line = "";
            } else {
                std::getline(file, next_line);
            }
            // new row, clear progress
            bool keep = false;
            std::string num = "";
            for (size_t c = 0; c < line.size(); c++) {
                if (line[c] == '.') {
                    if (!keep && num.size()) {
                        // Before finishing, check diagonal for previous char if num
                        keep = special_adj(c, prev_line, next_line);
                    }
                    finish(keep, num, all_nums);
                    continue;
                }
                if (isdigit(line[c])) {
                    if (!keep) {
                        // Check upper/lower neighbors for special char
                        keep = special_adj(c, prev_line, next_line);
                        // For first dig, check left
                        if (!keep && !num.size()) {
                            keep = special_adj(c - 1, prev_line, next_line);
                        }
                    }
                    num += line[c];
                    continue;
                }
                // Special chars
                keep = true;
                finish(keep, num, all_nums);
                keep = true; // for next number (if any)
            }
            finish(keep, num, all_nums);
            prev_line = line;
            line = next_line;
        }
        for (size_t n = 0; n < all_nums.size(); n++) {
            sum += all_nums[n];
        }
        std::cout << "\n\nSum of all: " << sum;
        file.close();
    }

    return 0;
}
