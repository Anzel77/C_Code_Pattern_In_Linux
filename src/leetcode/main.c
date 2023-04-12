/**
 * @file main.c
 * @author Karl Meng (karlmfloating@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "algorithm.h"

int main() {
    char str[] = "sfF2Rjq5ud4";
    sortString(str);
    printf("%s\n", str);

    int nums[4] = {3, 2, 2, 3};
    int len = removeElement(nums, 4, 3);
    for (int i = 0; i < len; ++i) {
        printf("%d", nums[i]);
    }

    printf("\n");

    int *p = nums;
    printf("%lu\n", sizeof(nums));
    printf("%lu\n", sizeof p);

    char str1[] = "hello";
    char str2[] = "ll";

    int ik = strStr(str1, str2);
    printf("%d\n", ik);

    char roman[] = "MCMXCIV";
    int romanTo = romanToInt(roman);
    printf("%d\n", romanTo);

    char *strs[3] = {"flower", "flow", "flight"};
    char **strs1 = strs;

    char str3[] = "flower";
    char str4[] = "flow";
    char str5[] = "flight";

    char *strs2[3] = {str3, str4, str5};

    printf("%s\n", strs[0]);
    printf("%s\n", strs1[0]);
    printf("%c\n", strs2[0][0]);
    printf("%lu\n", strlen(strs1[0]));

    printf("%s\n", longestCommonPrefix(strs2, 3));

    int x = 23;
    printf("%d\n", !x); // output is 0

    union uni {
        int a;
        char b;
    };

    struct str {
        int a;
        char b;
    };

    printf("size of union: %lu\nsize of struct: %lu\n", sizeof(union uni), sizeof(struct str));

    int nums35[] = {1, 3, 6, 8, 10, 15};
    int target = 13;
    int answer = searchInsert(nums35, 4, target);
    printf("%d", answer);
    printf("\n========================================================\n");

    // char zigzag[] = "PAHNAPLSIIGYIR";

    struct ListNode *head = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->val = 1;
    head->next = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->next->val = 2;
    head->next->next = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->next->next->val = 3;
    head->next->next->next = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->next->next->next->val = 4;
    head->next->next->next->next = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->next->next->next->next->val = 5;
    head->next->next->next->next->next = (struct ListNode *) malloc(sizeof(struct ListNode));
    head->next->next->next->next->next->val = 6;
    head->next->next->next->next->next->next = NULL;

    struct ListNode *re = reverseBetween(head, 1, 4);
    struct ListNode *reCopy_1 = re;
    while (re != NULL) {
        printf("%d", re->val);
        re = re->next;
    }

    printf("\n========================================================\n");

    char *strIntToRoman = intToRoman(388);
    printf("%s", strIntToRoman);

    printf("\n============================================================\n");

    struct ListNode *swapP = swapPairs(reCopy_1);
    while (swapP != NULL) {
        printf("%d", swapP->val);
        swapP = swapP->next;
    }

    printf("\n============================================================\n");












    return 0;
}
