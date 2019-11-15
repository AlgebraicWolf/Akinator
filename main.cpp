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

const size_t MAX_VOICE_SIZE = 10;
const size_t MAX_EMOTION_SIZE = 7;
const size_t MAX_DUMMY_SIZE = 30;

const size_t SUBQUESTION_SIZE = 65;

const size_t MAX_LINE_SIZE = 129;

const char *DECISION_TREE_PATH = "decisionTreeSerialized.txt";

enum EMOTIONS {
    GOOD,
    EVIL,
    NEUTRAL
};

size_t getFilesize(FILE *f);

char *loadFile(const char *filename);

void *deserializePrompt(char *ser);

void say(const char *prompt, VOICES voice, EMOTIONS emotion);

void akinatorPlayNode(tree_t *decisionTree, node_t *node);

int askMode();

void sayAndPrintw(char *str);

void drawDecisionTree(tree_t *tree, char *filename);

int askArbitraryMode(const char **modes, int numberOfModes);

char **getObjects(tree_t decisionTree);

int main() {
    setlocale(LC_ALL, "");
    initscr();
    char *serializedDT = loadFile(DECISION_TREE_PATH);
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
                akinatorPlayNode(decisionTree, decisionTree->head);
                break;

            case PRINT:
                drawDecisionTree(decisionTree, "dt.dot");
                addch('\n');
                sayAndPrintw("Дерево решений в формате DOT сохранено в файл dt.dot. Что дальше?\n");
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

char **getObjects(tree_t *tree, int *num) {
    assert(tree);

    char **objs = (char **) calloc(tree->size, sizeof(char *));
}

void sayAndPrintw(char *str) {
    printw("%s", str);
    refresh();
    say(str, ALENA, NEUTRAL);
}

void *deserializePrompt(char *ser) {
    return ser;
}

char *serializePrompt(void *ser) {
    return (char *) ser;
}

int askArbitraryMode(const char **modes, int numberOfModes) {
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

        if (pos == -1) pos = numberOfModes - 1;
        else if (pos == numberOfModes) pos = 0;


        wmove(stdscr, y, x);
        for (int i = 0; i < numberOfModes; i++) {
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

void addSplitRoutine(tree_t *decisionTree, node_t *node) {
    char answerWrapper[2 * MAX_LINE_SIZE] = "";
    char *ans = (char *) calloc(MAX_LINE_SIZE, sizeof(char));
    char *diff = (char *) calloc(MAX_LINE_SIZE, sizeof(char));

    int correct = false;
    while (!correct) {
        sayAndPrintw("Что вы загадали?\n");
        refresh();
        curs_set(1);
        printw("Я загадал: ");
        getstr(ans);

        // TODO check whether answer is already in tree
        sprintf(answerWrapper, "Чем %s отличается от %s? Пожалуйста, не используйте в ответе частицу НЕ.\n", ans,
                (char *) node->value);
        sayAndPrintw(answerWrapper);
        refresh();
        curs_set(1);
        getstr(diff);

        sprintf(answerWrapper, "Ваш ответ: %s\n", ans);
        sayAndPrintw(answerWrapper);
        sprintf(answerWrapper, "Если бы вы загадали это, вы бы положительно ответили на вопрос %s, верно?\n", diff);
        sayAndPrintw(answerWrapper);

        correct = askYesNo();
    }
    node_t *newAnswer = makeNode(nullptr, nullptr, nullptr, ans);
    node_t *newNode = makeNode(node->parent, node, newAnswer, diff);
    if (node->parent->left == node)
        node->parent->left = newNode;
    else
        node->parent->right = newNode;

    node->parent = newNode;
    newAnswer->parent = newNode;

    treeSerialize(decisionTree, (char *) DECISION_TREE_PATH, serializePrompt);

    sayAndPrintw("База данных успешно пополнена! Спасибо!\n");
}

void akinatorPlayNode(tree_t *decisionTree, node_t *node) {
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
        } else {
            addSplitRoutine(decisionTree, node);
        }

    } else {
        char *str = (char *) calloc(SUBQUESTION_SIZE + strlen((char *) node->value), sizeof(char));
        sprintf(str, "%s?\n", (char *) node->value);
        printw("%s", str);
        refresh();
        say(str, ALENA, NEUTRAL);

        ans = askYesNo();

        if (ans)
            akinatorPlayNode(decisionTree, node->right);
        else
            akinatorPlayNode(decisionTree, node->left);
    }

}

void printNode(node_t *node, FILE *dotFile) {
    fprintf(dotFile, "node%p [label=\"%s%c\", shape=box];\n", node, (char *) node->value,
            node->left && node->right ? '?' : ' ');
}

void drawNode(node_t *node, FILE *dotFile) {
    assert(node);
    assert(dotFile);

    if (node->left && node->right) {
        printNode(node->left, dotFile);
        printNode(node->right, dotFile);

        fprintf(dotFile, "node%p -> node%p [color=\"red\", label=\"Нет\"];\n", node, node->left);
        fprintf(dotFile, "node%p -> node%p [color=\"green\", label=\"Да\"];\n", node, node->right);

        drawNode(node->right, dotFile);
        drawNode(node->left, dotFile);
    }

}

void drawDecisionTree(tree_t *tree, char *filename) {
    assert(tree);
    assert(filename);

    FILE *dotFile = fopen(filename, "w");
    fprintf(dotFile, "digraph {\n");

    printNode(tree->head, dotFile);
    drawNode(tree->head, dotFile);

    fprintf(dotFile, "}\n");
    fclose(dotFile);
}