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
void extract_xhrb(char* time, FILE *log)
{  
   // 定义txt文件名
   char* txt_name = (char*)malloc( sizeof(char)*strlen("XHRB20241024.txt   ") );
   *txt_name = '\0';
   strcat(txt_name,"./temp/XHRB");
   strcat(txt_name,time);
   strcat(txt_name,".txt");

   // 新建txt
   FILE *output_file = fopen(txt_name, "w");
   if (!output_file) {
      log_record("无法创建文章保存文件\n",log);
      return;
   }      
   printf("抓取日期：%s，存放地址：%s\n",time,"./temp/xhrb");  

   log_record("开始新华日报抓取\n",log);

   // 1---生成对应日期格式
   char* date = (char*)malloc( sizeof(char)*10); // 转换日期格式
   if( date == NULL ){
      // 错误返回
      return;
   }
   strncpy(date,time,6);
   *(date+6) = '/';
   strncpy(date+7,time+6,2);
   *(date+9) = '\0';
   log_record("  日期转化为：",log);
   fputs(date,log);
   fputs("\n",log);

   // 2---获取指定日期文章标号（起始或中间或结尾）
   char* basic_html_first = "https://xh.xhby.net/pc/layout/";
   char* basic_html_end = "/node_1.html";
   char* article_num = extract_num( basic_html_first,basic_html_end, date, log);
   int num = atoi( article_num); // 转换为整数形式
   printf("提取到的xhrb文章标号%s\n",article_num);
   
   // 3---文章连续抓取
   fputs("每日新华日报\n",output_file);
   fputs("日期：",output_file);
   fputs(date,output_file);
   fputs("\n起始编号：",output_file);
   fputs(article_num,output_file);
   fputs("\n制作：QQQ\n\n\n",output_file);

   int count = 1; // 实际的文章标号
   int i = 150;
   while( i-- )
   {
      // 合成指定标号的文章地址
      char* article_html = extract_xhrb_article_html(date, num, log);
      // 提取图片
      int ret = extract_xhrb_jpg(article_html, &count, output_file, log);
      if( ret == 0){ // 正常抓取，异常跳过
      // 提取标题
      extract_xhrb_title(article_html, &count, output_file, log);
      // 提取正文
      extract_xhrb_content(article_html, &count, output_file, log);
      free(article_html);
      }
      num++;
   }   
   // 4---资源释放
   free(date);
   fclose(output_file);
   log_record("  新华日报抓取结束\n",log);   
} 

/*
// 合成新华日报指定num文章的网页地址
*/
char* extract_xhrb_article_html(char* date,int num, FILE* log)
{
   // 文章地址合成 例子：https://xh.xhby.net/pc/con/202410/14/content_1377538.html
   char*article_html =(char*)malloc( sizeof(char)*strlen("https://xh.xhby.net/pc/con/202410/14/content_1377538.html   ") );
   *article_html = '\0';
   strcat(article_html,"https://xh.xhby.net/pc/con/202410/14/content_1377538.html");
   // 地址局部修正
   html_fix(article_html+27,date,9);

   char* article_num ; //文章标号
   article_num = num2char(num);
   html_fix(article_html+45,article_num,strlen(article_num));

   printf("文章地址更新为：%s\n",article_html);
   return article_html;
}

/*
提取文章标题到文件
输入：
html:文章地址
output_file:保存的地址
count:文章计数
*/
void extract_xhrb_title(char* html,int* count, FILE* output_file, FILE* log)
{
   char* article_title;  //标题
   article_title = extract_concrete_content( html,"newsdetatit","<h3>", "</h3>",log);
   if( article_title == NULL ){
      log_record("  标题抓取出错\n",log);
      return;
   }

   log_record("抓取到文章标题：",log);
   fputs(article_title,log);
   fputs("\n",log);

   // 写入文件中
   // 开始地址，基本单元大小，写入个数，文件指针
   fputs(num2char(*count),output_file);  
   fputs("<标题:",output_file);
   fputs(article_title, output_file);
   //插入换行符
   fputs(">\n  ",output_file);

   log_record("  标题写入完成\n",log);
   // 释放资源
   free(article_title);
}

