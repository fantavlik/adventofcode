#include <iostream>
#include <fstream>

// Conversion Range (inclusive): [v1_start, v2_start], v2_offset
typedef std::array<long, 3> conv_range_t;
typedef std::vector<conv_range_t> rangemap_t;

// Simple range (inclusive): [start, end]
typedef std::array<long, 2> range_t;
typedef std::vector<range_t> ranges_t;

const range_t NULL_R = {0, 0};

struct range_result {
    bool in_range;
    size_t range_index;
    range_result():
        in_range(false),
        range_index(ULONG_MAX){}
};

// Sort ranges by start value
bool sort_ranges(conv_range_t r1, conv_range_t r2) {
    return r1[0] < r2[0];
}

// Convert input ranges to output offset ranges based on the input map
ranges_t convert_ranges(ranges_t in_ranges, rangemap_t map) {
    ranges_t out_ranges;
    for (size_t ir = 0; ir < in_ranges.size(); ir++) {
        range_t in_range = in_ranges[ir];
        long val = in_range[0]; // start with beg of range
        long end = in_range[1]; // end of range
        for (size_t mr = 0; mr < map.size(); mr++) {
            conv_range_t map_range = map[mr];
            // If input val < range start then we're done traversing
            if (val < map_range[0]) {
                // We're in a gap between ranges
                if (end < map_range[0]) {
                    // If in_range fits before next map_range then offset and output
                    range_t out = {val, end}; // no offset since we're not in a range
                    out_ranges.push_back(out);
                    val = LONG_MAX;
                    break;
                } else {
                    // Otherwise, split/convert/output and continue with updated val
                    range_t out = {val, map_range[0] - 1}; // no offset
                    out_ranges.push_back(out);
                    val = map_range[0];
                    in_range = {val, end};
                }
            }
            // If val > range end then continue
            if (val > map_range[1]) {
                continue;
            }
            // We're in a map range
            long offset = map_range[2];
            if (end <= map_range[1]) {
                // If in_range fits within this map_range then offset and output
                range_t out = {val + offset, end + offset};
                out_ranges.push_back(out); // no offset since we're not in a range
                val = LONG_MAX;
                break;
            } else {
                // Otherwise, split/offset/output and continue with updated val
                range_t out = {val + offset, map_range[1] + offset};
                out_ranges.push_back(out);
                val = map_range[1] + 1;
                in_range = {val, end};
            }
        }
        if (val != LONG_MAX) {
            range_t out = {val, end}; // no offset since we're not in a range
            out_ranges.push_back(out);
        }
    }
    return out_ranges;
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
            curr_map.push_back(conv_range_t{nums[1], nums[1] + nums[2] - 1, nums[0] - nums[1]});
        }
        // finish last map - sort by v1 start ranges
        sort(curr_map.begin(), curr_map.end(), sort_ranges);
        maps.push_back(curr_map);
        // Traverse all seeds:
        for (size_t s = 0; s < seeds.size(); s += 2) {
            long init_seed = seeds[s];
            ranges_t ranges, next_ranges;
            // Ranges will propagate to new ranges with every map converted
            ranges.push_back({seeds[s], seeds[s] + seeds[s+1] - 1}); // push initial seed range
            // for each input `range`:
            // 1) start with min value
            // 2 a) find a) first map range OR
            // 2 b) first pocket of no ranges
            // 3) calculate how much of `range` is covered in this section
            // 4 a) if all covered: simply offset the range by the correct amount and push
            // 4 b) if partially covered: create new subrange, offset and push and create remaining range
            //      and continue traversal in-place until remaining range is consumed
            for (size_t m = 0; m < maps.size(); m++) {
                next_ranges = convert_ranges(ranges, maps[m]);
                ranges = next_ranges;
            }
            // Last set of ranges are locations
            for (size_t v = 0; v < ranges.size(); v++) {
                if (ranges[v][0] < min_loc) { // only check min value of range
                    min_loc = ranges[v][0];
                }
            }
        }
        std::cout << "\nMin location: " << min_loc;
        file.close();
    }
    return 0;
}
