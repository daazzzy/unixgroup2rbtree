#include "stdio.h"
#include "../queue.h"
#include "../tree.h"
#include "xxhash.h"
#include "stdlib.h"
#include "string.h"


//TODO
// Take the students full names from the 
// UNIX General Channel and store them in RBTree (Red-Black) 
// with the key being their first name hash. 
// There obviously might be duplicates so in 
// that case you need a queue attached to a tree
// node to store the duplicates.
// The implementation must be generic enough and not assume the maximum length of 
// the name/surname nor the number of names to store in the tree.

// PLANNER
// 1. open names files. DONE
// 2. cut everything after first name. DONE
// 3. hash first names. DONE
// 4. create rbtree.
// 5. populate rbtree.
typedef unsigned int __uintptr_t;

typedef struct node {
    RB_ENTRY(node) entry;
    char *name;
};

int compare_names_hash(struct node *a, struct node *b){
    uint64_t hashA = XXH64(a->name, strlen(a->name), 0);
    uint64_t hashB = XXH64(b->name, strlen(b->name), 0);

    if (hashA < hashB){
        return -1;
    }else if (hashA > hashB){
        return 1;
    }else {
        return 0;
    }
}

RB_HEAD(studtree, node) head = RB_INITIALIZER(&head);
RB_GENERATE(studtree, node, entry, compare_names_hash)

void print_tree(struct node *n) {
    struct node *left, *right;

    if (n == NULL) {
        printf("nil");
        return;
    }
    left = RB_LEFT(n, entry);
    right = RB_RIGHT(n, entry);

    if(left == NULL && right == NULL){
        printf("%d", n -> name);
    }else {
        printf("%d\n (", n->name);
		print_tree(left);
        printf(",");
        print_tree(right);
        printf(")");
    }
}

int main() {
    FILE *fp = fopen("../teamsarray.txt", "r");
    if(fp == NULL) {
        printf("Error in opening file.\n");
        exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int i;
    struct node *n;

    while((read = getline(&line, &len, fp)) != -1){
        char *first_name = strtok(line, " \t\n");
        uint64_t hash = XXH64(first_name, strlen(first_name), 0);
        n = malloc(sizeof(struct node));
        n->name = malloc(strlen(first_name) + 1);
        strcpy(n->name, first_name);
        RB_INSERT(studtree, &head, n);
    }

    print_tree(RB_ROOT(&head));

    free(n);
    fclose(fp);
    exit(EXIT_SUCCESS);
}