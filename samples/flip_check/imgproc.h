#ifndef _IMGPROC_H_
#define _IMGPROC_H_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

typedef struct _IplImage {
    int width;             /**< Image width in pixels. */
    int height;            /**< Image height in pixels.  */
    int imageSize;         /**< Image data size in bytes)*/
    char *imageData;        /**< Pointer to aligned image data. */
    int widthStep;         /**< Size of aligned image row in bytes.    */
} IplImage;

IplImage *cvCreateImage(int width, int height);
void cvFreeImage(struct _IplImage *image);
// cvCopy
void cvCopyImage(struct _IplImage *src, struct _IplImage *dst);
void cvAbsDiff(struct _IplImage *src1, struct _IplImage *src2, struct _IplImage *dst);
void cvThreshold(struct _IplImage *src, struct _IplImage *dst, int threshold);
void cvErode(struct _IplImage *src, struct _IplImage *dst, int x, int iterations);

#ifdef __cplusplus
}
#endif

#endif //_IMGPROC_H_