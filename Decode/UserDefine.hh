// UserDefine.hh --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 五 10月 20 16:16:28 2017 (+0800)
// Last-Updated: 四 1月  3 18:14:57 2019 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 17
// URL: http://wuhongyi.cn 

// 用户需要修改本文件中的参数

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef _USERDEFINE_H_
#define _USERDEFINE_H_

#define ROOTFILEPATH "/home/wuhongyi/data/"  //要生成ROOT文件的路径
#define RAWFILEPATH "/home/wuhongyi/data/"   //原始二进制文件的路径
#define RAWFILENAME "data"                   // 原始文件的文件名

#define TimesHist 3600  // second

#define Crate0
#define Crate0num 4   //该机箱中使用插件数
const int Crate0SamplingRate[Crate0num] = {100,100,250,250};//分别指定每个插件的采样率  100/250/500三种采样率  0为跳过该插件


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// 示例
// 例如共有5个100MHz采集卡
// #define Crate0num 5
//const int Crate0SamplingRate[Crate0num] = {100,100,100,100,100};


#endif /* _USERDEFINE_H_ */
// 
// UserDefine.hh ends here
