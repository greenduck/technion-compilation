#include "common.h"
/**************************************************************************/
/*                             MAKE a node                                */
/**************************************************************************/

NODEPTR
  make_node (char* type,char* value, NODEPTR son)
{
  NODEPTR p;

  if ((p = (NODEPTR)(malloc (sizeof (struct node)))) == 0)
    printf ("could'nt allocate memory\n");
  else {
    p->type = strdup (type);
    p->value = strdup (value);
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

