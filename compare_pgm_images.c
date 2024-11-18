#include <stdio.h>
#include <stdlib.h>

#include "mean_absolute_error.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments. Received %d\n", argc - 1);
        exit(0);
    }

    char *first_image_name = argv[1];
    char *second_image_name = argv[2];

    float mae = mean_absolute_error(first_image_name, second_image_name);
    printf("mean absolute error = %f\n", mae);
    return mae;
}
