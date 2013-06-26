---
layout: post
title: "Solution for POJ 1045"
description: ""
category: ACM
tags: [ACM, POJ]
excerpt: The key is to derivate the fomular.
---
{% include JB/setup %}

The problem is on [here](http://poj.org/problem?id=1045).

## Analysis

It is hard to go with the direct fomular, because there is no build-in way to calculate a differential. However, we can apporach it with the defination of differential: ![\frac{df(x)}{dx} = \lim_{\Delta x \to +\infty}\frac{f(x+\Delta x)-f(x)}{\Delta x}](http://chart.apis.google.com/chart?cht=tx&chl=\frac{df%28x%29}{dx}%20=%20\lim_{\Delta%20x%20\to%2B\infty}\frac{f%28x%2B\Delta%20x%29-f%28x%29}{\Delta%20x}). Usually make ![\Delta x](http://chart.apis.google.com/chart?cht=tx&chl=\Delta%20x) equal to DOUBLE\_EPSILON.

The first thing we should do is to simplification the fomulars. From ![v_1=V_S\cos{\omega t}](http://chart.apis.google.com/chart?cht=tx&chl=v_1=V_S\cos{\omega%20t}), ![v_2=V_R\cos(\omega t + \theta)](http://chart.apis.google.com/chart?cht=tx&chl=v_2=V_R\cos%28\omega%20t%2B\theta%29), ![v_2=iR](chart.apis.google.com/chart?cht=tx&chl=v_2=iR), and ![i=C\frac{d(v_1-v_2)}{dt}](http://chart.apis.google.com/chart?cht=tx&chl=i=C\frac{d%28v_1-v_2%29}{dt}), we could come out an equation shown below:

![V_R\cos(\omega t+\theta)=CR(\frac{d v_1}{dt}-\frac{d v_2}{dt})](http://chart.apis.google.com/chart?cht=tx&chl=V_R\cos%28\omega%20t%2B\theta%29=CR%28\frac{d%20v_1}{dt}-\frac{d%20v_2}{dt}%29)

After simplification, it would turn into the equation shown below:

![V_R\cos(\omega t+\theta)=\omega CR\[V_R\sin(\omega t+\theta)-V_S\sin{\omega t}\]](http://chart.apis.google.com/chart?cht=tx&chl=V_R\cos%28\omega%20t%2B\theta%29=\omega%20CR%5BV_R\sin%28\omega%20t%2B\theta%29-V_S\sin{\omega%20t}%5D)

When ![\omega t+\theta=0](http://chart.apis.google.com/chart?cht=tx&chl=\omega%20t%2B\theta=0), it would show us:

![V_R=\omega CRV_S\sin\theta](http://chart.apis.google.com/chart?cht=tx&chl=V_R=\omega%20CRV_S\sin\theta)

When ![\omega t=0](http://chart.apis.google.com/chart?cht=tx&chl=\omega%20t=0), it would show us:

![V_R\cos\theta=\omega CRV_R\sin\theta](http://chart.apis.google.com/chart?cht=tx&chl=V_R\cos\theta=\omega%20CRV_R\sin\theta)

which would further lead to ![\tan\theta=\frac{1}{\omega CR}](http://chart.apis.google.com/chart?cht=tx&chl=\tan\theta=\frac{1}{\omega%20CR}).

As a consequence, we know how to calculate ![V_R](http://chart.apis.google.com/chart?cht=tx&chl=V_R).

## Easy Solution

{% highlight cpp %}
#include <cstdio>
#include <cmath>

int main(void)
{
    double Vs, R, C;
    int n;
    scanf("%lf%lf%lf%d", &Vs, &R, &C, &n);
    while(n--) {
        double omega;
        scanf("%lf", &omega);
        double CRw = C * R * omega;
        double phi = atan(1.0 / CRw);
        printf("%.3lf\n", CRw * Vs * sin(phi));
    }
    return 0;
}
{% endhighlight %}

Please pay more attention about "... output should be rounded to three digits after the decimal point.".
