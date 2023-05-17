#include <stdio.h>
#include "../queue.h"
#include "../tree.h"
#include <xxhash.h>
#include <stdlib.h>
#include <string.h>


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
// 6. create red-black balancing. ????
// 7. queue duplicates. DONE
// 8. fix memory leaks.

typedef unsigned int __uintptr_t;
typedef struct node {
    RB_ENTRY(node) entry;
    char *name;
    uint64_t hash_key;
    TAILQ_HEAD(dup_head, node) duplicates;
    TAILQ_ENTRY(node) dup_node;
} node_t;

int compare_names_hash(struct node *a, struct node *b){
    if (a->hash_key < b->hash_key){
        return -1;
    }else if (a->hash_key > b->hash_key){
        return 1;
    }else {
        return 0;
    }
}

RB_HEAD(stud_tree, node) head = RB_INITIALIZER(&head);
RB_GENERATE(stud_tree, node, entry, compare_names_hash)

void print_tree(struct node *n) {
    node_t *left, *right;
    
    if (n == NULL) {
        printf("nil");
        return;
    }

    left = RB_LEFT(n, entry);
    right = RB_RIGHT(n, entry);
    if (left == NULL && right == NULL) {
        printf("%lu", n->hash_key);
    }else{
        printf("%lu (", n->hash_key);
        node_t *dup_iterator;
        TAILQ_FOREACH(dup_iterator, &n->duplicates, dup_node) {
            printf("%lu, ", dup_iterator->hash_key);
        }
        printf(")");
    }

    printf(", left: ");
    print_tree(left);
    printf(", right: ");
    print_tree(right);

}
// total heap usage: 178 allocs, 134 frees, 12,639 bytes allocated.
void free_memory(node_t *n){
    if(n == NULL){
        return;
    }
    
    node_t *left = RB_LEFT(n, entry);
    node_t *right = RB_RIGHT(n, entry);

    TAILQ_HEAD(dup_head, node) * duplicates = &n->duplicates;
    node_t *dup_node;
    while ((dup_node = TAILQ_FIRST(duplicates)) != NULL){
        TAILQ_REMOVE(duplicates, dup_node, dup_node);
        free(dup_node->name);
        free(dup_node);
    }

    RB_REMOVE(stud_tree, &head, n);
    free(n->name);
    free(n);

    free_memory(left);
    free_memory(right);
}

int main() {
    FILE *fp = fopen("../teamsarray.txt", "r");
    if(fp == NULL) {
        printf("Error in opening file.\n");
        exit(1);
    }

    RB_INIT(&head);

    char *line = NULL;
    size_t len = 0;
    node_t *n;

    while(getline(&line, &len, fp) != -1) {
        char *first_name = strtok(line, " \t\n");
        printf("Name: %s\n", first_name);

        n = malloc(sizeof(node_t));
        n->name = malloc(strlen(first_name) + 1);
        strcpy(n->name, first_name);
        
        // uint64_t hash 
        n->hash_key = XXH64(n->name, strlen(n->name), 0);
        printf("Hash: %lu\n", n->hash_key);

        node_t *dup_stud = RB_FIND(stud_tree, &head, n);
        if(dup_stud != NULL) {
            TAILQ_INSERT_TAIL(&dup_stud->duplicates, n, dup_node);
        } else {
            TAILQ_INIT(&n->duplicates);
            RB_INSERT(stud_tree, &head, n);
        }
    }

  
    // RB_FOREACH(n, stud_tree, &head){
    // //     printf("%u\n", n->name);
    // //     node_t *dup_iterator;
    // //     TAILQ_FOREACH(dup_iterator, &n->duplicates, dup_node){
    // //         printf(", %lu ", dup_iterator->name);
    // //     }
    // //     printf("\n");
    // }

    printf("stud_tree:\n");
    print_tree(RB_ROOT(&head));
    printf("\n");

    // node_t *var;
    // node_t *nxt;
    // for(var = RB_MIN(stud_tree, &head); var != NULL; var = nxt){
    //     var = RB_NEXT(stud_tree, &head, var);
    //     RB_REMOVE(stud_tree, &head, var);
    //     free(var->name);
    //     free(var);
    //     free(var->hash_key);
    // }

    free_memory(RB_ROOT(&head));

    fclose(fp);
    if (line != NULL) {
        free(line);
    }

    return 0;
}