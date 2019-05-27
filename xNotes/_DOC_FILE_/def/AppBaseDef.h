//
//  AppBaseDef.h
//  _DOC_FILE_
//
//  Created by vivi wu on 2019/5/27.
//  Copyright © 2019 vivi wu. All rights reserved.
//

#ifndef AppBaseDef_h
#define AppBaseDef_h


#endif /* AppBaseDef_h */

#pragma mark -屏幕设备相关
//取设备屏幕宽度
#define TZTScreenWidth [[UIScreen mainScreen] bounds].size.width
//取设备屏幕高度
#define TZTScreenHeight [[UIScreen mainScreen] bounds].size.height
//导航栏高度
#define TZTNavbarHeight 44
//状态栏高度
#define TZTStatuBarHeight [[UIApplication sharedApplication] statusBarFrame].size.height

//底部工具栏高度
#define TZTToolBarHeight 44
//不带底部工具栏界面表格的最大显示高度
#define TZTValidHeightNoToolBar (TZTScreenHeight - TZTStatuBarHeight - TZTNavbarHeight)
//带底部工具栏界面表格的最大显示高度
#define TZTValidHeightWithToolBar (TZTScreenHeight - TZTStatuBarHeight - TZTNavbarHeight - TZTToolBarHeight)

#if TARGET_OS_IPHONE

//IPAD判断标识
#define IS_TZTIPAD  (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
//模拟器判断标识
#define IS_TZTSimulator (NSNotFound != [[[UIDevice currentDevice] model] rangeOfString:@"Simulator"].location)
//iOS版本判断
#define IS_TZTIOS(x) ([[UIDevice currentDevice].systemVersion intValue] >= x)
//iPhone4/4s判断
#define IS_TZTIphone4 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(320, 480), [[UIScreen mainScreen] bounds].size) : NO)

//iPhone5判断
#define IS_TZTIphone5 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(640, 1136), [[UIScreen mainScreen] currentMode].size) : NO)
//iPhone6判断
#define IS_TZTIphone6 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(375, 667), [UIScreen mainScreen].bounds.size) : NO)
//iPhone6Plus判断
#define IS_TZTIphone6P ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? (CGSizeEqualToSize(CGSizeMake(414, 736), [UIScreen mainScreen].bounds.size)) : NO)
//iphoneX判断
#define IS_TZTIphoneX ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? (CGSizeEqualToSize(CGSizeMake(375, 812), [UIScreen mainScreen].bounds.size)) : NO)
#define IS_TZTIphoneXRSM ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? (CGSizeEqualToSize(CGSizeMake(414, 896), [UIScreen mainScreen].bounds.size)) : NO)

#define IS_IPHONE_Xr ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(828, 1792), [[UIScreen mainScreen] currentMode].size) : NO)
//判断iPhoneXsMax
#define IS_IPHONE_Xs_Max ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(1242, 2688), [[UIScreen mainScreen] currentMode].size) : NO)


//iPhoneX 底部预留高度
#define TZTHomeBarHeight  (IS_TZTIphoneX ? 34:0)
//CGSizeEqualToSize(CGSizeMake(375, 667), [UIScreen mainScreen].bounds.size) ||
#else

#define IS_TZTIPAD NO
#define IS_TZTSimulator NO
#define IS_TZTIOS(x) NO
#define IS_TZTIphone5 NO

#endif

//iPhone6p额外高度增加，用于适配
#define tzt_iphone6_6p_exheight (((IS_TZTIphone6P || IS_TZTIphone6) ? 0 : 0))
//iPhone6p字体统一放大，用于适配
#define tzt_iphone6_6p_exfontsize (((IS_TZTIphone6P || IS_TZTIphone6) ? .0f : 0))


#pragma mark -内存，对象创建相关
//自动释放池
#define    BegAutoReleaseObj()    NSAutoreleasePool* autoPool = NewObject(NSAutoreleasePool)
#define    EndAutoReleaseObj()    DelObject(autoPool)

//创建Obj对象
#define NewObject(x) [[x alloc]init];
/**创建对象，创建前判断是否为空，不为空不创建*/
#define NewObjIfNil(obj,class) {if(obj == nil){obj = NewObject(class);}}
//创建自动释放obj对象
#define NewObjectAutoD(x) [[[x alloc]init]autorelease];

//删除Obj对象
#define DelObject(x) {if (x) {[x release];x=nil;}}
//设置obj对象为空
#define NilObject(x) {if (x) {x=nil;}}
#define Obj_RELEASE(x)      [x release]
#define Obj_AUTORELEASE(x)  [x autorelease]


//NSString长度检查，
#define ISNSStringValid(x) (x != NULL && [x length] > 0)
//NSString转intValue，长度为空则返回－1
#define TZTStringToIndex(x,y){if(ISNSStringValid(x)) {y = [x intValue];} else { y = -1;} }
//手机号长度检查
#define ISMobileCodeValid(x) (x != NULL && [x length] == 11)
//电子邮箱检查
#define ISEmailValid(x) (x != NULL && [[NSPredicate predicateWithFormat:@"SELF MATCHES %@", @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}"] evaluateWithObject:x])
//
#define TZTCalcMoneyWithPrice(price, amount) ([[[NSDecimalNumber decimalNumberWithString:amount] decimalNumberByMultiplyingBy:[NSDecimalNumber decimalNumberWithString:price]] stringValue])

#pragma mark -字体相关
//HelveticaNeue
//DIN Alternate
//统一加粗字体，根据字体大小创建，字体由g_nsFontNameBold制定，默认HelveticaNeue-Bold
#define tztUIBaseViewTextBoldFont(fontsize) [UIFont fontWithName:g_nsFontNameBold size:((fontsize > 0)?fontsize:g_fDefaultFontSize) + tzt_iphone6_6p_exfontsize]
//统一不加粗字体，根据字体大小创建，字体由g_nsFontName指定，默认HelveticaNeue
#define tztUIBaseViewTextFont(fontsize) [UIFont fontWithName:g_nsFontName size:((fontsize > 0)?fontsize:g_fDefaultFontSize) + tzt_iphone6_6p_exfontsize]
//根据字体名称，大小创建字体
#define tztUIBaseViewTextFontWithName(name, fontsize) [UIFont fontWithName:name size:(fontsize > 0 ? fontsize : g_fDefaultFontSize) + tzt_iphone6_6p_exfontsize]
//根据大小创建默认字体，字体刻有g_nsFontName指定，默认HelveticaNeue
#define tztUIBaseViewTextFontWithDefaultName(fontsize) tztUIBaseViewTextFontWithName(g_nsFontName,fontsize)

//像素转换成字体
#define tztUIBaseViewTextFontWithPx(pxsize) tztUIBaseViewTextFont((pxsize*72/96))
#define tztUIBaseViewTextBoldFontWithPx(pxsize) tztUIBaseViewTextBoldFont((pxsize*72/96))

//数字单独使用字体
#define tztUIBaseViewTextFontForNumberWithSize(pxsize) tztUIBaseViewTextFontWithName(@"DIN-Medium", pxsize)
