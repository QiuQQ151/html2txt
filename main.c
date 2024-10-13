#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// 动态内存结构体，用于存储网页数据
struct Memory {
    char *data;
    size_t size; //无符号整数类型，定义对象的索引或大小
};

// 回调函数：将HTTP响应数据写入内存
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

// 提取HTML中的特定标签内容
void extract_content(const char *html, const char *start_tag, const char *end_tag, FILE *output_file) {
    const char *start = strstr(html, start_tag);
    if (!start) {
        fprintf(stderr, "未找到标签: %s\n", start_tag);
        return;
    }

    // 跳过标签本身
    start += strlen(start_tag);

    const char *end = strstr(start, end_tag);
    if (!end) {
        fprintf(stderr, "未找到结束标签: %s\n", end_tag);
        return;
    }

    // 写入正文内容
    fwrite(start, 1, end - start, output_file);
}

int download_and_extract(const char *url, const char *output_file) {
    CURL *curl;
    CURLcode res;

    struct Memory chunk = {0}; //结构体全部赋值为0

    // 初始化libcurl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "无法初始化libcurl\n");
        return 1;
    }
    //配置回调函数（指定的curl，设置参数名称，设定值）
    curl_easy_setopt(curl, CURLOPT_URL, url); //定义网址
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory); //写入内存
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);  //

    // 执行HTTP请求
    res = curl_easy_perform(curl);
    if ( res != CURLE_OK ) {
        fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
        //释放内存
        curl_easy_cleanup(curl);
        free(chunk.data);
        return 1;
    }

    // 打开输出文件
    // fp为文件操作符
    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        fprintf(stderr, "无法打开文件 %s\n", output_file);
        //释放内存
        free(chunk.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    // 提取正文内容
    // 网页数据存放在chun.data中
    extract_content(chunk.data, "<!--enpcontent-->", "</founder-content>", fp);

    // 释放资源
    fclose(fp);
    free(chunk.data);
    curl_easy_cleanup(curl);

    printf("正文已保存到 %s\n", output_file);
    return 0;
}


//主函数//
/* 传递参数形式
url：网址
output_file：输出文件名
*/
int main(int argc, char *argv[]) {
    // 输入参数错误
    if (argc != 3) {
        fprintf(stderr, "用法: %s <URL> <输出文件>\n", argv[0]);
        return 1;
    }
    // 参数定义
    const char *url = argv[1];
    const char *output_file = argv[2];
    return download_and_extract(url, output_file);
}
