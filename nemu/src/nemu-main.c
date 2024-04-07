/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include </home/shiyang/ics2023/nemu/src/monitor/sdb/sdb.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  //engine_start();
  FILE *input_file = fopen("input", "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // 读取每一行并处理
    char line[256]; // 假设每行不超过 256 个字符
    while (fgets(line, sizeof(line), input_file) != NULL) {
        // 从当前行中提取结果和表达式
        unsigned result;
        char expression[256];
        if (sscanf(line, "%u %[^\n]", &result, expression) != 2) {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }

        // 在这里执行你的测试用例逻辑
        // 例如，调用 expr() 函数处理表达式
        bool *success=NULL;
        if(result==expr(expression,success)){
          printf("Result: %u, Expression: %s,right\n", result, expression);
        }
        else{
          printf("Result: %u, Expression: %s,wrong\n", result, expression);
        }

    }

    // 关闭文件
    fclose(input_file);

    

  return is_exit_status_bad();
}
