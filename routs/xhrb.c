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
   extract_article( num , date, output_file);


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
void extract_article(int num,char* date, FILE* output_file )
{
    // 文章地址合成 例子：https://xh.xhby.net/pc/con/202410/14/content_1377538.html
    // 资源请求
    char*article_html =(char*)malloc( sizeof(char)*70 );
    *article_html = '\0';
    strcat(article_html,"https://xh.xhby.net/pc/con/202010/10/content_1265423.html");
    // 地址局部修正
    html_fix(article_html+27,date,9);
    char* article_num ; //文章标号
    
    // 持续抓取
    int i = 100;
    while(i--)
    {
       article_num = num2char(num);
       html_fix(article_html+45,article_num,7);
       printf("文章地址为：%s\n",article_html);
       extract_title(article_html,output_file);
       extract_content(article_html,output_file);
       num++;
    }

}

/*
提取文章标题到文件
输入：
html:文章地址
output_file:保存的地址
*/
void extract_title(char *html, FILE *output_file)
{
   char* article_title;  //标题
   article_title = extract_concrete_content( html,"newsdetatit","<h3>", "</h3>");
   printf("抓取到文章标题：%s\n",article_title);

   // 写入文件中
   // 开始地址，基本单元大小，写入个数，文件指针
   fputs("Title:",output_file);
   fputs(article_title, output_file);
   //插入换行符
   fputs("\n    ",output_file);
   printf("标题写入完成\n");

   // 释放资源
   free(article_title);
}

/*
// 提取文章正文到文件
输入：
html:文章地址
output_file:保存的地址
*/
void extract_content(char *html, FILE *output_file)
{
    char* article_content; //正文
    article_content = extract_concrete_content( html,"<founder-content>","<!--enpcontent--><p>", "</p><!--");
    printf("抓取到文章正文\n");
    
    // 写入正文内容
    // 去除中间的</p><p>
    const char *mid1 = article_content;
    const char *mid2 = article_content;
    int flag = 1;
    while(1)
    {
        // 查找一段
        mid2 = strstr(mid1, "</p><p>");
        if ( !mid2 ) {
           fprintf(stderr, "结束标签: %s\n", "</p><p>");
           fputs("\n\n",output_file);
           // 释放资源
           free(article_content);
           return;
        }
        // 写入一段数据
        fwrite( mid1 , 1, mid2 - mid1, output_file);
        //插入换行符
        fputs("\n    ",output_file);
        // 更新
        mid1 = mid2 + strlen("</p><p>");;
    }
}
