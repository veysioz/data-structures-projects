#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

typedef struct words {
    char word[50];
    struct words *next;
} words;

typedef struct files {
    char fileName[50];
    int matches;
    int degree;
    struct files *next;
    words *words;
    struct files *parent;
    struct files *child;
    struct files *sibling;
} files;

void searchMatches();
words * newWord(char*);
void insertWord(char *word);
files * newFile(words*, char*);
void insertFile(words*, char*);
void getFiles();
void enqueue(files*);
files * dequeue(files*);
int revert(files*);

FILE *file;
DIR *d;
words *w = NULL;
files *f = NULL, *root = NULL, *Hr = NULL;
char searchedWord[50];
int numRelDoc = 0;

int main()
{
    getFiles();

    printf("Enter the word: ");
    scanf("%s", &searchedWord);

    searchMatches();

    printf("\nNUMBER OF RELEVANT DOCUMENTS: %d", numRelDoc);

    files *i = f;
    while(i != NULL) {
        enqueue(i);
        i = i->next;
    }

    files *p;
    p = dequeue(root);
    p->next = dequeue(root);
    p->next->next = dequeue(root);
    p->next->next->next = dequeue(root);
    p->next->next->next->next = dequeue(root);
    p->next->next->next->next->next = NULL;
    i = p;
    printf("\n\nThe relevance order is(for five): ");
    while(i != NULL) {
        if(i->matches == 0)
            break;
        printf("%s(%d) ", i->fileName, i->matches);
        i = i->next;
    }
    i = p;
    while(i != NULL) {
        if(i->matches == 0)
            break;
        printf("\n\n%s(%d): ", i->fileName, i->matches);
        words *w = i->words;
        while(w != NULL) {
            printf("%s ", w->word);
            w = w->next;
        }
        i = i->next;
    }
    printf("\n");

    return 0;
}

void searchMatches() {
    int matches = 0;

    files *i = f;
    while(i != NULL) {
        w = i->words;
        while(w != NULL) {
            if(strcmp(searchedWord, w->word) == 0)
                matches++;
            w = w->next;
        }
        if(matches > 0 ) numRelDoc++;
        i->matches = matches;
        matches = 0;
        i = i->next;
    }
}

words * newWord(char *word) {
    words *temp = (words*)malloc(sizeof(words));
    strcpy(temp->word, word);
    temp->next = NULL;
    return temp;
}

void insertWord(char *word) {
    if(w == NULL)
        w = newWord(word);
    else {
        words *i = w;
        while(i->next != NULL)
            i = i->next;
        i->next = newWord(word);
    }
}

files * newFile(words *words, char *fileName) {
    files *temp = (files*)malloc(sizeof(files));
    temp->matches = NULL;
    temp->next = NULL;
    temp->words = words;
    strcpy(temp->fileName, fileName);
    return temp;
}

void insertFile(words *words, char *fileName) {
    if(f == NULL)
        f = newFile(w, fileName);
    else {
        files *i = f;
        while(i->next != NULL)
            i = i->next;
        i->next = newFile(words, fileName);
    }
}

void getFiles() {
    struct dirent *dir;
    d = opendir("files");
    char location[50], ch, word[50];

    if(d) {
        while((dir = readdir(d)) != NULL) {
            if(strchr(dir->d_name, '.') == NULL) {
                sprintf(&location, "files\\%s", dir->d_name);
                file = fopen(location, "r");
                while((ch = fgetc(file)) != EOF) {
                    if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '\'')
                        sprintf(&word, "%s%c", word, ch);
                    else {
                        if(word[0] != '\0') {
                            insertWord(word);
                            word[0] = '\0';
                        }
                    }
                }
                fclose(file);
                insertFile(w, dir->d_name);
                w = NULL;
            }
        }
        closedir(d);
    }
}

