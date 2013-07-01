---
layout: post
title: "Solution to POJ 1009"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: 关键在于一定不能使用模拟法，并且要对几种情况进行有效的优化。
---
{% include JB/setup %}

关键在于一定不能使用模拟法，并且要对几种情况进行有效的优化。

原题链接：[POJ 1009 Edge Detection](http://poj.org/problem?id=1009)。

## 问题分析

题目本身很明确，就不多说了。关键在于输入和输出都是通过RLE的，而且run length还有可能长达![10^9](http://chart.apis.google.com/chart?cht=tx&chl=10^9)。题目对于图像的宽度没有规定范围，所以认为是从1（只有一列像素）到![10^9](http://chart.apis.google.com/chart?cht=tx&chl=10^9)（只有一行像素）。题目最后的Hint又说：A brute force solution that attempts to compute an output value for every individual pixel will likely fail due to space or time constraints. 这就构成了一个很强的提示。

因此，比较明确的有两点：
1. 不能使用模拟法。
2. 不能对每一个像素进行计算。

## 解题分析

首先来看第一点：不能使用模拟法。模拟法是指，使用三个数组分别记录上一行的像素，当前行的像素，以及下一行的像素。由于存在只有一行，长达![10^9](http://chart.apis.google.com/chart?cht=tx&chl=10^9)的可能性，因此行不通。

经过一番思索，可以发现，通过在RLE对上添加上位置信息（比如说index），可以在不展开RLE编码的情况下，得到任意给定位置的像素值。

接下来再看第二点：不能对每一个像素进行计算。通过仔细的观察Description节中的样例，可以发现有时接下来的若干像素的输出值都等于当前这一像素的计算结果。这种情况发生的条件是，接下来若干像素的相邻的8个像素值都和当前这一像素的相邻的8个像素的值相同。换句话说，他们的上面的邻居都落在同一个RLE对中，下面的邻居也落在同一个RLE对中，并且他们的左右邻居都相等。特别需要注意只有一列像素和只有两列像素的便捷情况。

## 关键代码

对应着问题一：

{% highlight cpp %}
struct ExtendedRLE {
    int index;
    int value;
    int length;
    ExtendedRLE(index, value, length)
        : index(index), value(value), length(length)
    {}
};

...

const ExtendedRLE UP_BOUNDARY_RLE;
const ExtendedRLE LEFT_RIGHT_BOUNDARY_RLE;
const ExtendedRLE DOWN_BOUNDARY_RLE;

ExtendedRLE pixels[INPUT_RLE_COUNT_MAX];

...

ExtendedRLE * GetRLEItByIndex(int index, int width) {
    if (index < 0) {
        return &UP_BOUNDARY_RLE;
    } else if (index % width == 0 || (index + 1) % width == 0) {
        return &LEFT_RIGHT_BOUNDARY_RLE;
    } else if (index >= EOI) {
        return &DOWN_BOUNDARY_RLE;
    } else {
        return upper_bound(
            &pixels[0],
            &pixels[pixelsSize],
            ExtendedRLE(index, -1, 0),
            PredByIndex) - 1;
    }
}
{% endhighlight %}

对应着问题二：

{% highlight cpp %}

if (IsCurrentInRLEMiddle(surroundingIts, width)) {
    int l = GetMinimumSameLength(index, currentIt, surroundingIts, width);
    if (l == 0) {
        l = 1;
    }
    PushOutputValue(stack, outputValue, l);
    index += l;
} else {
    PushOutputValue(stack, outputValue, 1);
    index++;
}

if (index >= currentIt->index + currentIt->length) {
    currentIt++;
}

...

bool IsCurrentInRLEMiddle(ExtendedRLE * surroundingIts[NEIGHBOURS_COUNT], int width) {
    if (width == 1) {
        return surroundingIts[SURROUNDING_MIDDLE_UP] == surroundingIts[SURROUNDING_MIDDLE_DOWN];
    } else if (width == 2) {
        ...
    } else {
        return surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_RIGHT_MIDDLE]
        && surroundingIts[SURROUNDING_LEFT_UP] == surroundingIts[SURROUNDING_MIDDLE_UP]
        && surroundingIts[SURROUNDING_LEFT_UP] == surroundingIts[SURROUNDING_RIGHT_UP]
        && surroundingIts[SURROUNDING_LEFT_DOWN] == surroundingIts[SURROUNDING_MIDDLE_DOWN]
        && surroundingIts[SURROUNDING_LEFT_DOWN] == surroundingIts[SURROUNDING_RIGHT_DOWN];
    }
}

void PushOutputValue(vector<pair<int, int> > &stack, int value, int length) {
    if (!stack.empty() && stack.back().first == value) {
        stack.back().second += length;
    } else {
        stack.push_back(make_pair(value, length));
    }
}
{% endhighlight %}

注意不要立即将结果输出出去，而是要经过一个合并的步骤，以免输出序列中相邻的两个RLE具有相同的像素值。

[Code]({{ BASE_URL }}/assets/POJ1009.cpp)
