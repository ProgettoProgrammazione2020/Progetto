#include<stdio.h>
#include "ip_lib.h"


int main()
{
    ip_mat *mat;
    mat = ip_mat_create(5, 5, 3, 1);
    ip_mat_show(mat);
    ip_mat_free(mat);
    return 0;
}
