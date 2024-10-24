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
    FILE * log_fp = fopen("./log/log.txt", "w"); //覆盖模式w，追加模式a+
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

    // 获取当前时间
    char* local_time = get_time();
    log_record(local_time,log_fp);
    log_record("\n",log_fp);
    printf("系统时间：%s\n",local_time);   

    // 抓取时间设定
    if( argc == 1 ) {
        // 未指定日期，抓取时间设定未系统时间
        log_record("---启动内容抓取----按系统当前时间抓取:",log_fp);
        html_fix(time,local_time,4);
        html_fix(time+4,local_time+5,2);
        html_fix(time+6,local_time+8,2);
        printf("抓取时间：%s\n",time);    
    }
    else if( argc == 2 ) {
        //指定了抓取日期
        log_record("---启动内容抓取----按指定日期抓取----------------\n",log_fp);
        strcat(time,argv[1]);     
        printf("抓取时间：%s\n",time);         
    }
    else{   
        // 输入参数错误
        log_record("输入参数错误\n\n\n\n\n\n",log_fp);
        fclose(log_fp);
        printf("输入参数错误\n");
        exit(1);
    }
    
    // 抓取函数，按不同新闻源依次执行
    extract_nfrb(time, log_fp);  // 南方日报
    extract_xhrb(time, log_fp);  // 新华日报
  
    // 资源释放
    log_record("文件操作符资源释放\n\n\n\n\n\n",log_fp);
    fclose(log_fp);
    return 0;
}
