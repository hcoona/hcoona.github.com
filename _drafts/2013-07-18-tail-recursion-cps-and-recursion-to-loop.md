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

接下来，我们来看一个比较复杂的例子：Fibonacci。（这里不考虑使用[动态规划 (Dynamic Programming)](http://en.wikipedia.org/wiki/Dynamic_Programming)对其进行优化的情况）

{% highlight fsharp %}
let fibonacci_cps n =
    let rec fibonacci_aux n cont =
        match n with
        | 1 | 2 -> cont 1
        | _ -> fibonacci_aux (n - 1) (fun acc1 acc2 -> 
{% endhighlight %}

## 递归转为循环

[^tail-recursion-optimization]: Tail call elimination allows procedure calls in tail position to be implemented as efficiently as goto statements, thus allowing efficient structured programming. In the words of Guy L. Steele "in general procedure calls may be usefully thought of as GOTO statements which also pass parameters, and can be uniformly coded as \[machine code\] JUMP instructions". [See wikipedia](http://en.wikipedia.org/wiki/Tail_call#History)
