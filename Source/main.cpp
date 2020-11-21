#include <cstdio>
#include "tree.h"

int
main()
{
    Node n1("node1"), n2("node2");
    n1.add_child(&n2);
    n1.visualize(1);
    return 0;
}
