#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    // Successful allocation, assign related value
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* TODO: How about freeing the list elements and the strings? */
    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;

    /* Return false if empty value input */
    if (!q || !s)
        return false;

    /* Allocate memory space */
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;  // Failed allocation, return false...

    /* Allocate char space in newh.
     * Don`t forget '\0' space in the end of string.
     */
    newh->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!newh->value) {
        // Failed allocation, free allocated space and return false...
        free(newh);
        return false;
    }

    // String assignment via strncpy
    memset(newh->value, '\0', sizeof(char) * (strlen(s) + 1));
    strncpy(newh->value, s, strlen(s));  // Avoid Buffer overflow attack!

    // Pointer manipulation
    newh->next = q->head;
    q->head = newh;
    if (!q->tail)
        q->tail = newh;  // For initialize case
    q->size++;           // Be aware of multiple access synchronization issue!

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* TODO: You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */
    /* TODO: Remove the above comment when you are about to implement. */
    list_ele_t *newt;

    /* Return false if empty value input */
    if (!q || !s)
        return false;

    /* Allocate memory space */
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;  // Failed allocation, return false...

    /* Allocate char space in newt.
     * Don`t forget '\0' space in the end of string.
     */
    newt->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!newt->value) {
        // Failed allocation, free allocated space and return false...
        free(newt);
        return false;
    }

    // String assignment via strncpy
    memset(newt->value, '\0', sizeof(char) * (strlen(s) + 1));
    strncpy(newt->value, s, strlen(s));  // Avoid Buffer overflow attack!

    // Pointer manipulation
    newt->next = NULL;  // Avoid non-zero initial value
    q->tail->next = newt;
    q->tail = newt;
    if (!q->head)
        q->head = newt;  // For initialize case
    q->size++;           // Be aware of multiple access synchronization issue!

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* TODO: You need to fix up this code. */
    /* TODO: Remove the above comment when you are about to implement. */
    q->head = q->head->next;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    /* TODO: Remove the above comment when you are about to implement. */
    return 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */
}
