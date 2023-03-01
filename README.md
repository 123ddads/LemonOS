# LemonOS
参考Linux0.11内核源码，学习源码中的一些思想（架构/细节处理），设计并完成了一个简易版本的操作系统，取名为LemonOS。LemonOS具备一个操作系统所应该有的基本功能（麻雀虽小，五脏俱全），支持多任务、内存管理、驱动程序、文件系统等基本功能。App目录下是一些用于测试的用户程序，Kernel目录下是内核代码（重点），Include目录下是一些基本的头文件，比如基本数据类型，计算偏移等相关宏函数。
makefile用于一键化编译，采用的是一款BochsX86虚拟机软件，bochs文件用于配置启动盘，此次设计采用的是软盘启动。
附：Bochs下载链接https://sourceforge.net/projects/bochs/files/bochs/
