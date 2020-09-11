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
    list_ele_t *ptr;  // Declare operating pointer

    /* check if q is NULL or not */
    if (q != NULL) {
        ptr = q->head;
    } else
        return;

    /* Free list nodes and its string space*/
    while (ptr != NULL) {
        free(ptr->value);
        ptr = ptr->next;
        free(q->head);
        q->head = ptr;
    }

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
    if (q->tail)
        q->tail->next =
            newt;  // only assign next value while q->tail is not NULL
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
    /* Declare operating pointer on nodes */
    list_ele_t *ptr;

    /* Reject q is NULL and q->head is NULL cases */
    if (!q)
        return false;

    if (!(q->head))
        return false;

    /* Operating Pointer Assignment */
    ptr = q->head;

    /* Copy string to *sp */
    if (sp) {
        memset(sp, '\0', sizeof(char) * bufsize);
        strncpy(sp, ptr->value, bufsize - 1);
    }

    /* Edit pointer and free node space */
    q->head = q->head->next;
    free(ptr->value);
    free(ptr);
    q->size--;
    if (!q->head)
        q->tail = NULL;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
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
    /* Operating pointer declaration */
    list_ele_t *prev, *curr, *nex;

    /* Reject null pointer case */
    if (!q || !(q->head))
        return;

    /* Size is 1, return */
    if (q->size == 1)
        return;

    /* Operating pointer initialize */
    curr = q->head;
    nex = q->head->next;
    prev = NULL;

    /* Do list reverse */
    while (curr != NULL) {
        curr->next = prev;
        prev = curr;
        curr = nex;
        /* To Avoid null pointer dereference,
           Add NULL pointer judge for pointer nex */
        if (nex != NULL)
            nex = nex->next;
    }

    /* Re-assign head and tail pointer */
    q->tail = q->head;
    q->head = prev;
}

/* merge() for q_sort() usage */
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    // int cnt = 0;
    size_t len_l1, len_l2;

    // merge with recursive
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    // Fetch string length
    len_l1 = strlen(l1->value);
    len_l2 = strlen(l2->value);

    // Use maximum length
    if (len_l1 < len_l2)
        len_l1 = len_l2;

    if (strncmp(l1->value, l2->value, len_l1) <= 0) {
        // l1 is smaller than or equal to l2
        l1->next = merge(l1->next, l2);
        return l1;
    } else {
        // l1 is bigger than l2
        l2->next = merge(l1, l2->next);
        return l2;
    }
}

/* mergeSortLitst() for q_sort() usage */
list_ele_t *mergeSortList(list_ele_t *head)
{
    // merge sort
    if (!head || !head->next)
        return head;

    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = mergeSortList(head);
    list_ele_t *l2 = mergeSortList(fast);

    // merge sorted l1 and sorted l2
    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* Sort list by [Merge Sort] */
    list_ele_t *ptr;

    /* Reject q is NULL case  */
    if (!q)
        return;

    /* Return directly while queue has less than or equal to 1 element */
    if (q->size <= 1)
        return;

    /* Call Sorting function */
    q->head = mergeSortList(q->head);

    /* Re-assign tail pointer */
    ptr = q->head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    q->tail = ptr;
}
