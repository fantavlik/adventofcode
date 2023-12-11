#include <iostream>
#include <fstream>
#include <map>

typedef std::map<std::string, std::vector<uint>> mapnums_t;

struct gear_nums {
    mapnums_t nums;
    static std::string key(size_t gear_row, size_t gear_col) {
        return std::to_string(gear_row) + "," + std::to_string(gear_col);
    }
    bool has_key(std::string key) {
        return nums.find(key) != nums.end();
    }
    void add_num(std::string key, uint num) {
        if (!has_key(key)) {
            nums[key] = {num};
        } else {
            nums[key].push_back(num);
        }
    }
};

// Start at line[c] and if that's a digit, continue gathering adjacent
// characters to form a multidigit number. Add it to gear_nums if found.
void get_num(size_t c, std::string line, gear_nums& gears, std::string gear_key) {
    if (c >= line.size() || !isdigit(line[c])) {
        return;
    }
    std::string num (1, line[c]);
    size_t pos = c;
    // Move left
    while ((pos > 0) && isdigit(line[pos - 1])) {
        num = line[pos - 1] + num;
        pos--;
    }
    pos = c;
    // Move right
    while ((++pos < line.size()) && isdigit(line[pos])) {
        num = num + line[pos];
    }
    gears.add_num(gear_key, atoi(num.c_str()));
}

// Gather any adjacent numbers to the gear at pos r,c and add them to gear_nums
void gather_nums(size_t r, size_t c, std::string prev_line, std::string line, std::string next_line, gear_nums& gears) {
    // For prev and next lines, maximum of two nums can be gathered
    // this is only possible if the char immediately above/below is not
    // a digit. Example:
    // 111.222
    // 333*444
    // 555.666
    std::string key = gears.key(r, c);
    if (prev_line.size()) {
        if (!isdigit(prev_line[c])) {
            get_num(c - 1, prev_line, gears, key);
            get_num(c + 1, prev_line, gears, key);
        } else { // only one number above is possible
            get_num(c, prev_line, gears, key);
        }
    }
    get_num(c - 1, line, gears, key);
    get_num(c + 1, line, gears, key);
    if (next_line.size()) {
        if (!isdigit(next_line[c])) {
            get_num(c - 1, next_line, gears, key);
            get_num(c + 1, next_line, gears, key);
        } else { // only one number below is possible
            get_num(c, next_line, gears, key);
        }
    }
    if (gears.has_key(key)) {
        std::cout << "g[" << key << "]: ";
        for (size_t n = 0; n < gears.nums[key].size(); n++) {
            std::cout << gears.nums[key][n] << " ";
        }
    }
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
        gear_nums gears;
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
            std::string num = "";
            for (size_t c = 0; c < line.size(); c++) {
                if (line[c] == '*') {
                    gather_nums(lineno - 1, c, prev_line, line, next_line, gears);
                }
            }
            prev_line = line;
            line = next_line;
        }
        for (mapnums_t::iterator it=gears.nums.begin(); it!=gears.nums.end(); it++) {
            if (it->second.size() == 2) {
                // For gears with exactly two numbers, get gear ratio and add to total
                sum += it->second[0] * it->second[1];
            }
        }
        std::cout << "\n\nSum of all: " << sum;
        file.close();
    }

    return 0;
}
