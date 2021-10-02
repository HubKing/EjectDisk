# EjectDisk

The default Safe Remove menu list, for some reason, does not show the device name. This makes it really difficult to know which is which.

![](https://i.stack.imgur.com/9rb18.png)

So, I have created this console application. If you run it, it will show the number and "friendly name" of all disks. You just type the number, and it will eject the disk.

Before writing this, I have searched the web for an existing utility for doing this and found [HowSwap!](http://mt-naka.com/hotswap/index_enu.htm#download).
It seemed to be nice application with more features, but unfortunately, ejecting a USB disk with it did not actually eject it. The entry was disappeared from HotSwap!,
but the number of "Eject Device" in Windows' menu stayed the same, and the disk was still spinning 
(I could feel the vibration and the drive's DC adapter was consuming 5W on the power meter), and executing "Scan for hardware changes" made the disk re-appear in HotSwap!.

With my programme, the disk was actually ejected. The adapter power usage became 0.5W, and the number of "Eject Device" in Windows' menu was reduced.
