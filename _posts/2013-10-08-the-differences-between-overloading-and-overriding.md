---
layout: post
title: "重载和覆盖的区别"
description: ""
category: interview
tags: [Interview]
excerpt: 重载和覆盖是笔试和面试中经常考的两个概念。这两个概念从概念本身到实现机理都不相同，但是表现形式却有相似之处。本文从实现机理入手，深入讲解两者的不同之处。
---
{% include JB/setup %}

<style type="text/css">
table {
    margin: 0 auto;
    text-align: center;
    border-collapse: collapse;
}

td {
    padding: 0 5px;
}

table, td, th {
    border: 1px solid;
}
</style>

重载(Overloading)和覆盖(Overriding)是笔试和面试中经常考的两个概念。这两个概念从概念本身到实现机理都不相同，但是表现形式却有相似之处。本文从实现机理入手，深入讲解两者的不同之处。

## 重载和覆盖的概念

重载(Overloading)主要指函数重载或方法重载。

    Function overloading or method overloading is a feature found in various programming languages such as Ada, C++, C#, D, and Java, that allows creating several methods with the same name which differ from each other in the type of the input and the output of the function. It is simply defined as the ability of one function to perform different tasks.

覆盖(Overriding)则是面向对象技术中特有的一个概念。

    Method overriding, in object oriented programming, is a language feature that allows a subclass or child class to provide a specific implementation of a method that is already provided by one of its superclasses or parent classes. The implementation in the subclass overrides (replaces) the implementation in the superclass by providing a method that has same name, same parameters or signature, and same return type as the method in the parent class. The version of a method that is executed will be determined by the object that is used to invoke it. If an object of a parent class is used to invoke the method, then the version in the parent class will be executed, but if an object of the subclass is used to invoke the method, then the version in the child class will be executed. Some languages allow a programmer to prevent a method from beinMethod overriding, in object oriented programming, is a language feature that allows a subclass or child class to provide a specific implementation of a method that is already provided by one of its superclasses or parent classes. The implementation in the subclass overrides (replaces) the implementation in the superclass by providing a method that has same name, same parameters or signature, and same return type as the method in the parent class. The version of a method that is executed will be determined by the object that is used to invoke it. If an object of a parent class is used to invoke the method, then the version in the parent class will be executed, but if an object of the subclass is used to invoke the method, then the version in the child class will be executed. Some languages allow a programmer to prevent a method from being overridden.g overridden.

## 重载和覆盖的实现方式

这里主要讲述重载和覆盖在C++语言中的实现方式。主要参考资料为*The Design and Evolution of C++*中的第三章。

函数的重载是在编译时期确定的。几个不同的函数可以具有同样的名字，编译器通过调用参数类型的不同，来确认调用的函数；如果不能通过调用参数类型的不同来确认，编译器会报错。

方法的覆盖是在运行时期确定的。在C++语言中，父类中将允许覆盖的函数标记为虚函数，子类中严格使用与父类中标记为虚函数的函数相同的函数名、参数类型和返回值类型，重新定义这一函数，通过这样一种方式，来完成方法的覆盖。编译时期，编译器生成一个虚函数指针列表，在这一列表中，存储了所有的虚函数的定义的代码所在的位置。运行时期，通过虚函数列表指针来确定虚函数指针列表的位置，进而确定所调用的虚函数的定义的代码所在的位置。

假设有如下代码：

{% highlight cpp %}
class A
{
public:
    void MA(int);
    void MB(double);
    virtual void MC(int, double);
private:
    int a;
    double b;
};

class B : public A
{
public:
    virtual void MC(int, double) override;
private:
    int c;
};
{% endhighlight %}

此时，编译器将类`A`中三个方法的代码，以及类`B`中`MC`方法的代码放到代码段中，然后为类`A`和类`B`分别生成虚函数指针列表。类`A`或`B`的实例视分配方法而定，会分配到栈或者堆上。类`A`的实例的内存布局通常如下：

<table><tbody>
<tr>
    <td width="50%">vptr</td>
    <td>a</td>
</tr>
<tr>
    <td colspan="2">b</td>
</tr>
</tbody></table>

其中`vptr`指向这样一个表格：

<table><tbody><tr><td>A中MC方法的代码的地址</td></tr></tbody></table>

而类`B`的实例的内存布局通常如下：

<table><tbody>
<tr>
    <td width="50%">a</td>
    <td></td>
</tr>
<tr>
    <td colspan="2">b</td>
</tr>
<tr><td>vptr</td><td>c</td></tr>
</tbody></table>

其中`vptr`指向这样一个表格：

<table><tbody><tr><td>B中MC方法的代码的地址</td></tr></tbody></table>

## 笔试、面试中常考的一类题型

重载和覆盖的概念和实现方式斗截然不同，但是为什么笔试、面试的时候还经常考呢？主要是他们的表现形式比较相近，并且还有“函数隐藏”这个东西在捣乱。“函数隐藏”不是一个好的概念，我的观点是在任何时候都不要使用“函数隐藏”，并且在一些新型的语言中，也对“函数隐藏”作出了一定限制，比如说在C#语言中，必须使用`new`关键字标注，才能够使用“函数隐藏”。我之前也发过一篇文章讨论过“函数隐藏”的不必要性，[原文见此](http://www.cnblogs.com/HCOONa/archive/2010/07/11/member-function-hide-is-not-necessary.html)。

先说一下函数隐藏，这是重载的一种特殊情况。如果在子类中定义了一个同父类中的方法具有完全相同的函数名、参数类型和返回值类型时，如果父类中的这一方法没有标记为虚方法，则会发生函数隐藏。在子类中，必须通过特殊的语法才能够调用父类中的同名方法。由于这是重载的一种特殊情况，调用这一函数时，判断还是发生在编译时期的。

下面来一个比较复杂的例子，也是笔试中比较容易考的类型：

{% highlight cpp %}
class A
{
public:
    void MA(int);
    void MA(double);
    void MB(double);
    virtual void MC(int, double);
    virtual void MD(double);
};

class B : public A
{
public:
    void MA(double);
    void MB(int);
    void MD(double);
};

...

A a0;
A a1 = B();
B b;

a0.MA(0.5);
a1.MA(0.5);
b.MA(0.5);
a0.MB(0.5);
a1.MB(0.5);
b.MB(0.5);
a0.MD(0.5);
a1.MD(0.5);
b.MD(0.5);
{% endhighlight %}

问调用的这些函数分别是哪些函数？

首先呢`a0.MA(0.5)`和`a1.MA(0.5)`肯定调用的是`void A::MA(double)`，因为`MA`方法没有被声明为虚函数，因此在编译时期就可以确定应该调用的方法，由于在调用`MA`方法时，`a0`和`a1`声称自己类型都是`A`因此调用的是类`A`中的`MA`方法。而`b.MA(0.5)`调用的则是`void B::MA(double)`，因为基类`A`中的方法`void MA(double)`没有被声明为虚方法，因此要做静态判断。而`b`声称自己的类型是`B`，因此要调用`B`中的`MA`方法。

其次看`MB`这一组，由于只有一个`void A::MB(double)`函数定义，因此调用的都是这个函数。

最后看`MD`这一组。由于基类`A`中声明`MD`方法为虚函数，因此需要动态判断。`a0`在运行时期的类型就是`A`，因此调用的是`void A::MD(double)`。`a1`在运行时期的类型是`B`，因此调用的是`void B::MD(double)`。`b`在运行时期的类型是`B`，因此调用的是`void B::MD(double)`。

只要掌握好原理，牢记重载就看静态类型，覆盖就看动态类型，就能准确的判断调用的函数到底是哪一个了。
