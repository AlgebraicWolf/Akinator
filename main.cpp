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

const size_t MAX_LINE_SIZE = 256;

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

int askArbitraryMode(char **modes, int numberOfModes);

char **getValues(node_t **nodes, int num);

node_t **getObjects(tree_t *decisionTree, int *counter);

void sayAndPrintwArray(char **lines, int count);

void showDifference(tree_t *tree, node_t *node1, node_t *node2);

int gameMaster(tree_t *decisionTree);

int main() {
    setlocale(LC_ALL, "");
    initscr();
    char *serializedDT = loadFile(DECISION_TREE_PATH);
    tree_t *decisionTree = treeDeserialize(serializedDT, deserializePrompt);

    sayAndPrintw("Привет! Пожалуйста, выбери режим работы:\n");

//
//    node_t **nodes = nullptr;
//    char **names = nullptr;
//    int count = 0;
//    int mode = 0;
//    int y = 0;
//    int x = 0;
//    int ans1 = 0;
//    int ans2 = 0;
//    while ((mode = askMode()) != 3) {
//        switch (mode) { // TODO separate into functions
//            case PLAY:
//                akinatorPlayNode(decisionTree, decisionTree->head);
//                break;
//
//            case PRINT:
//                drawDecisionTree(decisionTree, "dt.dot");
//                sayAndPrintw("Дерево решений в формате DOT сохранено в файл dt.dot. Что дальше?\n");
//                break;
//
//            case COMPARE:
//                sayAndPrintw("Список доступных для сравнения объектов:\n");
//                count = 0;
//                nodes = getObjects(decisionTree, &count);
//                names = getValues(nodes, count);
//                sayAndPrintwArray(names, count);
//                sayAndPrintw("Выберите первый объект:\n");
//                ans1 = askArbitraryMode(names, count);
//                sayAndPrintw("Выберите второй объект:\n");
//                ans2 = askArbitraryMode(names, count);
//                showDifference(decisionTree, nodes[ans1], nodes[ans2]);
//                break;
//
//            default:
//                say("Простите, но эта фича не имплементирована", ALENA, NEUTRAL);
//                getyx(stdscr, y, x);
//                move(y, 1);
//                break;
//        }
//        clear();
//    }

    while(gameMaster(decisionTree)) {
        clear();
    }

    sayAndPrintw("Всего доброго!\n");
    endwin();

    deleteTree(decisionTree);
    free(serializedDT);
    return 0;
}

int gameMaster(tree_t *decisionTree) {
    assert(decisionTree);

    node_t **nodes = nullptr;
    char **names = nullptr;
    int count = 0;
    int y = 0;
    int x = 0;
    int ans1 = 0;
    int ans2 = 0;

    int mode = askMode();
    switch (mode) { // TODO separate into functions
        case PLAY:
            akinatorPlayNode(decisionTree, decisionTree->head);
            break;

        case PRINT:
            drawDecisionTree(decisionTree, "dt.dot");
            sayAndPrintw("Дерево решений в формате DOT сохранено в файл dt.dot. Что дальше?\n");
            break;

        case COMPARE:
            sayAndPrintw("Список доступных для сравнения объектов:\n");
            count = 0;
            nodes = getObjects(decisionTree, &count);
            names = getValues(nodes, count);
            sayAndPrintwArray(names, count);
            sayAndPrintw("Выберите первый объект:\n");
            ans1 = askArbitraryMode(names, count);
            sayAndPrintw("Выберите второй объект:\n");
            ans2 = askArbitraryMode(names, count);
            showDifference(decisionTree, nodes[ans1], nodes[ans2]);
            break;

        case EXIT:
            return false;

        default:
            say("Простите, но эта фича не имплементирована", ALENA, NEUTRAL);
            getyx(stdscr, y, x);
            move(y, 1);
            break;
    }
    return true;
}

node_t **findNullptr(node_t **objs) {
    while (*objs) objs++;
    return objs;
}

int nodeDepth(tree_t *tree, node_t *node) {
    assert(tree);
    assert(node);

    int depth = 0;
    while (node != tree->head) {
        depth++;
        node = node->parent;
    }

    return depth;
}

node_t *
findLCA(tree_t *tree, node_t *node1, node_t *node2, unsigned long long *node1Path, unsigned long long *node2Path) {
    assert(tree);
    assert(node1);
    assert(node2);

    int depth1 = nodeDepth(tree, node1);
    int depth2 = nodeDepth(tree, node2);

    *node1Path = 0;
    *node2Path = 0;

    while (depth1 != depth2) {
        if (depth1 > depth2) {
            depth1--;

            *node1Path = (*node1Path) << 1;
            if (node1 == node1->parent->right)
                (*node1Path)++;

            node1 = node1->parent;
        } else {
            depth2--;

            *node2Path = (*node2Path) << 1;
            if (node2 == node2->parent->right)
                (*node2Path)++;

            node2 = node2->parent;
        }
    }

    while (node1 != node2) {
        *node1Path = (*node1Path) << 1;
        if (node1 == node1->parent->right)
            (*node1Path)++;

        node1 = node1->parent;

        *node2Path = (*node2Path) << 1;
        if (node2 == node2->parent->right)
            (*node2Path)++;

        node2 = node2->parent;
    }
    return node1;
}

