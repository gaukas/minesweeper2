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
         * 
         * @param safe_x A x-coordinate to guarantee the first revealed cell is not a mine.
         * @param safe_y A y-coordinate to guarantee the first revealed cell is not a mine.
         */
        void reset(int safe_x, int safe_y);

        /**
         * @brief User takes an action on the minefield.   
         * 
         * @param x The x-coordinate of the action.
         * @param y The y-coordinate of the action.
         * 
         * @return the time mine has been moved if the user wins, -1 if user is not yet winning.
         */
        int flag(int x, int y);
        int reveal(int x, int y);

        /**
         * @brief report the minefield as a 2D array of characters.
         * @param size The size of the minefield, as both width and height. If not matching the size of the minefield, the function will return nullptr.
         * 
         * @return a 2D array of characters.
         */
        char** field_report(int size);

    private:
        int _size; // not to be modified
        int _mines; // not to be modified
        int _unrevealed;

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
         * @brief Reveal all mines in the minefield. Unless the mine is flagged.
         */        
        void _reveal_all_mines_unless_flagged();

        /**
         * @brief Recount the mine near each cell on field to reflect the current state.
         */
        void _recount();
};
