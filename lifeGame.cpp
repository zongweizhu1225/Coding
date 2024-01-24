#include <random>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>

using namespace std::chrono_literals;

class CUniverse
{
private:
    CUniverse() = delete;

public:
    enum class SEED
    {
        RANDOM,               /* tested okay for now */
        FIVE_POINT,           /* tested okay for now */
        TEN_CELL_ROW,         /* tested okay for now */
        LOAD_INIT_MATRIX_FROM_FILE, /* not tested */
        LOAD_FROM_MATRIX,     /* needed to do code refactoring and debugging */
    };

    CUniverse(size_t const width, size_t const height) : rows_(height), columns_(width), grid_(width * height), dist_(0, 4)
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        mt.seed(seq);
    }

    void Run(SEED const s, int const generations, std::chrono::microseconds const ms = std::chrono::microseconds(100))
    {
        Reset();
        initialize(s);
        display();

        int i = 0;
        do
        {
            NextGeneration();
            display();
            std::this_thread::sleep_for(ms);
        } while (i++ < generations || generations == 0);
    }

    void SetLoadedFileName(const std::string& filename) {
        LoadedFilename_ = filename;
    }

    void SetStoredFileName(const std::string& filename) {
        StoredFilename_ = filename;
    }

    // TODO:: verify it later
    void LoadFromFile() {
        std::ifstream file(LoadedFilename_, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char *>(grid_.data()), grid_.size());
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for loading." << std::endl;
        }
    }
    
    // TODO:: verify it later
    void StoreToFile()
    {
        std::ofstream file(StoredFilename_, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char *>(grid_.data()), grid_.size());
            file.close();
        }
        else {
            std::cerr << "Error: Unable to open file for storing." << std::endl;
        }
    }

    // TODO:: partial coding and verify it later
    void LoadFromMatrix(const std::vector<std::vector<unsigned char>> &matrix)
    {
        if (matrix.size() != rows_ || matrix[0].size() != columns_)
        {
            std::cerr << "Error: Matrix size does not match the CUniverse size." << std::endl;
            return;
        }

        for (size_t r = 0; r < rows_; ++r)
        {
            for (size_t c = 0; c < columns_; ++c)
            {
                cell(c, r) = matrix[r][c];
            }
        }
    }

private:

    void NextGeneration() {
        std::vector<unsigned char> newgrid(grid_);
        for (size_t r = 0; r < rows_; ++r) {
            for (size_t c = 0; c < columns_; ++c) {
                auto count = CountNeighbors(r, c);
                if (cell(c, r) == alive) {
                    newgrid[r * columns_ + c] = (count == 2 || count == 3) ? alive : dead;
                } else {
                    newgrid[r * columns_ + c] = (count == 3) ? alive : dead;
                }
            }
        }
        std::swap(grid_, newgrid);
    }

    void ResetDisplay() {
    #ifdef __APPLE__
        system("clear");
    #elif __linux__
        system("clear");
    #elif WIN32
        system("cls");
    #endif
    }

    void display() {
        ResetDisplay();

        for (size_t r = 0; r < rows_; ++r) {
            for (size_t c = 0; c < columns_; ++c) {
                std::cout << (cell(c, r) ? '*' : ' ');
            }
            std::cout << std::endl;
        }
    }

    void initialize(SEED const s) {
        if (s == SEED::TEN_CELL_ROW) {
            for (size_t c = columns_ / 2 - 5; c < columns_ / 2 + 5; c++) {
                cell(c, rows_ / 2) = alive;
            }
        } else if(s == SEED::FIVE_POINT) {
            cell(1, 2) = alive;
            cell(2, 3) = alive;
            cell(3, 3) = alive;
            cell(3, 2) = alive;
            cell(3, 1) = alive;
        } else if(s == SEED::RANDOM) {
            for (size_t r = 0; r < rows_; ++r) {
                for (size_t c = 0; c < columns_; ++c) {
                    cell(c, r) = dist_(mt) == 0 ? alive : dead;
                }
            }
        } else if(s == SEED::LOAD_INIT_MATRIX_FROM_FILE) {
            // TODO:: verify later
            LoadFromFile();
        } else if(s == SEED::LOAD_FROM_MATRIX) {
            // TODO:: use this fixed matrix for now and will be changed later.
            // TODO:: partial coding needed to be verify later and change it adapt to different matrix 
            std::vector<std::vector<unsigned char>> _GosperGliderGunMatrix = {
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,1,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
            };
            LoadFromMatrix(_GosperGliderGunMatrix);
        } else {
            std::cerr << "Error: not support mode." << std::endl;
        }
    }

    void Reset() {
        for (size_t r = 0; r < rows_; ++r) {
            for (size_t c = 0; c < columns_; ++c) {
                cell(c, r) = dead;
            }
        }
    }

    int CountNeighbors(size_t const row, size_t const col) {
      int count = 0;
      // Check eight neighbors
      count += CellState(col - 1, row - 1);
      count += CellState(col, row - 1);
      count += CellState(col + 1, row - 1);
      count += CellState(col - 1, row);
      count += CellState(col + 1, row);
      count += CellState(col - 1, row + 1);
      count += CellState(col, row + 1);
      count += CellState(col + 1, row + 1);
      return count;
    }

    unsigned char CellState(int const col, int const row) {
      // Wrap around the grid (toroidal topology)
      size_t neighborRow = (row + rows_) % rows_;
      size_t neighborCol = (col + columns_) % columns_;
      return cell(neighborCol, neighborRow);
    }

    unsigned char &cell(size_t const col, size_t const row) {
        return grid_[row * columns_ + col];
    }

private:
    size_t rows_;
    size_t columns_;
    std::vector<unsigned char> grid_;
    const unsigned char alive = 1;
    const unsigned char dead = 0;
    std::uniform_int_distribution<> dist_;
    std::mt19937 mt;
    std::string LoadedFilename_;
    std::string StoredFilename_;
};

// TODO:: use config to test different init of life game
int main()
{
    CUniverse u(50, 20);
    // TODO:: find a valid gosper_glider_gun.bin
    u.SetLoadedFileName("gosper_glider_gun.bin");
    u.SetStoredFileName("gosper_glider_gun.bin");
    // test okay for random init
    //u.Run(CUniverse::SEED::RANDOM, 100, 100ms);
    // test okay for TEN_CELL_ROW
    //u.Run(CUniverse::SEED::TEN_CELL_ROW, 100, 100ms);
    u.Run(CUniverse::SEED::FIVE_POINT, 100, 100ms);
    // TODO:: find a valid matrix for init of gosper_glider_gun
    // u.Run(CUniverse::SEED::LOAD_FROM_MATRIX, 100, 100ms);
    // TODO:: find a valid gosper_glider_gun.bin file
    //u.Run(CUniverse::SEED::LOAD_INIT_MATRIX_FROM_FILE, 100, 100ms);
    u.StoreToFile();
    return 0;
}