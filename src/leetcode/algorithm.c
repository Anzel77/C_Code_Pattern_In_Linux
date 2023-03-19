/**
 * @file algorithm.c
 * @author Karl Meng (karlmfloating@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "algorithm.h"


void bubbleSort(char *arr, unsigned start, unsigned end) {
    char temp;
    for (int i = start; i < end; ++i) {
        for (int j = start; j < end - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}


void sortString(char *str) {
    int len = strlen(str);
    char *buffer = (char *) malloc(sizeof(char) * len);
    unsigned upper, lower, number;
    unsigned index = 0;
    for (int i = 0; i < len; ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            buffer[index] = str[i];
            index++;
        }
    }
    upper = index; //大写字母后的索引

    for (int i = 0; i < len; ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            buffer[index] = str[i];
            index++;
        }
    }
    lower = index; // 小写字母后的索引

    for (int i = 0; i < len; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            buffer[index] = str[i];
            index++;
        }
    }
    number = index; // 数字后的索引

    bubbleSort(buffer, 0, upper);
    bubbleSort(buffer, upper, lower);
    bubbleSort(buffer, lower, number);

    for (int i = 0; i < len; ++i) {
        str[i] = buffer[i];
    }
    free(buffer);
}

/**
 * @brief 合并两个非降序的单链表
 *
 * @param list1 链表1
 * @param list2 链表2
 * @return 合并后的链表
 */
struct ListNode *mergeTwoLists(struct ListNode *list1, struct ListNode *list2) {
    struct ListNode *head = NULL;
    struct ListNode *tail = NULL;

    if (list1 == NULL && list2 == NULL) {
        return head;
    } else if (list1 == NULL) {
        return list2;
    } else if (list2 == NULL) {
        return list1;
    }

    if (list1->val < list2->val) {
        head = list1;
        tail = list1;
        list1 = list1->next;
    } else {
        head = list2;
        tail = list2;
        list2 = list2->next;
    }

    while (list1 != NULL && list2 != NULL) {
        if (list1->val < list2->val) {
            tail->next = list1;
            tail = tail->next;   // 尾指针后移
            list1 = list1->next; // 链表指针后移
        } else {
            tail->next = list2;
            tail = tail->next;
            list2 = list2->next;
        }
    }

    if (list1 != NULL) {
        tail->next = list1;
    } else {
        tail->next = list2;
    }

    return head;
}

/**
 * @brief 删除整形数组中特定的整数
 *
 * @param nums 输入数组
 * @param numsSize 数组大小
 * @param val 需要删除的整数
 * @return 删除特定整数后的数组大小
 */
int removeElement(int *nums, int numsSize, int val) {
    if (numsSize == 0)
        return 0;
    int index = 0;
    for (int i = 0; i < numsSize; ++i) {
        if (nums[i] != val) {
            nums[index] = nums[i];
            index++;
        }
    }
    return index;
}

/**
 * @brief Get the Intersection Node object
 *
 * @param headA 第一个链表的头指针
 * @param headB 第二个链表的头指针
 * @return struct ListNode* 交叉节点
 */
struct ListNode *getIntersectionNode(struct ListNode *headA, struct ListNode *headB) {
    struct ListNode *tailA = headA;
    struct ListNode *tailB;

    while (tailA != NULL) {
        tailB = headB;
        while (tailB != NULL) {
            if (tailA == tailB && tailA->val == tailB->val) {
                return tailA;
            } else {
                tailB = tailB->next;
            }
        }
        tailA = tailA->next;
    }
    return NULL;
}

/**
 * @brief
 *
 * @param haystack
 * @param needle
 * @return int
 */
int strStr(char *haystack, char *needle) {
    int len1 = strlen(haystack);
    int len2 = strlen(needle);
    for (int i = 0; i < len1; ++i) {
        if (haystack[i] != needle[0]) {
            continue;
        } else {
            for (int j = 0; j < len2; ++j) {
                if (haystack[i + j] != needle[j]) {
                    break;
                }
                if (j == len2 - 1) {
                    return i;
                }
            }
        }
    }

    return -1;
}

/**
 * @brief 将阿拉伯数转换为罗马形式。限制：1 <= num <= 3999  MMMCMXCIX
 *
 * @param num 需要转换的整数
 * @return char* 对应的罗马字符
 */
