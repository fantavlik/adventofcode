#include <iostream>
#include <fstream>

// Range (inclusive): [v1_start, v2_start, v2_offset]
typedef std::array<long, 3> range_t;
typedef std::vector<range_t> rangemap_t;

// Sort ranges by start value
bool sort_ranges(range_t r1, range_t r2) {
    return r1[0] < r2[0];
}

std::vector<long> split_nums(std::string line) {
    size_t pos = 0;
    std::vector<long> nums;
    while (pos < line.size()) {
        while ((pos < line.size()) && !isdigit(line[pos])) {
            pos++;
        }
        size_t newpos = line.find_first_of(' ', pos);
        if (newpos == std::string::npos) {
            newpos = line.size();
        }
        uint dig = atoi(line.substr(pos, newpos).c_str());
        nums.push_back(dig);
        pos = newpos + 1;
    }
    return nums;
}

int main()
{
    std::ifstream file("input.txt");
    if (file.is_open()) {
        std::string line;
        long min_loc = LONG_MAX;
        size_t lineno = 0;
        size_t pos = 0;
        std::vector<long> seeds = {};
        std::vector<rangemap_t> maps = {};
        rangemap_t curr_map;
        while (std::getline(file, line)) {
            if (lineno++ == 0) {
                seeds = split_nums(line);
                continue;
            }
            if (!line.size()) {
                continue;
            }
            if (line.substr(line.size() - 4, line.size()) == "map:") {
                // finish prev map - sort by v1 start ranges'
                if (curr_map.size()) {
                    sort(curr_map.begin(), curr_map.end(), sort_ranges);
                    maps.push_back(curr_map);
                    curr_map = {};
                }
                continue;
            }
            // Read and convert maps of form:
            // v1 v2 range -> v1_start v1_end offset
            // and order by v1_start
            std::vector<long> nums = split_nums(line);
            if (nums.size() != 3) {
                std::cout << "\n! Unexpected size of extracted nums, line: '" << line << "' ";
                std::cout << "lineno: " << lineno << " nums.size() = " << nums.size();
                continue;
            }
            // Convert format from v2_start v1_start range -> v1_start v1_end offset
            curr_map.push_back(range_t{nums[1], nums[1] + nums[2] - 1, nums[0] - nums[1]});
        }
        // finish last map - sort by v1 start ranges
        sort(curr_map.begin(), curr_map.end(), sort_ranges);
        maps.push_back(curr_map);
        // Traverse all seeds:
        for (size_t s = 0; s < seeds.size(); s++) {
            long val = seeds[s];
            std::cout << "\n\nSeed: " << val;
            for (size_t m = 0; m < maps.size(); m++) {
                std::cout << "\nNext map...\n";
                // Get the next value if it's conained in one of our ranges
                for (size_t r = 0; r < maps[m].size(); r++) {
                    range_t range = maps[m][r];
                    // If input val < range start then we're done traversing
                    if (val < range[0]) {
                        break; // the default case is to use current val for next val
                    }
                    // If val > range end then continue
                    if (val > range[1]) {
                        continue;
                    }
                    // We're in the range, so get the next val and break
                    std::cout << "(found! in range " << range[0] << ") ";
                    val += range[2];
                    break;
                }
                std::cout << " -> " << val;
            }
            // Current val is the location, check against the current minimum
            if (val < min_loc) {
                min_loc = val;
            }
        }
        std::cout << "\nMin location: " << min_loc;
        file.close();
    }
    return 0;
}
