---
layout: post
title: "POJ 1001 解题分析"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: 这道题做了很多很多天，实在是非常不爽。一向最恶心高精度计算的题了，等有空了就写套类库出来，以后碰见就用=w=
---
{% include JB/setup %}

这道题做了很多很多天，实在是非常不爽。一向最恶心高精度计算的题了，等有空了就写套类库出来，以后碰见就用=w=

[题目链接](http://poj.org/problem?id=1001)

## 解题分析

如果不算高精度的话，实际上是一道非常简单的题o(╯□╰)o，看到好多Java的同学用BigDecimal类直接就AC了，⊙﹏⊙b汗。

难度可以说几乎没有，做这道题才发现其实POJ限制相当松啊，内存居然给10M，时间500MS。

个人觉得本着对自己认真负责的原则，该优化的地方还是要优化一下的，而这道题最值得优化的就是乘方部分：

![R^{n}=\lbrace{}R\times{}R^{n-1},\when{}n是奇数\or{}R^{n/2}\times{}R^{n/2},\when{}n是偶数]({{ BASE_URL }}/assets/POJ1001-eq1.png)

这样能够大幅度减少乘法的次数。

由于输入的数既有小数又有整数，为了方便处理，我们可以将所有的输入都转换为整数，并记录小数点的位置；待计算完毕后，再添加上小数点。

下面给出伪代码：

	Procedure R, n Begin
	    Shift R to get an integer IntR and e which present where the dot is.
IntResult <- BigIntegerPow(IntR, n)
	    e <- e * n;
	    Shift IntResult by e bit to get an BigDecimal result.
	    Trim the result to remove zero at front of integer and zero at end of decimal.
	End Procedure

## 优化余地

做乘法的时候，可以把乘数分为若干段，分别相乘后再相加。

还可以在一开始转换的时候，就不用字符串存储，而用若干个long分别存储若干段。

## 总结

我只能说，这道题的数据和格式要求非常变态，真的非常非常变态╮(╯▽╰)╭。下面给不明真相的同学一套测试数据：

[POJ1001TestCase.zip](http://files.cnblogs.com/HCOONa/POJ1001TestCase.zip)