char *intToRoman(int num) {
    char *roman = (char *) malloc(sizeof(char) * 20);
    int i = 0;
    while (num > 0) {
        if (num >= 1000) {
            roman[i++] = 'M';
            num -= 1000;
        } else if (num >= 900) {
            roman[i++] = 'C';
            roman[i++] = 'M';
            num -= 900;
        } else if (num >= 500) {
            roman[i++] = 'D';
            num -= 500;
        } else if (num >= 400) {
            roman[i++] = 'C';
            roman[i++] = 'D';
            num -= 400;
        } else if (num >= 100) {
            roman[i++] = 'C';
            num -= 100;
        } else if (num >= 90) {
            roman[i++] = 'X';
            roman[i++] = 'C';
            num -= 90;
        } else if (num >= 50) {
            roman[i++] = 'L';
            num -= 50;
        } else if (num >= 40) {
            roman[i++] = 'X';
            roman[i++] = 'L';
            num -= 40;
        } else if (num >= 10) {
            roman[i++] = 'X';
            num -= 10;
        } else if (num >= 9) {
            roman[i++] = 'I';
            roman[i++] = 'X';
            num -= 9;
        } else if (num >= 5) {
            roman[i++] = 'V';
            num -= 5;
        } else if (num >= 4) {
            roman[i++] = 'I';
            roman[i++] = 'V';
            num -= 4;
        } else if (num >= 1) {
            roman[i++] = 'I';
            num -= 1;
        }
    }
    roman[i] = '\0';
    return roman;
}

/**
 * @brief 识别罗马数的字符，转换为阿拉伯数
 *
 * @param s 需要识别的罗马字符
 * @return int 罗马数字对应的整数
 */
int romanToInt(char *s) {
    int sum = 0;
    int len = strlen(s);
    int current = 0;
    int prev = 0;

    for (int i = 0; i < len; ++i) {
        switch (s[i]) {
            case 'I':
                current = 1;
                break;
            case 'V':
                current = 5;
                break;
            case 'X':
                current = 10;
                break;
            case 'L':
                current = 50;
                break;
            case 'C':
                current = 100;
                break;
            case 'D':
                current = 500;
                break;;
            case 'M':
                current = 1000;
                break;
            default:
                break;
        }
        if (prev < current) {
            sum = sum + current - prev * 2;
        } else {
            sum += current;
        }
        prev = current;
    }
    return sum;
}

/**
 * @brief 最长公共前缀
 *
 * @param strs 字符串数组
 * @param strsSize 字符串数组个数
 * @return 最长公共前缀
 */
char *longestCommonPrefix(char **strs, int strsSize) {
    //    if (strsSize == 0) {
    //        return "";
    //    }
    //    if(strsSize == 1) {
    //        return strs[0];
    //    }
    //    // 获取第一个字符串的长度
    //    int len = strlen(strs[0]);
    //    int index = 0;
    //    int flag = 1;
    //    for (int i = 0; i < len; ++i) {
    //        for (int j = 1; j < strsSize; ++j) {
    //            if(strs[j][i] != strs[0][i]) {
    //                flag = 0;
    //                break;
    //            } else if (j == strsSize - 1){
    //                index++;
    //            }
    //        }
    //        if (flag == 0){
    //            break;
    //        }
    //    }
    //
    //    // 创建字符串，最后需要加上'\0'，所以长度为 index+1
    //    char *result = (char *)malloc(sizeof(char) * (index + 1));
    //    strncpy(result, strs[0], index);
    //    result[index] = '\0';
    //    return result;

    if (strsSize == 0) {
        char *result = (char *) calloc(1, sizeof(char));
        return result;
    }
    int index = 0, i = 0;
    char flag = strs[i][index];
    while (flag) {
        for (i = 1; i < strsSize; i++) {
            if (strs[i][index] != flag)
                break;
        }
        if (i < strsSize)
            break;
        flag = strs[0][++index];
    }
    strs[0][index] = '\0';
    return strs[0];
}

/**
 * @brief 判断括号数组是否合法排序
 *
 * @param s 括号数组
 * @return true 合法排序
 * @return false 非法排序
 */
