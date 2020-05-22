#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"
#include <string.h>

#define COUNT_TEST 5
#define FORMAT ".bmp"


char * concat_string(char const *string1, char const *string2)
{
  int i;
  char *result = (char *) malloc(sizeof(string1) + sizeof(string2));
  result = strcpy(result, string1);
  for(i = 0; i < strlen(string2); i++)
  {
    result[i + strlen(string1)] = string2[i];
  }
  result[++i] = 0;

  return result;
}

void test(char const *source, char const *destination, int select)
{
  ip_mat *filter = NULL;

  ip_mat *test_ipmat = NULL, *result = NULL;
  Bitmap *test_Bitmap = NULL, *result_ = NULL;

  test_Bitmap = bm_load(source); /*carico il file*/

  test_ipmat = bitmap_to_ip_mat(test_Bitmap);/*converto in ip_mat*/

  switch(select)/*FACCIO ROBA*/
  {
    case 0: filter = create_sharpen_filter();
            break;
    case 1: filter = create_edge_filter();
            break;
    case 2: filter = create_emboss_filter();
            break;
    case 3: filter = create_average_filter(3,3,3);
            break;
    case 4: filter = create_gaussian_filter(9,9,3,5.0);
            break;
  }

  result = ip_mat_convolve(test_ipmat,filter);
  
  clamp(result,0.0,255.0);

  result_ = ip_mat_to_bitmap(result); /*converto in bitmap*/

  ip_mat_free(test_ipmat);/*LIBERAZIONE MEMORIA*/
  ip_mat_free(result);
  ip_mat_free(filter);
  bm_free(test_Bitmap);

  bm_save(result_,destination);/*salvo l'immagine su filter*/
  bm_free(result_);

}

int main(){
    int i;
    char source[] = "flower.bmp";
    char destination[][30] = {"sharpen_flower.bmp", "edge_flower.bmp", "emboss_flower.bmp", "average_filter.bmp", "gaussian_filter.bmp"};
    for(i = 0; i < COUNT_TEST; i++)
    {
      printf("%s --> %s\n", source, destination[i]);
      test(source, destination[i],i);
    }
    return 0;

}
