The goal
Implement the Conway’s Game Of Life algorithm in C++. The Game Of Life is a cellular automaton algorithm. It runs on a universe, which is two-dimensional a grid of cells. Each cell can be in two states: alive or dead. There is an initial state for the universe, which defines alive cells and their positions in the universe. The automaton progresses one step at a time by applying a set of rules to each cell in the universe. There rules are the following: 
Any live cell with two or three live neighbors survives;
Any dead cell with exactly three live neighbors becomes a live cell;
All other live cells die in the next generation. Similarly, all other dead cells stay dead.

Life game software has been tested with https://app.coderpad.io/sandbox?view=candidate&switcher=true

To run different init, enum class SEED is used for it.
    enum class SEED
    {
        RANDOM,               /* tested okay for now */
        FIVE_POINT,           /* tested okay for now */
        TEN_CELL_ROW,         /* tested okay for now */
        LOAD_INIT_MATRIX_FROM_FILE, /* not tested */
        LOAD_FROM_MATRIX,     /* needed to do code refactoring and debugging */
    };

    // to create a game instance with CUniverse
    CUniverse u(50, 20);
    
    // test okay for random init
    u.Run(CUniverse::SEED::RANDOM, 100, 100ms);
    
    // test okay for TEN_CELL_ROW
    u.Run(CUniverse::SEED::TEN_CELL_ROW, 100, 100ms);
    // test okay for five valid points
    u.Run(CUniverse::SEED::FIVE_POINT, 100, 100ms);

    Note The below method not tested
    // TODO:: find a valid matrix for init of gosper_glider_gun
    u.Run(CUniverse::SEED::LOAD_FROM_MATRIX, 100, 100ms);
    // TODO:: find a valid gosper_glider_gun.bin file
    u.Run(CUniverse::SEED::LOAD_INIT_MATRIX_FROM_FILE, 100, 100ms);
