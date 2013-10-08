---
layout: post
title: "面向对象技术中的三大特征"
description: ""
category: interview
tags: [Interview]
excerpt: 面试中经常会问面向对象技术中的三大特征，但是却没有一本书把这三个特征讲透彻，这里尝试综合几本书中的内容，从不同角度理解这三大特征。
---
{% include JB/setup %}

面试中经常会问面向对象技术中的三大特征，但是却没有一本书把这三个特征讲透彻，这里尝试综合几本书中的内容，从不同角度理解这三大特征。

现在一般认为，面向对象技术中的三大特征分别是：封装（Encapsulation）、继承（Inheritance）和多态（Polymorphism）。

老实说，我不知道这个答案是怎么出来的，为此我特意在Google和各大学术搜索引擎上找了半天，也没找到出处；反倒是[维基百科](https://en.wikipedia.org/wiki/Object-oriented_programming#Fundamental_features_and_concepts)，给出了几大特性，还有出处。

    Benjamin C. Pierce and some other researchers view any attempt to distill OOP to a minimal set of features as futile. He nonetheless identifies fundamental features that support the OOP programming style in most object-oriented languages:

    - Dynamic dispatch – when a method is invoked on an object, the object itself determines what code gets executed by looking up the method at run time in a table associated with the object. This feature distinguishes an object from an abstract data type (or module), which has a fixed (static) implementation of the operations for all instances. It is a programming methodology that gives modular component development while at the same time being very efficient.
    - Encapsulation (or multi-methods, in which case the state is kept separate)
    - Subtype polymorphism
    - Object inheritance (or delegation)
    - Open recursion – a special variable (syntactically it may be a keyword), usually called this or self, that allows a method body to invoke another method body of the same object. This variable is late-bound; it allows a method defined in one class to invoke another method that is defined later, in some subclass thereof.
    
    Similarly, in his 2003 book, Concepts in programming languages, John C. Mitchell identifies four main features: dynamic dispatch, abstraction, subtype polymorphism, and inheritance. Michael Lee Scott in Programming Language Pragmatics considers only encapsulation, inheritance and dynamic dispatch.

既然如此，我就不讨论其出处和最初解释了，直接解释封装、继承和多态这三个概念，以免面试官以为我不懂面向对象技术。

## 封装

封装的主要意义是可以限制对于对象内容的访问。在C++中，有三种（不算友元的话）访问级别：public、protected和private。在Java、C#或其他语言中，可能还会有更加细致的访问控制方式。

这种对于对象内部访问的控制，有两点好处：

<ol>
<li>可以保护内部数据不被意外修改。</li>
<li>可以对外隐藏内部实现细节，待以后需要修改实现方法时，不会破坏其他已有代码。</li>
</ol>

下面举例说明这两点。

首先，看看没有面向对象技术的C语言如何实现一个栈：

{% highlight cpp %}
#define STACK_SIZE_MAX 10000
int stack[STACK_SIZE_MAX];
int stackTop = 0
#define PUSH_STACK(x) (stack[stackTop++]=(x))
#define TOP_STACK() (stack[stackTop-1])
#define POP_STACK() (stack[--stackTop])
{% endhighlight %}

可谓短小精悍，但是这段代码有几个问题：

<ol>
<li>只能存储`int`类型。</li>
<li>只有一个栈。</li>
<li>线程不安全。</li>
<li>栈的大小有限。</li>
<li>`stack`变量和`stackTop`变量有可能被意外或者有意的修改。</li>
</ol>

第一个问题，即使是使用面向对象技术，也不能解决，而是通过泛型解决的。当然了，如果不考虑类型安全性，也可以使用`void *`类型存储。对于第二个问题，通常是采用模仿面向对象技术的方式来实现的，代码如下：

{% highlight cpp %}
typedef int stack_item_type;
struct stack_struct
{
    int capacity;
    int top;
    stack_item_type * stack;
};
typedef struct stack_struct * stack_type;

inline stack_type new_stack_ex(int capacity)
{
    stack_type stack = (stack_type)malloc(sizeof(struct stack_struct));
    stack->capacity = capacity;
    stack->top = 0;
    stack->stack = (stack_item_type *)malloc(sizeof(stack_item_type) * capacity);
    return stack;
}

inline void delete_stack_ex(stack_type stack)
{
    free(stack->stack);
    free(stack);
}

inline void push_stack_ex(stack_type stack, stack_item_type item)
{
    stack->stack[stack->top++] = item;
}

inline stack_item_type top_stack_ex(stack_type stack)
{
    return stack->stack[stack->top - 1];
}

inline void pop_stack_ex(stack_type stack)
{
    stack->top--;
}
{% endhighlight %}

随着这样一种解决方式，问题二三四也都解决了。但是最后一个问题，必须要语言提供隐藏信息的机制支持，才能够解决。这就是C++引入的信息隐藏机制所解决的问题，即信息的封装。且看C++语言中如何实现一个栈：

{% highlight cpp %}
template<typename T, int STACK_CAPACITY = 10000>
class Stack
{
public:
    inline void Push(T item) { stack[top++] = item; }
    inline T Top(void) { return stack[top - 1]; }
    inline void Pop(void) { top--; }
private:
    int stack[STACK_CAPACITY];
    int top;
public:
    Stack(void) : top(0) { }
};
{% endhighlight %}

通过将`stack`变量和`top`变量的访问级别设为`private`，使得除了`Stack`类中的成员函数，没有任何方法能够访问这两个变量，有效的防止了意外的修改导致非预期结果。

另一方面，通过限制`Stack`中内容的访问，使得调用者只能通过`Stack`类中所公布的共有方法进行访问，又对`Stack`类的实现进行了抽象。这里相当于在调用者和实现者之间建立了一个接口（契约），使得双方达成了一种共识。这样带来一个额外的好处，只要接口不发生改变，一个类的内部实现可以任意变化。例如实现者认为使用数组实现的栈对象太浪费内存了，不如使用单链表实现，于是修改了实现如下：
{% highlight cpp %}
template<typename T>
struct ListNode
{
    T data;
    std::shared_ptr<ListNode> next;
    ListNode(T data, std::shared_ptr<ListNode> next)
        : data(data), next(next)
    { }
};

template<typename T>
class Stack
{
public:
    inline void Push(T item) { list = std::make_shared<ListNode<T>>(item, list); }
    inline T Top(void) { return list->data; }
    inline void Pop(void) { list = list->next; }
private:
    std::shared_ptr<ListNode<T>> list;
public:
    Stack(void) : list(nullptr) { }
};
{% endhighlight %}

由于对外开放的接口没有发生任何变化，所有`Stack`类的调用者都不需修改代码，这一点是C语言等没有提供这样强的封装机制的语言所无法实现的。

## 继承

继承最初的用意是直接复用父类中所写的代码，但是人们很快发现，仅仅是使用继承去复用代码，很容易造成更大的混乱，必须对此加以限制，这一限制就是里氏替换原则。里氏替换原则的内容可以描述为：“派生类（子类）对象能够替换其基类（超类）对象被使用。”以上内容并非利斯科夫的原文，而是译自罗伯特·马丁（Robert Martin）对原文的解读。这是一种符合直觉的限制，如果在用到父类型的地方不能够替换成子类型使用，说明这两个类不应该具有继承关系，即子类应该是一个（is-a）父类的特例。

下面说一个违反直觉的例子，这是一个不好的例子，只是为了说明问题而构造出来的例子，大家不要模仿这个例子。

直觉上，正方形是长方形的一种特例，因此应该让正方形继承自长方形。

{% highlight cpp %}
class Rectangle
{
public:
    inline int getWidth(void) const { return width; }
    inline int getHeight(void) const { return height; }
    inline int getArea(void) const { return width * height; }
private:
    int width;
    int height;
public:
    Rectangle(int width, int height)
        : width(width), height(height)
    { }
};

class Square : public Rectangle
{
public:
    Square(int sideLength) : base(sideLength, sideLength) { }
};
{% endhighlight %}

这种情况下，任何使用到`Rectangle`的地方，都可以使用`Square`。但是假设`Rectangle`类还有两个成员函数：

{% highlight cpp %}
class Rectangle
{
    ...
public:
    virtual void setWidth(int width) { this->width = width; }
    virtual void setHeight(int height) { this->height = height; }
};

class Square : public Rectangle
{
    ...
public:
    virtual int setWidth(int width) override { setSideLength(width); }
    virtual int setHeight(int height) override { setSideLength(height); }
private:
    void setSideLength(int length)
    {
        width = length;
        height = length;
    }
};
{% endhighlight %}

则`Square`类不应该继承自`Rectangle`类，因为`Square`类处理改变宽和高的逻辑与`Rectangle`截然不同，并非对原有功能的扩充。这样一来，原本使用`Rectangle`的代码，如果使用的是`Square`，则有可能出现非预期的结果，例如这样的测试用例在传入的对象是一个`Square`的实例的时候，就会失败：

{% highlight cpp %}
void TestArea(Rectangle &rectangle)
{
    const int deltaWidth = 10;
    int originArea = rectangle.getArea();
    const int exceptedArea = originArea + deltaWidth * rectangle.getHeight();

    rectangle.setWidth(rectangle.getWidth() + deltaWidth);
    int actualArea = rectangle.getArea();
    TEST_ASSERT_EQUAL(exceptedArea, actualArea);
}
{% endhighlight %}

这是因为原本长方形的长和宽可以被独立的改变，而正方形的要求更加严格，从而导致了对于里氏替换原则的违反。

对于支持逆变和协变，以及异常的语言，要求：

<ul>
<li>子类中，方法的参数必须是逆变的。</li>
<li>子类中，方法的返回值必须是协变的。</li>
<li>如果父类中的方法没有抛出异常，那么子类中的该方法也不应抛出异常。</li>
<li>如果父类中的方法抛出了异常，那么子类中的该方法可以抛出该异常的子类异常。</li>
</ul>

除此之外，子类的设计还应该遵循以下一般原则：

<ul>
<li>子类中，前置条件不能被强化。</li>
<li>子类中，后置条件不能被弱化。</li>
<li>子类中，必须遵守父类中的不变式。</li>
<li>子类中如果引入了父类中不存在的方法，则不得通过这些方法改变继承自父类的状态。</li>
</ul>

上面长方形正方形的例子，就是违反了后置条件不能被弱化的原则。

## 多态

多态（特指面向对象技术中的术语“多态”）是这样一种技术：直到运行时期，才确定被调用的方法应该执行的代码。

通常，在C/C++、Java、C#这一类语言中，执行的代码是在编译期间就确定的，这被称为“早绑定”。编译器知道被调用的函数所在的位置，因此在调用这一函数时，直接将参数按照调用约定压栈，然后跳转到该函数所在的位置继续执行即可。但是多态可以延迟这一过程，直到运行时期，程序才知道调用的函数的代码的真正位置。由于执行的代码实在运行时期才确定的，这又被称为“晚绑定”。很多动态语言默认就是“晚绑定”的。

多态和继承的关系非常密切。在C++语言中，只有在父类中声明为虚函数的函数，才能在子类中被覆盖（Override），从而实现多态。

多态的好处在于，我们可以在不修改现有代码的基础上，扩充功能。还用上面栈的代码做例子：

{% highlight cpp %}
template<typename T>
class Stack
{
public:
    virtual void Push(T) = 0;
    virtual T Top(void) const = 0;
    virtual void Pop(void) = 0;
};

template<typename T, int CAPACITY>
class ArrayStack : public Stack
{
public:
    virtual void Push(T item) override { stack[top++] = item; }
    virtual T Top(void) const override { return stack[top - 1]; }
    virtual void Pop(void) override { top--; }
private:
    T stack[CAPACITY];
    int top;
public:
    ArrayStack(void) : top(0) { }
};

...

template<typename T>
class LinkedListStack : public Stack
{
public:
    virtual void Push(T item) override
    {
        list = std::make_shared<ListNode<T>>(item, list); 
    }
    virtual T Top(void) const override { return list->data; }
    virtual void Pop(void) override { list = list->next; }
private:
    std::shared_ptr<ListNode<T>> list;
public:
    LinkedListStack(void) : list(nullptr) { }
};
{% endhighlight %}

在使用的时候，比如说有这样一个函数：

{% highlight cpp %}
void PushZero(Stack<int> &stack)
{
    stack.Push(0);
}
{% endhighlight %}

那么这里调用的`Push`函数是哪个函数呢？首先肯定不是`Stack`类中的`Push`函数，因为`Stack`是个纯虚类，其`Push`方法根本就没有实现。其次，就要看传给这一函数的参数，其运行时期的真正类型是什么。如果传进来的`stack`变量是`ArrayStack`类型的，那么调用的就是`ArrayStack`类中的`Push`方法，如果传进来的是`LinkedListStack`类型的，那么调用的就是`LinkedListStack`中的`Push`方法。如果我们实现了另一个类，比如是这样的：

{% highlight cpp %}
template<typename T>
class VectorStack : public Stack
{
public:
    virtual void Push(T item) override { stack.push_back(item); }
    virtual T Top(void) const override { return stack.back(); }
    virtual void Pop(void) override { stack.pop_back(); }
private:
    std::vector<T> stack;
};
{% endhighlight %}

那么如果传给`PushZero`的参数`stack`是`VectorStack`类型的，那么调用的就是`VectorStack`中的`Push`方法。注意，我们实现并不知道还会有`VectorStack`这样一个类，但是在之后，我们为了扩充`Stack`类的功能，新写了这样一个类，却没有修改`PushZero`函数中的任何代码，就可以发挥`VectorStack`类的作用。

C++语言中实现这一功能是采用的虚函数列表的方式，具体情况参见*The Design and Evolution of C++*中的3.5 Virtual Functions。
