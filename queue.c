#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *h = malloc(sizeof(struct list_head));
    if (h == NULL)
        return NULL;  // it's not enough for Linux. Should consider the over
                      // commit situation

    INIT_LIST_HEAD(h);  // Todo: what about using LIST_HEAD(head)?
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;
    element_t *entry, *tmp;
    list_for_each_entry_safe (entry, tmp, l, list) {
        q_release_element(entry);
    }
    free(l);
}

/* Recurrence version of q_free */
/*
void q_free_recurrence()
{
    swap q_free with q_free_recurrence by adding #define q_free
    q_free_recurrence
}
*/

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *q = malloc(sizeof(element_t));
    if (q == NULL)
        return false;

    q->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (q->value == NULL) {
        free(q);
        return false;
    }

    strncpy(q->value, s, strlen(s));
    q->value[strlen(s)] = '\0';
    list_add(&q->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *q = malloc(sizeof(element_t));
    if (q == NULL)
        return false;

    q->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (q->value == NULL) {
        free(q);
        return false;
    }

    strncpy(q->value, s, strlen(s));
    q->value[strlen(s)] = '\0';
    list_add_tail(&q->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    // cppcheck-suppress nullPointer
    element_t *first_entry = list_first_entry(head, element_t, list);
    if (sp) {
        sp = strncpy(sp, first_entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&first_entry->list);  // how about list_del_init?
    return first_entry;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    // cppcheck-suppress nullPointer
    element_t *tail_entry = list_last_entry(head, element_t, list);
    if (sp) {
        sp = strncpy(sp, tail_entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&tail_entry->list);
    return tail_entry;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
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

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return false;

    element_t *entry, *tmp;
    int size = q_size(head);
    list_for_each_entry_safe (entry, tmp, head, list) {
        size -= 2;
        if (size <= 0) {
            list_del(&entry->list);
            q_release_element(entry);
            return true;
        }
    }

    return false;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs
    if (head == NULL)
        return;

    struct list_head *iterator = head->prev;
    bool last = true;
    int size = q_size(head);
    if ((size & 1) == 1) {
        list_move(iterator, head);
        size -= 1;
    }
    while (size--) {
        iterator = head->prev;
        if (last)
            iterator = iterator->prev;
        list_move(iterator, head);
        last = !last;
    }
}

/*
 * Swap the next and prev of a list_head
 */
void inline node_np_swap(struct list_head *node)
{
    // Todo: uses XOR to swap
    struct list_head *tmp;
    tmp = node->next;
    node->next = node->prev;
    node->prev = tmp;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;

    struct list_head *node, *tmp;
    list_for_each_safe (node, tmp, head) {
        node_np_swap(node);
    }
    node_np_swap(head);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head) {}
