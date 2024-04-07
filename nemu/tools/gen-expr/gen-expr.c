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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 256] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

//TODO
uint32_t choose(uint32_t n){
  return rand() % n;
}

static void gen_rand_expr() {
    char append[2] = {'\0', '\0'}; // Initialize append as a null-terminated string
    //int length=0;
    switch (choose(3)) {
        case 0: 
            append[0] = '0' + choose(10); // Generate a random digit
            strcat(buf, append); // Append the character to buf
            break;
        case 1: 
            append[0] = '('; // Add opening parenthesis
            strcat(buf, append); // Append the character to buf
            gen_rand_expr(); // Generate a random expression
            append[0] = ')'; // Add closing parenthesis
            strcat(buf, append); // Append the character to buf
            break;
        default: 
            gen_rand_expr(); // Generate a random expression
            switch (choose(4)) {
                case 0:
                    append[0] = '+'; // Choose a random arithmetic operator
                    break;
                case 1:
                    append[0] = '-';
                    break;
                case 2:
                    append[0] = '*';
                    break;
                default:
                    append[0] = '/';
                    break;
            }
            strcat(buf, append); // Append the operator to buf
            gen_rand_expr(); // Generate another random expression
            break;
    }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    strcpy(buf, "");

    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