void enqueue(files *temp) {
    temp->parent = NULL;
    temp->child = NULL;
    temp->sibling = NULL;
    temp->degree = 0;
    files *prev_x, *next_x, *x, *H, *y = root, *z = temp, *a, *b;

    if (y != NULL) {
        if (z != NULL && y->degree <= z->degree)
            H = y;
        else if (z != NULL && y->degree > z->degree)
            H = z;
        else
            H = y;
    } else
        H = z;

    while (y != NULL && z != NULL) {
        if (y->degree < z->degree) {
            y = y->sibling;
        } else if (y->degree == z->degree) {
            a = y->sibling;
            y->sibling = z;
            y = a;
        } else {
            b = z->sibling;
            z->sibling = y;
            z = b;
        }
    }
    if (H == NULL) {
        root = H;
        return;
    }
    prev_x = NULL;
    x = H;
    next_x = x->sibling;
    while (next_x != NULL) {
        if ((x->degree != next_x->degree) || ((next_x->sibling != NULL)
                && (next_x->sibling)->degree == x->degree)) {
            prev_x = x;
            x = next_x;
        } else {
            if (x->matches >= next_x->matches) {
                x->sibling = next_x->sibling;
                next_x->parent = x;
                next_x->sibling = x->child;
                x->child = next_x;
                x->degree = x->degree + 1;
            } else {
                if (prev_x == NULL)
                    H = next_x;
                else
                    prev_x->sibling = next_x;
                x->parent = next_x;
                x->sibling = next_x->child;
                next_x->child = x;
                next_x->degree = next_x->degree + 1;
                x = next_x;
            }
        }
        next_x = x->sibling;
    }
    root = H;
}

files * dequeue(files *H1) {
    int max;
    files *t = NULL, *x = H1, *p;
    Hr = NULL;
    if (x == NULL) {
        printf("\nNOTHING TO EXTRACT");
        return x;
    }
    p = x;
    max = p->matches;
    while (p->sibling != NULL) {
        if ((p->sibling)->matches > max) {
            max = (p->sibling)->matches;
            t = p;
            x = p->sibling;
        }
        p = p->sibling;
    }
    if (t == NULL && x->sibling == NULL)
        H1 = NULL;
    else if (t == NULL)
        H1 = x->sibling;
    else if (t->sibling == NULL)
        t = NULL;
    else
        t->sibling = x->sibling;
    if (x->child != NULL) {
        revert(x->child);
        (x->child)->sibling = NULL;
    }

    files *prev_x, *next_x, *m, *H = NULL, *y = H1, *z = Hr, *a, *b;
    if (y != NULL) {
        if (z != NULL && y->degree <= z->degree)
            H = y;
        else if (z != NULL && y->degree > z->degree)
            H = z;
        else
            H = y;
    } else
        H = z;
    while (y != NULL && z != NULL) {
        if (y->degree < z->degree) {
            y = y->sibling;
        } else if (y->degree == z->degree) {
            a = y->sibling;
            y->sibling = z;
            y = a;
            break;
        } else {
            b = z->sibling;
            z->sibling = y;
            z = b;
            y->sibling = z;
            break;
        }
    }
    if (H == NULL) {
        root = H;
        return x;
    }
    prev_x = NULL;
    m = H;
    next_x = m->sibling;
    while (next_x != NULL) {
        if ((m->degree != next_x->degree) || ((next_x->sibling != NULL)
                && (next_x->sibling)->degree == m->degree)) {
            prev_x = m;
            m = next_x;
        } else {
            if (m->matches >= next_x->matches) {
                m->sibling = next_x->sibling;
                next_x->parent = m;
                next_x->sibling = m->child;
                m->child = next_x;
                m->degree = m->degree + 1;
            } else {
                if (prev_x == NULL)
                    H = next_x;
                else
                    prev_x->sibling = next_x;
                m->parent = next_x;
                m->sibling = next_x->child;
                next_x->child = m;
                next_x->degree = next_x->degree + 1;
                m = next_x;
            }
        }
        next_x = m->sibling;
    }
    root = H;
    return x;
}

int revert(files *y) {
    if (y->sibling != NULL) {
        revert(y->sibling);
        (y->sibling)->sibling = y;
    } else {
        Hr = y;
    }
}
