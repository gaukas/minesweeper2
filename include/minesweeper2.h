#pragma once

// constants for MineField::_status
const int STATUS_NEW = 0;
const int STATUS_REVEALED = 1;
const int STATUS_FLAGGED = 2;
const int STATUS_QUESTION = 3;

// constants for MineField::act()
const int ACTION_CLEAR = 0;
const int ACTION_REVEAL = 1;
const int ACTION_FLAG = 2;
const int ACTION_QUESTION = 3;

class MineField {
    public:
        /**
         * @brief Default constructor for MineField. 
         * 
         * @param size The size of the minefield, as both width and height.
         * @param mines The number of mines in the minefield.
         */
        MineField(int size, int mines);

        /**
         * @brief Reset the minefield to its initial state.
         */
        void reset();

        /**
         * @brief User takes an action on the minefield.   
         * 
         * @param x The x-coordinate of the action.
         * @param y The y-coordinate of the action.
         * @param action The action to take.
         * 
         * @return the time mine has been moved if the user wins, -1 if user is not yet winning.
         */
        int act(int x, int y, int action);

        /**
         * @brief Print the minefield to the console.
         */
        void print_field();

    private:
        int _size;
        int _mines;
        int** _field;
        int** _status;
        int _mine_move_count;

        /**
         * @brief Recursively reveal the cell: If a field is not adjacent to a mine, reveal all adjacent fields as well.
         * 
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         */
        void _reveal(int x, int y);

        /**
         * @brief Reveal all cells in the minefield.
         */        
        void _reveal_all();

        /**
         * @brief Refresh the minefield to reflect the current state.
         */
        void _refresh();
};
