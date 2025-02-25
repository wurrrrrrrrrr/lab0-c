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
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node = head->next;
    struct list_head *safe = head->next->next;
    while (head != node) {
        node->next = node->prev;
        node->prev = safe;
        node = safe;
        safe = safe->next;
    }
    struct list_head *temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

/* Reverse the nodes of the list k at a time
    } */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    int times = q_size(head) / k;
    struct list_head *tail;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    for (int i = 0; i < times; i++) {
        int j = 0;
        list_for_each (tail, head) {
            if (j >= k)
                break;
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);
    }
    list_splice_init(&new_head, head);
}

struct list_head *merge(struct list_head *l1,
                        struct list_head *l2,
                        bool descend)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;


    struct list_head dummy;
    struct list_head *temp = &dummy;
    dummy.next = NULL;
    dummy.prev = NULL;

    while (l1 && l2) {
        element_t *node1 = list_entry(l1, element_t, list);
        element_t *node2 = list_entry(l2, element_t, list);

        if (!descend) {
            if (strcmp(node1->value, node2->value) <= 0) {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            } else {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            }
        } else {
            if (strcmp(node1->value, node2->value) < 0) {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            } else {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            }
        }
    }

    if (l1)
        temp->next = l1;
    if (l2)
        temp->next = l2;

    return dummy.next;
}

struct list_head *mergeSortList(struct list_head *head, bool descend)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;


    // sort each list
    struct list_head *l1 = mergeSortList(head, descend);
    struct list_head *l2 = mergeSortList(fast, descend);

    // merge sorted l1 and sorted l2
    return merge(l1, l2, descend);
}


void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *temp = head;
    head->prev->next = NULL;
    head->next = mergeSortList(head->next, descend);

    struct list_head *curr = head->next;

    while (curr->next != NULL) {
        curr->prev = temp;
        temp = curr;
        curr = curr->next;
    }
    curr->prev = temp;
    curr->next = head;
    head->prev = curr;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return 0;
    }


    char *tmp_max = list_entry(head->next, element_t, list)->value;
    struct list_head *cur = head->next->next;


    while (cur != head) {
        element_t *cur_node = list_entry(cur, element_t, list);
        char *tmp1 = cur_node->value;

        if (strcmp(tmp1, tmp_max) > 0) {
            tmp_max = tmp1;
            cur = cur->next;
        } else {
            cur = cur->next;
            element_t *free_node = list_entry(cur->prev, element_t, list);
            list_del(&free_node->list);
            free(free_node->value);
            free(free_node);
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return 0;
    }


    char *tmp_max = list_entry(head->prev, element_t, list)->value;
    struct list_head *cur = head->prev->prev;


    while (cur != head) {
        element_t *cur_node = list_entry(cur, element_t, list);
        char *tmp1 = cur_node->value;

        if (strcmp(tmp1, tmp_max) > 0) {
            tmp_max = tmp1;
            cur = cur->prev;
        } else {
            cur = cur->prev;
            element_t *free_node = list_entry(cur->next, element_t, list);
            list_del(&free_node->list);
            free(free_node->value);
            free(free_node);
        }
    }

    return q_size(head);
}
/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    queue_contex_t *node = list_entry(head->next, queue_contex_t, chain);
    node->q->prev->next = NULL;
    struct list_head *temp = node->chain.next;
    for (int i = 0; i < ((node->size) - 1); i++) {
        queue_contex_t *next_node = list_entry(temp, queue_contex_t, chain);
        next_node->q->prev->next = NULL;
        node->q->next = merge(node->q->next, next_node->q->next, descend);
        INIT_LIST_HEAD(next_node->q);
        temp = temp->next;
    }

    struct list_head *curr = node->q->next;
    struct list_head *pre = node->q;
    while (curr->next != NULL) {
        curr->prev = pre;
        pre = curr;
        curr = curr->next;
    }
    curr->prev = pre;
    curr->next = node->q;
    node->q->prev = curr;

    return q_size(node->q);
}
