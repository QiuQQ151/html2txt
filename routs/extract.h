#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>




size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp,FILE *log);
char* extract_concrete_content( char * basic_html,  char * start_tag, char* concrete_start_tag, char* concrete_end_tag,FILE *log);
void html_fix(char* dest, char* source, int num ); //替换html指定位置的num个字符
char* num2char(int num); //整数转化为字符串
char* get_time(void); // 获取系统时间
void log_record(char* message,FILE*log); //日志记录