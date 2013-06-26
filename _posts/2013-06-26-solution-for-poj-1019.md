---
layout: post
title: "Solution for POJ 1019"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: Easy but need more patient.
---
{% include JB/setup %}

The problem is on [here](http://poj.org/problem?id=1019).

## Analysis

![S_k=\bar{123\dots k}](http://chart.apis.google.com/chart?cht=tx&chl=S_k=\bar{123\dots%20k})

![S_{S_k}=\bar{S_1S_2\dots S_k}](http://chart.apis.google.com/chart?cht=tx&chl=S_{S_k}=\bar{S_1S_2\dots%20S_k})

If we want to know what digit is on specified position, we need to locate the ![S_{S_n}](http://chart.apis.google.com/chart?cht=tx&chl=S_{S_n}) which contains what we want, and further need to locate the ![S_m](http://chart.apis.google.com/chart?cht=tx&chl=S_m) in ![S_{S_n}](http://chart.apis.google.com/chart?cht=tx&chl=S_{S_n}) which contains what we want, and finally the *k* in ![S_m](http://chart.apis.google.com/chart?cht=tx&chl=S_m). As a result, we need the length of ![S_{S_k}](http://chart.apis.google.com/chart?cht=tx&chl=S_{S_k}) and the length of ![S_k](http://chart.apis.google.com/chart?cht=tx&chl=S_k).

![|S_k|=\sum_{1 \leq l \leq k}{l \times \text{Counts of number with %24l%24 digits}}](http://chart.apis.google.com/chart?cht=tx&chl=|S_k|=\sum_{1\leq%20l\leq%20k}{l\times\text{Counts%20of%20number%20with%20$l$%20digits}})

![|S_{S_k}|=\sum_{1 \leq l \leq k}{|S_l|}](http://chart.apis.google.com/chart?cht=tx&chl=|S_{S_k}|=\sum_{1%20\leq%20l%20\leq%20k}{|S_l|})

We can firstly calculate some of ![|S_k|](http://chart.apis.google.com/chart?cht=tx&chl=|S_k|), and then the ![|S_{S_k}|](http://chart.apis.google.com/chart?cht=tx&chl=|S_{S_k}|) until the maximum value less than [**INT_MAX**](http://msdn.microsoft.com/en-us/library/296az74e(v=vs.110).aspx). We can use this predication to examine the overflow of add operator: <code>sum + k < sum</code>. As the arrays containing ![|S_k|](http://chart.apis.google.com/chart?cht=tx&chl=|S_k|) and ![|S_{S_k}|](http://chart.apis.google.com/chart?cht=tx&chl=|S_{S_k}|) are naturally sorted, we can use binary search to improve the performance. Finally, we just need to know the digit in specified position of a number *k*. It's a easy job and can be approached by converting *k* into string and seek its specified position.

## Solution

After some experiences, I know only first 31267 of ![|S_{S_k}|](http://chart.apis.google.com/chart?cht=tx&chl=|S_{S_k}|) need to be calculated.

{% highligh cpp %}
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace std;

const int MaxNumberCountByBit[] = {
    0, 9, 90, 900, 9000, 90000, 900000, 9000000, 90000000, 900000000
};
const int MinNumberByBit[] = {
    0, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000
};

const int MaxK = 31267;
int LengthOfSkTable[MaxK + 1] = { 0 };
int LengthOfSSkTable[MaxK + 1] = { 0 };

int GetNumberBits(int number)
{
    int bits = 0;
    for(; number; number /= 10) {
        bits++;
    }
    return bits;
}

int LengthOfSk(int k)
{
    int bits = GetNumberBits(k);
    int sum = 0;
    for (int i = 1; i < bits; i++) {
        sum += i * MaxNumberCountByBit[i];
    }
    sum += (k - MinNumberByBit[bits] + 1) * bits;
    return sum;
}

void Initialize(void)
{
    for (int k = 1, sum = 0; k <= MaxK; k++) {
        int l = LengthOfSk(k);
        LengthOfSkTable[k] = l;
        LengthOfSSkTable[k] = (sum += l);
    }
}

int Solve(int position)
{
    int *previousSSkLengthIt = lower_bound(&LengthOfSSkTable[1], &LengthOfSSkTable[MaxK + 1], position) - 1;
    int positionInSk = position - *previousSSkLengthIt;

    int *previousSkLengthIt = lower_bound(&LengthOfSkTable[1], &LengthOfSkTable[MaxK + 1], positionInSk) - 1;
    int positionInKK = positionInSk - *previousSkLengthIt;
    int kk = distance(&LengthOfSkTable[0], previousSkLengthIt) + 1;

    stringstream ss;
    ss << kk;
    char number = ss.str().at(positionInKK - 1);

    return number - '0';
}

int main(void)
{
    Initialize();

    int t;
    cin >> t;
    while (t--) {
        int i;
        cin >> i;
        cout << Solve(i) << endl;
    }

    return 0;
}
{% endhighlight %}