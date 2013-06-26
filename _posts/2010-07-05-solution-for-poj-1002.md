---
layout: post
title: "POJ 1002 解题分析"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: 这道题使用排序来解决基本上一定会超时，使用二叉查找树来做就可以了
---
{% include JB/setup %}

[题目链接](http://poj.org/problem?id=1002)

## 解题分析

题目非常简单，可以分为以下三个步骤：

1. 对输入进行翻译（格式化）使之成为标准的电话号码格式XXX-XXXX
2. 统计电话号码的出现次数
3. 按照字典序升序输出重复出现的电话号码及其重复次数，没有重复的输出"No duplicates."

第一个步骤没有什么难度，遇到"-"就跳过，遇到数字直接减"0"就可以了，遇到字母就按照题目中的规则进行Map。

第二个步骤就出现分歧了，大约有以下三种可能：

1. 直接把解析后的电话号码插入到序列容器里，等到扫描完毕后对序列进行排序，然后统计并输出重复的电话号码
2. 使用哈希表存放包含电话号码和出现次数的一个数据结构，然后排序输出
3. 使用二叉查找树存放包含电话号码和出现次数的一个数据结构，然后遍历并输出

### 排序法

第一种方法最为直接，但是后来我从论坛里看到很多这么做的人都超时了。

个人感觉这种方法不会超时，不知道为什么那么多人卡住了。

使用数组作为容器，快速排序作为排序方法，分析一下时间复杂性：

假设每个输入的电话号码的长度不超过m，一共有n个电话号码

则解析一个电话号码的时间为O(m)，插入的时间为O(1)，排序的时间为O(nlogn)，最后统计加输出的时间为O(n)，

整体时间复杂度为![T(n)=n(O(m)+O(1))+O(n\log n)+O(n)=O(n\log n)](http://chart.apis.google.com/chart?cht=tx&chl=T%28n%29=n%28O%28m%29+O%281%29%29+O%28n\log%20n%29+O%28n%29=O%28n\log%20n%29)

给出伪代码：

	Procedure
	Begin
	    telNumberArray <- empty
	    For currentLine <- each line of input
	    Begin
	        telNumber <- parse tel number from currentLine
	        push telNumber at the end of telNumberArray
	    End For
	 
	    Quick sort telNumberArray
	 
	    duplicates <- 0
	    noDuplicates <- true
	    For i from 1 to n
	    Begin
	        If telNumberArray(i – 1) = telNumberArray(i)
	            duplictates <- duplicates + 1
	        Else If duplicates > 0
	        Begin
	            Print telNumberArray(i – 1) and duplicates
	            duplicates <- 0
	            noDuplicates <- false
	        End If
	 
	        If duplicates > 0
	        Begin
	            Print telNumberArray(i – 1) and duplicates
	            duplicates <- 0
	            noDuplicates <- false
	        End If
	    End For
	 
	    If noDuplicates
	        Print “No duplicates.”
	 
	End Procedure

### 哈希表

这是我最先想到的方法，不过在这里用起来感觉还不如其他两种方法爽。

总的来说就是把上一种方法中的数组换成哈希表，用于查找已经存在的电话号码。

哈希函数可以用电话号码右移若干位得到。（位移速度远快于取模运算）

时间复杂度与上一个方法在同一阶上，不过实际上应该会快不少。

### 平衡二叉查找树

在用哈希表解这道题的时候，我注意到一句话："Arrange the output lines by telephone number in ascending lexicographical order."

而二叉查找树遍历时恰好会产生一个有序的输出。

而且二叉查找树的查找性能虽然低于哈希表，但是仍有不俗的表现，因此我觉得用AVL来解这道题是比较好的。

平均插入时间为![O(n\log n)](http://chart.apis.google.com/chart?cht=tx&chl=O%28n\log%20n%29)，平均查找时间也为![O(n\log n)](http://chart.apis.google.com/chart?cht=tx&chl=O%28n\log%20n%29)，

总的时间复杂度![T(n) = n(O(m) + O(n\log n) + O(n\log n)) + O(n)](http://chart.apis.google.com/chart?cht=tx&chl=T%28n%29%20=%20n%28O%28m%29%20+%20O%28n\log%20n%29%20+%20O%28n\log%20n%29%29%20+%20O%28n%29)。

## 总结

从算法分析的结果来看，第一种方法反而是时空复杂度和实现难度比较优秀的一种解法，事实又一次证明维护有序比一次排序消耗要大得多。

汗，因此用第一种方法实现1002而超时的同学们应该好好检查一下自己的代码~

另外，还有一些同学和我一样，没有注意到"If there are no duplicates in the input print the line:No duplicates."而WA了，在此提醒大家注意审题T_T

## 额外补充

昨天晚上跟同学聊天，同学提到这道题如果用Trie树一定非常快。汗了，确实没有想到，不过用Trie树绝对是最快的了。对于这道题来说，Trie树插入时间和查找时间都是O(1)，然后遍历的时候就会得到一个有序的输出，总时间复杂度![T(n) = n(O(1)) + O(n) = O(n)](http://chart.apis.google.com/chart?cht=tx&chl=T%28n%29%20=%20n%28O%281%29%29%20+%20O%28n%29%20=%20O%28n%29)。

对不了解Trie树的同学有一幅图可以说明问题：

![Trie Tree Sample Image](http://images.cnblogs.com/cnblogs_com/HCOONa/WindowsLiveWriter/POJ1002_EE8C/image_thumb.png)
