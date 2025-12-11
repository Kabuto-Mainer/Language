#include <assert.h>
#include "string.h"

#include "NodeFunc.h"
#include "NodeType.h"
#include "ConfigTreeConst.h"

Node_t* readNode (Node_t* parent,
                  char** pose)
{
    assert (parent);
    assert (pose);

    if (**pose != '(')
    {
        printf ("ERROR in %s:%d", __FILE__, __LINE__);
        return;
    }
    ++ *pose;

    char type[10] = "";
    Node_t* cur_node = newNode ();

    if (sscanf (*pose, "%s", type) != 1)
    {
        printf ("ERROR in %s:%d", __FILE__, __LINE__);
        return;
    }
    if (strcmp ("key:", type) == 0)
    {
        cur_node->type = NODE_TYPE_KEY_WORD;

    }
    else if (strcmp ("var:", type) == 0)
    {

    }
    else if (strcmp ("oper:", type) == 0)
    {

    }
    else if (strcmp ("num:", ))
    {}
    else if (strcmp ("def:"))
    {

    }
    else if (strcmp ("block", type))
    {

    }
    else if (strcmp ("next", type))
    {

    }
    else if (strcmp (".", type) == 0)
    {}
}






