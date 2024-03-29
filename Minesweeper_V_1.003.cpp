#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>
#include <cmath>
#include <iomanip>
#include <io.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

/// <summary>
/// Функция для воспроизведения звука взрыва
/// </summary>
void playExplosionSound() {
    PlaySound(TEXT("C:/Users/sadev/Downloads/explosion.wav"), NULL, SND_FILENAME | SND_ASYNC);
}
/// <summary>
/// Функция для воспроизведения звука победы
/// </summary>
void playVictorySound() {
    PlaySound(TEXT("C:/Users/sadev/Downloads/victory.wav"), NULL, SND_FILENAME | SND_ASYNC);
}
/// <summary>
/// Функция для воспроизведения звука установки флага
/// </summary>
void playFlagSound() {
    PlaySound(TEXT("C:/Users/sadev/Downloads/aktivator.wav"), NULL, SND_FILENAME | SND_ASYNC);
}
/// <summary>
/// Функция для воспроизведения звука снятия флага
/// </summary>
void playDeactSound() {
    PlaySound(TEXT("C:/Users/sadev/Downloads/deton.wav"), NULL, SND_FILENAME | SND_ASYNC);
}
/// <summary>
/// Функция для освобождения памяти, выделенной для поля
/// </summary>
/// <param name="field">Указатель на массив указателей, представляющий игровое поле.</param>
/// <param name="rows">Количество строк в игровом поле.</param>
void delete_field(char** field, int rows) {
    for (int i = 0; i < rows; i++)
        delete[] field[i];
    delete[] field;
}
/// <summary>
/// Генерация поля с минами
/// </summary>
/// <param name="a">Высота поля (количество строк)</param>
/// <param name="b">Ширина поля (количество столбцов)</param>
/// <param name="bombs">Количество мин на поле</param>
/// <param name="autoplay">Флаг автоматического запуска игры</param>
/// <returns>Указатель на массив, представляющий игровое поле</returns>
char** gen_field(int a, int b, int bombs, bool autoplay = false) {
    char** field = new char* [a];
    const char Free_Cell = 0;
    const char Mine = 9;
    // Заполнение поля пустыми клетками
    for (int i = 0; i < a; i++) {
        field[i] = new char[b];
        for (int j = 0; j < b; j++)
            field[i][j] = Free_Cell;
    }

    // Размещение мин на поле
    int bombs_placed = 0;
    while (bombs_placed < bombs) {
        int row = rand() % a;
        int col = rand() % b;
        if (field[row][col] != Mine) {
            field[row][col] = Mine;
            bombs_placed++;
        }
    }

    // Увеличение значений клеток вокруг мин (для каждой мины)
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            // Если клетка содержит мину
            if (field[i][j] == Mine) {
                // Увеличиваем значения клеток вокруг мин
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int ni = i + dx;
                        int nj = j + dy;
                        if (ni >= 0 && ni < a && nj >= 0 && nj < b && field[ni][nj] != Mine)
                            field[ni][nj]++;
                    }
                }
            }
        }
    }

    cout << "\n";
    cout << "Field is generated\n\n";
    return field;
}
/// <summary>
/// Рекурсивно открывает пустые клетки и раскрывает соседние клетки.
/// </summary>
/// <param name="field">Игровое поле, представленное в виде массива.</param>
/// <param name="player_view">Вид игрока на поле, представленный в виде массива.</param>
/// <param name="a">Количество строк в игровом поле.</param>
/// <param name="b">Количество столбцов в игровом поле.</param>
/// <param name="x">Координата X текущей клетки.</param>
/// <param name="y">Координата Y текущей клетки.</param>
void open_space_recursive(char** field, char** player_view, int a, int b, int x, int y) {
    const char Closed_Cell = 0;
    const char Exploded_Mine = 1;
    const char Opened_Cell = 3;
    const char Free_Cell = 0;
    const char Mine = 9;
    // Проверяем, находится ли текущая клетка в пределах поля
    if (x >= 0 && x < 2 * b && y >= 0 && y < a) {
        // Проверяем, закрыта ли текущая клетка и является ли она пустой
        if (player_view[y][x / 2] == Closed_Cell && field[y][x / 2] == Free_Cell) {
            // Открываем текущую клетку и переходим к проверке соседних клеток
            player_view[y][x / 2] = Opened_Cell;
            // Рекурсивно вызываем функцию для соседних клеток
            open_space_recursive(field, player_view, a, b, x - 2, y); // клетка слева
            open_space_recursive(field, player_view, a, b, x + 2, y); // клетка справа
            open_space_recursive(field, player_view, a, b, x, y - 1); // клетка сверху
            open_space_recursive(field, player_view, a, b, x, y + 1); // клетка снизу
        }
        else if (field[y][x / 2] == Mine) {
            // Если текущая клетка содержит бомбу, отмечаем ее как взорванную
            player_view[y][x / 2] = Exploded_Mine;
        }
        else if (player_view[y][x / 2] == Closed_Cell && field[y][x / 2] > 0 && field[y][x / 2] < 9) {
            // Если текущая клетка содержит число от 1 до 8, открываем ее
            player_view[y][x / 2] = Opened_Cell;
        }
    }
}
/// <summary>
/// Открываем пустые клетки и раскрываем соседние клетки (внешний интерфейс).
/// </summary>
/// <param name="field">Игровое поле, представленное в виде массива.</param>
/// <param name="player_view">Вид игрока на поле, представленный в виде массива.</param>
/// <param name="a">Количество строк в игровом поле.</param>
/// <param name="b">Количество столбцов в игровом поле.</param>
/// <param name="x">Координата X выбранной клетки.</param>
/// <param name="y">Координата Y выбранной клетки.</param>
void open_space(char** field, char** player_view, int a, int b, int x, int y) {
    int xDiv2 = x / 2;
    // Проверяем, является ли выбранная клетка четной
    if (!(x % 2)) {
        // Вызываем рекурсивную функцию для открытия пустых клеток и раскрытия соседних
        open_space_recursive(field, player_view, a, b, x, y);
    }
}
/// <summary>
/// Управление игровым процессом.
/// </summary>
/// <param name="field">Игровое поле, представленное в виде массива.</param>
/// <param name="a">Количество строк в игровом поле.</param>
/// <param name="b">Количество столбцов в игровом поле.</param>
/// <param name="bombs">Общее количество бомб на игровом поле.</param>

