#include "common.h"
/**************************************************************************/
/*                             MAKE a node                                */
/**************************************************************************/

NODEPTR
  make_node (const char* type,char* value, NODEPTR son)
{
  NODEPTR p;

  if ((p = (NODEPTR)(malloc (sizeof (struct node)))) == 0)
    printf ("could'nt allocate memory\n");
  else {
    p->type = (type != NULL) ? strdup (type) : NULL;
    p->value = (value != NULL) ? strdup (value) : NULL;
    p->son = son;
    p->brother = 0;
  }
  return(p);
}


/**************************************************************************/
/*                           Concate item to list                         */
/**************************************************************************/

NODEPTR
concat_list(NODEPTR list_head,NODEPTR new_item)
{
  NODEPTR last_brother;

  if (list_head == NULL) return new_item;

  last_brother = list_head;
  while (last_brother->brother != (NODEPTR) NULL)
    last_brother = last_brother->brother;  /* findes the last brother in 
                                              the list */

  last_brother->brother = new_item;  /* concate the new item to the list */

return (list_head);  
}


/*
 * Tree Traversal
 */

void traverse_dfs(NODEPTR list_head)
{
	printf("(");
	printf("<%s,%s>", list_head->type, list_head->value != NULL ? list_head->value : "");

	if (list_head->son != NULL) {
		traverse_dfs(list_head->son);
	}

	printf(")");

	if (list_head->brother != NULL) {

		printf("\n");

		traverse_dfs(list_head->brother);
	}
}

