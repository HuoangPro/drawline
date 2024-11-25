#include "RobotDraw.h"

int main() {
    Grid grid;
    Robot robot(grid);

    std::ifstream inputFile("commands.txt");
    if (!inputFile) {
        std::cerr << "Error: Could not open file.\n";
        return 1;
    }

    std::string line;
    try {
        while (std::getline(inputFile, line)) {
            if (line.empty()) continue;
            auto command = CommandParser::parseCommand(line);
            command->execute(robot, grid);
        }
        grid.print();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
