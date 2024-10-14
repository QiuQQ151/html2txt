#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// 路由函数
#include "./routs/head.h"  //路由函数入口

//主函数//
/* 传递参数形式
data: 日期 格式：年+月+日 例子：20241014
*/
int main(int argc, char *argv[]) {
    // 输入参数错误
    if (argc != 2) {
        fprintf(stderr, "用法: %s <日期> <输出文件名>\n", argv[0]);
        return 1;
    }
    printf("当前日期：%s\n",argv[1]);
    
    // 资源申请
    // 定义文件名
    char* filename = (char*)malloc( sizeof(char)*20 );
    *filename = '\0';
    strcat(filename,"xhrb");
    strcat(filename,argv[1]);
    strcat(filename,".txt");

    // 打开输出文件 //按日期命名
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件 %s\n", filename);
        //释放内存
        return 1;
    } 

    // 内容抓取并保存
    extract_xhrb(argv[1],fp);


    // 资源释放
    fclose(fp);
    return 0;
}
