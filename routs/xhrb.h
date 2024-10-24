
void extract_xhrb(char* time, FILE *log);  // 新华日报抓取主函数
char* extract_xhrb_article_html(char* date,int num, FILE* log); // 合成新华日报指定num文章的网页地址
int extract_xhrb_jpg(char* html,int* count, FILE* output_file, FILE* log);  //抓取图片 //正常返回0，异常返回1
void extract_xhrb_title(char* html,int* count, FILE* output_file, FILE* log);// 提取文章标题到文件
int extract_xhrb_content(char* html,int* count, FILE* output_file, FILE* log);// 提取文章正文 // 正常返回正文长度，异常返回0