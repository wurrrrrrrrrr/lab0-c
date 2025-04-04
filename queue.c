#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

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

        if ((strcmp(node1->value, node2->value) <= 0) ^ descend) {
            temp->next = l1;
            temp = temp->next;
            l1 = l1->next;
        } else {
            temp->next = l2;
            temp = temp->next;
            l2 = l2->next;
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

/* linux list_sort
 * order */

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif


typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

static struct list_head *list_merge(void *priv,
                                    list_cmp_func_t cmp,
                                    struct list_head *a,
                                    struct list_head *b);

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *l1,
                        struct list_head *l2);


int cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    element_t *node_a = container_of(a, element_t, list);
    element_t *node_b = container_of(b, element_t, list);

    if (node_a->value < node_b->value)
        return -1;
    else if (node_a->value > node_b->value)
        return 1;
    else
        return 0;
}



void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = list_merge(priv, cmp, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);
    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = list_merge(priv, cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(priv, cmp, head, pending, list);
}

static struct list_head *list_merge(void *priv,
                                    list_cmp_func_t cmp,
                                    struct list_head *a,
                                    struct list_head *b)
{
    // cppcheck-suppress unassignedVariable
    struct list_head *head, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned char count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        if (unlikely(!++count))
            cmp(priv, b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}


/* Implement the  Fisher–Yates shuffle algo*/
static void swap(struct list_head *a, struct list_head *b)
{
    element_t *a_entry = list_entry(a, element_t, list);
    element_t *b_entry = list_entry(b, element_t, list);
    char *tmp = b_entry->value;
    b_entry->value = a_entry->value;
    a_entry->value = tmp;
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int size = q_size(head);
    struct list_head *new = head->prev;
    for (int i = size - 1; i > 0; i--) {
        struct list_head *old = head->next;
        int r = rand() % (i + 1);
        while (r > 0) {
            old = old->next;
            r--;
        }
        swap(old, new);
        new = new->prev;
    }
}

/* Implement the list_quicksort*/
struct listitem {
    uint16_t i;
    struct list_head list;
};

static inline int cmpint(const void *p1, const void *p2)
{
    const uint16_t *i1 = (const uint16_t *) p1;
    const uint16_t *i2 = (const uint16_t *) p2;

    return *i1 - *i2;
}

void list_quicksort(struct list_head *head)
{
    struct list_head list_less, list_greater;
    struct listitem *pivot;
    struct listitem *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    pivot = list_first_entry(head, struct listitem, list);
    list_del(&pivot->list);

    list_for_each_entry_safe (item, is, head, list) {
        if (cmpint(&item->i, &pivot->i) < 0)
            list_move_tail(&item->list, &list_less);
        else
            list_move_tail(&item->list, &list_greater);
    }

    list_quicksort(&list_less);
    list_quicksort(&list_greater);

    list_add(&pivot->list, head);
    list_splice(&list_less, head);
    list_splice_tail(&list_greater, head);
}