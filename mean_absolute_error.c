#include "libpnm.h"
#include "stdlib.h"

void scale_image(struct PGM_Image *image, int new_max_gray_value) {
    for (int row = 0; row < image->height; row++) {
        for (int col = 0; col < image->width; col++) {
            float percentage =
                (image->image[row][col] * 1.0) / image->maxGrayValue;
            int new_value = percentage * new_max_gray_value;

            image->image[row][col] = new_value;
        }
    }

    image->maxGrayValue = new_max_gray_value;
}

float mean_absolute_error(char *file_name_1_ptr, char *file_name_2_ptr) {
    struct PGM_Image image_1, image_2;

    load_PGM_Image(&image_1, file_name_1_ptr);
    load_PGM_Image(&image_2, file_name_2_ptr);

    if (image_1.height != image_2.height || image_1.width != image_2.width) {
        return -1.0;
    }

    // scale image with smaller max gray value
    if (image_1.maxGrayValue < image_2.maxGrayValue) {
        scale_image(&image_1, image_2.maxGrayValue);
    } else if (image_2.maxGrayValue < image_1.maxGrayValue) {
        scale_image(&image_2, image_1.maxGrayValue);
    }

    float result = 0.0;
    int size = image_1.height * image_1.width;

    for (int row = 0; row < image_1.height; row++) {
        for (int col = 0; col < image_1.width; col++) {
            result += abs(image_1.image[row][col] - image_2.image[row][col]);
        }
    }

    return result / size;
}
