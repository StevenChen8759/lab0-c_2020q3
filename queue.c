#include <assert.h>
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
    strncpy(newh->value, s, strlen(s));  // Avoid Buffer overflow attack!
    *(newh->value + strlen(s)) = '\0';   // Edit end-of-string

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
    strncpy(newt->value, s, strlen(s));  // Avoid Buffer overflow attack!
    *(newt->value + strlen(s)) = '\0';   // Edit end-of-string

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
        strncpy(sp, ptr->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';  // Edit end-of-string
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

/*
 * List-Element-Is-Less-or-Equal function
 * This function compare with two list elements by input argument.
 * If ele1->value is less or equal to ele2->value, then return true.
 * Otherwise or NULL list element pointer input, return false.
 * TODO: make this function as a Marco call to improve performance
 */
bool listelement_isle(list_ele_t *ele1, list_ele_t *ele2)
{
    size_t len1, len2;
    char *str1 = NULL, *str2 = NULL;

    /* Input pointer check */
    if (!ele1 || !ele2)
        return false;

    /* Assign input string and do NULL chcek */
    str1 = ele1->value;
    str2 = ele2->value;
    if (!str1 || !str2)
        return false;

    /* Decrease strlen() function call count,
     * We store length in the local variable.
     */
    len1 = strlen(str1);
    len2 = strlen(str2);

    /* Call strncmp() to compare two string based on shorter string length
     * This method can avoid buffer overflow attack.
     * Based on return value of strncmp, this function return assigned
     * comparison result
     */

    return (len1 >= len2) ? (strncmp(str1, str2, len2) <= 0)
                          : (strncmp(str1, str2, len1) <= 0);
}

/*
 * Merge function for iterative Linked-list merge sort.
 */

void merge(list_ele_t *listarr[], int lb, int mid, int rb)
{
    int i, j, k;
    int n1 = mid - lb + 1;
    int n2 = rb - mid;
    list_ele_t *L[n1], *R[n2];

    for (i = 0; i < n1; i++)
        L[i] = listarr[lb + i];

    for (j = 0; j < n2; j++)
        R[j] = listarr[mid + 1 + j];


    i = 0, j = 0, k = lb;
    while (i < n1 && j < n2) {
        if (listelement_isle(L[i], R[j])) {
            listarr[k] = L[i];
            i++;
        } else {
            listarr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        listarr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        listarr[k] = R[j];
        j++;
        k++;
    }
}

/*
 * Iterative merge sort (main function) for a linked-list.
 */
void itMergeSort(list_ele_t **listarr, int lb, int rb)
{
    if (lb < rb) {
        int mid = lb + (rb - lb) / 2;
        // assert(mid & 0x80000000); // Assert if mid goes to negative value!

        itMergeSort(listarr, lb, mid);
        itMergeSort(listarr, mid + 1, rb);
        merge(listarr, lb, mid, rb);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    list_ele_t *ptr;
    int i;

    /* Reject q is NULL case  */
    if (!q)
        return;

    /* Return directly while queue has less than or equal to 1 element */
    if (q->size <= 1)
        return;

    /* Declare Array for list element pointer */
    list_ele_t *listarray[q->size];
    list_ele_t **laptr;

    /* Assign list element address from head to tail (O(n) cost) */
    ptr = q->head;
    laptr = listarray;
    while (ptr != NULL) {
        *laptr = ptr;
        // printf("%s\n", (*laptr)->value);
        laptr++;
        ptr = ptr->next;
    }

    /*
     * Traverse array and do merge sort based on string comparison result
     * Time: O(nlogn)
     */
    itMergeSort(listarray, 0, (q->size - 1));

    /* Based on the result of sorted list array, assign next field of each node
     */
    for (i = 0; i < q->size; i++)
        listarray[i]->next = (i != q->size - 1) ? listarray[i + 1] : NULL;

    /* Finally, assign new head and tail pointers */
    q->head = listarray[0];
    q->tail = listarray[q->size - 1];
}
