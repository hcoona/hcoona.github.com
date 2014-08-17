---
layout: post
title: "检查加法溢出的几种方法的比较"
description: ""
category: tips
tags: [Tips]
excerpt: 实际使用中会有需要提前检测出是否会发生算数溢出，C# 语言中提供了 checked 关键字来解决这一问题，C/C++ 语言又该怎样做呢？本文从汇编语句的层面，对几种检查加法溢出的方法进行了比较和分析。受实验环境和个人知识所限，本文的应用范围仅限于 X86-64 体系结构。
---
{% include JB/setup %}

实际使用中会有需要提前检测出是否会发生算数溢出，C# 语言中提供了 checked 关键字来解决这一问题，C/C++ 语言又该怎样做呢？本文从汇编语句的层面，对几种检查加法溢出的方法进行了比较和分析。受实验环境和个人知识所限，本文的应用范围仅限于 X86-64 体系结构。

但是需要注意的是，即便是其他的体系结构，也很有可能提供了类似的指令用于检测溢出位。\[1\]

为方便起见，下面讨论将以两个 32 bit 有符号整数的加法操作为例。对于无符号整数的操作，只需检查 Carry 位即可。\[2\]

本文中用到的方法有三种：

1. 使用嵌入汇编代码检查处理器的 Overflow 标记；
1. 将 32 位加法转换为 64 位加法，然后再与 `INT_MAX` 或 `INT_MIN` 进行比较；
1. 对运算结果所在的区间进行判断和验证。

## 原理简述

### 嵌入汇编检查处理器 Overflow 标记

该方法的理论基础是这样的：\[3\]

    The status register in a traditional processor design includes at least three central flags: Zero, Carry, and Overflow, which are set or cleared automatically as effects of arithmetic and bit manipulation operations.

在 X86 体系结构中，我们可以利用这些标记来判断最近的一次算数操作是否发生了溢出。特别的，X86 提供了 `JO` 和 `JNO` 指令，根据 Overflow 标记进行条件跳转。\[4\]

可惜事情并不是总这么美好，有时编译器会使用 `LEA` 指令对算数操作进行优化。由于 `LEA` 指令不是一个算数操作指令，所以并不会设置 Overflow 标记，从而会令我们这种检测方式失效。一个折中的解决方法是，使用嵌入汇编指令，强制编译器生成 `ADD` 指令进行加法运算。

### 将 32 位加法转换为 64 位加法，然后再与 `INT_MAX` 或 `INT_MIN` 进行比较

显然，32 位加法操作至多可能生成 33 位的结果，X86 处理器中提供对 64 位整数运算的一些支持，因此我们可以使用 64 位整数运算进行计算，然后将运算结果与 32 位整数的边界值进行比较，从而判断溢出。

### 对运算结果所在的区间进行判断和验证

由于补码的定义，导致一旦发生溢出，结果的区间是可以预先判断的。如果两个正整数相加发生了溢出，则结果一定小于其中任一操作数；如果两个负整数相加发生了溢出，则结果一定大于其中任一操作数。\[5\]

根据这一原理，我们可以对结果所在的区间进行验证，从而得知是否发生了溢出。

## 实验代码简述

为了最大程度发挥编译器优化的优势，我构造了以下代码用于实验：

{% highlight cpp %}
inline int Add1(int a, int b)
{
    asm ("addl %1, %0"
        : "=r" (a)
        : "r" (a), "r" (b));
    asm goto ("jo %l0"
        : /* no output */
        : /* no input */
        : /* no clobber */
        : OVERFLOW);
    return a;

OVERFLOW:
    exit(1);
}

inline int Add2(int a, int b)
{
    long long tmp = (long long)a + (long long)b;
    if (tmp <= (long long)INT_MAX && tmp >= (long long)INT_MIN) {
        return (int)tmp;
    } else {
        exit(1);
    }
}

inline int Add3(int a, int b)
{
    int tmp = (int)((unsigned int)a + (unsigned int)b);

    if (a >= 0) {
        if (b >= 0 && tmp < a) {
            exit(1);
        }
    } else {
        if (b <= 0 && tmp > a) {
            exit(1);
        }
    }

    return tmp;
}

void T1(void) {
    int a, b;
    scanf("%d%d", &a, &b);
    int c = Add1(a, b);
    printf("%d + %d = %d\n", a, b, c);
}

void T2(void) {
    int a, b;
    scanf("%d%d", &a, &b);
    int c = Add2(a, b);
    printf("%d + %d = %d\n", a, b, c);
}

