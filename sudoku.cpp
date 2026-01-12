#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>

int sudoku[9][9] = {0};  
bool fixed[9][9] = {false};  
int selectedX = -1, selectedY = -1;  
bool solvingMode = false;  
bool gameMode = true;     


bool rowConflict[9][10] = {false};    
bool colConflict[9][10] = {false};    
bool boxConflict[9][10] = {false};    

std::mt19937 rng(static_cast<unsigned>(time(0)));


void copyBoard(int dest[9][9], int src[9][9]) {
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            dest[i][j] = src[i][j];
}


void updateConflicts() {
    
    memset(rowConflict, 0, sizeof(rowConflict));
    memset(colConflict, 0, sizeof(colConflict));
    memset(boxConflict, 0, sizeof(boxConflict));
    
    
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudoku[i][j] != 0) {
                int num = sudoku[i][j];
                int box = (i/3)*3 + j/3;
                
                
                for(int k = 0; k < 9; k++) {
                    if(k != j && sudoku[i][k] == num) {
                        rowConflict[i][num] = true;
                        break;
                    }
                }
                
                
                for(int k = 0; k < 9; k++) {
                    if(k != i && sudoku[k][j] == num) {
                        colConflict[j][num] = true;
                        break;
                    }
                }
                
                
                int startRow = (i/3)*3, startCol = (j/3)*3;
                for(int r = 0; r < 3; r++) {
                    for(int c = 0; c < 3; c++) {
                        if((startRow+r != i || startCol+c != j) && 
                           sudoku[startRow+r][startCol+c] == num) {
                            boxConflict[box][num] = true;
                            goto next_cell;
                        }
                    }
                }
                next_cell:;
            }
        }
    }
}


bool hasConflict(int row, int col) {
    if(sudoku[row][col] == 0) return false;
    int num = sudoku[row][col];
    int box = (row/3)*3 + col/3;
    return rowConflict[row][num] || colConflict[col][num] || boxConflict[box][num];
}


bool isValid(int row, int col, int num) {
    
    for(int x = 0; x < 9; x++)
        if(sudoku[row][x] == num) return false;
    
    
    for(int x = 0; x < 9; x++)
        if(sudoku[x][col] == num) return false;
    
    
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(sudoku[i + startRow][j + startCol] == num) return false;
    
    return true;
}


int findAllSingleCandidates() {
    int filled = 0;
    bool progress = true;
    
    while(progress) {
        progress = false;
        
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(sudoku[i][j] == 0 && !fixed[i][j]) {
                    std::vector<int> candidates;
                    
                    for(int num = 1; num <= 9; num++) {
                        if(isValid(i, j, num)) {
                            candidates.push_back(num);
                        }
                    }
                    
                    if(candidates.size() == 1) {
                        sudoku[i][j] = candidates[0];
                        fixed[i][j] = true;
                        filled++;
                        progress = true;
                        updateConflicts();
                    }
                }
            }
        }
    }
    return filled;
}


bool fillOneHintCell() {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudoku[i][j] == 0 && !fixed[i][j]) {
                std::vector<int> candidates;
                
                for(int num = 1; num <= 9; num++) {
                    if(isValid(i, j, num)) {
                        candidates.push_back(num);
                    }
                }
                
                
                if(candidates.size() == 1) {
                    sudoku[i][j] = candidates[0];
                    fixed[i][j] = true;
                    updateConflicts();
                    return true;  
                }
            }
        }
    }
    return false;  
}



bool solveSudokuIterative() {
    int initialEmpty = 0;
    
    
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            if(sudoku[i][j] == 0) initialEmpty++;
    
    
    while(true) {
        int filled = findAllSingleCandidates();
        
        
        if(filled == 0) break;
        
        
        bool complete = true;
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(sudoku[i][j] == 0) {
                    complete = false;
                    goto check_complete;
                }
            }
        }
        check_complete:
        if(complete) return true;
    }
    
    return false;  
}


bool solveSudokuIterativeOnBoard(int board[9][9]) {
    
    int saveSudoku[9][9], saveFixed[9][9];
    copyBoard(saveSudoku, sudoku);
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            saveFixed[i][j] = fixed[i][j];
    
    
    copyBoard(sudoku, board);
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            fixed[i][j] = (sudoku[i][j] != 0);
    
    bool result = solveSudokuIterative();
    
    
    copyBoard(sudoku, saveSudoku);
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            fixed[i][j] = saveFixed[i][j];
    
    return result;
}


