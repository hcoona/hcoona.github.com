---
layout: post
title: "在Debian上部署GoAgent客户端"
description: ""
category: Tips
tags: [ Linux, Debian, Tips ]
excerpt: 本文介绍如何在Debian平台上部署GoAgent客户端
---
{% include JB/setup %}

本文介绍如何在Debian平台上部署GoAgent客户端。

1. 下载GoAgent。[传送门](http://code.google.com/p/goagent/)
1. 解压缩下载到的文件中的Local文件夹，并拷贝到适当的目录中。
1. 修改proxy.ini文件。
1. 安装依赖包python-openssl libnss3-tools。
1. 重建Certificate Database：
   1. mkdir -p $HOME/.pki/nssdb
   1. certutil -d $HOME/.pki/nssdb -N (不设定密码)
1. 导入证书certutil -d sql:$HOME/.pki/nssdb -A -t TC -n GoAgent -i /opt/GoAgentLocal/CA.crt。
1. 在Firefox或Chrome中导入证书。
1. 运行python /opt/GoAgentLocal/proxy.py。
1. 设定代理服务器为自动地址：http://127.0.0.1:8086/proxy.pac。

如果想要在Gnome3环境下开机自动启动代理程序：
1. 修改proxy.ini文件，将visible=1改为0。
1. 按Alt+F2输入gnome-session-properties并回车。
1. 添加项proxy /opt/GoAgentLocal/proxy.py。
