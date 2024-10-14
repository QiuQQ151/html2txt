

void extract_xhrb(char* time, FILE *output_file,FILE *log);  // 新华日报抓取主函数
char* extract_article_num(char* date,FILE *log); // 提取出当天文章标号起始
void extract_article(int start_num,char* date, FILE* output_file ,FILE *log); //抓取文章内容
void extract_title(char *html, FILE *output_file,int count,FILE *log);// 提取文章标题到文件
void extract_content(char *html, FILE *output_file,FILE *log);// 提取文章正文