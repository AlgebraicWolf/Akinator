#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <clocale>
#include <ncurses.h>
#include "../Tree/Tree.h"

enum GAMEMODES {
    PLAY = 0,
    COMPARE = 1,
    PRINT = 2,
    EXIT = 3
};

enum VOICES {
    OKSANA,
    JANE,
    OMAZH,
    ZAHAR,
    ERMIL,
    SILAERKAN,
    ERKANYAVAS,
    ALYSS,
    NICK,
    ALENA,
    FILIPP
};
const int KEY_ACTUAL_ENTER = 10;

size_t MAX_VOICE_SIZE = 10;
size_t MAX_EMOTION_SIZE = 7;
size_t MAX_DUMMY_SIZE = 30;

size_t SUBQUESTION_SIZE = 65;

size_t MAX_LINE_SIZE = 129;

enum EMOTIONS {
    GOOD,
    EVIL,
    NEUTRAL
};

size_t getFilesize(FILE *f);

char *loadFile(const char *filename);

void *deserializePrompt(char *ser);

void say(const char *prompt, VOICES voice, EMOTIONS emotion);

void akinatorPlayNode(node_t *node);

int askMode();

int main() {
    setlocale(LC_ALL, "");
    initscr();
    char *serializedDT = loadFile("decisionTreeSerialized.txt");
    tree_t *decisionTree = treeDeserialize(serializedDT, deserializePrompt);
    printw("Привет! Пожалуйста, выбери режим работы:\n");
    say("Привет! Пожалуйста, выбери режим работы", ALENA, NEUTRAL);

    int mode = 0;
    int y = 0;
    int x = 0;
    while ((mode = askMode()) != 3) {
        switch (mode) {
            case PLAY:
                addch('\n');
                akinatorPlayNode(decisionTree->head);
                break;
            default:
                say("Простите, но эта фича не имплементирована", ALENA, NEUTRAL);
                getyx(stdscr, y, x);
                move(y, 1);
                break;
        }
    }

    say("Всего доброго!", ALENA, NEUTRAL);
    endwin();
    return 0;
}

void *deserializePrompt(char *ser) {
    return ser;
}

int askMode() {
    char modes[][20] = {"Играть", "Сравнить", "Дерево", "Выйти"};
    int modes_num = 4;

    curs_set(0);
    keypad(stdscr, true);
    noecho();

    int pos = 0;
    int ch = 0;
    int x = 0;
    int y = 0;
    getyx(stdscr, y, x);

    while (ch != KEY_ACTUAL_ENTER) {
        switch (ch) {
            case KEY_RIGHT:
                pos++;
                break;
            case KEY_LEFT:
                pos--;
                break;
        }

        if (pos == -1) pos = modes_num - 1;
        else if (pos == modes_num) pos = 0;


        wmove(stdscr, y, x);
        for (int i = 0; i < modes_num; i++) {
            if (i == pos)
                attron(A_STANDOUT);

            printw(modes[i]);

            if (i == pos)
                attroff(A_STANDOUT);

            addch(' ');
        }
        refresh();

        ch = wgetch(stdscr);
    }

    keypad(stdscr, false);
    echo();
    return pos;
}

int askYesNo() {
    curs_set(0);
    keypad(stdscr, true);
    noecho();
    int pos = 0;
    int ch = 0;
    int x = 0;
    int y = 0;
    getyx(stdscr, y, x);
    while (ch != KEY_ACTUAL_ENTER) {
        switch (ch) {
            case KEY_RIGHT:
                pos += 1;
                break;

            case KEY_LEFT:
                pos -= 1;
                break;
        }
        if (pos == 2) pos = 0;
        else if (pos == -1) pos = 1;

        if (pos == 0) {
            attron(A_STANDOUT);
            mvprintw(y, x, "Да");
            attroff(A_STANDOUT);
            printw(" Нет");
        }

        if (pos == 1) {
            mvprintw(y, x, "Да ");
            attron(A_STANDOUT);
            printw("Нет");
            attroff(A_STANDOUT);
        }

        ch = wgetch(stdscr);
    }

    keypad(stdscr, false);
    echo();

    if (pos == 0)
        mvprintw(y, x, "Ваш ответ: Да\n");

    if (pos == 1)
        mvprintw(y, x, "Ваш ответ: Нет\n");
    refresh();
    return !pos;
}

