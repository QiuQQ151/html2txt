#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "./xhrb.h"
#include "./extract.h"

/*
新华日报抓取主函数
输入：
time：时间，例子：20241013
output_file: 输出文件操作符
*/
void extract_xhrb(char* time, FILE * output_file)
{
   // 资源请求 
   char* date = (char*)malloc( sizeof(char)*10); // 转换日期格式
   strncpy(date,time,6);
   *(date+6) = '/';
   strncpy(date+7,time+6,2);
   *(date+9) = '\0';
   printf("输入日期%s,日期转化为：%s\n",time,date);
   
   // 文章起始标号获取
   char* article_num = extract_article_num(date);
   int num = atoi(article_num);
   printf("文章起始号：%d\n",num);
   
   // 文章内容循环抓取



} 


/*
用于提取新华日报date日期的文章起始号
输入：
date：例子：202410/14
输出：
article_num: char类型
*/
char* extract_article_num(char* date)
{
   // 版面1地址组成
   char* basic_html_first = "https://xh.xhby.net/pc/layout/";
   char* basic_html_end = "/node_1.html";
   // 资源请求
   char* basic_html = (char*)malloc( sizeof(char)*( strlen(basic_html_first) + 9 + strlen(basic_html_end) + 1 ) );
   *basic_html = '\0';

   strcat(basic_html,basic_html_first); //前缀
   strcat(basic_html,date); //中间
   strcat(basic_html,basic_html_end); //后缀
   printf("版面1地址%s\n",basic_html);

   //抽取当天的文章起始号
   char* basic_start = "a href";
   char* basic_concrete1 =  "content_";
   char* basic_concrete2 =  ".html";
   
   char* ret = extract_concrete_content( basic_html, basic_start,basic_concrete1,basic_concrete2);
   printf("获取到当天文章起始号%s\n",ret);

   // 资源释放
   free(basic_html);

   return ret;
}

/*
循环抓取文章内容，保存到output_file中
输入：
start_num:
date:
output_file:
输出：
NULL
*/
void extract_article(int start_num,char* date, FILE* output_file )
{




}

// // 提取HTML中的特定标签内容
// void extract_title(const char *html, const char *start_tag, const char *end_tag, FILE *output_file) {
//     // 被检索的字符串html--要被检索的起始标签start_tag
//     // 返回值：标签起始位置
//     const char *start = strstr(html, start_tag);  
//     if (!start) {
//         fprintf(stderr, "无法找到要检索标签: %s\n", start_tag);
//         return;
//     }
//     // 跳过标签本身
//     start += strlen(start_tag);
//     const char *end = strstr(start, end_tag);
//     if (!end) {
//         fprintf(stderr, "未找到结束标签: %s\n", end_tag);
//         return;
//     }
//     // 写入正文内容
//     // 开始地址，基本单元大小，写入个数，文件指针
//     fwrite("title:", 1,6,output_file);
//     fwrite(start, 1, end - start, output_file);
//     //插入换行符
//     fwrite("\n   ", 1,4,output_file);
// }

// // 提取HTML中的特定标签内容
// void extract_content(const char *html, const char *start_tag, const char *end_tag, FILE *output_file) {
//     // 被检索的字符串html--要被检索的起始标签start_tag
//     // 返回值：正文起始位置
//     const char *start = strstr(html, start_tag);  
//     if (!start) {
//         fprintf(stderr, "无法找到要检索标签: %s\n", start_tag);
//         return;
//     }
//     start += strlen(start_tag);
//     // 返回值：正文结束位置
//     const char *end = strstr(start, end_tag);
//     if (!end) {
//         fprintf(stderr, "未找到结束标签: %s\n", end_tag);
//         return;
//     }
    
//     // 写入正文内容
//     // 去除中间的</p><p>
//     const char *mid1 = start;
//     const char *mid2 = start;
//     int flag = 1;
//     while(1)
//     {
//         // 查找一段
//         mid2 = strstr(mid1, "</p>");
//         if ( mid2 == end ) {
//            fwrite( mid1 , 1, mid2 - mid1, output_file);
//            fwrite("\n   ", 1,4,output_file);
//            fprintf(stderr, "结束标签: %s\n", "</p>");
//            break;
//         }
//         // 写入一段数据
//         fwrite( mid1 , 1, mid2 - mid1, output_file);
//         //插入换行符
//         fwrite("\n   ", 1,4,output_file);
//         // 更新
//         mid1 = mid2 + strlen("</p><p>");;
//     }

    
// }

// int download_and_extract(const char *url, const char *output_file) {
//     CURL *curl;
//     CURLcode res;

//     struct Memory chunk = {0}; //结构体全部赋值为0

//     // 初始化libcurl
//     curl = curl_easy_init();
//     if (!curl) {
//         fprintf(stderr, "无法初始化libcurl\n");
//         return 1;
//     }
//     //配置回调函数（指定的curl，设置参数名称，设定值）
//     curl_easy_setopt(curl, CURLOPT_URL, url); //定义网址
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory); //写入内存
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);  //

//     // 执行HTTP请求
//     res = curl_easy_perform(curl);
//     if ( res != CURLE_OK ) {
//         fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
//         //释放内存
//         curl_easy_cleanup(curl);
//         free(chunk.data);
//         return 1;
//     }

//     // 打开输出文件
//     // fp为文件操作符
//     FILE *fp = fopen(output_file, "w");
//     if (!fp) {
//         fprintf(stderr, "无法打开文件 %s\n", output_file);
//         //释放内存
//         free(chunk.data);
//         curl_easy_cleanup(curl);
//         return 1;
//     }

//     // 提取正文内容
//     // 网页数据存放在chun.data中
//     //第一步：提取标题
//     extract_title(chunk.data, "<h3>", "</h3>", fp);
//     // 第二步：提取正文部分
//     extract_content(chunk.data, "<!--enpcontent--><p>", "</p><!--", fp);

//     // 释放资源
//     fclose(fp);
//     free(chunk.data);
//     curl_easy_cleanup(curl);

//     printf("正文已保存到 %s\n", output_file);
//     return 0;
// }
