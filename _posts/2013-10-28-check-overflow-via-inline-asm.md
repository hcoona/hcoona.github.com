---
layout: post
title: "使用内嵌汇编检查溢出"
description: ""
category: tips
tags: [Tips,Interview]
excerpt: 在面试中经常考到，实际项目中也经常需要面临这样一个问题，即如何判断一个整数操作是否发生了溢出。C#语言提供了checked关键字用于处理这一问题，在C语言中我们可以使用内嵌汇编语句来检查溢出。
---
{% include JB/setup %}

在面试中经常考到，实际项目中也经常需要面临这样一个问题，即如何判断一个整数操作是否发生了溢出。C#语言提供了checked关键字用于处理这一问题，在C语言中我们可以使用内嵌汇编语句来检查溢出。

    In computer processors, the overflow flag (sometimes called V flag) is usually a single bit in a system status register used to indicate when an arithmetic overflow has occurred in an operation, indicating that the signed two's-complement result would not fit in the number of bits used for the operation (the ALU width).

C语言中不能够直接访问寄存器，但是可以通过内嵌汇编语言，来访问寄存器的值。不过这样做有一点缺陷是，汇编代码与计算机体系结构相关，在可移植性上需要更多考虑。

经过查资料，在X86体系结构中，可以通过汇编指令JO和JNO来实现基于Overflow flag的条件跳转。JO (Jump on Overflow)意味着有溢出时跳转，JNO (Jump on Not Overflow)则反之。下面通过一个小例子来解释如何进行溢出检查。

输入：一个符合`[+|-]?\n+`正则表达式的字符串。

输出：输入的字符串是否可以转换为一个`int`型常量而不发生溢出。

{% highlight cpp %}
bool TryParseInt(const char *str, int *pn)
{
    bool isAddOp = true;
    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        isAddOp = false;
        str++;
    }

    int n = 0;
    while (*str != '\0') {
        n *= 10;
        __asm__ goto ("jo %l0"
            : /* no output */
            : /* no input */
            : /* no clobber */
            : PARSE_INT_FAILD
        );
        if (isAddOp) {
            n += *str - '0';
        } else {
            n -= *str - '0';
        }
        __asm__ goto ("jo %l0"
            : /* no output */
            : /* no input */
            : /* no clobber */
            : PARSE_INT_FAILD
        );
        str++;
    }
    *pn = n;
    return true;

PARSE_INT_FAILD:
    return false;
}
{% endhighlight %}
