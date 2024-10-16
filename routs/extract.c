#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>

#include "./extract.h"

/*
此文件定义了通用的网页抓取工具
*/

/*
动态内存结构体，用于存储网页数据
*/
struct Memory {
    char *data;
    size_t size; //无符号整数类型，定义对象的索引或大小
};

/*
回调函数：将HTTP响应数据写入内存
处理文本的
*/
size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp,FILE *log) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *temp = realloc(mem->data, mem->size + total_size + 1);
    if (temp == NULL) {
        log_record("内存申请失败\n",log);
        return 0;
    }

    mem->data = temp;
    memcpy(&(mem->data[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0'; // 添加字符串结束符
   // log_record("HTTP响应数据写入内存",log);  //这里会报错？为何？
    return total_size;
}


/*
写入回调函数，将数据写入文件
处理图片的
*/
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}


/*
提取版面HTML中的具体标签中的内容
输入值：
basic_html: 版面基本地址
article_start_html: 文章地址前缀
start_tag：起始搜索位置（大范围）
concrete_start_tag：具体位置（精确范围）
concrete_end_tag：具体位置（精确范围）
返回值：标签中的具体内容
*/

char* extract_concrete_content( char * basic_html,  char * start_tag, char* concrete_start_tag, char* concrete_end_tag,FILE *log) 
{
    log_record("开始抓取指定内容\n",log);
    //  资源请求，最后要释放！
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0}; //结构体全部赋值为0
    // 初始化libcurl
    curl = curl_easy_init();
    if (!curl) {
        log_record("  无法初始化libcurl，错误返回\n",log); 
        char* err =(char*)malloc( sizeof(char)*1 );
        *err = '\0';
        return NULL;
    }
    log_record("  curl资源请求完成\n",log);

    //配置回调函数（指定的curl，设置参数名称，设定值）
    curl_easy_setopt(curl, CURLOPT_URL, basic_html);  //定义网址查找的网页
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory); //写入内存
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);  //
    log_record("  curl设定完成\n",log);
    fflush(log);

    // 执行HTTP请求
    res = curl_easy_perform(curl);
    if ( res != CURLE_OK ) {
        log_record("  HTTP请求失败，错误返回\n",log);
        fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
       // 释放内存
        curl_easy_cleanup(curl);
        free(chunk.data);
        fflush(log);
        char* err =(char*)malloc( sizeof(char)*1 );
        *err = '\0';
        return NULL;
    }
    log_record("  http请求完成\n",log);


    // 提取正文内容// 网页数据存放在chun.data中
    // 第一步：大范围定位start
    char *start = strstr(chunk.data, start_tag); 
    if( start == NULL ){
        log_record("  大范围定位失败\n",log);
        return NULL;
    }
    // 第二步：小范围定位
    char *concrete_start = strstr(start, concrete_start_tag);
    if( concrete_start == NULL ){
        log_record("  小范围左定位失败\n",log);
        return NULL;
    }
    concrete_start+=strlen(concrete_start_tag); //跳过标签     //问题点
    char *concrete_end = strstr(concrete_start, concrete_end_tag);  // 问题点
    if( concrete_end == NULL ){
        log_record("  小范围右定位失败\n",log);
        return NULL;
    }
    log_record("  内容定位完成\n",log);

    //读取具体的char
    char* ret = (char*)malloc( sizeof(char)*( concrete_end - concrete_start + 2 ) );
    *ret = '\0';
    *concrete_end = '\0';  //标记结束符
    strcat( ret, concrete_start);  //写入字符

    log_record("  内容提取完成\n",log);


    // 释放资源！
    free(chunk.data);
    curl_easy_cleanup(curl);

    log_record("  资源释放完成，结果返回\n",log);
    // 返回
    return ret;

}

/*
下载指定网址的图片到指定保存位置
*/
void download_image(char *url, char *filename) 
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            fprintf(stderr, "无法打开文件 %s\n", filename);
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "下载图片失败: %s\n", curl_easy_strerror(res));
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
}

/*
//替换html指定位置的num个字符
输入：
dest：html
soure：字符来源
num：个数
*/
void html_fix(char* dest, char* source, int num )
{
  for(int i = 0; i<num; i++ ){
       *(dest+i) = *(source+i);
  }
}

/*
整数转化为字符串
*/
// 整数转化为字符串的函数
char* num2char(int num) {
    // 计算所需的字符串长度（包括符号和'\0'结尾符）
    int length = snprintf(NULL, 0, "%d", num) + 1;
    // 动态分配内存以存储字符串
    char* str = (char*)malloc(length);
    if (str == NULL) {
        perror("内存分配失败\n");
        return NULL;
    }
    // 将整数转换为字符串并存入str
    snprintf(str, length, "%d", num);
    return str;  // 返回指向字符串的指针
}

/*
获取系统时间
输出：
时间字符串
*/
char* get_time(void)
{
    // 获取当前系统时间
    time_t now = time(NULL);
    if (now == -1) {
        perror("无法获取系统时间\n");
        return NULL;
    }

    // 转换为本地时间结构体
    struct tm* local_time = localtime(&now);

    // 格式化时间为 "YYYY-MM"
    // YYYY-MM 占用 7 个字符 + 1 个 '\0'
    char* time_str = (char*)malloc( sizeof(char)*40 );
    strftime(time_str, 40, "%Y-%m-%d %H:%M:%S", local_time);
    *(time_str+19) ='\0';
    return time_str;
}

/*
日志记录，自带换行
输入：
message：记录事件
log:记录保存位置
*/
void log_record(char* message,FILE*log)
{
   fputs(get_time(),log);
   fputs("  ",log);
   fputs(message,log);
}