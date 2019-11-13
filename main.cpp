#include <cstdio>
#include "../Tree/Tree.h"

size_t getFilesize(FILE *f);

char *loadFile(const char *filename);

void *deserializePrompt(char *ser);

int main() {
    char *serializedDT = loadFile("decisionTreeDump.txt");
    tree_t *decisionTree = treeDeserialize(serializedDT, deserializePrompt);


    return 0;
}

void *deserializePrompt(char *ser) {
    return ser;
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