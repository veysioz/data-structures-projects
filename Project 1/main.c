#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
    char word[50];
    int number, frequency, accessTime, rcount, lcount;
    struct node *left, *right, *next;
} node;

node * newNode(char*, int, int, int);
node * insert(node*, char*, int, int, int);
void inputToTree();
char * toLowerString(char*);
void printTree(node*);
node * sortedInsertLL(node*, node*);
void lltoTree();
node * insertLevelOrder(node*, char*, int, int, int);
int isTrue(int);

node *treeA = NULL, *linkedList = NULL, *treeB = NULL;
FILE *file;
int treeNodeCounter = 0, totalAccss = 0, accssTime = 0;

int main() {
    inputToTree();
    printf("PROBLEM A \n\n(number - word - frequency - access time)\n");
    printTree(treeA);
    printf("\n\nPROBLEM B\n\n");
    printf("Total Access Time = Frequency * Access Time | (Total of All Nodes)\n");
    printf("Total Access Time = %d\n", totalAccss);
    totalAccss = 0;
    treeNodeCounter = 0;
    lltoTree();
    treeNodeCounter = 0;
    printf("\nPROBLEM C\n\n(number - word - frequency - access time)\n");
    printTree(treeB);
    printf("\n\nPROBLEM D\n\n");
    printf("Total Access Time for New Tree = %d\n", totalAccss);
    printf("\nPROBLEM E\n\n");
    printf("In order to reduce the total access time, we need to reduce the access times of nodes with high frequency values. ");
    printf("For this, I kept the frequency values in the linked list in descending order. ");
    printf("Then I have minimized the total access time by adding the frequencies in the linked list to a new tree sequentially.\n");
    return 0;
}

node * newNode(char *word, int number, int frequency, int accessTime) {
    node *temp = (node*)malloc(sizeof(node));
    strcpy(temp->word, word);
    temp->number = number;
    temp->frequency = frequency;
    temp->accessTime = accessTime;
    temp->right = NULL;
    temp->left = NULL;
    temp->next = NULL;
    temp->rcount = 0;
    temp->lcount = 0;
}

node * insert(node *node, char *word, int number, int frequency, int accessTime) {
    if(node == NULL)
        return newNode(word, number, frequency, accessTime);

    if(strcmp(word, node->word) < 0)
        node->left = insert(node->left, word, number, frequency, ++accessTime);
    else if(strcmp(word, node->word) > 0)
        node->right = insert(node->right, word, number, frequency, ++accessTime);

    return node;
}

void inputToTree() {
    char line[50], word[50], *token;
    int counter, number, frequency;

    file = fopen("input.txt", "r");

    while(fgets(line, sizeof(line), file)) {
        counter = 1;
        token = strtok(line, ",\n");
        while(token != NULL) {
            if(counter == 1)
                number = atoi(token);
            else if(counter == 2)
                strcpy(word, toLowerString(token));
            else if(counter == 3)
                frequency = atoi(token);

            token = strtok(NULL, ",\n");
            counter++;
        }
        treeA = insert(treeA, word, number, frequency, 1);
    }
    fclose(file);
}

char * toLowerString(char * str) {
    int i;
    for(i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

void printTree(node *node) {
    if(node == NULL)
        return;

    printTree(node->left);
    printf("%d) %d-%s-%d-%d | ", ++treeNodeCounter, node->number, node->word, node->frequency, node->accessTime);
    totalAccss += node->accessTime * node->frequency;
    linkedList = sortedInsertLL(linkedList, node);
    printTree(node->right);
}

node * sortedInsertLL(node *node, struct node *x) {
    if(node == NULL)
        return x;
    else if(node->frequency < x->frequency) {
        x->next = node;
        return x;
    }

    struct node *iter = node;
    while(iter->next != NULL && iter->next->frequency > x->frequency)
        iter = iter->next;

    x->next = iter->next;
    iter->next = x;
    return node;
}

void lltoTree() {
    while(linkedList != NULL) {
        treeB = insertLevelOrder(treeB, linkedList->word, linkedList->number, linkedList->frequency, 1);
        linkedList = linkedList->next;
    }
}

node * insertLevelOrder(node *node, char *word, int number, int frequency, int accessTime) {
    if(node == NULL) {
        if(isTrue(treeNodeCounter))
            accssTime++;
        treeNodeCounter++;
        return newNode(word, number, frequency, accssTime);
    }

    if(node->rcount == node->lcount) {
        node->left = insertLevelOrder(node->left, word, number, frequency, accessTime);
        node->lcount++;
    } else if(node->rcount < node->lcount) {
        if(isTrue(node->lcount)) {
            node->right = insertLevelOrder(node->right, word, number, frequency, accessTime);
            node->rcount++;
        } else {
            node->left = insertLevelOrder(node->left, word, number, frequency, accessTime);
            node->lcount++;
        }
    }
    return node;
}

int isTrue(int count) {
    count++;

    while(count % 2 == 0)
        count /= 2;

    if(count == 1)
        return 1;
    else
        return 0;
}
