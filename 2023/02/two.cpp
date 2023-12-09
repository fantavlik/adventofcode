#include <iostream>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>

// Regex for matching example line of input:
// Game 4: 14 blue, 8 red, 10 green; 7 green, 9 blue, 4 red; 4 green, 5 red"
std::regex game_rex ("^(Game ([0-9]+): ).*");
std::regex set_rex ("(?:[0-9]+ (?:blue|green|red)(?:, )?)+(?:; )?");
const uint MAX_BLUE  = 14;
const uint MAX_GREEN = 13;
const uint MAX_RED   = 12;
const std::string BLUE = "blue";
const std::string GREEN = "green";
const std::string RED = "red";
const std::array<std::string, 3> COLORS = {BLUE, GREEN, RED};

bool clean_set(std::string& set) {
    std::size_t sz = set.size();
    if (sz < 3) {
        return false;
    }
    if (set[sz-2] == ';') { // remove trailing "; "
        set = set.substr(0, sz - 2);
    }
    return true;
}

bool get_result(std::string& set, std::array<int, 3>& result) {
    result = {0, 0, 0};
    size_t pos = 0;
    const size_t sz = set.size();
    do {
        // pos is here: [<-# color]
        // Look for [# color] pair
        std::size_t space_pos = set.find_first_of(' ', pos);
        if (space_pos == std::string::npos) {
            return false;
        }
        int num = std::atoi(set.substr(pos, space_pos).c_str());
        pos = space_pos + 1;
        // pos is here: [# <-color]
        bool found = false;
        for (size_t c = 0; c < COLORS.size(); c++) {
            // optimization trick since b/g/r start with different chars
            // -> just check the first char
            if (pos >= sz) {
                return false; // no color for this pair, malformed
            }
            if (set[pos] == COLORS[c][0]) {
                result[c] = num;
                pos = pos + 2 + COLORS[c].size();
                // pos is here: [# color], <-[# color]
                found = true;
                break;
            }
        }
        if (!found) {
            return false; // no color for this pair, malformed
        }
    } while (pos < sz);
    return true;
}

int main()
{
    std::ifstream file("input.txt");
    uint sum_valid_games = 0;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.size() < 7) { // need at least "Game #:"
                continue;
            }
            std::smatch matches;
            // Get the game number
            std::regex_match(line, matches, game_rex);
            if (matches.size() != 3) {
                continue;
            }
            int game = std::atoi(matches.str(2).c_str());
            
            // Advance
            std::string game_str = matches.str(1);
            if (game_str.size() == line.size()) {
                continue;
            }
            line = line.substr(game_str.size(), line.size() - 1);
            // Process each set
            // [# color], [# color],.. ;
            bool results_valid = true;
            std::vector<std::array<int, 3>> results; // [[#blue, #green, #red], ...]
            while (results_valid && regex_search(line, matches, set_rex)) {
                if (matches.size() != 1) {
                    results_valid = false;
                    break;
                }
                std::string set_str = matches.str(0);
                if (!clean_set(set_str)) {
                    results_valid = false;
                    break;
                }
                // Break set into results
                std::array<int, 3> result;
                if (!get_result(set_str, result)) {
                    results_valid = false;
                    break;
                }
                results_valid = (
                    (result[0] <= MAX_BLUE) &&
                    (result[1] <= MAX_GREEN) &&
                    (result[2] <= MAX_RED)
                );
                line = matches.suffix().str(); // move to next set
            }
            if (results_valid) {
                sum_valid_games += game;
            }
        }
        std::cout << "Valid game total: " << sum_valid_games;
        file.close();
    }
    return 0;
}