int countSolutions(int tempBoard[9][9]) {
    int boardCopy1[9][9], boardCopy2[9][9];
    copyBoard(boardCopy1, tempBoard);
    
    
    bool hasSolution1 = solveSudokuIterativeOnBoard(boardCopy1);
    if (!hasSolution1) return 0;
    
    
    copyBoard(boardCopy2, tempBoard);
    
    
    bool foundAlternative = false;
    for (int i = 0; i < 9 && !foundAlternative; i++) {
        for (int j = 0; j < 9; j++) {
            if (boardCopy2[i][j] == 0) {
                
                std::vector<int> candidates;
                for (int num = 1; num <= 9; num++) {
                    if (isValid(i, j, num)) {
                        candidates.push_back(num);
                    }
                }
                
                
                if (candidates.size() >= 2) {
                    
                    boardCopy2[i][j] = candidates[0];
                    
                    
                    int testBoard[9][9];
                    copyBoard(testBoard, tempBoard);
                    testBoard[i][j] = candidates[1];  
                    
                    bool hasSolution2 = solveSudokuIterativeOnBoard(testBoard);
                    if (hasSolution2) {
                        return 2;  
                    }
                    foundAlternative = true;  
                    break;
                }
            }
        }
    }
    
    return 1;  
}



bool generateFullSudoku(int row, int col) {
    if(row == 9) return true;
    if(col == 9) return generateFullSudoku(row + 1, 0);
    
    std::vector<int> nums = {1,2,3,4,5,6,7,8,9};
    std::shuffle(nums.begin(), nums.end(), rng);
    
    for(int num : nums) {
        if(isValid(row, col, num)) {
            sudoku[row][col] = num;
            if(generateFullSudoku(row, col + 1)) return true;
            sudoku[row][col] = 0;
        }
    }
    return false;
}


void generatePuzzle(int emptyCells = 40) {
    
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            sudoku[i][j] = 0;
    
    generateFullSudoku(0, 0);
    
    
    int solution[9][9];
    copyBoard(solution, sudoku);
    
    
    std::vector<std::pair<int,int>> cells;
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            cells.emplace_back(i, j);
    
    
    for(int attempt = 0; attempt < 5; attempt++)
        std::shuffle(cells.begin(), cells.end(), rng);
    
    int removed = 0;
    int attempts = 0;
    const int maxAttempts = 100;  
    
    for(auto [row, col] : cells) {
        if (removed >= emptyCells) break;
        
        
        int backup[9][9];
        copyBoard(backup, sudoku);
        
        
        sudoku[row][col] = 0;
        
        
        if(countSolutions(sudoku) == 1) {
            removed++;
        } else {
            
            copyBoard(sudoku, backup);
        }
        
        attempts++;
        if (attempts > maxAttempts) break;  
    }
    
    
    while (removed < emptyCells && attempts < maxAttempts * 2) {
        int row = rand() % 9;
        int col = rand() % 9;
        
        if (sudoku[row][col] != 0) {
            int backup[9][9];
            copyBoard(backup, sudoku);
            sudoku[row][col] = 0;
            
            if (countSolutions(sudoku) == 1) {
                removed++;
            } else {
                copyBoard(sudoku, backup);
            }
        }
        attempts++;
    }
    
    
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            fixed[i][j] = (sudoku[i][j] != 0);
    
    updateConflicts();
}


bool isButtonClicked(float x, float y, float btnX, float btnY, float btnW = 60.f, float btnH = 30.f) {
    return x >= btnX && x <= btnX + btnW && y >= btnY && y <= btnY + btnH;
}

