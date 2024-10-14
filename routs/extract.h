#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>




size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp);
char* extract_concrete_content( char * basic_html,  char * start_tag, char* concrete_start_tag, char* concrete_end_tag);