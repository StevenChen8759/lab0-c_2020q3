#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define QUEUE_SHOW_INTERNAL

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

int string_compare(char *s1, char *s2);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *queue_head_ptr;

    queue_head_ptr = (struct list_head *) malloc(sizeof(struct list_head));
    if (queue_head_ptr)
        INIT_LIST_HEAD(queue_head_ptr);

    return queue_head_ptr;
}


#ifdef QUEUE_SHOW_INTERNAL
void q_show_internal(struct list_head *head)
{
    element_t *element = NULL;

    printf("[");

    if (head->prev == head)
        printf("]\n");

    list_for_each_entry (element, head, list) {
        printf("%s%s", element->value,
               element->list.next == head ? "]\n" : " ");
    }
}
#endif

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    struct list_head *node, *next;
    // element_t *element;

    if (!head)
        return;

    // Free all elements
    list_for_each_safe (node, next, head) {
        // element = list_entry(node, element_t, list);
        list_del(node);
        free(list_entry(node, element_t, list)->value);
        free(list_entry(node, element_t, list));
    }
    free(head);
}

bool q_copy_string_to_element(element_t *new_element, char *s)
{
    size_t s_len;
    char *ret;

    // Forbid NULL pointer sent into strlen()
    if (!s) {
        printf("queue.c ERROR: input string cannot be NULL!");
        return false;
    }

    s_len = strlen(s) + 1;

    // Do not forget '\0'
    new_element->value = (char *) malloc(s_len * sizeof(char));
    if (!new_element->value) {
        return false;
    }

    // Length set ref: https://hackmd.io/@unknowntpo/strncpy
    // Will auto pacth '\0'
    ret = strncpy(new_element->value, s, s_len);
    if (!ret) {
        printf("queue.c ERROR: strncpy() returns NULL pointer!");
        return false;
    }

    return true;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }

    if (!q_copy_string_to_element(new_element, s)) {
        free(new_element);
        return false;
    }

    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (!new_element)
        return false;

    if (!q_copy_string_to_element(new_element, s)) {
        free(new_element);
        return false;
    }

    list_add_tail(&new_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *element;

    if (head->next == head)
        return NULL;

    element = list_first_entry(head, element_t, list);

    // Copy content
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';  // Edit end-of-string
    }

    // Pointer OPs for removal
    head->next = element->list.next;
    head->next->prev = head;

    // Remove Node
    list_del(&element->list);
    // free(element->value);
    // free(element);

    // FIXME: Use after free?
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *element;

    if (head->next == head)
        return NULL;

    element = list_last_entry(head, element_t, list);

    // Copy content
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';  // Edit end-of-string
    }

    // Pointer OPs for removal
    head->prev = element->list.prev;
    head->prev->next = head;

    // Remove Node
    list_del(&element->list);
    // free(element->value);
    // free(element);

    // FIXME: Use after free?
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    // TODO: Can use use atomic to implemnet O(1) q_size get?
    struct list_head *node;
    int queue_size = 0;

    list_for_each (node, head)
        queue_size++;

    return queue_size;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *left = head->next, *right = head->prev;
    element_t *entry_to_del = NULL;

    if (left == head)
        return false;

    // Looking for middle node by sandwich method
    while (left != right) {
        if (left->next == right)
            break;

        left = left->next;
        right = right->prev;
    }


    entry_to_del = list_entry(right, element_t, list);
    list_del(&entry_to_del->list);
    free(entry_to_del->value);
    free(entry_to_del);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    // TODO: Implement hash to make complexity in O(n) <Still need scan whole
    // input list>

    int *node_need_del_mask;
    struct list_head *node;
    element_t *element, *element_next;
    // bool remove_self;
    int cnt_1, cnt_2, cnt_3;
    int queue_size = q_size(head);

    // FIXME: Use calloc() will cause false alarm under make test
    node_need_del_mask = (int *) malloc(queue_size * sizeof(int));
    if (!node_need_del_mask)
        return false;

    memset(node_need_del_mask, 0x00, queue_size * sizeof(int));

    // Scan list and check if node delete is needed
    // If needed, mark delete mask to 1
    cnt_1 = 0;
    list_for_each_entry (element, head, list) {
        // Node needs to be deleted, skip
        if (node_need_del_mask[cnt_1]) {
            cnt_1++;
            continue;
        }

        node = element->list.next;
        cnt_2 = cnt_1 + 1;

        // printf("Base element: %s\n", element->value);
        while (node != head) {
            // printf(" -> Compare element: %s", list_entry(node, element_t,
            // list)->value);
            if (!string_compare(element->value,
                                list_entry(node, element_t, list)->value)) {
                node_need_del_mask[cnt_1] = 1;
                node_need_del_mask[cnt_2] = 1;

                // printf(" [Deleted]");
            }

            // printf("\n");

            node = node->next;
            cnt_2++;
        }

        cnt_1++;
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Original] => ");
    q_show_internal(head);

    printf("Delete Map => ");
    for (int i = 0; i < queue_size; i++)
        printf("%d%s", node_need_del_mask[i], i != queue_size - 1 ? " " : "\n");
