---
layout: post
title: "一道面试题的多种解法"
description: ""
category: interview
tags: [Interview]
excerpt: 一棵二叉搜索树中，有一个节点不满足二叉搜索树的性质，找出这一节点。
---
{% include JB/setup %}

一棵二叉搜索树中，有一个节点不满足二叉搜索树的性质，找出这一节点。

## 问题分析

先说一下二叉搜索树的性质：

<ol>
<li>若它的左子树不为空，则左子树上的所有节点的值均小于它的根节点的值。</li>
<li>若它的右子树不为空，则右子树上的所有节点的值均大于它的跟节点的值。</li>
<li>它的左右子树也分别为一棵二叉搜索树。</li>
</ol>

如果直接递归地判断二叉搜索树中的每一节点是否满足上述性质，固然可以解决问题，但是效率却出奇的低。实际上，中序遍历一棵二叉搜索树得到的结果，恰为一个升序序列。只需检查这一序列中不满足升序的元素，就可以找出二叉搜索树中不符合上述性质的节点。

因此，首先需要解决的问题就是二叉树的中序遍历。

## 二叉树的中序遍历

根据二叉树的定义，我们可以建立这样的数据结构：

{% highlight ocaml %}
type 'a btree =
    | Nil
    | Node of ('a * 'a btree * 'a btree)
;;
{% endhighlight %}

对于这样的数据结构，我们只需要考虑两种情况：

<ol>
<li>当前节点为<code>Nil</code>。</li>
<li>当前节点为<code>Node</code>。</li>
</ol>

对应的中序遍历程序如下：

{% highlight ocaml %}
let to_inorder tree =
    let rec aux tree ls =
        match tree with
        | Nil -> ls
        | Node (d, l, r) -> aux l (d :: (aux r ls))
    in
    aux tree []
;;
{% endhighlight %}

或者用我们熟悉的C++语言表示：

{% highlight cpp %}
template<typename T>
struct btree
{
    T data;
    btree *left;
    btree *right;
};

template<typename T>
void ToInOrder(btree<T> *node, std::vector<T> &container) {
    if (node) {
        ToInOrder(node->left, container);
        container.push_back(node->data);
        ToInOrder(node->right, container);
    }
}
{% endhighlight %}

通过二叉树中序遍历，我们可以得到所需的序列，然后检查这一序列，就可以找到不符合二叉搜索树性质的节点了。

## 只用O(1)的空间

之前所述的方法，需要O(n)的空间存放中序遍历的结果，但是实际上我们并不需要这一序列，而是只需要序列中的前一个元素的值即可。或者更进一步的，我们在中序遍历二叉搜索树的时候，如果能够知道当前节点的上一个节点，就可以验证该节点是否符合二叉搜索树的性质。

为了做到这一点，我们需要知道一棵子树的最后一个节点。这样，根节点的上一个节点就是左子树的最后一个节点，右子树的上一个节点就是根节点，左子树的上一个节点就是当前子树的上一个节点。

{% highlight ocaml %}
let check tree =
    let rec do_check tree previousNode =
        let latest tree =
            let rec latest_aux tree parentNode =
                match tree with
                | Nil -> parentNode
                | Node (_, _, r) -> latest_aux r tree
            in
            latest_aux tree Nil
        in
        let rec check_current_right currentNode previousNode =
            match currentNode with
            | Nil -> Nil
            | Node (d, _, r) -> match previousNode with
                                | Nil -> do_check r currentNode
                                | Node (pd, _, _) -> if pd >= d then
                                                        currentNode
                                                     else
                                                        do_check r currentNode
        in
        match tree with
        | Nil -> Nil
        | Node (_, Nil, _) -> check_current_right tree previousNode
        | Node (_, l, _) -> match (do_check l previousNode) with
                            | Nil -> check_current_right tree (latest l)
                            | Node _ as ill_node -> ill_node
    in
    do_check tree Nil
;;
{% endhighlight %}

但是实际上，我们在遍历左子树的同时，就能知道左子树的最后一个节点是谁，因此，稍作修改就可以抛弃`latest`函数。

{% highlight ocaml %}
let check tree =
    let rec do_check tree previousNode =
        match tree with
        | Nil -> (true, previousNode)
        | Node (d, l, r) ->
            let (l_valid, l_node) = do_check l previousNode in
            if l_valid then
                match l_node with
                | Nil -> do_check r tree
                | Node (pd, _, _) -> if pd >= d then
                                        (false, tree)
                                     else
                                        do_check r tree
            else
                (l_valid, l_node)
    in
    match (do_check tree Nil) with
    | (true, _) -> Nil
    | (false, n) -> n
;;
{% endhighlight %}

对应的C++语言代码如下：

{% highlight cpp %}
template<typename T>
static std::tuple<bool, btree<T> *> DoCheck(btree<T> *tree, btree<T> *previousNode)
{
    if (tree) {
        bool leftValid;
        btree<T> *leftInvalidNode;
        std::tie(leftValid, leftInvalidNode) = DoCheck<T>(tree->left, previousNode);
        if (leftValid) {
            if (leftInvalidNode) {
                if (leftInvalidNode->data >= tree->data) {
                    return std::make_tuple(false, tree);
                } else {
                    return DoCheck<T>(tree->right, tree);
                }
            } else {
                return DoCheck<T>(tree->right, tree);
            }
        } else {
            return std::make_tuple(leftValid, leftInvalidNode);
        }
    } else {
        return std::make_tuple(true, previousNode);
    }
}

template<typename T>
inline btree<T> * Check(btree<T> *tree)
{
    bool valid;
    btree<T> *invalidNode;
    std::tie(valid, invalidNode) = DoCheck<T>(tree, nullptr);
    if (valid) {
        return nullptr;
    } else {
        return invalidNode;
    }
}
{% endhighlight %}

可见，这样的代码比起一开始的中序遍历代码要复杂太多了。那么如何既享受中序遍历代码的简洁，又实现这样的复杂功能呢？

## 引入状态

有的人认为，纯函数不应该具有状态。我并不认同这一看法，只要函数对于给定的输入必然给出同样的输出，那么这就是一个纯函数。适当的情况下，引入状态可以极大的简化程序的设计。对于上面提出的问题，我们只需引入状态，存储之前访问的节点的值，就可以使用非常少的代码解决问题。

最直接的想法如下：

{% highlight ocaml %}
let check tree =
    let p = ref Nil in
    let i = ref Nil in
    let rec check_aux tree =
        match tree with
        | Nil -> ()
        | Node (d, l, r) -> check_aux l;
                            match !p with
                            | Node (pd, _, _) when pd >= d -> i := tree
                            | _ -> p := tree; check_aux r
    in
    check_aux Nil;
    !i
;;
{% endhighlight %}

转化成C++语言的代码就有点麻烦了。C++语言标准不允许嵌套定义函数，因此比较直接的想法是使用全局变量：

{% highlight cpp %}
static btree<int> *previousNode;

static btree<int> *DoCheck(btree<int> *tree)
{
    if (tree) {
        btree<int> *r = DoCheck(tree->left);
        if (r) {
            return r;
        } else {
            if (previousNode && previousNode->data >= tree->data) {
                return tree;
            } else {
                previousNode = tree;
                return DoCheck(tree->right);
            }
        }
    } else {
        return nullptr;
    }
}

inline btree<int> *Check(btree<int> *tree)
{
    previousNode = nullptr;
    return DoCheck(tree);
}
{% endhighlight %}

但是使用全局变量会引起两个问题。第一个问题是，线程不安全；第二个问题是，全局变量不能是泛型的。第一个问题可以通过添加关键字`thread_local`来解决，但是第二个问题怎么办？我们可以模仿闭包的思想，使用仿函数将函数和数据捆绑到一起，具体代码如下：

{% highlight cpp %}
template<typename T>
class CheckBinarySearchTreeFunctor
{
public:
    inline btree<T> * operator() (btree<T> *tree)
    {
        previousNode = nullptr;
        return DoCheck(tree);
    }
private:
    thread_local btree<T> *previousNode;
    btree<T> * DoCheck(btree<T> *tree)
    {
        ...
    }
};
{% endhighlight %}

为了不在每次使用仿函数的时候都创建一个新对象，我们还可以为该仿函数类添加单例模式：

{% highlight cpp %}
template<typename T>
class CheckBinarySearchTreeFunctor
{
    ...
public:
    static CheckBinarySearchTreeFunctor instance;
};

CheckBinarySearchTreeFunctor CheckBinarySearchTreeFunctor::instance;
{% endhighlight %}

难道这就是我们的终极解决方案了吗？

## 更进一步

上面的解决方案已经相当完美了，但是还不是终极的解决方案。为什么这么说呢？因为上面的方法将二叉树中序遍历的普遍算法和判定二叉搜索树是否正确这一特殊算法混在了一起，虽然后者应用了前者，但是却不能复用前者的算法，而是自己又实现了一遍。这使得我发现这一算法还有改进的余地。

想想STL里面的`for_each`函数，可以在遍历一个数据结构的时候，执行某一动作。参考这一点，我本应该给二叉树添加上中序遍历的迭代器的，不过这对于一个面试题而言就太重量级了，不如直接改造中序遍历的函数：

{% highlight cpp %}
template<typename T>
void InOrderVisit(btree<T> *tree, std::function<void(btree<T>*)> action)
{
    if (tree) {
        InOrderVisit(tree->left, action);
        action(tree);
        InOrderVisit(tree->right, action);
    }
}
{% endhighlight %}

这样，判断二叉搜索树是否合法的函数就可以写成这样了：

{% highlight cpp %}
template<typename T>
btree<T> *Check(btree<T> *tree)
{
    btree<T> *previousNode = nullptr;
    btree<T> *invalidNode = nullptr;
    InOrderVisit(tree, [&](btree<T> *node) {
        if (previousNode && previousNode->data >= node->data) {
            invalidNode = node;
        }
        previousNode = node;
    });
    return invalidNode;
}
{% endhighlight %}

之前的`ToInOrder`方法也可以写成这样：

{% highlight cpp %}
template<typename T>
std::vector<T> ToInOrder(const btree<T> *tree)
{
    std::vector<T> container;
    InOrderVisit(tree, [&container](const btree<T> *node) {
        container.push_back(node->data);
    });
    return container;
}
{$ endhighlight %}