void play(char** field, int a, int b, int bombs) {
    const char Closed_Cell = 0;
    const char Exploded_Mine = 1;
    const char Opened_Cell = 3;
    const char Flag = 4;
    const char Free_Cell = 0;
    const char Mine = 9;
    HANDLE hIn, hOut;
    INPUT_RECORD InRec;
    DWORD NumRead;
    CONSOLE_CURSOR_INFO Curs;
    CONSOLE_SCREEN_BUFFER_INFO Buff;
    COORD zero = { 0,0 };
    bool game_over = false, win = false;
    int flag = 0, clean = 2, x, y, i, j, input = 1, marked_bombs = 0, opened_cells = 0, bombs_at_beginning = bombs, xDiv2;

    // Выделение памяти для массива player_view, представляющего вид игрока на поле
    char** player_view = new char* [a];
    for (i = 0; i < a; i++) {
        player_view[i] = new char[b];
        for (j = 0; j < b; j++)
            player_view[i][j] = Closed_Cell;
    }
    // Определение количества символов для корректного отображения оставшихся мин
    for (i = bombs; (i /= 10) > 0;)
        clean++;
    // Очистка консоли перед началом игры
    system("cls");

    // Инициализация консольных дескрипторов и установка режима ввода
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hOut, &Curs);
    SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);


    // Основной игровой цикл
    while (!game_over) {
        // Скрытие курсора и перемещение его в начало консоли
        Curs.bVisible = false;
        SetConsoleCursorInfo(hOut, &Curs);
        SetConsoleCursorPosition(hOut, zero);

        // Обход всех клеток на поле
        for (i = 0; i < a; i++) {
            for (j = 0; j < b; j++) {
                // Обработка различных состояний клеток поля и вывод соответствующего символа
                switch (player_view[i][j]) {
                    // Клетка закрыта
                case Closed_Cell: {
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 6));
                    WriteConsoleW(hOut, L"■ ", wcslen(L"■ "), NULL, NULL);
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                    break;
                }
                  // В клетке бомба, которая взорвалась
                case Exploded_Mine: {
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
                    WriteConsoleW(hOut, L"💥 ", wcslen(L"💥 "), NULL, NULL);
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                    playExplosionSound(); 
                    game_over = true;
                    break;
                }
                   // Клетка открыта
                case Opened_Cell: {
                    opened_cells++;
                    // В зависимости от содержимого клетки выводится соответствующий символ или число. Для чисел выводится определенный цвет
                    switch (field[i][j]) {
                    case Free_Cell: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 8));
                        WriteConsoleW(hOut, L"□ ", wcslen(L"□ "), NULL, NULL);
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 1: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 9));
                        cout << "1 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 2: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 10));
                        cout << "2 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 3: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 12));
                        cout << "3 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 4: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 5));
                        cout << "4 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 5: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
                        cout << "5 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 6: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 1));
                        cout << "6 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 7: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 2));
                        cout << "7 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                    case 8: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 8));
                        cout << "8 ";
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        break;
                    }
                          // Клетка с бомбой
                    case Mine: {
                        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                        WriteConsoleW(hOut, L"💥 ", wcslen(L"💥 "), NULL, NULL);
                        playExplosionSound(); 
                        game_over = true;
                        break;
                    }
                    }
                    break;
                }
                     // Клетка помечена флагом
                case Flag: {
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 12));
                    WriteConsoleW(hOut, L"⚐ ", wcslen(L"⚐ "), NULL, NULL);
                    SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
                     // Подсчет количества отмеченных бомб
                    if (field[i][j] == 9)
                        marked_bombs++;
                    else
                        marked_bombs--;
                }
                }
            }
            cout << "\n";
        }
        // Проверка условий победы или поражения
        if (marked_bombs == bombs_at_beginning || opened_cells >= (a * b - bombs_at_beginning))
            win = game_over = true;
        // Сброс счетчиков
        opened_cells = 0;
        marked_bombs = 0;
        cout << "\nMines left: ";
        for (i = clean; i != 0; i--)
            cout << ' ';
        GetConsoleScreenBufferInfo(hOut, &Buff);
        Buff.dwCursorPosition.X -= clean;
        SetConsoleCursorPosition(hOut, Buff.dwCursorPosition);
        cout << bombs;
        cout << "\nPress 0 to exit\nYour choice: ";
        Curs.bVisible = true;
        SetConsoleCursorInfo(hOut, &Curs);
        // Обработка ввода пользователя в зависимости от режима игры
        ReadConsoleInput(hIn, &InRec, 1, &NumRead);
        switch (InRec.EventType) {
            // Обработка событий мыши
        case MOUSE_EVENT: {
            if (InRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                x = InRec.Event.MouseEvent.dwMousePosition.X;
                y = InRec.Event.MouseEvent.dwMousePosition.Y;
                open_space(field, player_view, a, b, x, y);
            }
            if (InRec.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                x = InRec.Event.MouseEvent.dwMousePosition.X;
                y = InRec.Event.MouseEvent.dwMousePosition.Y;
                xDiv2 = x / 2;
                if (y < a && xDiv2 < b && !(x % 2)) {
                    if (player_view[y][xDiv2] == Closed_Cell)
                        flag = 1;
                    if (player_view[y][xDiv2] == Flag)
                        flag = 2;
                    if (flag == 1) {
                        player_view[y][xDiv2] = Flag;
                        bombs--;
                        playFlagSound(); 
                    }
                    if (flag == 2) {
                        player_view[y][xDiv2] = Closed_Cell;
                        bombs++;
                        playDeactSound(); 
                    }
                    flag = 0;
                }
            }
            break;
        }
            // Обработка событий клавиатуры
        case KEY_EVENT: {
            if (InRec.Event.KeyEvent.uChar.AsciiChar == '0') {
                game_over = true;
                cout << "0\n";
            }
        }
        }
    }
    if (game_over) {
        GetConsoleScreenBufferInfo(hOut, &Buff);
        Buff.dwCursorPosition.X--;
        Buff.dwCursorPosition.Y--;
        SetConsoleCursorPosition(hOut, Buff.dwCursorPosition);
        if (!win) {
            SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
            cout << "\nGAME OVER      \n";
        }
        if (win) {
            SetConsoleTextAttribute(hOut, (WORD)(0 | 10));
            cout << "\nYOU WIN       \n";
            SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
            playVictorySound(); 
        }
        SetConsoleTextAttribute(hOut, (WORD)(0 | 2));
        cout << "Press any button to exit...";
        SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
        do {
            ReadConsoleInput(hIn, &InRec, 1, &NumRead);
        } while (InRec.EventType != KEY_EVENT);
    }
    system("cls");
    delete_field(player_view, a);
}