#endif

    // Delete duplicated elements
    cnt_3 = 0;
    list_for_each_entry_safe (element, element_next, head, list) {
        if (node_need_del_mask[cnt_3++]) {
            list_del(&element->list);
            free(element->value);
            free(element);
        }
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Delete Dup Final] => ");
    q_show_internal(head);
#endif

    // Don't forget to free allocated queue! (Checked by Address Santizer)
    free(node_need_del_mask);

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *node_a, *node_b;

    for (node_a = head->next; node_a != head; node_a = node_a->next) {
        node_b = node_a->next;

        if (node_b == head)
            break;

        // Outer link modify
        node_a->prev->next = node_b;
        node_b->next->prev = node_a;

        node_a->next = node_b->next;
        node_b->next = node_a;

        node_b->prev = node_a->prev;
        node_a->prev = node_b;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *curr = head->next, *next, *temp;


    while (1) {
        // Set next node to move before swap prev and next
        next = curr->next;

        // Swap prev and next of a node
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;

        if (curr == head)
            break;

        // Move to next node
        curr = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    struct list_head *node, *node_next;
    struct list_head temp_list, merged_list;
    int cnt = 0;

    INIT_LIST_HEAD(&temp_list);
    INIT_LIST_HEAD(&merged_list);

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Reverse k original] => ");
    q_show_internal(head);
#endif

    list_for_each_safe (node, node_next, head) {
        list_del(node);
        list_add_tail(node, &temp_list);

        if (++cnt == k) {
            q_reverse(&temp_list);

            temp_list.next->prev = merged_list.prev;
            temp_list.next->prev->next = temp_list.next;

            merged_list.prev = temp_list.prev;
            (merged_list.prev)->next = &merged_list;

            INIT_LIST_HEAD(&temp_list);
            cnt = 0;
        }
    }

    if (!list_empty(&temp_list)) {
        temp_list.next->prev = merged_list.prev;
        temp_list.next->prev->next = temp_list.next;

        merged_list.prev = temp_list.prev;
        (merged_list.prev)->next = &merged_list;
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Reverse k Final] => ");
    q_show_internal(&merged_list);
#endif

    head->prev = merged_list.prev;
    head->prev->next = head;

    head->next = merged_list.next;
    head->next->prev = head;
}

// Return 1: s1 > s2, Return 0: s1 = s2, Return -1: s1 < s2
int string_compare(char *s1, char *s2)
{
    size_t s1_len, s2_len;
    int ret;

    if (!s1 || !s2)
        return -2;

    s1_len = strlen(s1);
    s2_len = strlen(s2);

    // if (s1_len > s2_len)
    //     return 1;
    if (s1_len < s2_len)
        s1_len = s2_len;
    // return -1;

    ret = strncmp(s1, s2, s1_len);

    if (ret > 0)
        return 1;
    else if (ret < 0)
        return -1;
    else
        return 0;
}

struct list_head *q_merge_list(struct list_head *left,
                               struct list_head *right,
                               struct list_head *merged,
                               bool descend)
{
    struct list_head *left_node = left->next, *right_node = right->next, *temp;
    // struct list_head *left_node_next, *right_node_next;
#ifdef QUEUE_SHOW_INTERNAL
    int cnt = 0;
#endif
    int ret;

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Merge Left] Sorted List:");
    q_show_internal(left);
    printf("[Merge Right] Sorted List:");
    q_show_internal(right);
#endif

    while (left_node != left || right_node != right) {
        // Left list all merged, directly merge all content from right list
        // TODO: Directly merge whole list rather than element-wise add to list
        if (left_node == left) {
            temp = right_node->next;
            list_add_tail(right_node, merged);
            right_node = temp;
            continue;
        } else if (right_node == right) {
            // Right list all merged, directly merge all content from left list
            temp = left_node->next;
            list_add_tail(left_node, merged);
            left_node = temp;
            continue;
        }

        ret = string_compare(list_entry(left_node, element_t, list)->value,
                             list_entry(right_node, element_t, list)->value);

        switch (ret) {
        case 1:  // left > right
            if (descend) {
                temp = left_node->next;
                list_add_tail(left_node, merged);
                left_node = temp;
            } else {
                temp = right_node->next;
                list_add_tail(right_node, merged);
                right_node = temp;
            }

            break;
        case 0:  // left = right
            temp = left_node->next;
            list_add_tail(left_node, merged);
            left_node = temp;
            break;
        case -1:  // left < right
            if (descend) {
                temp = right_node->next;
                list_add_tail(right_node, merged);
                right_node = temp;
            } else {
                temp = left_node->next;
                list_add_tail(left_node, merged);
                left_node = temp;
            }
            break;
        default:
            break;
        };

        if (left_node == left) {
            left->prev = left;
            left->next = left;
        } else if (right_node == right) {
            right->prev = right;
            right->next = right;
        }

#ifdef QUEUE_SHOW_INTERNAL
        printf("[%d] => ", ++cnt);
        q_show_internal(merged);
#endif
    }

    // Since our temp list is declared in stack and will be free after return
    // We should utilize space from caller and copy final result to there
    // left->prev = list_merged.prev;
    // left->next = list_merged.next;

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Merged] Sorted merged List:");
    // q_show_internal(&list_merged);
    q_show_internal(merged);