size_t getFilesize(FILE *f) {
    assert(f);

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

char *loadFile(const char *filename) {
    assert(filename);

    FILE *input = fopen(filename, "r");

    size_t size = getFilesize(input);
    char *content = (char *) calloc(size, sizeof(char));
    fread(content, sizeof(char), size, input);

    fclose(input);

    return content;
}

void say(const char *prompt, VOICES voice, EMOTIONS emotion) {
    assert(prompt);

    size_t length = strlen(prompt);
    char *request = (char *) calloc(MAX_DUMMY_SIZE + MAX_EMOTION_SIZE + MAX_VOICE_SIZE + length, sizeof(char));
    size_t offset = sprintf(request, "./say.sh \"%s\" ", prompt);

    switch (voice) {
        case OKSANA:
            offset += sprintf(request + offset, "%s ", "oksana");
            break;
        case JANE:
            offset += sprintf(request + offset, "%s ", "jane");
            break;
        case OMAZH:
            offset += sprintf(request + offset, "%s ", "omazh");
            break;
        case ZAHAR:
            offset += sprintf(request + offset, "%s ", "zahar");
            break;
        case ERMIL:
            offset += sprintf(request + offset, "%s ", "ermil");
            break;
        case SILAERKAN:
            offset += sprintf(request + offset, "%s ", "silaerkan");
            break;
        case ERKANYAVAS:
            offset += sprintf(request + offset, "%s ", "erkanyavas");
            break;
        case ALYSS:
            offset += sprintf(request + offset, "%s ", "alyss");
            break;
        case NICK:
            offset += sprintf(request + offset, "%s ", "nick");
            break;
        case ALENA:
            offset += sprintf(request + offset, "%s ", "alena");
            break;
        case FILIPP:
            offset += sprintf(request + offset, "%s ", "filipp");
            break;
    }

    switch (emotion) {
        case GOOD:
            sprintf(request + offset, "%s", "good 1&>2 /dev/null");
            break;
        case EVIL:
            sprintf(request + offset, "%s", "evil 1&>2 /dev/null");
            break;
        case NEUTRAL:
            sprintf(request + offset, "%s", "neutral 1&>2 /dev/null");
            break;
    }
    system(request);
    free(request);
}

void addSplitRoutine(node_t *node) {
    printw("Что вы задали?");
    refresh();
    printw("Я загадал: ");
    getline()
}

void akinatorPlayNode(node_t *node) {
    assert(node);
    int ans = -1;
    if (!node->right && !node->left) {
        char *str = (char *) calloc(SUBQUESTION_SIZE + strlen((char *) node->value), sizeof(char));
        sprintf(str, "Вы загадали %s, не так ли?\n", (char *) node->value);
        printw("%s", str);
        refresh();

        say(str, ALENA, NEUTRAL);
        ans = askYesNo();
        if (ans) {
            printw("Так и знала! Что будем делать дальше?\n");
            say("Так и знала! Что будем делать дальше?", ALENA, NEUTRAL);
        }

    } else {
        char *str = (char *) calloc(SUBQUESTION_SIZE + strlen((char *) node->value), sizeof(char));
        sprintf(str, "%s?\n", (char *) node->value);
        printw("%s", str);
        refresh();
        say(str, ALENA, NEUTRAL);

        ans = askYesNo();

        if (ans)
            akinatorPlayNode(node->right);
        else
            akinatorPlayNode(node->left);
    }

}