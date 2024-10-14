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
    // 资源申请
    // 打开输出文件 //按日期命名
    FILE * log_fp = fopen("./log/log.txt", "w"); //追加模式a+
    if (!log_fp) {
        fprintf(stderr, "无法打开log文件 %s\n", "./log/log.txt");
        return 1;
    } 
    // 使用 fseek 将文件指针定位到文件末尾
    fseek(log_fp, 0, SEEK_END);

    
    // 用于定义抓取的时间
    char* time = (char*)malloc( sizeof(char)*9 );
    *time = '\0';
    *(time+8) = '\0';

    // 抓取时间设定
    if( argc == 1 | argc == 2)// 未指定日期，抓取时间设定未系统时间
    {
        // 内容抓取并保存
        log_record("---启动内容抓取----按系统当前时间抓取:",log_fp);
        char* local_time = get_time();
        log_record(local_time,log_fp);
        log_record("\n",log_fp);
        html_fix(time,local_time,4);
        html_fix(time+4,local_time+5,2);
        html_fix(time+6,local_time+8,2);
        printf("系统时间：%s，转换时间：%s\n",local_time,time);    
    }
    else if( argc == 2 ) //指定了抓取日期
    {
        // // 内容抓取并保存
        // log_record("---启动内容抓取----按指定日期抓取----------------\n",log_fp);
        //    html_fix(time,argv+1,8);     
        // printf("抓取日期：%s\n",time);        
    }
    else{
        // 资源释放
        log_record("输入参数错误\n\n\n\n\n\n",log_fp);
        fclose(log_fp);
        return 1;

    }

    // 定义文件名
    char* filename = (char*)malloc( sizeof(char)*20 );
    *filename = '\0';
    strcat(filename,"./temp/xhrb");
    strcat(filename,time);
    strcat(filename,".txt");
    // 打开输出文件 //按日期命名
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        log_record("无法创建文章保存文件\n",log_fp);
        return 1;
    } 
    extract_xhrb(time,fp,log_fp);        
    printf("抓取日期：%s，存放地址：%s\n",time,"./temp/xhrb");     

    // 资源释放
    log_record("文件操作符资源释放\n\n\n\n\n\n",log_fp);
    fclose(fp);
    fclose(log_fp);
    return 0;
}
