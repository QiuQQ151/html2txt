#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
*/
size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *temp = realloc(mem->data, mem->size + total_size + 1);
    if (temp == NULL) {
        fprintf(stderr, "内存分配失败\n");
        return 0;
    }

    mem->data = temp;
    memcpy(&(mem->data[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0'; // 添加字符串结束符
    return total_size;
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

char* extract_concrete_content( char * basic_html,  char * start_tag, char* concrete_start_tag, char* concrete_end_tag) 
{
    //  资源请求，最后要释放！
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0}; //结构体全部赋值为0
    // 初始化libcurl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "无法初始化libcurl\n");
        return "err";
    }
    printf("资源请求完成\n");

    //配置回调函数（指定的curl，设置参数名称，设定值）
    curl_easy_setopt(curl, CURLOPT_URL, basic_html);  //定义网址查找的网页
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory); //写入内存
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);  //
    printf("curl设定完成\n");

    // 执行HTTP请求
    res = curl_easy_perform(curl);
    if ( res != CURLE_OK ) {
        fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
        //释放内存
        curl_easy_cleanup(curl);
        free(chunk.data);
        return "err";
    }
    printf("http请求完成\n");

    // 提取正文内容// 网页数据存放在chun.data中
    // 第一步：大范围定位
    char *start = strstr(chunk.data, start_tag); 
    // 第二步：小范围定位
    char *concrete_start = strstr(start, concrete_start_tag);
    concrete_start+=strlen(concrete_start_tag); //跳过标签
    char *concrete_end = strstr(concrete_start, concrete_end_tag);

    int ge = concrete_end - concrete_start;
    printf("内容定位完成,起始%p,结束%p,间隔%d\n",concrete_start,concrete_end, ge);

    //读取具体的char
    char* ret = (char*)malloc( sizeof(char)*( concrete_end - concrete_start + 2 ) );
    *ret = '\0';
    *concrete_end = '\0';  //标记结束符
    printf("内容%s\n",concrete_start);
    strcat( ret, concrete_start);  //写入字符
    printf("内容提取完成%s\n",ret);

    // 释放资源！
    free(chunk.data);
    curl_easy_cleanup(curl);
    printf("资源释放完成\n");

    // 返回
    return ret;

}

