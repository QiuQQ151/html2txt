
void extract_nfrb(char* time, FILE *log);  // 南方日报抓取主函数
char* extract_nfrb_article_html(char* date,int num, FILE* log); // 合成南方日报指定num文章的网页地址
void extract_nfrb_title(char* html,int* count, FILE* output_file, FILE* log); // 提取南方日报标题
int extract_nfrb_content(char* html,int* count, FILE* output_file, FILE* log); // 提取南方日报正文  // 正常返回正文长度，异常返回0
int extract_nfrb_jpg(char* html,int* count, FILE* output_file, FILE* log); // 提取南方日报图片 // 正常返回0，异常返回1
