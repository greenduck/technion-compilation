#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	char * type;
	char * value;
        struct node * brother;
        struct node * son;
      } NODE , * NODEPTR;



//used functions
NODEPTR
  make_node (char* type,char* value, NODEPTR son);


NODEPTR
concat_list(NODEPTR list_head,NODEPTR new_item);

#endif //COMMON_H