#endif

    return merged;
}

struct list_head *q_merge_sort(struct list_head *head, bool descend)
{
    // TODO: Simplify variable usage
    struct list_head *left = head->next, *right = head->prev;
    struct list_head left_list, right_list, list_merged_inst;
    struct list_head *left_list_merged, *right_list_merged;
    // struct list_head *head_original = head;
    int ret;

    INIT_LIST_HEAD(&list_merged_inst);

#ifdef QUEUE_SHOW_INTERNAL
    printf("Input List len: %d -> ", q_size(head));
    q_show_internal(head);
#endif

    // Zero element case
    if (left == head && right == head) {
        printf("WARING: q_merge_sort() merged zero length list!\n");
        return head;
    }

    // 1 element list case
    if (left == right) {
        return head;
    }

    // 2 element list case, check content and swap if needed.
    // Then, return sorted list.
    if (left->next == right) {
        ret = string_compare(list_entry(left, element_t, list)->value,
                             list_entry(right, element_t, list)->value);
        if ((ret == 1 && !descend) || (ret == -1 && descend))
            q_reverse(head);
        return head;
    }

    // For more element list case, split the list first
    // Split while two elements are collected
    while (1) {
        if (left->next == right)
            break;

        left = left->next;
        right = right->prev;

        if (left == right) {
            right = right->next;
            break;
        }
    }

    // Fetch right node as middle point and split
    // Then, do merge sort for each list
    left_list.next = head->next;
    (left_list.next)->prev = &left_list;
    left_list.prev = left;
    left->next = &left_list;

    right_list.next = right;
    right->prev = &right_list;
    right_list.prev = head->prev;
    (right_list.prev)->next = &right_list;

    head->prev = head;
    head->next = head;

    // printf("[Split] Left Len: %d, Right Len: %d\n", q_size(&left_list),
    // q_size(&right_list));

    left_list_merged = q_merge_sort(&left_list, descend);
    right_list_merged = q_merge_sort(&right_list, descend);

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Left] Sorted List:");
    q_show_internal(left_list_merged);
    printf("[Right] Sorted List:");
    q_show_internal(right_list_merged);
#endif

    q_merge_list(left_list_merged, right_list_merged, &list_merged_inst,
                 descend);

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Already Final] Sorted List:");
    q_show_internal(&list_merged_inst);
    // q_show_internal(head);
#endif

    head->prev = list_merged_inst.prev;
    head->next = list_merged_inst.next;

    head->prev->next = head;
    head->next->prev = head;

    return head;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    struct list_head **head_original = &head;

    if (!head)
        return;

    // Empty queue or only 1 element
    if (head->prev == head->next)
        return;

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Original] =>");
    q_show_internal(head);
#endif

    head = q_merge_sort(head, descend);

    (*head_original)->prev = head->prev;
    (*head_original)->next = head->next;

    (*head_original)->prev->next = (*head_original);
    (*head_original)->next->prev = (*head_original);

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Final] =>");
    q_show_internal(*head_original);
#endif
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *element_node = NULL, *element_node_next;
    struct list_head *node_max = NULL, *node;
    bool check_ascend;
    int ret;

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Q Ascend] => ");
    q_show_internal(head);
#endif

    // Find maximum value first <O(n)>
    list_for_each_entry (element_node, head, list) {
        // Should assign first element to node_max
        if (&element_node->list == head->next) {
            node_max = &element_node->list;
            continue;
        }

        ret = string_compare(list_entry(node_max, element_t, list)->value,
                             element_node->value);

        if (ret < 0)
            node_max = &element_node->list;
    }

    check_ascend = true;
    list_for_each_entry_safe (element_node, element_node_next, head, list) {
        // If we moved to max postition, directly remove incoming nodes
        if (&element_node->list == node_max) {
            check_ascend = false;
            continue;
        }


        if (!check_ascend) {
            list_del(&element_node->list);
            free(element_node->value);
            free(element_node);
            continue;
        }

        // Remove current node if it caused non-ascend sequence
        node = element_node->list.next;
        while (node != node_max) {
            ret = string_compare(element_node->value,
                                 list_entry(node, element_t, list)->value);

            if (ret > 0) {
                list_del(&element_node->list);
                free(element_node->value);
                free(element_node);
                break;
            }

            node = node->next;
        }
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Q Ascend Final] => ");
    q_show_internal(head);
