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

#include "sdb.h"

#define NR_WP 32

/*typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *expr_of_wp;
  word_t oldval;
  word_t newval;

  

  } WP;*/

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

//move a wp from free_ to head's tail
WP* new_wp(){
  if(free_==NULL){
    assert(0);
  }

  if(head!=NULL){
    WP *returnptr=free_;
    free_=free_->next;
    returnptr->next=NULL;
    head->next=returnptr;
    return head;
  }
  else{
    WP *returnptr=free_;
    free_=free_->next;
    returnptr->next=NULL;
    head=returnptr;
    return head;
  }

}

void free_wp(WP *wp){
  wp->next=free_;
  free_=wp;
}