/// <summary>
/// Основная функция программы.
/// </summary>
/// <returns>Целочисленный код завершения программы.</returns>
int main() {
    int input = 0, a, b, x, bombs;
    char** field = nullptr;
    bool field_was_generated = false;
    srand(time(NULL));
    do {
        cout << "   I========================I" << "\n";
        cout << "   | --=<| Minesweeper |>=--|" << "\n";
        cout << "   I========================I" << "\n" << "\n\n";
        cout << "          -=<><><>=-         " << "\n";
        cout << "<>==<> Choose an option <>==<>\n";
        cout << "           -=<><>=-         " << "\n";
        cout << "              <>         " << "\n\n";
        cout << "\t1.Generate field\n\t2.Play\n\t3.Creator\n\t0.Exit\n\nYour choice: ";
        cin >> input;
        cout << '\n';
        system("cls");
        switch (input) {
        case 1: {
            cout << "1.Field 9x9, 10 bombs\n2.Field 16x16, 40 bombs\n3.Create custom\n0.Go back\nYour choice: ";
            cin >> x;
            cout << '\n';
            system("cls");
            switch (x) {
            case 1: {
                if (field_was_generated) {
                    delete_field(field, a);
                }
                field = gen_field(9, 9, 10, false);
                a = b = 9;
                bombs = 10;
                field_was_generated = true;
                continue;
            }
            case 2: {
                if (field_was_generated) {
                    delete_field(field, a);
                }
                field = gen_field(16, 16, 40, false);
                a = b = 16;
                bombs = 40;
                field_was_generated = true;
                continue;
            }
            case 3: {
                if (field_was_generated) {
                    delete_field(field, a);
                }
                cout << "Enter a height: ";
                cin >> a;
                if (a > 48)
                    a = 48;
                cout << "Enter a width: ";
                cin >> b;
                if (b > 105)
                    b = 105;
                cout << "Enter the number of bombs: ";
                cin >> bombs;
                system("cls");
                if (bombs > a * b)
                    bombs = a * b;
                field = gen_field(a, b, bombs, false);
                field_was_generated = true;
                continue;
            }
            default: {
                x = 0;
                continue;
            }
            }
       
        }
        case 2: {
            if (field_was_generated) {
                play(field, a, b, bombs);
                cout << "Let`s play!\n\n";
                delete_field(field, a); 
                field_was_generated = false; 
            }
            else {
                cout << "Generate field at first\n\n";
            }
            break;
        }
        case 3: {
            cout << "Creator:\nEugenia Sadovaya\n\n";
            break;
        }
        default:
            input = 0;
        }
    } while (input != 0);
    return 0;
}