void T3(void) {
    int a, b;
    scanf("%d%d", &a, &b);
    int c = Add3(a, b);
    printf("%d + %d = %d\n", a, b, c);
}
{% endhighlight %}

此处的 `Add1`，`Add2` 和 `Add3` 方法分别对应上面提出的三种方法的 C 语言实现。使用 `inline` 标记指示编译器进行内联以进行更深层次的优化。其中 `Add2` 和 `Add3` 方法参考了微软的 SafeInt 库。\[6\]

`T1`，`T2` 和 `T3` 方法分别对应了三个测试方法。此处没有直接为 `a` 和 `b` 指定一个值，而是使用 `scanf` 函数进行输入，以防止编译器在优化时直接进行离线计算，将生成的结果直接写入目标位置。

## 汇编代码分析

我使用下面的语句生成汇编代码：

    gcc -O3 -march=native -S test.c -o test.s

顺便说一下我的实验环境：

- CPU: Intel Core2 P7450
- GCC: 4.8.2

由于完整的代码比较长，这里只摘录部分关键代码。`T1` 生成的代码中，与 `Add1` 相关的代码（内联后）片段如下：

{% highlight gas %}
	movl 12(%rsp), %edx
	movl 8(%rsp), %esi
	addl %esi, %ecx
	jo   .L26
	# No Overflow
.L26:
	# Overflow
{% endhighlight %}

很简单，这段汇编代码就是将 `scanf` 函数的结果存到寄存器中，然后执行 `addl` 运算，之后（根据溢出位的设置情况）进行条件跳转。

`T2` 生成的代码中，与 `Add2` 相关的代码（内联后）片段如下：

{% highlight gas %}
	movl   12(%rsp), %edx
	movl   $2147483648, %edi
	movl   8(%rsp), %esi
	movslq %edx, %rax
	movslq %esi, %rcx
	addq   %rax, %rcx
	movl   $4294967295, %eax
	addq   %rcx, %rdi
	cmpq   %rax, %rdi
	ja     .L30
	# No Overflow
.L30:
	# Overflow
{% endhighlight %}

`movslq` 可以将 `long` 长度的数据移动并进行符号位扩展到 `quad` 长度的位置。这段代码首先将 `scanf` 的结果取出到 32 位寄存器 `edx` 和 `esi` 中，然后又分别将结果进行符号扩展并移动到 64 位寄存器 `rax` 和 `rcx` 中。接下来进行 64 位加法运算，结果保存在 `rcx` 寄存器中。然后编译器做了一个人脑看来相当复杂的优化，我证明了这个优化是正确的，但是不准备在这里详细展开。

`T3` 生成的代码中，与 `Add3` 相关的代码（内联后）片段如下：

{% highlight gas %}
	movl	8(%rsp), %esi
	movl	12(%rsp), %edx
	testl	%esi, %esi
	leal	(%rdx,%rsi), %ecx
	js	.L34
	cmpl	%ecx, %esi
	jg	.L45
.L35:
    # No Overflow
.L34:
	cmpl	%ecx, %esi
	jge	.L35
	movl	%edx, %eax
	shrl	$31, %eax
	testb	%al, %al
	je	.L35
.L36:
    # Overflow
.L45:
	testl	%edx, %edx
	js	.L35
	jmp	.L36
{% endhighlight %}

不用分析也知道，这段代码比较复杂，运行效率不会比前两段代码效率更高。

## 实验设计简述

使用预先生成的随机数，分别调用 `Add1`，`Add2`，`Add3` 以及不检查溢出直接进行加法运算，统计时间性能。此时，需对这几个函数稍作修改，不能在发生溢出时调用 `exit` 函数，而是使用 `setjmp` 和 `longjmp` 进行处理。

主要代码如下：

{% highlight cpp %}
{% endhighlight %}

## 实验结果分析

## 总结

## 参考资料

\[1\] ARM Processors: Condition Codes 1: Condition Flags and Codes. July, 2010. http://community.arm.com/groups/processors/blog/2010/07/16/condition-codes-1-condition-flags-and-codes

\[2\] Ian! D. Allen. The CARRY flag and OVERFLOW flag in binary arithmetic. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

\[3\] Status register. http://en.wikipedia.org/wiki/Status_register

\[4\] X86 instruction listings. http://en.wikipedia.org/wiki/X86_instruction_listings

\[5\] Randal E. Bryant and David R. O'Hallaron. Computer System: A Programmer's Perspective, 2ed. Addison-Wesley. February 2010.

\[6\] SafeInt. http://safeint.codeplex.com/