/*
// 提取文章正文到文件
输入：
html:文章地址
output_file:保存的地址
*/
int extract_xhrb_content(char* html,int* count, FILE* output_file, FILE* log)
{
   char* article_content; //正文
   article_content = extract_concrete_content( html,"<founder-content>","<!--enpcontent--><p>", "</p><!--",log);
   if( article_content == NULL ){
   log_record("  正文抓取出错\n",log);
   return 0;
   }
   
   int length = 0; //记录正文长度
   log_record("抓取到文章正文\n",log);
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
         //fprintf(stderr, "结束标签: %s\n", "</p><p>");
         fputs("<end>",output_file);
         fputs("\n\n",output_file);
         log_record("  正文内容处理结束\n\n",log);
         *count = *count + 1;
         // 释放资源
         free(article_content);
         return length;
      }
      // 写入一段数据
      fwrite( mid1 , 1, mid2 - mid1, output_file);
      length += mid2 - mid1;
      //插入换行符
      fputs("\n  ",output_file);
      // 更新
      mid1 = mid2 + strlen("</p><p>");;
   }
}


/*
提取图片到文件
输入：
html:文章地址
文章计数：1-xxx
输出：
指定保存位置为./jpg/
*/
int extract_xhrb_jpg(char* html,int* count, FILE* output_file, FILE* log)
{
   // 广告或没正文判断
   FILE* temp = fopen("./temp/temp.txt","w"); // 临时用
   int temp_count = 100;
   int article_len = extract_xhrb_content( html, &temp_count, temp, log );
   fclose(temp);
   if( article_len <= 10 ){
      // 无正文 或 正文太短
      printf("正文异常，舍弃！\n");
      return 1; // 异常返回
   }
   
   fputs(html,output_file); // 测试用
   fputs("\n",output_file);

   char* article_jpg;  //文章对应的jpg
   article_jpg = extract_concrete_content( html,"<img","../pic/", "\">",log);
   if( article_jpg == NULL){
      log_record("  图片地址抓取出错，无图片？\n",log);
      fputs("<图片:n>\n",output_file); 
      return 0;
   }
   log_record("抓取到图片，开始下载",log);
   // 合成图片网页地址 https://doss.xhby.net/zpaper/xhrb/pc/pic/+article_jpg
   char* jpg_html = (char*)malloc( sizeof(char)*( 40 + strlen(article_jpg) + 5 ) );
   if( jpg_html == NULL ){
      log_record("  内存分配错误\n",log);
      fputs("<图片:n>\n",output_file);
      return 0;
   }
   *jpg_html = '\0';
   strcat(jpg_html,"https://doss.xhby.net/zpaper/xhrb/pc/pic/");
   strcat(jpg_html,article_jpg);

   // 合成图片存储路径
   char* num = num2char(*count);
   char* jpg_num = (char*)malloc( sizeof(char)*strlen( "./jpg/xhrbjpg/999.jpg   " ) );
   if( jpg_num == NULL ){
      log_record("  内存分配错误\n",log);
      fputs("<图片:n>\n",output_file);
      return 0;
   }
   *jpg_num = '\0';
   strcat(jpg_num,"./jpg/xhrbjpg/");
   strcat(jpg_num,num);
   strcat(jpg_num,".jpg");
   // 保存在./jpg/*.jpg
   download_image(jpg_html,jpg_num);
   log_record("  jpg保存完成\n",log);
   fputs("<图片:",output_file);
   fputs(num,output_file);
   fputs(".jpg>\n",output_file);
   // 释放资源
   free(num);
   free(jpg_num);
   return 0;
}
