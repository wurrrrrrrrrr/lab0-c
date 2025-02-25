#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_qhead =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!new_qhead) {
        return NULL;
    }
    INIT_LIST_HEAD(new_qhead);
    return new_qhead;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *free_node = list_entry(node, element_t, list);
        free(free_node->value);
        free(free_node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_qelement = (element_t *) malloc(sizeof(element_t));
    if (!new_qelement) {
        return false;
    }
    new_qelement->value = strdup(s);
    if (!new_qelement->value) {
        free(new_qelement);
        return false;
    }
    list_add(&new_qelement->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    return q_insert_head(head->prev, s);
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    if (sp) {
        memset(sp, '\0', bufsize);
        strncpy(sp, container_of(head->next, element_t, list)->value,
                bufsize - 1);
    }
    element_t *del_element_t = container_of(head->next, element_t, list);
    list_del(head->next);
    return del_element_t;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    if (sp) {
        memset(sp, '\0', bufsize);
        strncpy(sp, container_of(head->prev, element_t, list)->value,
                bufsize - 1);
    }
    element_t *del_element_t = container_of(head->prev, element_t, list);
    list_del(head->prev);
    return del_element_t;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *first = head->next;
    struct list_head *last = head->prev;
    while (first != last && first->next != last) {
        first = first->next;
        last = last->prev;
    }
    list_del(last);
    free(list_entry(last, element_t, list)->value);
    free(list_entry(last, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *node, *safe;
    bool dup = false;
    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        if (safe != head) {
            element_t *tmp = list_entry(safe, element_t, list);
            if (!strcmp(cur->value, tmp->value)) {
                list_del(node);
                free(cur->value);
                free(cur);
                dup = true;
            } else if (strcmp(cur->value, tmp->value) && dup) {
                list_del(node);
                free(cur->value);
                free(cur);
                dup = false;
            }
        } else {
            if (dup) {
                list_del(node);
                free(cur->value);
                free(cur);
                dup = false;
            }
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *l1 = head->next;
    struct list_head *l2 = head->next->next;
    while (l1 != head && l2 != head) {
        list_move(l1, l2);
        l1 = l1->next;
        l2 = l1->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