int main() {
    
    sf::RenderWindow window(sf::VideoMode({500u, 600u}), "Sudoku");
    
    
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Error: arial.ttf not found! Put arial.ttf in the same folder." << std::endl;
        return 1;
    }
    
    
    int puzzle[9][9] = {
        {5,3,0,0,7,0,0,0,0},
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };
    
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            sudoku[i][j] = puzzle[i][j];
            fixed[i][j] = (sudoku[i][j] != 0);
        }
    }
    updateConflicts();
    
    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f coords = window.mapPixelToCoords(mousePos);
        
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    float x = static_cast<float>(mouseBtn->position.x);
                    float y = static_cast<float>(mouseBtn->position.y);
                    
                    
                    if(y >= 470.f && y <= 580.f) {
                        
                        if(isButtonClicked(x, y, 20.f, 475.f, 70.f, 35.f)) {
                            fillOneHintCell();
                        }
                        
                        else if(isButtonClicked(x, y, 100.f, 475.f, 90.f, 35.f)) {
                            solvingMode = true;
                            gameMode = false;
                            for(int i = 0; i < 9; i++)
                                for(int j = 0; j < 9; j++)
                                    fixed[i][j] = false;
                            updateConflicts();
                        }
                        
                        else if(isButtonClicked(x, y, 200.f, 475.f, 70.f, 35.f)) {
                            generatePuzzle(40);
                            solvingMode = false;
                            gameMode = true;
                        }
                        
                        else if(isButtonClicked(x, y, 280.f, 475.f, 70.f, 35.f)) {
                            solveSudokuIterative();
                            for(int i = 0; i < 9; i++)
                                for(int j = 0; j < 9; j++)
                                    fixed[i][j] = true;
                            updateConflicts();
                        }
                        
                        else if(isButtonClicked(x, y, 360.f, 475.f, 70.f, 35.f)) {
                            for(int i = 0; i < 9; i++)
                                for(int j = 0; j < 9; j++)
                                    sudoku[i][j] = 0;
                            updateConflicts();
                        }
                    }
                    
                    else if(x < 470.f && y < 470.f) {
                        int col = static_cast<int>((x - 20.f) / 50.f);
                        int row = static_cast<int>((y - 20.f) / 50.f);
                        
                        if(col >= 0 && col < 9 && row >= 0 && row < 9) {
                            selectedX = col;
                            selectedY = row;
                        }
                    }
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(selectedX != -1 && selectedY != -1) {
                    
                    if(gameMode && fixed[selectedY][selectedX]) {
                        
                        if(keyPressed->code == sf::Keyboard::Key::Num0 ||
                           keyPressed->code == sf::Keyboard::Key::Backspace) {
                            sudoku[selectedY][selectedX] = 0;
                            updateConflicts();
                        }
                    }
                    
                    else if(!gameMode || !fixed[selectedY][selectedX]) {
                        
                        if(keyPressed->code >= sf::Keyboard::Key::Num1 && 
                           keyPressed->code <= sf::Keyboard::Key::Num9) {
                            int num = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Num1) + 1;
                            
                            if(!solvingMode || isValid(selectedY, selectedX, num)) {
                                sudoku[selectedY][selectedX] = num;
                                updateConflicts();
                            }
                        }
                        
                        else if(keyPressed->code == sf::Keyboard::Key::Num0 ||
                                keyPressed->code == sf::Keyboard::Key::Backspace) {
                            sudoku[selectedY][selectedX] = 0;
                            updateConflicts();
                        }
                    }
                }
                
                
                if(keyPressed->code == sf::Keyboard::Key::R) {
                    generatePuzzle(40);
                    solvingMode = false;
                    gameMode = true;
                }
            }
        }
        
        
        window.clear(sf::Color(200, 200, 200));
        
        
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                sf::RectangleShape cell(sf::Vector2f(50.f, 50.f));
                cell.setPosition(sf::Vector2f(static_cast<float>(j * 50 + 20), static_cast<float>(i * 50 + 20)));
                
                
                if(i == selectedY && j == selectedX) {
                    cell.setFillColor(sf::Color::Yellow);
                }
                else if(fixed[i][j]) {
                    cell.setFillColor(sf::Color(150, 150, 150));
                }
                else if(hasConflict(i, j)) {
                    cell.setFillColor(sf::Color(255, 100, 100));  
                }
                else {
                    cell.setFillColor(sf::Color::White);
                }
                
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(2);
                window.draw(cell);
                
                if(sudoku[i][j] != 0) {
                    sf::Text text(font, std::to_string(sudoku[i][j]), 30);
                    text.setPosition(sf::Vector2f(static_cast<float>(j * 50 + 32), static_cast<float>(i * 50 + 15)));
                    text.setFillColor(fixed[i][j] ? sf::Color::Black : 
                                    hasConflict(i, j) ? sf::Color(200, 50, 50) : sf::Color::Blue);
                    window.draw(text);
                }
            }
        }
        
        
        sf::RectangleShape line(sf::Vector2f(4.f, 450.f));
        line.setFillColor(sf::Color::Black);
        line.setPosition({170.f, 20.f});
        window.draw(line);
        line.setPosition({320.f, 20.f});
        window.draw(line);
        
        line.setSize(sf::Vector2f(450.f, 4.f));
        line.setPosition({20.f, 170.f});
        window.draw(line);
        line.setPosition({20.f, 320.f});
        window.draw(line);
        
        
        std::vector<std::pair<std::string, sf::Vector2f>> buttons = {
            {"Hint", {25.f, 480.f}},
            {"Input", {110.f, 480.f}},
            {"New", {215.f, 480.f}},
            {"Solve", {295.f, 480.f}},
            {"Clear", {375.f, 480.f}}
        };
        
        sf::Vector2f mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        for(auto& [text, pos] : buttons) {
            sf::RectangleShape btn(sf::Vector2f(60.f, 35.f));
            btn.setPosition(pos);
            btn.setFillColor(sf::Color(220, 220, 220));
            btn.setOutlineColor(sf::Color::Black);
            btn.setOutlineThickness(2);
            
            if(btn.getGlobalBounds().contains(mouseCoords)) {
                btn.setFillColor(sf::Color(180, 180, 180));
            }
            
            window.draw(btn);
            
            sf::Text btnText(font, text, 16);
            btnText.setPosition({pos.x + 5.f, pos.y + 8.f});
            btnText.setFillColor(sf::Color::Black);
            window.draw(btnText);
        }
        
        
        sf::Text status(font, solvingMode ? "INPUT MODE (1-9 valid)" : 
                               gameMode ? "GAME MODE - UNIQUE PUZZLES" : "SOLVE MODE", 18);
        status.setFillColor(sf::Color(100, 100, 100));
        status.setPosition({20.f, 525.f});
        window.draw(status);
        
        sf::Text info(font, "Click cell, 1-9, 0=erase, R=new game", 16);
        info.setFillColor(sf::Color(100, 100, 100));
        info.setPosition({20.f, 555.f});
        window.draw(info);
        
        window.display();
    }
    
    return 0;
}
