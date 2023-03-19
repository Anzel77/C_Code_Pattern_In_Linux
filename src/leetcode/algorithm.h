/**
 * @file algorithm.h
 * @author Karl Meng (karlmfloating@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stdbool.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

/**
 * @brief 对一串字符串进行排序
 *
 * @param str 需要进行特定排序的字符串
 */
void sortString(char *str);

/**
 * @brief 对字符串的特定字串进行冒泡排序
 *
 * @param arr 需要排序的字符串
 * @param start 排序起始点
 * @param end 排序结束点
 */
void bubbleSort(char *arr, unsigned start, unsigned end);

char *intToRoman(int num);

int romanToInt(char *s);

char *longestCommonPrefix(char **strs, int strsSize);

bool isValid(char *s);

struct ListNode *mergeTwoLists(struct ListNode *list1, struct ListNode *list2);

struct ListNode *swapPairs(struct ListNode *head);

int removeElement(int *nums, int numsSize, int val);

int strStr(char *haystack, char *needle);

int searchInsert(int *nums, int numsSize, int target);

struct ListNode *getIntersectionNode(struct ListNode *headA, struct ListNode *headB);

struct ListNode *reverseList(struct ListNode *pHead);

struct ListNode *reverseBetween(struct ListNode *head, int m, int n);

int lengthOfLastWord(char *s);


#endif // NEW_ALGORITHM_H
