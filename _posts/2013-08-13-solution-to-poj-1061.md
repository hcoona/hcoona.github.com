---
layout: post
title: "Solution to POJ 1061"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: 将问题转换为二元线性丢番图方程，然后运用扩展欧几里德算法求解。
---
{% include JB/setup %}

将问题转换为二元线性丢番图方程，然后运用扩展欧几里德算法求解。

原题链接：[POJ 1061 青蛙的约会](http://poj.org/problem?id=1061)。

## 问题分析

题目相当于在GF(L)上的追及问题，但是又跟传统的追及问题有少许不同。传统的这类追及问题是在环形跑道上两个人同向跑步，只要两个人的速度不同，总有追上的时候。但是青蛙约会这一问题不一样，青蛙每次跳动是离散的运动，而不像人跑步是连续运动。这就使得，即使两只青蛙的速度不同，也有可能无法相遇，例如x=1，y=2，m=2，n=4，L=4时，两只青蛙就永远都不会相遇。另一方面，环形跑道跑步的问题中，如果速度快的人在后面，则追及距离就是他们之间的距离，否则追及距离就是跑道长度减去它们之间的距离。但是青蛙约会问题不同，追及距离并不能够确定，因为有可能在下一个连续运动的相遇点他们都无法相遇，这使得问题大为复杂。这两点不同，使得我们不能够用传统追及问题的解决方法来解这道题。

## 解题分析

既然无法用追及问题的解决方案，我们就只能用“笨办法了”。首先，把所有的已知条件利用起来，可以得到如下等式（其中x，y，m，n，L均为整数常数）：

![x+mk \equiv y+nk \pmod{L}, k \in \mathbb{Z}]({{ BASE_URL }}/assets/P1061-eq1.png)

经过一定变换，可以得到这样的等式：

![(m-n)k+Lq=y-x, k,q\in\mathbb{Z}]({{ BASE_URL }}/assets/P1061-eq2.png)

我们要求的则是k的最小正整数解。

做变换![a=m-n，b=L，c=y-x，x=k，y=q]({{ BASE_URL }}/assets/P1061-eq3.png))则上述二元一次不定方程可以表示为如下形式：

![ax+by=c]({{ BASE_URL }}/assets/P1061-eq4.png)

根据[裴蜀定理](https://en.wikipedia.org/wiki/B%C3%A9zout%27s_identity)，当且仅当![gcd(a,b) | c]({{ BASE_URL }}/assets/P1061-eq5.png)时，方程有解，且有无穷多组解。此时，若有一组特解![x_0]({{ BASE_URL }}/assets/x_0.png)，![y_0]({{ BASE_URL }}/assets/y_0.png)，则通解为：

![x=x_0+b/d*t,y=y_0-a/d*t]({{ BASE_URL }}/assets/P1061-eq6.png)

其中t是整数。

我们想要求x的最小正整数解，不妨设![x>=0]({{ BASE_URL }}/assets/x_geq_0.png)，则如果![b/d>0]({{ BASE_URL }}/assets/P1061-eq7.png)，![t<=-x\_0\*d/b]({{ BASE_URL }}/assets/P1061-eq8.png)，否则![t>=-x\_0\*d/b]({{ BASE_URL }}/assets/P1061-eq9.png)。

## 关键代码

{% highlight cpp %}
int a = m - n, b = L, c = y - x;
int x0, gcd;
tie(x0, ignore, gcd) = Extended_GCD(a, b);
if (a && (c % gcd == 0)) {
    int scale = c / gcd;
    double t = -static_cast<double>(x0) * c / b;
    int x = x0 * scale + b / gcd * static_cast<int>(floor(t));
    return x < 0 ? (x + b / gcd) : x;
} else if (c % L == 0) {
    return 0;
} else {
    return INVALID;
}
{% endhighlight %}

如果![b/d>0]({{ BASE_URL }}/assets/P1061-eq7.png)，则![t<=-x\_0\*d/b]({{ BASE_URL }}/assets/P1061-eq8.png)，使用`floor`函数就是正确的；否则![t>=-x\_0\*d/b]({{ BASE_URL }}/assets/P1061-eq9.png)，应该使用`ceil`函数，但是由于`ceil`函数的结果和`floor`函数的结果只差`1`，因此可以直接加上相差的部分。

## 参考资料

- [丢番图方程](https://en.wikipedia.org/wiki/Diophantine_equation)
- [扩展欧几里德算法](https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)
- [裴蜀定理](https://en.wikipedia.org/wiki/B%C3%A9zout%27s_identity)
- [Linear Diophantine Equation (PDF) Theorem 4.6](http://www-groups.mcs.st-andrews.ac.uk/~martyn/teaching/1003/1003linearDiophantine.pdf)
