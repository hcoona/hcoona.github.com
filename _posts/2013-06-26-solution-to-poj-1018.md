---
layout: post
title: "Solution to POJ 1018"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: Enumerate all possible bandwidth would make the problem easier to solve.
---
{% include JB/setup %}

The problem is on [here](http://poj.org/problem?id=1018).

## Analysis

There are n group of devices. We should take one and only one device from any group. 

Firstly I think it is another [Dynamic Programming](http://en.wikipedia.org/wiki/Dynamic_programming) problem, because what I would choose in each group depends on what I already took in before groups. However, it is not. Because what device I take now would affect the result before. For example, if I took a device with a very low bandwidth, the result before might not be optimal then. If I want to use Dynamic Programming, I have to say, when I already take several devices to make the total price rise to P and bandwidth limited to B, I would choose a optimal device in current group together with the optimal devices I choose before under new limitation P' and B'. That's too much complex. What's worse, the combination of P would be very very large. Under the worst case, it would be ![\prod_{0< i \leq n} m_i \leq 100^{100}](http://chart.apis.google.com/chart?cht=tx&chl=\prod_{0%3C%20i%20\leq%20n}m_i%20\leq%20100^{100}).

Obviously, we cannot use Dynamic Programming to solve this problem. What make it not is we need to judge the optimal with B/P, which depends on both B and P, and the B is the minimum of the bandwidths which means it depends on all possilbe groups.

However, we now the possible final bandwidths are very limited, with no more than the count of all devices which is ![\sum_{0&lti\leq n} m_i \leq 10000](http://chart.apis.google.com/chart?cht=tx&chl=\sum_{0%3Ci\leq%20n}m_i%20\leq%2010000). If we fix the final bandwidth, the problem would turn to find the minimum total price P, which is a very simple problem.

## Solution

1. Enumerate all possible bandwidths.
2. For each possible bandwidth, find the minimum total price P.
3. Find the maximum B/P for each optimal P and B.

In the second step, we can follow these steps.

1. Sort devices in each group by its bandwidth. (Do only once, hence should be done before second step above.)
2. Find the slice of each group whose bandwidth all be equal or greater than specified bandwidth B.
3. Find the minimum price P in each sliced group.
4. Sum the total price.

Code is [here]({{ BASE_URL }}/assets/POJ1018.cpp).