void enumerateProperties(node_t *lca, node_t *node, unsigned long long path) {
    char textBuf[MAX_LINE_SIZE] = "";
    while (lca != node) {
        if (path % 2 == 0) {
            sprintf(textBuf, "не %s ", lca->value);
            lca = lca->left;
        } else {
            sprintf(textBuf, "%s ", lca->value);
            lca = lca->right;
        }
        path = path >> 1;
        sayAndPrintw(textBuf);
    }
    addch('\n');
}

void showDifference(tree_t *tree, node_t *node1, node_t *node2) {
    assert(tree);
    assert(node1);
    assert(node2);

    char textBuf[MAX_LINE_SIZE] = "";

    unsigned long long path1 = 0;
    unsigned long long path2 = 0;
    node_t *lca = findLCA(tree, node1, node2, &path1, &path2);
    node_t *tmp = lca;


    if (lca != tree->head) {
        sprintf(textBuf, "%s и %s схожи тем, что они:\n", node1->value, node2->value);
        sayAndPrintw(textBuf);
        while (tmp != tree->head) {
            if (tmp->parent->right == tmp) {
                sprintf(textBuf, "%s\n", tmp->parent->value);
            } else {
                sprintf(textBuf, "Не %s\n", tmp->parent->value);
            }
            sayAndPrintw(textBuf);

            tmp = tmp->parent;
        }
    }

    if (node1 != node2) {
        sprintf(textBuf, "Отличия %s от %s: ", node1->value, node2->value);
        sayAndPrintw(textBuf);

        enumerateProperties(lca, node1, path1);

        sprintf(textBuf, "В свою очередь, отличия %s от %s: ", node2->value, node1->value);
        sayAndPrintw(textBuf);
        enumerateProperties(lca, node2, path2);
    }
}

void addNodetoArray(node_t *node, node_t **objs, int *num) {
    if (!node->left && !node->right) {
        objs = findNullptr(objs);
        *objs = node;
        (*num)++;
    } else {
        addNodetoArray(node->left, objs, num);
        addNodetoArray(node->right, objs, num);
    }
}

node_t **getObjects(tree_t *tree, int *counter) {
    assert(tree);
    *counter = 0;

    node_t **objs = (node_t **) calloc(tree->size / 2 + 1, sizeof(char *));
    addNodetoArray(tree->head, objs, counter);

    return objs;
}

void sayAndPrintwArray(char **lines, int count) {
    for (int i = 0; i < count; i++) {
        sayAndPrintw(lines[i]);
        addch('\n');
    }
}

char **getValues(node_t **nodes, int num) {
    assert(nodes);
    char **names = (char **) calloc(num, sizeof(char *));
    for (int i = 0; i < num; i++) {
        names[i] = (char *) nodes[i]->value;
    }

    return names;
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

int askArbitraryMode(char **modes, int numberOfModes) {
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
        printw("| ");
        for (int i = 0; i < numberOfModes; i++) {
            if (i == pos)
                attron(A_STANDOUT);

            printw(modes[i]);

            if (i == pos)
                attroff(A_STANDOUT);

            printw(" | ");
        }
        refresh();
        ch = wgetch(stdscr);
    }
    mvprintw(y, x, "Вы выбрали: %s", modes[pos]);
    addch('\n');
    keypad(stdscr, false);
    echo();
    return pos;
}

int askMode() {
    char *modes[4] = {"Играть", "Сравнить", "Дерево", "Выйти"};
    int modes_num = 4;
    return askArbitraryMode(modes, modes_num);
}

int askYesNo() {
    char *modes[2] = {"Нет", "Да"};
    int modes_num = 2;
    return askArbitraryMode(modes, modes_num);
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
    decisionTree->size += 2;

    sayAndPrintw("База данных успешно пополнена! Спасибо!\n");
}

void akinatorPlayNode(tree_t *decisionTree, node_t *node) {
    assert(node);
    int ans = -1;
    if (!node->right && !node->left) {
        char *str = (char *) calloc(SUBQUESTION_SIZE + strlen((char *) node->value), sizeof(char));
        sprintf(str, "Вы загадали %s, не так ли?\n", (char *) node->value);
        sayAndPrintw(str);
        ans = askYesNo();
        if (ans) {
            sayAndPrintw("Так и знала! Что будем делать дальше?\n");
        } else {
            addSplitRoutine(decisionTree, node);
        }
        free(str);
    } else {
        char *str = (char *) calloc(SUBQUESTION_SIZE + strlen((char *) node->value), sizeof(char));
        sprintf(str, "%s?\n", (char *) node->value);
        sayAndPrintw(str);

        ans = askYesNo();

        if (ans)
            akinatorPlayNode(decisionTree, node->right);
        else
            akinatorPlayNode(decisionTree, node->left);
        free(str);
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