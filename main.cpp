#include "map.hpp"
#include <iostream>
#include <string>

using namespace sjtu;

int main() {
    // Read operations from stdin
    std::string cmd;
    map<int, int> m;

    while (std::cin >> cmd) {
        if (cmd == "insert") {
            int k, v;
            std::cin >> k >> v;
            m.insert(pair<const int, int>(k, v));
        } else if (cmd == "erase") {
            int k;
            std::cin >> k;
            auto it = m.find(k);
            if (it != m.end()) {
                m.erase(it);
            }
        } else if (cmd == "find") {
            int k;
            std::cin >> k;
            auto it = m.find(k);
            if (it != m.end()) {
                std::cout << it->second << std::endl;
            } else {
                std::cout << "not found" << std::endl;
            }
        } else if (cmd == "size") {
            std::cout << m.size() << std::endl;
        } else if (cmd == "empty") {
            std::cout << (m.empty() ? "true" : "false") << std::endl;
        } else if (cmd == "clear") {
            m.clear();
        } else if (cmd == "at") {
            int k;
            std::cin >> k;
            try {
                std::cout << m.at(k) << std::endl;
            } catch (const std::exception &e) {
                std::cout << "error" << std::endl;
            }
        } else if (cmd == "bracket") {
            int k;
            std::cin >> k;
            std::cout << m[k] << std::endl;
        } else if (cmd == "iterate") {
            for (auto it = m.begin(); it != m.end(); ++it) {
                std::cout << it->first << " " << it->second << std::endl;
            }
        }
    }

    return 0;
}