bool isValid(char *s) {

    int len = strlen(s);
    // 长度为基数的必定不合法
    if (len % 2 == 1) {
        return false;
    }

    // 创建存储栈
    char *stack = (char *) malloc(sizeof(char) * len);
    int top = -1;
    for (int i = 0; i < len; ++i) {
        // 将所有开括号压栈
        if (s[i] == '(' || s[i] == '[' || s[i] == '{') {
            stack[++top] = s[i];
        } else {
            if (top == -1) {
                return false;
            }
            if (s[i] == ')' && stack[top] == '(') {
                top--;
            } else if (s[i] == ']' && stack[top] == '[') {
                top--;
            } else if (s[i] == '}' && stack[top] == '{') {
                top--;
            } else {
                return false;
            }
        }
    }
    if (top == -1) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Search Insert Position。二分查找
 *
 * @param nums 已排序的整数数组
 * @param numsSize 数组大小
 * @param target 需要查找的目标
 * @return int 目标位置
 */
int searchInsert(int *nums, int numsSize, int target) {
    int low = 0, high = numsSize - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (nums[mid] == target) {
            return mid;
        } else if (nums[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return low;
}

/**
 * @brief 逆转链表
 *
 * @param pHead 链表头指针
 * @return struct ListNode* 逆转后的链表的头指针
 */
struct ListNode *reverseList(struct ListNode *pHead) {
    struct ListNode *pre = NULL;
    struct ListNode *cur = pHead;
    struct ListNode *next = NULL;
    while (cur != NULL) {
        next = cur->next;
        cur->next = pre;
        pre = cur;
        cur = next;
    }
    return pre;
}

/**
 * @brief 逆转链表的特定位置
 *
 * @param head 链表头指针
 * @param m 逆转的开始位置
 * @param n 逆转的结束位置
 * @return struct ListNode* 特定位置逆转后的链表头指针
 */
struct ListNode *reverseBetween(struct ListNode *head, int m, int n) {
    struct ListNode *pre = NULL;
    struct ListNode *cur = head;
    struct ListNode *next = NULL;
    struct ListNode *mp = NULL;
    struct ListNode *mpn = NULL;
    struct ListNode *np = NULL;

    if (m == n) {
        return head;
    }

    int index = 0;
    while (cur != NULL) {
        index++;
        if (index == m - 1 || (m == 1 && index == 1)) {
            mp = cur;
            mpn = cur->next;
        }
        if (index == n) {
            np = cur;
        }
        if (index >= m && index <= n) {
            next = cur->next;
            cur->next = pre;
            pre = cur;
            cur = next;
        } else {
            pre = cur;
            cur = cur->next;
        }
        if (index == n) {
            if (m == 1) {
                mp->next = cur;
                mpn->next = mp;
            } else {
                mp->next = np;
                mpn->next = cur;
            }
        }
    }

    if (m == 1) {
        return np;
    }
    return head;
}

// 123456
// 213456
// 214356
// 214365
/**
 * @brief 将链表成对调换顺序
 *
 * @param head 链表头指针
 * @return struct ListNode* 调换顺序后的链表
 */
struct ListNode *swapPairs(struct ListNode *head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    struct ListNode *pre = head;
    struct ListNode *cur = head->next;
    struct ListNode *next = cur->next;
    struct ListNode *link = head;
    head = head->next;

    while (true) {
        if (next == NULL || next->next == NULL) {
            // swap pair
            pre->next = next;
            cur->next = pre;
            return head;
        }
        // swap pair
        pre->next = next;
        cur->next = pre;
        // move pointer
        pre = next;
        cur = pre->next;
        next = cur->next;
        // link previous pair with current pair
        link->next = cur;
        // move pointer link
        link = pre;
    }
}

/**
 * @brief // 58. Length of Last Word
 *
 * @param s
 * @return int 一句话最后一个单词的长度
 */
int lengthOfLastWord(char *s) {
    int len = strlen(s);
    int result = 0;
    for (int i = len - 1; i >= 0; --i) {
        if ((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z')) {
            result++;
        } else if (s[i] == ' ' && ((s[i + 1] >= 'A' && s[i + 1] <= 'Z') || (s[i + 1] >= 'a' && s[i + 1] <= 'z'))) {
            break;
        }
    }
    return result;
}