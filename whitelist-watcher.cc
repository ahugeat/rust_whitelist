#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <thread>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

bool isWhitelisted(const std::vector<std::string> &whitelist, const std::string &playerName) {
    return std::find(whitelist.begin(), whitelist.end(), playerName) != whitelist.end();
}

void loadPlayersWhitelisted(const std::string &filename, std::vector<std::string> &whitelist) {
    std::ifstream file(filename);

    if (file.fail()) {
        return;
    }

    while (!file.eof()) {
        std::string playerName;
        file >> playerName;
        whitelist.push_back(playerName);
    }
}

std::string getPlayerName(const std::string &line) {
    // Check if a new player connect
    std::regex playerNameRegex("([a-zA-Z0-9]+) .*joined.*"); // Set the right regex
    std::smatch playerNameMatch;

    // If it isn't a new player
    if (!std::regex_match(line, playerNameMatch, playerNameRegex)) {
        return "";
    }

    // Error in regex, we haven't to never enter here!
    if (playerNameMatch.size() != 2) {
        std::cerr << "Error during the exctraction of player name! FIX IT!" << std::endl;
        return "";
    }
    // The first sub_match is the whole string; the next
    // sub_match is the first parenthesized expression.
    std::ssub_match playerNameSubMatch = playerNameMatch[1];

    // Return the player name
    return playerNameSubMatch.str();
}

int main() {
    std::string filename = "/tmp/test.txt";

    // Load the whitelisted players
    std::vector<std::string> whitelist;
    loadPlayersWhitelisted("/tmp/whitelist.txt", whitelist);

    // Open the file log
    // TODO: test with a ifstream
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("Error open()");
        return -1;
    }

    char c;
    std::string line;

    for (;;) {
        // Read char by char
        c = '\0';
        ssize_t n = read(fd, &c, sizeof(char));
        if (n < 0) {
            perror("Error read()");
            return -1;
        }
        else if (n < (ssize_t)(sizeof(char))) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        line.push_back(c);

        // If it's a end of new line
        if (c == '\n') {
            // Remove the newline char
            line.pop_back();

            // Check if it's a new player
            std::string playerName = getPlayerName(line);
            if (playerName != "") {
                // If the player is not whitelisted
                if (!isWhitelisted(whitelist, playerName)) {
                    std::cout << "Not whitelited: " << playerName << std::endl;
                    // Kick the player
                }
            }

            line.clear();
        }
    }

    return 0;
}
