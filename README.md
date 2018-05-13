<!-- README.md --- 
;; 
;; Description: 
;; Author: Hongyi Wu(吴鸿毅)
;; Email: wuhongyi@qq.com 
;; Created: 日 3月 25 20:23:51 2018 (+0800)
;; Last-Updated: 日 5月 13 14:05:38 2018 (+0800)
;;           By: Hongyi Wu(吴鸿毅)
;;     Update #: 5
;; URL: http://wuhongyi.cn -->

# README

本程序适用于 XIA Pixie16 系列采集卡，支持100/250/500MHz采集卡(具体支持型号可查看图形软件中的File->About)，最大支持8个机箱同步运行，即1600+路信号同时采集。**本程序包要求使用ROOT6版本。要求采用1920x1080分辨率显示屏**

本程序的设计兼容 100/250/500 MHz 采集卡的混合使用，只需在 cfgPixie16.txt 添加各类采样率采集卡的固件位置即可，程序在线能够自动识别采集卡类型并加载相应的固件。当前我们手头只有100/250MHz 14bit的采集卡，因此默认可运行该类型的采集卡，如要支持其它类型，请联系 XIA LLC 获取对应固件或者联系吴鸿毅(wuhongyi@qq.com)。


- 对本获取程序有任何的意见及建议(功能添加及改进)，欢迎给吴鸿毅(wuhongyi@qq.com)发邮件。
- 我们将会继续完善软件的中英文使用说明书，当前基本都是通过操作演示来讲解软件的使用为主。


用户使用程序包中包含以下文件夹:

- Decode(将原始二进制数据转为ROOT)
- docs(使用说明书，网页版，pdf版。暂未完成)
- firmware(固件)
- GUI(图形软件)
- MakeEvent(事件重构程序，可选)
- NOGUI(非图形软件。升级中，暂时不提供)
- OnlineStattics(在线监视程序)
- parset(参数设置文件)
- PlxSdk(Plx9054驱动)
- software(pixie16驱动API)
- firmware/firmware.md(历史各版本固件说明)


## 软件安装步骤

- 删除个人目录下的老版本XIADAQ文件夹
- 将本程序包解压缩到个人目录中($HOME)
- 设置环境变量
- 编译Plx9054驱动
- 编译pixie16驱动API(该API被吴鸿毅修改过，非官方标准驱动)
- 编译图形化获取软件
- 编译在线监视程序
- 编译数据转换程序
- 编译事件重构程序(可选)


```shell
##设置环境变量

#在 .bashrc 文件中添加
export PLX_SDK_DIR=$HOME/XIADAQ/PlxSdk

# 将 XIADAQ.tar.gz 放置到 /home 下的个人目录下，即 ~/ 位置
tar -zxvf XIADAQ.tar.gz  #解压缩

#得到 XIADAQ 目录
```

```shell
##编译Plx9054驱动

#打开新终端
cd ~
cd XIADAQ/
rm -rf PlxSdk #删除可能存在的未删除驱动，如果没有该目录则不用执行该行命令
tar -zxvf PlxSdk.tar.gz
cd PlxSdk/PlxApi/
make clean
make 
#成功后你将会看到 Library "Library/PlxApi.a" built successfully

cd ../Samples/ApiTest/
make clean
make
#成功后你将会看到 Application "App/ApiTest" built successfully

cd ../../Driver/
./builddriver 9054

#成功后你将会看到 Driver "Plx9054/Plx9054.ko" built sucessfully
```


```shell
##编译pixie16

cd ~
cd XIADAQ/software/
make clean
make 
#只要没报错，并且该文件夹内生成libPixie16App.a libPixie16Sys.a
```


```shell
##编译图形化获取软件

#修改设置参数
cd ~
cd XIADAQ/parset/
#修改cfgPixie16.txt文件。
#其中CrateID 后面的数值表示机箱编号，时值允许0-15。如果单机箱则随意设置(一般就采用默认的0)，如果多个机箱同步运行务必让每个机箱的该编号设置未不同的数值。
#SettingPars 后面为参数设置文件，写入要采用的参数配置文件即可。
#ModuleSlot 后面第一个数值表示插件个数，如果有3个插件则为3。之后的数字未为每个插件在机箱的插槽位置（插槽位置从2开始计数），有三个插件则之后分别为2 3 4。
#参数 ModuleSampingRate与ModuleBits 只对离线模式生效，当主界面采用Offline模式初始化时则读取该参数。

#修改Run.config文件，该文件中第一行为原始数据存放路径，第二行为文件名。
#修改RunNumber文件，该文件中的数值为运行的run number。

cd ~
cd XIADAQ/GUI/
make clean
make 
```


```shell
##编译在线监视程序

cd ~
cd XIADAQ/OnlineStattics/

#修改 PixieOnline.config 文件中的参数
#第一行为获取数据文件存放路径
#第二行为获取文件名

make clean
make 
```

```shell
## 编译数据转换程序

cd ~
cd XIADAQ/Decode/
#修改 UserDefine.hh，按照程序中的说明修改即可

make clean
make 
```


```shell
## 编译事件重构程序

cd ~
cd XIADAQ/MakeEvent/
#修改 UserDefine.hh，按照程序中的说明修改即可

make clean
make
```

## 程序使用说明

- 开机机箱后重启电脑(电脑必须晚于机箱开启)
- 开启机箱后ROOT权限下加载Plx9054驱动
- 正常获取


```shell
## ROOT权限下加载Plx9054驱动
cd ~
cd XIADAQ/PlxSdk/Bin/
su #输入ROOT密码
./Plx_load 9054
#将会看到加载成功的提示
exit  #退出ROOT权限
```

----

```shell
##启动图形界面程序

cd ~
cd ~/XIADAQ/GUI
./pku
#将会弹出图形化界面
#可选择 Online/Offline Mode 然后按 Boot 初始化
#等待初始化成功后，可修改输出数据文件路径，文件名，run number。按 Complete 按钮确认。
#此时 LSRunStart 按钮变为可操作。即可开始按Start，之后第二次按即为Stop。
#Online Statistics选项选择表示发送在线统计
#Update Energy Monitor每选择一次则从插件内部读取一次能谱信息并发送给在线程序（频繁选择会影响获取）

```

```shell
##启动在线监视程序

cd ~
cd XIADAQ/OnlineStattics/
./online
#将会弹出图形化界面
#查看上面的原始数据文件夹路径、文件名是否正确。按 Complete 确认。
#按 RunStart开始启动监视，每3秒更新一次每路的输入率、输出率。(开启机箱后第一次启用该程序需要在获取开启之后)
#监视界面右下角有对写入硬盘使用量的监视。

#EnergyMonitor页面用来查看能谱。由于插件内部寄存器大小限制，该能谱与实际能谱道址范围存在差别。
```


```shell
##执行数据转换程序

cd ~
cd XIADAQ/Decode/

#在上一轮获取结束之后，我们便可将上一轮数据转为ROOT文件
./decode xxx
#xxx 表示 Run Number
```


----

## 建议操作流程

- 先用放射源
- 初步调节参数
- 采集波形
- 离线调节参数







<!-- README.md ends here -->
