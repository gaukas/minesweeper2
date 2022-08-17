#include "minesweeper2.h"
#include <iostream>
#include <random>
#include <ctime>

MineField::MineField(int size, int mines) {
    // validate input
    if (size <= 0 || size > 10) {
        size = 10;
    }
    this->_size = size;

    if (mines <= 0 || mines >= size * size) {
        mines = 20;
    }
    this->_mines = mines;

    this->_field = NULL;
    this->_status = NULL;

    this->reset();
}

void MineField::reset() {
    // reset rand seed to time now
    srand( (unsigned int)time(NULL));

    // free the memory for the 2D array for field and revealed
    if (this->_field != NULL) {
        for (int i = 0; i < this->_size; i++) {
            delete[] this->_field[i];
        }
        delete[] this->_field;
        this->_field = nullptr;
    }
    if (this->_status != NULL) {
        for (int i = 0; i < this->_size; i++) {
            delete[] this->_status[i];
        }
        delete[] this->_status;
        this->_status = nullptr;
    }    

    // initialize the 2D array for field and status
    this->_field = new int*[this->_size];
    for (int i = 0; i < this->_size; i++) {
        this->_field[i] = new int[_size];
    }
    for (int i = 0; i < _size; i++) {
        for (int j = 0; j < _size; j++) {
            this->_field[i][j] = 0;
        }
    }

    this->_status = new int*[this->_size];
    for (int i = 0; i < this->_size; i++) {
        this->_status[i] = new int[_size];
    }
    for (int i = 0; i < _size; i++) {
        for (int j = 0; j < _size; j++) {
            this->_status[i][j] = STATUS_NEW;
        }
    }

    // randomly place mines
    for (int i = 0; i < this->_mines; i++) {
        int x = rand() % this->_size;
        int y = rand() % this->_size;
        if (this->_field[x][y] == 0) {
            this->_field[x][y] = -1;
        } else {
            i--;
        }
    }

    this->_refresh(); // must refresh after reset
}

// The only way user interact with the minefield is by calling act().
// User wins when all mines are not revealed and all non-mine fields are revealed. Function returns true. 
// User CANNOT really lose. Function returns false when user is not yet winning.
bool MineField::act(int x, int y, int action) {
    // validate input
    if (x < 0 || x >= this->_size || y < 0 || y >= this->_size) {
        return false; // invalid coordinate, user won't win yet
    }

    if (action < ACTION_CLEAR || action > ACTION_QUESTION) {
        return false; // invalid action, user won't win yet
    }
    
    // when revealing a mine, try to move the mine to an unrevealed cell
    if (action == ACTION_REVEAL) {
        if (this->_field[x][y] == -1) {
            // scan the field to find all unrevealed cell that is not a mine
            // and randomly move the mine to one of them
            std::vector<std::pair<int, int>> unrevealed;
            for (int i = 0; i < this->_size; i++) {
                for (int j = 0; j < this->_size; j++) {
                    // not revealed and not a mine
                    if (this->_status[i][j] != STATUS_REVEALED && this->_field[i][j] != -1) {
                        unrevealed.push_back(std::make_pair(i, j));
                    }
                }
            }

            if (unrevealed.size() > 0) {
                // randomly select one of the unrevealed cell
                int index = rand() % unrevealed.size();
                int new_x = unrevealed[index].first;
                int new_y = unrevealed[index].second;
                this->_field[new_x][new_y] = -1;
                this->_field[x][y] = 0;
                this->_refresh(); // must refresh after move
            } else {
                // all unrevealed cells are mines, user wins. 
                // this is also the only way user can win.
                this->_reveal_all(); // reveal all mines
                return true;
            }
        }
        this->_reveal(x, y);
    } else if (action == ACTION_CLEAR) {
        if (this->_status[x][y] == STATUS_REVEALED) {
            return false; // can't clear a revealed field, user won't win yet
        } else {
            this->_status[x][y] = STATUS_NEW;
        }
    } else if (action == ACTION_FLAG) {
        if (this->_status[x][y] == STATUS_REVEALED) {
            return false; // can't question a revealed field, user won't win yet
        } else {
            this->_status[x][y] = STATUS_FLAGGED;
        }
    } else if (action == ACTION_QUESTION) {
        if (this->_status[x][y] == STATUS_REVEALED) {
            return false; // can't question a revealed field, user won't win yet
        } else {
            this->_status[x][y] = STATUS_QUESTION;
        }
    }

    return false; // user won't win yet
}


void MineField::print_field() {
    /********************
    + A B C D E F G H I J +
    0                     0
    1                     1
    2                     2
    3                     3
    4                     4
    5                     5
    6                     6
    7                     7
    8                     8  
    9                     9
    + A B C D E F G H I J +
    **********************/

    // First line is the column numbers A-J
    std::cout << "+ ";
    for (int i = 0; i < this->_size; i++) {
        std::cout << (char)(i + 65) << " ";
    }
    std::cout << "+" << std::endl;

    // print the field
    for (int i = 0; i < this->_size; i++) {
        std::cout << i << " ";
        for (int j = 0; j < this->_size; j++) {
            if (this->_status[i][j] == STATUS_REVEALED) {
                if (this->_field[i][j] == -1) {
                    std::cout << "X ";
                } else {
                    std::cout << this->_field[i][j] << " ";
                }
            } else if (this->_status[i][j] == STATUS_NEW) {
                std::cout << "  ";
            } else if (this->_status[i][j] == STATUS_FLAGGED) {
                std::cout << "F ";
            } else if (this->_status[i][j] == STATUS_QUESTION) {
                std::cout << "? ";
            } else {
                std::cout << "E "; // error, map corrupted
            }
        }
        std::cout << i << std::endl;
    }


    // Last line is the column numbers A-J
    std::cout << "+ ";
    for (int i = 0; i < this->_size; i++) {
        std::cout << (char)(i + 65) << " ";
    }
    std::cout << "+" << std::endl;
}

// Recursively reveal the field around the cell (x, y) 
// until (x, y) is not adjacent to a mine.
void MineField::_reveal(int x, int y) {
    if (this->_status[x][y] == STATUS_REVEALED) {
        return;
    }
    this->_status[x][y] = STATUS_REVEALED;
    if (this->_field[x][y] == 0) {
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (i >= 0 && i < this->_size && j >= 0 && j < this->_size) {
                    this->_reveal(i, j);
                }
            }
        }
    }
}

void MineField::_reveal_all() {
    for (int i = 0; i < this->_size; i++) {
        for (int j = 0; j < this->_size; j++) {
            if (this->_field[i][j] == -1) {
                this->_status[i][j] = STATUS_REVEALED;
            }
        }
    }
}

void MineField::_refresh() {
    // count the number of mines around each cell, write to field if not on a mine
    for (int i = 0; i < this->_size; i++) {
        for (int j = 0; j < this->_size; j++) {
            if (this->_field[i][j] != -1) { // not a mine cell: re-count the number of mines around it
                int count = 0;
                for (int x = i - 1; x <= i + 1; x++) {
                    for (int y = j - 1; y <= j + 1; y++) {
                        if (x >= 0 && x < this->_size && y >= 0 && y < this->_size) {
                            if (this->_field[x][y] == -1) {
                                count++;
                            }
                        }
                    }
                }
                this->_field[i][j] = count;
            }
        }
    }
}
