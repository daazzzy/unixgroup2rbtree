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
// 2. extract only first names. DONE
// 3. hash first names. DONE
// 4. INIT rbtree. DONE
// 5. insert hashed values to rbtree. DONE
// 6. create red-black balancing
// 7. queue duplicates.
// 8. free up memory.

typedef unsigned int __uintptr_t;

typedef struct node {
    RB_ENTRY(node) entry;
    char *name;
    TAILQ_HEAD(dup_head, node) duplicates;
    TAILQ_ENTRY(node) dup_node;
};

int compare_names_hash(struct node *a, struct node *b){
    uint64_t ah = XXH64(a->name, strlen(a->name), 0);
    uint64_t bh = XXH64(b->name, strlen(b->name), 0);

    if (ah < bh){
        return -1;
    }else if (ah > bh){
        return 1;
    }else {
        return 0;
    }
}

RB_HEAD(stud_tree, node) head = RB_INITIALIZER(&head);
RB_GENERATE(stud_tree, node, entry, compare_names_hash)

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
    }else{
        printf("%d\n", n->name);
		print_tree(left);
        printf(",");
        print_tree(right);
        printf(")");
    }

}

// CHATGPT GENERATED TREE PRINT:
// void print_tree(struct node *n, int depth) {
//     if (n == NULL) {
//         // print "nil" for null nodes
//         printf("%*s|------- nil\n", depth * 2, "");
//         return;
//     }

//     // print the current node with indentation and vertical bars
//     if (RB_PARENT(n, entry) != NULL && RB_LEFT(RB_PARENT(n, entry), entry) == n) {
//         // current node is the left child of its parent
//         printf("%*s|------- %d\n", depth * 2, "", n->name);
//     } else if (RB_PARENT(n, entry) != NULL && RB_RIGHT(RB_PARENT(n, entry), entry) == n) {
//         // current node is the right child of its parent
//         printf("%*s\\------- %d\n", depth * 2, "", n->name);
//     } else {
//         // current node is the root
//         printf("%d\n", n->name);
//     }

//     // recursively print the left and right subtrees
//     print_tree(RB_LEFT(n, entry), depth + 1);
//     print_tree(RB_RIGHT(n, entry), depth + 1);
// }

int main() {
    FILE *fp = fopen("../teamsarray.txt", "r");
    struct stud_tree node;
    RB_INIT(&node);
    if(fp == NULL) {
        printf("Error in opening file.\n");
        exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    struct node *n;

    while(getline(&line, &len, fp) != -1) {
        char *first_name = strtok(line, " \t\n");
        //printf("%s \n", first_name);
        uint64_t hash = XXH64(first_name, strlen(first_name), 0);
        n = malloc(sizeof(struct node));
        n->name = malloc(strlen(first_name) + 1);
        strcpy(n->name, first_name);
        struct node *dup_stud = RB_INSERT(stud_tree, &head, n);
        if(dup_stud != NULL) {
            TAILQ_INSERT_TAIL(&dup_stud->duplicates, n, dup_node);
        } else {
            TAILQ_INIT(&n->duplicates);
        }
    }
    RB_FOREACH(n, stud_tree, &head){
        // printf("%d\n", n->name);
        struct node *dup_iterator;
        TAILQ_FOREACH(dup_iterator, &n->duplicates, dup_node){
            printf("%d, ", dup_iterator->name);
        }
    }
    printf("\n");
    printf("stud_tree:\n");
    print_tree(RB_ROOT(&head));

    // struct node *var;
    // for (var = RB_MIN(n, &head); var != NULL; var = n->name){
    //     var = RB_NEXT(n, &head, var);
    //     RB_REMOVE(n, &head, var);
    //     free(var);
    // }
    fclose(fp);
    exit(EXIT_SUCCESS);
}