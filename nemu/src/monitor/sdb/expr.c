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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  
  TK_NOTYPE = 256, TK_EQ,
  TK_NUMBER,TK_PLUS,TK_MINUS,TK_MUL,TK_DIV,
  TK_LEFTPAR,TK_RIGHTPAR,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"[0-9]+",TK_NUMBER},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus
  {"-",TK_MINUS},
  {"\\*",TK_MUL},
  {"/",TK_DIV},
  {"==", TK_EQ},        // equal
  {"\\(", TK_LEFTPAR},
  {"\\)", TK_RIGHTPAR},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}
//write token down
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
            case TK_NOTYPE:break;
            case TK_NUMBER:
                tokens[nr_token].type=TK_NUMBER;
                strncpy(tokens[nr_token].str,substr_start,substr_len);
                tokens[nr_token].str[substr_len]='\0';
                nr_token++;
                break;
            case TK_EQ:
                tokens[nr_token].type=TK_EQ;
                nr_token++;
                break;
            case TK_PLUS:
                tokens[nr_token].type=TK_PLUS;
                nr_token++;
                break;
            case TK_MINUS:
                tokens[nr_token].type=TK_MINUS;
                nr_token++;
                break;
            case TK_MUL:
                tokens[nr_token].type=TK_MUL;
                nr_token++;
                break;
            case TK_DIV:
                tokens[nr_token].type=TK_DIV;
                nr_token++;
                break;
            case TK_LEFTPAR:
                tokens[nr_token].type=TK_LEFTPAR;
                nr_token++;
                break;
            case TK_RIGHTPAR:
                tokens[nr_token].type=TK_RIGHTPAR;
                nr_token++;
                break;
          default: printf("I DONT KNOW");
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(Token *tokens,int leftpositon,int rightposition){
  int stack=0;
  //if(leftpositon==rightposition&&tokens[leftpositon].type== TK_NUMBER){
  //  return  true;
  //}
  if(tokens[leftpositon].type!=TK_LEFTPAR||tokens[rightposition].type!=TK_RIGHTPAR)
    return false;
  else {
    while(leftpositon+1<=rightposition-1){
      if(tokens[leftpositon+1].type==TK_LEFTPAR)
        stack++;
      if(tokens[leftpositon+1].type==TK_RIGHTPAR)
        stack--;
      
      if(stack<0){
        return false;
      }
      
      leftpositon++;
    }
  }
  if(stack==0)
    return true;
  else
    return false;
}

bool onlycheck_parentheses(Token *tokens,int leftpositon,int rightposition){
  int stack=0;
  while(leftpositon<=rightposition){
    if(tokens[leftpositon].type==TK_LEFTPAR)
        stack++;
    if(tokens[leftpositon].type==TK_RIGHTPAR)
        stack--;

    if(stack<0){
      return false;
    }
      
    leftpositon++;
  }
  
  if(stack==0)
    return true;
  else
    return false;
}

word_t evaluate(Token *tokens,int leftpositon,int rightposition){
  if(leftpositon>rightposition){
    assert(0);
  }
  if(leftpositon==rightposition){
    word_t sum=0;
    word_t strpos=0;
    while(tokens[leftpositon].str[strpos]!='\0'){
      sum=sum*10+(tokens[leftpositon].str[strpos]-'0');
      strpos++;
    }
    return sum; 
  }
  else if(check_parentheses(tokens,leftpositon,rightposition) == true){

    return evaluate(tokens,leftpositon+1,rightposition-1);
  }
  else{
    word_t op=0;
    for(int i=rightposition;i>=leftpositon;i++){
      if((tokens[i].type==TK_PLUS||tokens[i].type==TK_MINUS)&&onlycheck_parentheses(tokens,leftpositon,i-1)&&onlycheck_parentheses(tokens,i+1,rightposition)){
        op=i;
        break;
      }
      if((tokens[i].type==TK_MUL||tokens[i].type==TK_DIV)&&onlycheck_parentheses(tokens,leftpositon,i-1)&&onlycheck_parentheses(tokens,i+1,rightposition)){
        op=i;
      }
    }
  
    /*op = the position of 主运算符 in the token expression;*/
    word_t val1 = evaluate(tokens,leftpositon, op - 1);
    word_t val2 = evaluate(tokens,op + 1, rightposition);

    switch (tokens[op].type) {
      case TK_PLUS: return val1 + val2;
      case TK_MINUS: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: return val1 / val2;
      default: assert(0);
    }  

  }
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  //the token has been stored in tokens[32]
  /* TODO: Insert codes to evaluate the expression. */
  printf("the result is %u\n",evaluate(tokens,0,nr_token-1)) ;

  return evaluate(tokens,0,nr_token-1);
}
