#include "minesweeper2.h"
#include <iostream>
#include <random>
#include <ctime>

MineField::MineField(int size, int mines) {
    // validate input
    if (size <= 0) {
        std::cout << "Size must be positive" << std::endl;
        return;
    }
    this->_size = size;

    if (mines <= 0 || mines >= size * size) {
        std::cout << "Mines must be positive and less than size * size" << std::endl;
        return;
    }
    this->_mines = mines;

    this->_field = NULL;
    this->_status = NULL;
}

void MineField::reset(int safe_x, int safe_y) {
    this->_mine_move_count = 0;
    this->_unrevealed = this->_size * this->_size - 1; // -1 for safe cell

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
    // reveal the safe cell
    this->_status[safe_x][safe_y] = STATUS_REVEALED;

    // randomly place mines
    for (int i = 0; i < this->_mines; i++) {
        int x = rand() % this->_size;
        int y = rand() % this->_size;
        if (this->_field[x][y] == 0 && (x != safe_x || y != safe_y)) {
            this->_field[x][y] = -1;
        } else {
            i--;
        }
    }

    this->_recount(); // must recount after reset
}

// revealed -> revealed
// new -> flagged
// flagged -> question
// question -> new
int MineField::flag(int x, int y) {
    // validate input
    if (x < 0 || x >= this->_size || y < 0 || y >= this->_size) {
        return -1; // invalid coordinate
    }

    if (this->_status[x][y] == STATUS_REVEALED) {
        return -1; // can't flag a revealed field
    } else if (this->_status[x][y] == STATUS_NEW) {
        this->_status[x][y] = STATUS_FLAGGED;
        return STATUS_FLAGGED;
    } else if (this->_status[x][y] == STATUS_FLAGGED) {
        this->_status[x][y] = STATUS_QUESTION;
        return STATUS_QUESTION;
    } else if (this->_status[x][y] == STATUS_QUESTION) {
        this->_status[x][y] = STATUS_NEW;
        return STATUS_NEW;
    } else {
        return -1; // invalid status
    }
}

// User wins when all mines are not revealed and all non-mine fields are revealed. Function returns true. 
// User CANNOT really lose. Function returns false when user is not yet winning.
int MineField::reveal(int x, int y) {
    // validate input
    if (x < 0 || x >= this->_size || y < 0 || y >= this->_size) {
        return -1; // invalid coordinate, user won't win yet
    }
    
    // when revealing a mine, try to move the mine to an unrevealed cell
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
            this->_recount(); // must refresh after move
            this->_mine_move_count++;
        }
    }
    this->_reveal(x, y);

    if (this->_unrevealed == this->_mines) {
        // all unrevealed cells are mines, user wins.
        this->_reveal_all_mines_unless_flagged(); // reveal all mines
        return this->_mine_move_count;
    }

    return -1; // user won't win yet
}

char** MineField::field_report(int size) {
    if (size != this->_size) {
        return NULL; // callee dosn't know the size of the minefield
    }

    char** report = new char*[size];
    for (int i = 0; i < size; i++) {
        report[i] = new char[size];
    }

    // generate the report
    for (int i = 0; i < this->_size; i++) {
        for (int j = 0; j < this->_size; j++) {
            if (this->_status[i][j] == STATUS_REVEALED) {
                if (this->_field[i][j] == -1) {
                    report[i][j] = 'X';
                } else {
                    if (this->_field[i][j] >= 0 && this->_field[i][j] < 10) {
                        report[i][j] = '0' + this->_field[i][j];
                    } else {
                        report[i][j] = 'E'; // error, map corrupted
                    }
                }
            } else if (this->_status[i][j] == STATUS_NEW) {
                report[i][j] = ' ';
            } else if (this->_status[i][j] == STATUS_FLAGGED) {
                report[i][j] = 'F';
            } else if (this->_status[i][j] == STATUS_QUESTION) {
                report[i][j] = '?';
            } else {
                report[i][j] = 'E'; // error, map corrupted
            }
        }
    }
    return report;
}

// Recursively reveal the field around the cell (x, y) 
// until (x, y) is not adjacent to a mine.
void MineField::_reveal(int x, int y) {
    if (this->_status[x][y] == STATUS_REVEALED) {
        return;
    }
    this->_status[x][y] = STATUS_REVEALED;
    this->_unrevealed--;
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

void MineField::_reveal_all_mines_unless_flagged() {
    for (int i = 0; i < this->_size; i++) {
        for (int j = 0; j < this->_size; j++) {
            if (this->_field[i][j] == -1 && this->_status[i][j] != STATUS_FLAGGED) {
                this->_status[i][j] = STATUS_REVEALED;
            }
        }
    }
}

void MineField::_recount() {
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
