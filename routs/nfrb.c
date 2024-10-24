#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "./nfrb.h"
#include "./extract.h"

/*
// 南方日报抓取主函数
传入：
抓取时间
日志保存位置
*/
void extract_nfrb(char* time, FILE *log)
{   // 合成txt名字
    char* txt_name =(char*)malloc( sizeof(char)*strlen("./temp/NFRB20241024.txt  ") );
    *txt_name = '\0';
    strcat(txt_name,"./temp/NFRB");
    strcat(txt_name,time);
    strcat(txt_name,".txt");

    // 新建txt
    FILE* output_file = fopen(txt_name,"w");
    if(output_file == NULL ){
        // 创建文件失败
        printf("NFRB创建失败%s\n",txt_name);
        return;
    }
    
    // 提取当日新闻标号
    // 格式https://epaper.nfnews.com/nfdaily/html/202410/24/node_A01.html
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
    char* basic_html_first = "https://epaper.nfnews.com/nfdaily/html/";
    char* basic_html_end = "/node_A01.html";
    char* article_num = extract_num( basic_html_first,basic_html_end, date, log);
    int num = atoi( article_num); // 转换为整数形式
    printf("提取到的nfrb文章标号%s\n",article_num);
    
    // 3---在获取的文章标号附近批量抓取
    fputs("每日南方日报\n",output_file);
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
        char* article_html = extract_nfrb_article_html(date, num, log);
        // 提取图片
        int ret = extract_nfrb_jpg(article_html, &count, output_file, log);
        if( ret == 0){ // 正常抓取，异常跳过
        // 提取标题
        extract_nfrb_title(article_html, &count, output_file, log);
        // 提取正文
        extract_nfrb_content(article_html, &count, output_file, log);
        free(article_html);
        }
        num--;
    }
    num = atoi( article_num)+1; // 转换为整数形式
    i = 150;
    while( i-- )
    {
        // 合成指定标号的文章地址
        char* article_html = extract_nfrb_article_html(date, num, log);
        // 提取图片
        int ret = extract_nfrb_jpg(article_html, &count, output_file, log);
        if( ret == 0){ // 正常抓取，异常跳过
        // 提取标题
        extract_nfrb_title(article_html, &count, output_file, log);
        // 提取正文
        extract_nfrb_content(article_html, &count, output_file, log);
        free(article_html);
        }
        num++;
    }

    // 4---释放资源
    fclose(output_file);
}

/*
// 合成南方日报指定num文章的网页地址
*/
char* extract_nfrb_article_html(char* date,int num, FILE* log)
{
    // 文章地址合成 例子：https://epaper.nfnews.com/nfdaily/html/202410/24/content_10115758.html
    char*article_html =(char*)malloc( sizeof(char)*100 );
    *article_html = '\0';
    strcat(article_html,"https://epaper.nfnews.com/nfdaily/html/202410/24/content_10115758.html");
    // 地址局部修正
    html_fix(article_html+39,date,9);
    
    char* article_num ; //文章标号
    article_num = num2char(num);
    html_fix(article_html+57,article_num,strlen(article_num));
    
    printf("文章地址更新为：%s\n",article_html);
    return article_html;
}

/*
// 提取南方日报标题
*/
void extract_nfrb_title(char* html, int* count, FILE* output_file, FILE* log)
{
   char* article_title;  //标题
   article_title = extract_concrete_content( html,"primers","<h1><p>", "</p></h1>",log);
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
// 提取南方日报正文
*/
int extract_nfrb_content(char* html, int* count, FILE* output_file, FILE* log)
{   
    //printf("提取正文的网址：%s\n",html);
    char* article_content; //正文
    article_content = extract_concrete_content( html,"<!--enpcontent--><html>","<p>&nbsp;&nbsp;&nbsp;&nbsp;", "<p></p>\n</body>",log);
    if( article_content == NULL ){
      log_record("  正文抓取出错\n",log);
      // printf("没抓取到南方日报文章\n");
      return 0;
    }
      
   log_record("抓取到文章正文\n",log);
   int length =0; // 正文长度计数
    // 写入正文内容
    // 去除中间的<p> 正文 </p>
    const char *mid1 = article_content; //mid1初始值指向正文第一个字
    const char *mid2 = article_content;
    while(1)
    {
        // 查找一段
        mid2 = strstr(mid1, "</p>");
        if ( !mid2) { 
           //标签不全
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
        length +=(mid2 - mid1);
        //插入换行符
        fputs("\n  ",output_file);
        // 更新midl
        mid1 =strstr(mid2,"<p>&nbsp;&nbsp;&nbsp;&nbsp;");
        if ( !mid1 ) { 
            //标签不全
            fputs("<end>",output_file);
            fputs("\n\n",output_file);
            log_record("  正文内容处理结束\n\n",log);
            // 释放资源
            free(article_content);
            *count = *count + 1;
            return length;
        }
        mid1 +=strlen("<p>&nbsp;&nbsp;&nbsp;&nbsp;");
    }
}

/*
// 提取南方日报图片
*/
int extract_nfrb_jpg(char* html, int* count, FILE* output_file, FILE* log) 
{
   // 广告或没正文判断
   FILE* temp = fopen("./temp/temp.txt","w"); // 临时用
   int temp_count = 100;
   int article_len = extract_nfrb_content( html, &temp_count, temp, log );
   fclose(temp);
   if( article_len <= 10 ){
      // 无正文 或 正文太短
      printf("正文异常，舍弃！\n");
      return 1; // 异常返回
   }
   
   fputs(html,output_file); // 测试用
   fputs("\n",output_file);

   // 图片抓取
   char* article_jpg;  //文章对应的jpg
   article_jpg = extract_concrete_content( html,"gallery","<img src=\"../../../", "\"></td></tr>",log);
   if( article_jpg == NULL){
      log_record("  图片地址抓取出错，无图片？\n",log);
      printf("无图片\n");
      fputs("<图片:n>\n",output_file); 
      return 0;
   }
   //printf("图片下载局部地址：%s\n",article_jpg);
   log_record("抓取到图片，开始下载",log);

   // 合成图片网页地址 https://epaper.nfnews.com/nfdaily/res/202410/24/c2b7dbeb-dda9-47dc-993a-c8dbb4ac854d.jpg.2
   char* jpg_html = (char*)malloc( sizeof(char)*( strlen("https://epaper.nfnews.com/nfdaily/") + strlen(article_jpg) + 5 ) );
   if( jpg_html == NULL ){
      log_record("  内存分配错误\n",log);
      fputs("<图片:n>\n",output_file);
      return 0;
   }
   *jpg_html = '\0';
   strcat(jpg_html,"https://epaper.nfnews.com/nfdaily/");
   strcat(jpg_html,article_jpg);
   //printf("图片下载地址合成：%s\n",jpg_html);

   // 合成图片存储路径
   char* num = num2char(*count);
   char* jpg_num = (char*)malloc( sizeof(char)*strlen( "./jpg/nfrbjpg/99999.jpg") );
   if( jpg_num == NULL ){
      log_record("  内存分配错误\n",log);
      fputs("<图片:n>\n",output_file);
      free(article_jpg);
      return 0;
   }
   *jpg_num = '\0';
   strcat(jpg_num,"./jpg/nfrbjpg/");
   strcat(jpg_num,num);
   strcat(jpg_num,".jpg");
   // 保存在./jpg/nfrbjpg/*.jpg
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