#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>
#include <bitset>

int sudoku[9][9] = {0};  
bool fixed[9][9] = {false};  
int selectedX = -1, selectedY = -1;  
bool inputMode = false;  
bool gameMode = true;     
bool notesMode = false;


bool rowConflict[9][10] = {false};    
bool colConflict[9][10] = {false};    
bool boxConflict[9][10] = {false};   
std::bitset<10> notes[9][9];

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
                        }
                    }
                }
            }
        }
    }
}

bool hasConflict(int row, int col) {
    if(sudoku[row][col] == 0) return false;
    int num = sudoku[row][col];
    int box = (row/3)*3 + col/3;
    return (rowConflict[row][num] || colConflict[col][num] || boxConflict[box][num]);
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

int fillAllSingleCandidates() {
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
        int filled = fillAllSingleCandidates();
        
        
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

void generatePuzzle(int emptyCells = 50) {
    
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
        for(int j = 0; j < 9; j++) {
            fixed[i][j] = (sudoku[i][j] != 0);
            notes[i][j].reset();
        }
    
    updateConflicts();

    
}

void toggleNote(int row, int col, int num) {
    if (notes[row][col][num]) {
        notes[row][col][num] = 0;
    } else {
        notes[row][col][num] = 1;
    }
}

void clearNotes(int row, int col) {
    notes[row][col].reset();
}

bool isButtonClicked(float x, float y, float btnX, float btnY, float btnW = 60.f, float btnH = 30.f) {
    return x >= btnX && x <= btnX + btnW && y >= btnY && y <= btnY + btnH;
}

bool saveGame(const std::string& filename = "sudoku_save.dat") {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file.write(reinterpret_cast<const char*>(&sudoku[i][j]), sizeof(int));
        }
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file.write(reinterpret_cast<const char*>(&fixed[i][j]), sizeof(bool));
        }
    }
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            unsigned long notesData = notes[i][j].to_ulong();
            file.write(reinterpret_cast<const char*>(&notesData), sizeof(unsigned long));
        }
    }

    file.write(reinterpret_cast<const char*>(&inputMode), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&gameMode), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&notesMode), sizeof(bool));
    
    file.close();
    return true;
}

bool loadGame(const std::string& filename = "sudoku_save.dat") {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file.read(reinterpret_cast<char*>(&sudoku[i][j]), sizeof(int));
        }
    }
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file.read(reinterpret_cast<char*>(&fixed[i][j]), sizeof(bool));
        }
    }
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            unsigned long notesData;
            file.read(reinterpret_cast<char*>(&notesData), sizeof(unsigned long));
            notes[i][j] = std::bitset<10>(notesData);
        }
    }
    
    file.read(reinterpret_cast<char*>(&inputMode), sizeof(bool));
    file.read(reinterpret_cast<char*>(&gameMode), sizeof(bool));
    file.read(reinterpret_cast<char*>(&notesMode), sizeof(bool));
    
    file.close();
    updateConflicts();
    return true;
}


