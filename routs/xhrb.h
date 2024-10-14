

void extract_xhrb(char* time, FILE *output_file);  // 新华日报抓取主函数
char* extract_article_num(char* date); // 提取出当天文章标号起始
void extract_article(int start_num,char* date, FILE* output_file ); //抓取文章内容
void extract_title(char *html, FILE *output_file);// 提取文章标题到文件
void extract_content(char *html, FILE *output_file);// 提取文章正文