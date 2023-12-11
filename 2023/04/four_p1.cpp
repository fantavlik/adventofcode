#include <iostream>
#include <fstream>
#include <map>
#include <regex>
#include <set>

typedef std::map<uint, uint> dig2count_t;

// Regex for matching example line of input:
// Card  [XX]: [...] | [...]
std::regex card_rex ("^Card\\s+([0-9]+):\\s+(.*) \\|\\s+(.*)$");

dig2count_t split(std::string results) {
    size_t pos = 0;
    dig2count_t nums = {};
    while (pos < results.size()) {
        while ((pos < results.size()) && (results[pos] == ' ')) {
            pos++;
        }
        size_t newpos = results.find_first_of(' ', pos);
        if (newpos == std::string::npos) {
            newpos = results.size();
        }
        uint dig = atoi(results.substr(pos, newpos).c_str());
        // std::cout << "\n[" << dig << "]:";
        if (nums.find(dig) == nums.end()) {
            nums[dig] = 1;
        } else {
            nums[dig] = nums[dig] + 1;
        }
        pos = newpos + 1;
    }
    return nums;
}

int main()
{
    std::ifstream file("input.txt");
    if (file.is_open()) {
        std::string line;
        u_long score = 0;
        while (std::getline(file, line)) {
            std::smatch matches;
            std::regex_match(line, matches, card_rex);
            if (matches.size() != 4) {
                continue;
            }
            int cardno = std::atoi(matches.str(1).c_str());
            // std::cout << "\nwinning " << matches.str(2) << "\n";
            dig2count_t win_nums = split(matches.str(2));
            // for (dig2count_t::iterator it=win_nums.begin(); it != win_nums.end(); it++) {
            //     std::cout << it->first << " ";
            // }
            // std::cout << "\nours " << matches.str(3) << "\n";
            dig2count_t our_nums = split(matches.str(3));
            // for (dig2count_t::iterator it=our_nums.begin(); it != our_nums.end(); it++) {
            //     std::cout << it->first << " ";
            // }
            uint n_winners = 0;
            std::cout << "\nCard " << cardno << " winners: ";
            for (dig2count_t::iterator it=our_nums.begin(); it != our_nums.end(); it++) {
                if (win_nums.find(it->first) != win_nums.end()) {
                    std::cout << it->first << " ";
                    if (it->second > 1) {
                        std::cout << " (x" << it->second << ") ";
                    }
                    n_winners += it->second;
                }
            }
            if (!n_winners) {
                continue;
            }
            double card_score = pow(2, n_winners - 1);
            std::cout << "Score = " << card_score << "";
            score += card_score;
            std::cout << "\nNew Total = " << score << "";
        }
        std::cout << "Total score: " << score;
        file.close();
    }
    return 0;
}
