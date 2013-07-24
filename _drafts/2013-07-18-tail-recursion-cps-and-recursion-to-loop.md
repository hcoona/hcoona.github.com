---
layout: post
title: "尾递归、CPS和将递归转化为循环的一般方法"
description: ""
category: "Functional Programming"
tags: ["Functional Programming", Algorithms]
excerpt: 本文介绍了什么是尾递归，什么是CPS，以及如何应用前两者将递归转化为循环。
---
{% include JB/setup %}

本文介绍了什么是尾递归，什么是CPS，以及如何应用前两者将递归转化为循环。

## 尾递归 (Tail Recursion)

在[Wikipedia](http://en.wikipedia.org/)中是这样定义尾递归的：

    尾部递归是一种编程技巧。递归函数是指一些会在函数内调用自己的函数，如果在递归函数中，递归调用返回的结果总被直接返回，则称为尾部递归。

尾递归相对于普通的递归有一个好处：由于在递归调用结束后没有任何其他的操作，直接将递归调用的结果作为当前调用的结果返回，因此不需要保存当前调用栈帧，在递归调用的时候，可以直接修改当前栈帧上的变量。[^tail-recursion-optimization]

总的来说，尾递归可以被认为等价于循环。

我们来看一个例子：阶乘函数。

{% highlight cpp %}
int factorial(int n) {
    assert (n >= 0);
    if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
{% endhighlight %}

这个函数不是尾递归的，因为它在调用完自己后，还要进行一些操作，比如说乘*n*。但是我们可以轻易的将其改写为尾递归的：

{% highlight cpp %}
static int factorial_recursive_aux(int acc, int n) {
    if (n) {
        return factorial_recursive_aux(acc * n, n - 1);
    } else {
        return acc;
    }
}

int factorial_recursive(int n) {
    assert (n >= 0);
    return factorial_recursive_aux(1, n);
}
{% endhighlight %}

通过引入一个参数`acc`，我们可以将原本在递归调用之后才进行的计算提前到递归调用时，从而将一个普通的递归调用转化为尾递归调用。根据上面提到的优化原则[^tail-recursion-optimization]，我们还可以这样优化这一尾递归调用函数：

{% highlight cpp %}
static int factorial_recursive_opt(int acc, int n) {
FRO_START:
    if (n) {
        acc *= n;
        n--;
        goto FRO_START;
    } else {
        return acc;
    }
}
{% endhighlight %}

容易发现，这样一个函数等价于这样一个循环：

{% highlight cpp %}
int acc = 1;
for (; n; n--) {
    acc *= n;
}
{% endhighlight %}

注意，虽然这里用一个`while (n--)`更为简洁，但是我还是特别写了一个`for`循环以跟上面的代码一一对应。这是因为，在有些情况下，判断和更改变量的步骤更为复杂，此时用一个`while`循环就比较困难了。

可惜将一个递归调用转换为尾递归调用并不总是这么容易。比如下面这个例子：

{% highlight cpp %}
int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    } else if (n == 1 || n == 2) {
        return 1;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}
{% endhighlight %}

再比如更加复杂的归并排序，快速排序。

因为下面的内容用函数式语言叙述更加方便，所以在此先给出Fibonacci函数对应的用FSharp语言写的代码：

{% highlight fsharp %}
let rec fibonacci n =
    match n with
    | _ when n <= 0 -> 0
    | 1 | 2 -> 1
    | _ -> (fibonacci (n - 1)) + (fibonacci (n - 2))
;;
{% endhighlight %}

## Continuation Passing Style (CPS)

在[Wikipedia](http://en.wikipedia.org/)中是这样定义CPS的：

    In functional programming, continuation-passing style (CPS) is a style of programming in which control is passed explicitly in the form of a continuation.

理论总是那么抽象，我们不妨来看一个例子：

{% highlight fsharp %}
let rec factorial n =
    match n with
    | _ when n < 0 -> failwith "n must not be less than 0."
    | 0 -> 1
    | _ -> n * (factorial (n - 1))
;;

let factorial_cps n =
    let rec factorial_aux n cont =
        match n with
        | 0 -> cont 1
        | _ -> factorial_aux (n - 1) (fun acc -> cont (acc * n))
    in
    if n < 0 then failwith "n must not be less than 0."
    else factorial_aux n (fun x -> x)
;;
{% endhighlight %}

可以看到，我们将原本会发生在递归调用之后进行的操作——乘`n`，通过Continuation的方式，保留到之后的某一时刻再进行调用（具体于这个例子而言，是在`n = 0`时）。并且在每次递归调用的时候，都将Continuation作为参数之一进行传递。这就是Continuation-passing style。

CPS的好处就是，能够将原本不是尾递归的普通递归调用转化为尾递归形式。至此，如何将一般递归转化为循环的方法，就比较明朗了。

接下来，我们来看一个比较复杂的例子[^lambda]：Fibonacci。（这里不考虑使用[动态规划 (Dynamic Programming)](http://en.wikipedia.org/wiki/Dynamic_Programming)对其进行优化的情况）

{% highlight fsharp %}
let fibonacci_cps n =
    let rec fibonacci_aux n cont =
        match n with
        | 1 | 2 -> cont 1
        | _ -> fibonacci_aux
                    (n - 1)
                    (fun acc1 -> fibonacci_aux
                                    (n - 2)
                                    (fun acc2 -> cont (acc1 + acc2)))
    in
    match n with
    | _ when n <= 0 -> 0
    | 1 | 2 -> 1
    | _ -> fibonacci_aux n (fun x -> x)
;;
{% endhighlight %}

回顾一下我们之前写的`fibonacci`函数，我们容易发现，唯一的不同之处就在于，我们使用`fibonacci_aux`函数来完成以前`fibonacci`函数的主要功能（递归）。而`fibonacci_aux`函数与原来的`fibonacci`函数的不同之处，除了没有处理`n <= 0`的情况以外，主要在于使用匿名函数将完成当前语句之后要做的事情作为参数进行传递。这样一来，通过将递归调用之后所做的操作通过参数进行传递，在执行完了所有的递归步骤之后，再进行计算，使得我们将一个复杂的普通递归调用转换成为了尾递归调用。

对于这点不明白的听众，不妨手工展开`n = 4`的情况。

    以下将fibonacci_aux简写为f
       f 4 (fun x -> x)
    => f 3 (fun x -> f 2 (fun y -> (fun xx -> xx) (x + y)))
    => f 3 (fun x -> f 2 (fun y -> x + y))
    => f 3 (fun x -> (fun y -> x + y) 1)
    => f 3 (fun x -> x + 1)
    => f 2 (fun x -> f 1 (fun y -> (fun xx -> xx + 1) (x + y)))
    => f 2 (fun x -> (fun y -> (fun xx -> xx + 1) (x + y)) 1)
    => f 2 (fun x -> (fun xx -> xx + 1) (x + 1))
    => (fun x -> (fun xx -> xx + 1) (x + 1)) 1
    => (fun xx -> xx + 1) 2
    => 3

## 递归转为循环

到这里，我们至少知道了在函数式编程语言中，如何将一个一般递归调用转换为尾递归调用。下面，我们讨论在C++和C语言中，应该怎么做。

对于C++11而言，由于引入了Lambda[^C++11-lambda]，使得理论上做这件事的难度和函数式编程语言差不多。

{% highlight cpp %}
template<typename _FContTy>
static int fibonacci_aux(int n, _FContTy cont)
{
    if (n == 1 || n == 2) {
        return cont(1);
    } else {
        return fibonacci_aux(n - 1, [=](int acc1) {
            return fibonacci_aux(n - 2, [=](int acc2) {
                return cont(acc1 + acc2);
            });
        });
    }
}

int fibonacci(int n)
{
    return fibonacci_aux(n, [](int x) { return x; });
}
{% endhighlight %}

但是实际上，这段代码无论是使用最新版本的Visual Studio[^latest-vs]或者是最新版本的G++[^latest-g++]都无法编译（表现为长时间无响应，占用内存疯狂增长）。但是这并不是编译器的Bug，而是因为这段递归程序在编译时期转换成仿函数(Functor)时会产生一个无穷递归，感兴趣的听众手工模拟一下编译器的工作就能发现这一点。况且实际上最好的情况下，也会有两个fibonacci_aux函数产生（因为有两个不同的lambda作为参数cont），使得生成的代码实际上不是尾递归调用（而是间接尾递归调用）。

[^tail-recursion-optimization]: Tail call elimination allows procedure calls in tail position to be implemented as efficiently as goto statements, thus allowing efficient structured programming. In the words of Guy L. Steele "in general procedure calls may be usefully thought of as GOTO statements which also pass parameters, and can be uniformly coded as \[machine code\] JUMP instructions". [See wikipedia](http://en.wikipedia.org/wiki/Tail_call#History).
[^lambda]: F#使用`fun`关键字来创建一个匿名函数。[See MSDN](http://msdn.microsoft.com/en-us/library/dd233201.aspx).
[^C++11-lambda]: [See C++11 FAQ](http://www.stroustrup.com/C++11FAQ.html#lambda).
[^latest-vs]: Visual Studio 2012 update 3. CL版本17.00.60610.1.
[^latest-g++]: g++ 4:4.7.2-1 in Debian jessie.
