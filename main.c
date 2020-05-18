#include <stdio.h>
#include <string.h>
#include "ip_lib.h"
#include "bmp.h"


int main()
{
    ip_mat *mat, *mat_new;
    mat = ip_mat_create(5, 5, 3, 1);
    mat_new = ip_mat_padding(mat, 2, 2);
    ip_mat_show(mat);
    ip_mat_show(mat_new);
    ip_mat_free(mat);
    ip_mat_free(mat_new);
    return 0;
}