#endif

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the rig ht side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *element_node = NULL, *element_node_next;
    struct list_head *node_max = NULL, *node;
    bool check_descend;
    int ret;

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Q Descend] => ");
    q_show_internal(head);
#endif

    // Find minimum value first <O(n)>
    list_for_each_entry (element_node, head, list) {
        // Should assign first element to node_max
        if (&element_node->list == head->next) {
            node_max = &element_node->list;
            continue;
        }

        ret = string_compare(list_entry(node_max, element_t, list)->value,
                             element_node->value);

        if (ret > 0)
            node_max = &element_node->list;
    }

    check_descend = false;
    list_for_each_entry_safe (element_node, element_node_next, head, list) {
        if (&element_node->list == node_max)
            check_descend = true;

        if (!check_descend) {
            list_del(&element_node->list);
            free(element_node->value);
            free(element_node);
            continue;
        }

        // Remove current node if it caused non-descend sequence
        node = element_node->list.next;
        while (node != head) {
            ret = string_compare(element_node->value,
                                 list_entry(node, element_t, list)->value);

            if (ret < 0) {
                list_del(&element_node->list);
                free(element_node->value);
                free(element_node);
                break;
            }

            node = node->next;
        }
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Q Descend Final] => ");
    q_show_internal(head);
#endif

    return q_size(head);
}

struct list_head *q_merge_k_way_select(struct list_head *head, bool descend)
{
    return NULL;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    queue_contex_t *input_queue = NULL, *queue_to_merge = NULL, *final_queue;
    struct list_head merged_list, *node;
    int total_element_cnt = 0, actually_merged_cnt = 0;
    int queue_cnt = 0;
    int ret;

    INIT_LIST_HEAD(&merged_list);

    // Evaluate queue count and total element count
    list_for_each_entry (input_queue, head, chain) {
#ifdef QUEUE_SHOW_INTERNAL
        printf("[QID=%d] => ", input_queue->id);
        q_show_internal(input_queue->q);
#endif

        total_element_cnt += input_queue->size;
        queue_cnt++;
    }


    while (actually_merged_cnt != total_element_cnt) {
        // Assign 1st non-empty queue for merge
        list_for_each_entry (input_queue, head, chain) {
            queue_to_merge = list_entry(head->next, queue_contex_t, chain);
            if (!list_empty(input_queue->q)) {
                queue_to_merge = input_queue;
                break;
            }
        }
#ifdef QUEUE_SHOW_INTERNAL
        printf("[Test Iter] => %d ", actually_merged_cnt);
        q_show_internal(queue_to_merge->q);
#endif

        // Select node from each queue and get node to merge
        // Comparision based on string.
        list_for_each_entry (input_queue, head, chain) {
            if (queue_to_merge == input_queue || list_empty(input_queue->q))
                continue;

            // if (input_queue->chain.next == head->next) {
            //     // Select 1st queue to merge as default
            //     queue_to_merge = input_queue;
            //     continue;
            // }

            ret = string_compare(
                list_entry(queue_to_merge->q->next, element_t, list)->value,
                list_entry(input_queue->q->next, element_t, list)->value);

            if ((ret == 1 && !descend) || (ret == -1 && descend)) {
                queue_to_merge = input_queue;
            }
        }

        // printf("[%d] Selectd Queue ID: %d, first element value: %s\n",
        //     actually_merged_cnt + 1,
        //     queue_to_merge->id,
        //     list_entry(queue_to_merge->q->next, element_t, list)->value
        // );

        // Delete from original list and attached to merged list
        node = queue_to_merge->q->next;
        list_del(node);
        list_add_tail(node, &merged_list);

        actually_merged_cnt++;
    }

#ifdef QUEUE_SHOW_INTERNAL
    printf("[Merged Queue] => ");
    q_show_internal(&merged_list);
#endif

    // Attach merged list to qtest input queue along with modifying context.
    final_queue = list_entry(head->next, queue_contex_t, chain);

    // Rebind list head of merged queue on 1st queue node
    final_queue->q->prev = merged_list.prev;
    final_queue->q->prev->next = final_queue->q;
    final_queue->q->next = merged_list.next;
    final_queue->q->next->prev = final_queue->q;

    list_for_each_entry (input_queue, head, chain) {
#ifdef QUEUE_SHOW_INTERNAL
        printf("[QID=%d] => ", input_queue->id);
        q_show_internal(input_queue->q);
#endif
    }

    return actually_merged_cnt;
}