int main() {
    
    sf::RenderWindow window(sf::VideoMode({600u, 600u}), "Sudoku");
    
    
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

        std::vector<std::pair<std::string, sf::Vector2f>> buttons = {
            {"Hint", {20.f, 480.f}},
            {"Input", {100.f, 480.f}},
            {"New", {180.f, 480.f}},
            {"Solve", {260.f, 480.f}},
            {"Clear", {340.f, 480.f}},
            {"Notes", {420.f, 480.f}},
            {"Save", {20.f, 520.f}},
            {"Load", {100.f, 520.f}}
        };
        
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                // saveGame();  // нужен ли автосейв
                window.close();
            }
            else if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    float x = static_cast<float>(mouseBtn->position.x);
                    float y = static_cast<float>(mouseBtn->position.y);
                    
                    
                    if(y >= 470.f && y <= 580.f) {
                        
                        if(isButtonClicked(x, y, 20.f, 480.f, 60.f, 35.f)) { // HINT
                            fillOneHintCell();
                        }
                        
                        else if(isButtonClicked(x, y, 100.f, 480.f, 60.f, 35.f)) { // INPUT
                            if (inputMode == false){
                                inputMode = true;
                                gameMode = false;
                                for(int i = 0; i < 9; i++)
                                    for(int j = 0; j < 9; j++)
                                        fixed[i][j] = false;
                                updateConflicts();
                            }
                            else {
                                gameMode = true;
                                inputMode = false;
                                for(int i = 0; i < 9; i++)
                                    for(int j = 0; j < 9; j++)
                                        if (sudoku[i][j]==0)
                                            fixed[i][j] = false;
                                        else 
                                            fixed[i][j] = true;
                                updateConflicts();
                            }
                        }
                        
                        else if(isButtonClicked(x, y, 180.f, 480.f, 60.f, 35.f)) { // NEW
                            generatePuzzle(40);
                            inputMode = false;
                            gameMode = true;
                        }
                        
                        else if(isButtonClicked(x, y, 260.f, 480.f, 60.f, 35.f)) { // SOLVE
                            fillAllSingleCandidates();
                            for(int i = 0; i < 9; i++)
                                for(int j = 0; j < 9; j++)
                                    fixed[i][j] = true;
                            updateConflicts();
                        }
                        
                        else if(isButtonClicked(x, y, 340.f, 480.f, 60.f, 35.f)) { // CLEAR
                            for(int i = 0; i < 9; i++)
                                for(int j = 0; j < 9; j++){
                                    if (!fixed[i][j])
                                    sudoku[i][j] = 0;
                                }
                            updateConflicts();
                        }

                        else if(isButtonClicked(x, y, 420.f, 480.f, 60.f, 35.f)) { //Notes
                            notesMode = !notesMode;
                        }

                        else if(isButtonClicked(x, y, 20.f, 520.f, 60.f, 35.f)) { // SAVE
                            saveGame();
                        }
                       
                        else if(isButtonClicked(x, y, 120.f, 520.f, 60.f, 35.f)) { // LOAD
                            loadGame();
                        }

                    }
                    
                    else if(x < 470.f && x > 20.f && y < 470.f && y > 20.f) { // Выбор клетки судоку
                        int col = static_cast<int>((x - 20.f) / 50.f);
                        int row = static_cast<int>((y - 20.f) / 50.f);
                        
                        if(col >= 0 && col < 9 && row >= 0 && row < 9) {
                            selectedX = col;
                            selectedY = row;
                        }
                    }
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) { //Клавиши клавиатуры
                if(selectedX != -1 && selectedY != -1 && !fixed[selectedY][selectedX]) {

                    if(notesMode && sudoku[selectedY][selectedX] == 0 && !fixed[selectedY][selectedX]) {
                        if(keyPressed->code >= sf::Keyboard::Key::Num1 && 
                           keyPressed->code <= sf::Keyboard::Key::Num9) {
                            int num = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Num1) + 1;
                            toggleNote(selectedY, selectedX, num);
                        }
                        else if(keyPressed->code >= sf::Keyboard::Key::Numpad1 && 
                                keyPressed->code <= sf::Keyboard::Key::Numpad9) {
                            int num = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Numpad1) + 1;
                            toggleNote(selectedY, selectedX, num);
                        }
                        else if(keyPressed->code == sf::Keyboard::Key::Backspace ||
                                keyPressed->code == sf::Keyboard::Key::Delete) {
                            clearNotes(selectedY, selectedX);
                        }
                    }

                    else if (!notesMode){
                        if(keyPressed->code >= sf::Keyboard::Key::Num1 && 
                           keyPressed->code <= sf::Keyboard::Key::Num9) {
                            int num = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Num1) + 1;
                            
                            if(!inputMode || isValid(selectedY, selectedX, num)) {
                                sudoku[selectedY][selectedX] = num;
                                updateConflicts();
                            }
                        }

                        if(keyPressed->code >= sf::Keyboard::Key::Numpad1 && 
                           keyPressed->code <= sf::Keyboard::Key::Numpad9) {
                            int num = static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Numpad1) + 1;
                            
                            if(!inputMode || isValid(selectedY, selectedX, num)) {
                                sudoku[selectedY][selectedX] = num;
                                updateConflicts();
                            }
                        }
                        
                        if(keyPressed->code == sf::Keyboard::Key::Num0 ||
                            keyPressed->code == sf::Keyboard::Key::Numpad0 ||
                            keyPressed->code == sf::Keyboard::Key::Backspace) {
                            sudoku[selectedY][selectedX] = 0;
                            updateConflicts();
                        }
                    }
                }
                
                
                if(keyPressed->code == sf::Keyboard::Key::R) {
                    generatePuzzle(40);
                    inputMode = false;
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
                else if(fixed[i][j] && (i == selectedY || j == selectedX)) {
                    cell.setFillColor(sf::Color(135, 138, 81));
                }
                else if(i == selectedY || j == selectedX) {
                    cell.setFillColor(sf::Color(250, 255, 163));
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
                
                if(sudoku[i][j] == 0 && !fixed[i][j]) { // нотесы
                    float offset = 4.f;
                    float noteSize = 12.f;
                    
                    for(int num = 1; num <= 9; num++) {
                        if(notes[i][j][num]) {
                            int noteRow = 2 - ((num - 1) / 3);
                            int noteCol = (num - 1) % 3;
                            
                            sf::Text noteText(font, std::to_string(num), noteSize);
                            noteText.setPosition(sf::Vector2f(
                                static_cast<float>(j * 50 + 20 + noteCol * 14 + offset),
                                static_cast<float>(i * 50 + 20 + noteRow * 12 + offset)
                            ));
                            noteText.setFillColor(sf::Color(128, 128, 128));
                            window.draw(noteText);
                        }
                    }
                }

                if(sudoku[i][j] != 0) {
                    sf::Text text(font, std::to_string(sudoku[i][j]), 30);
                    text.setPosition(sf::Vector2f(static_cast<float>(j * 50 + 35), static_cast<float>(i * 50 + 25)));
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
        
        std::string statusText = inputMode ? "INPUT MODE" : "SOLVE MODE";

        if(notesMode) statusText += " NOTES MODE";
        sf::Text status(font, statusText, 18);
        
        status.setFillColor(sf::Color(100, 100, 100));
        status.setPosition({180.f, 525.f});
        window.draw(status);
        
        sf::Text info(font, "LMB - select cell, 1-9 - input, backspace or 0 - erase", 16);
        info.setFillColor(sf::Color(100, 100, 100));
        info.setPosition({20.f, 560.f});
        window.draw(info);
        
        window.display();
    }
    
    return 0;
}
