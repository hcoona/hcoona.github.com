---
layout: post
title: "使用GhostScript将PDF中的指定页面导出为清晰的PNG文件"
description: ""
category: "Tips"
tags: [ Tips ]
excerpt: 通常使用ImageMagick导出PDF文件为图像，获得的图像通常都不够清晰。GhostScript因为能够解析PDF的底层，从而能够获得更为清晰的图像文件。
---
{% include JB/setup %}

通常使用ImageMagick导出PDF文件为图像，获得的图像通常都不够清晰。GhostScript因为能够解析PDF的底层，从而能够获得更为清晰的图像文件。

我使用的命令如下：

    gs \
     -sDEVICE=png16 \
     -o %03d.png \
     -dTextAlphaBits=4 \
     -dAlignToPixels=1 \
     -dFirstPage=1 \
     -dLastPage=1 \
     -r300 \
      file.pdf

解释一下，如果你想导出的页面只有很少的几种颜色，则可以指定DEVICE为png256或者png16减小生成的文件大小，如果导出的页面只有黑白两种颜色，则可以使用pngmono。如果你想导出的页面中包含一张细节十分丰富的图片，那么我建议你使用jpeg格式。通常，如果你想要导出的图片可以打印出来也看的十分清楚，则最好指定DPI为300，否则110足矣。

## 参考资料

- [http://stackoverflow.com/questions/5527818/ghost-script-extract-a-single-page-from-a-pdf-and-convert-it-to-a-jpg](http://stackoverflow.com/questions/5527818/ghost-script-extract-a-single-page-from-a-pdf-and-convert-it-to-a-jpg)
- [http://www.ghostscript.com/doc/current/Use.htm#Rendering_parameters](http://www.ghostscript.com/doc/current/Use.htm#Rendering_parameters)
- [http://www.ghostscript.com/doc/current/Devices.htm#PNG](http://www.ghostscript.com/doc/current/Devices.htm#PNG)
