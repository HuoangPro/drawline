#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <memory>
#include <stdexcept>
#include <fstream>

class Grid {
private:
    int dimension;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> lines;
public:
    Grid() : dimension(0) {}

    void setDimension(int n) {
        if (n <= 0) {
            throw std::invalid_argument("Grid size must be greater than 0");
        }
        dimension = n;
        grid.assign(n, std::vector<int>(n, 0));
    }

    void drawLine(int x1, int y1, int x2, int y2) {
        if (dimension == 0) {
            throw std::runtime_error("Grid dimension is not set");
        }
        lines.push_back(std::vector<int>{x1, y1, x2, y2});
        auto drawPoint = [&](int x, int y) {
            if (x >= 0 && x < dimension && y >= 0 && y < dimension) {
                grid[x][y] = 1;
            } else {
                throw std::runtime_error("Position is out of range");
            }
        };

        int dx = x2 - x1;
        int dy = y2 - y1;
        
        int t = std::max(abs(dx), abs(dy)) * 2;
        
        for(int i = 0; i <= t; i++) {
            int x = x1 + 0.5 + 1.0 * dx * i / t;
            int y = y1 + 0.5 + 1.0 * dy * i / t;
            drawPoint(x, y);
        }

    }

    void print() const {
        if (dimension == 0) {
            std::cout << "Grid is not initialized.\n";
            return;
        }
        for (const auto &row : grid) {
            for (bool cell : row) {
                std::cout << (cell ? '+' : ' ');
            }
            std::cout << '\n';
        }
    }

    void saveBitmap(std::string filename) {
        filename += ".bmp";
        int height = grid.size();
        int width = grid[0].size();

        // BMP Header setup
        int paddedWidth = (width * 3 + 3) & (~3); // Each row padded to be a multiple of 4 bytes
        int fileSize = 54 + paddedWidth * height; // Header (54 bytes) + pixel data

        // BMP headers
        uint8_t bmpFileHeader[14] = {
            'B', 'M',                       // Signature
            fileSize, fileSize >> 8, fileSize >> 16, fileSize >> 24, // File size
            0, 0,                           // Reserved
            0, 0,                           // Reserved
            54, 0, 0, 0                     // Pixel data offset
        };

        uint8_t bmpInfoHeader[40] = {
            40, 0, 0, 0,                    // Header size
            width, width >> 8, width >> 16, width >> 24, // Width
            height, height >> 8, height >> 16, height >> 24, // Height
            1, 0,                           // Planes
            24, 0,                          // Bits per pixel
            0, 0, 0, 0,                     // Compression (0 = none)
            0, 0, 0, 0,                     // Image size (can be 0 for uncompressed)
            0, 0, 0, 0,                     // X pixels per meter
            0, 0, 0, 0,                     // Y pixels per meter
            0, 0, 0, 0,                     // Colors in color table
            0, 0, 0, 0                      // Important color count
        };

        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }

        // Write headers
        file.write(reinterpret_cast<char*>(bmpFileHeader), sizeof(bmpFileHeader));
        file.write(reinterpret_cast<char*>(bmpInfoHeader), sizeof(bmpInfoHeader));

        // Write pixel data
        for (int y = height - 1; y >= 0; --y) { // BMP stores rows bottom to top
            for (int x = 0; x < width; ++x) {
                uint8_t grayscale = static_cast<uint8_t>(grid[y][x] * 255); // Scale [0,1] to [0,255]
                file.put(grayscale); // Blue
                file.put(grayscale); // Green
                file.put(grayscale); // Red
            }
            // Add padding bytes if necessary
            for (int p = 0; p < paddedWidth - width * 3; ++p) {
                file.put(0);
            }
        }

        file.close();
        std::cout << "Bitmap saved to " << filename << std::endl;
    }

    int getDimension() const {
        return dimension;
    }

    const std::vector<std::vector<int>>& getData() const {
        return grid;
    }

    const std::vector<std::vector<int>>& getLines() const {
        return lines;
    }

};

class Robot {
private:
    Grid& grid;
    int x, y;

public:
    Robot(Grid& g) : grid(g), x(0), y(0) {}

    void moveTo(int newX, int newY) {
        if (newX >= 0 && newX < grid.getDimension() && newY >= 0 && newY < grid.getDimension()) {
            x = newX;
            y = newY;
        } else {
            throw std::runtime_error("Position is out of range");
        }

    }

    void lineTo(int newX, int newY) {
        if (newX >= 0 && newX < grid.getDimension() && newY >= 0 && newY < grid.getDimension()) {
            grid.drawLine(x, y, newX, newY);
            moveTo(newX, newY);
        } else {
            throw std::runtime_error("Position is out of range");
        }
    }
};

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Robot& robot, Grid& grid) = 0;
};

class DimensionCommand : public Command {
private:
    int n;

public:
    explicit DimensionCommand(int dimension) : n(dimension) {}
    void execute(Robot&, Grid& grid) override {
        grid.setDimension(n);
    }
};

class MoveToCommand : public Command {
private:
    int x, y;

public:
    MoveToCommand(int x, int y) : x(x), y(y) {}
    void execute(Robot& robot, Grid&) override {
        robot.moveTo(x, y);
    }
};

class LineToCommand : public Command {
private:
    int x, y;

public:
    LineToCommand(int x, int y) : x(x), y(y) {}
    void execute(Robot& robot, Grid&) override {
        robot.lineTo(x, y);
    }
};

class SaveBmpCommand : public Command {
private:
    std::string filename;

public:
    SaveBmpCommand(std::string name) : filename(name) {} 
    void execute(Robot&, Grid& grid) override {
        grid.saveBitmap(filename);
    }
};

class CommandParser {
public:
    static std::unique_ptr<Command> parseCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "DIMENSION") {
            int n;
            if (iss >> n) {
                return std::make_unique<DimensionCommand>(n);
            }
        } else if (cmd == "MOVE_TO") {
            int x, y;
            if (iss >> x >> y) {
                return std::make_unique<MoveToCommand>(x, y);
            }
        } else if (cmd == "LINE_TO") {
            int x, y;
            if (iss >> x >> y) {
                return std::make_unique<LineToCommand>(x, y);
            }
        } else if (cmd == "SAVE_TO") {
            std::string filename;
            if(iss >> filename) {
                return std::make_unique<SaveBmpCommand>(filename);
            }
        }
        throw std::invalid_argument("Invalid command: " + line);
    }
};
