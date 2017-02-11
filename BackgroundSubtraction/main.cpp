#include <stdio.h>
#include <math.h>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#define hige_scale_x 1.5
#define hige_scale_y 1
#define sg_scale_x 1.5
#define sg_scale_y 1.5

int main(){
    
    /*画像読み込み グレースケール作成*/
    IplImage *img;
	IplImage *sunglass_img=cvLoadImage("C:/Users/ejikken/Desktop/face2/sunglass.png",1);
	IplImage *sunglass_img2=cvLoadImage("C:/Users/ejikken/Desktop/face2/sunglass2.png",1);
	IplImage *sunglass_img3=cvLoadImage("C:/Users/ejikken/Desktop/face2/sunglass3.png",1);
	IplImage *hige_img = cvLoadImage("C:/Users/ejikken/Desktop/face2/hige.jpeg",1);
	IplImage *hige_img2 = cvLoadImage("C:/Users/ejikken/Desktop/face2/hige2.png",1);
	IplImage *hige_img3 = cvLoadImage("C:/Users/ejikken/Desktop/face2/hige3.png",1);
	
    CvCapture *capture;

	// カメラを初期化する
	if ((capture = cvCreateCameraCapture(-1)) == NULL)
	{
		return -1;
	}

	img = cvQueryFrame(capture);
	
	IplImage *gray= cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
    
    
    /*分類器のカスケードを読み込み*/
	CvHaarClassifierCascade* face_cas = (CvHaarClassifierCascade *) cvLoad("C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml", 0, 0, 0);
	CvHaarClassifierCascade* eye_cas = (CvHaarClassifierCascade *) cvLoad("C:/OpenCV2.1/data/haarcascades/haarcascade_mcs_eyepair_small.xml", 0, 0, 0);
	CvHaarClassifierCascade* nose_cas = (CvHaarClassifierCascade *) cvLoad("C:/OpenCV2.1/data/haarcascades/haarcascade_mcs_nose.xml", 0, 0, 0);
    
    /*メモリの確保*/
    CvMemStorage *storage = 0;
    storage = cvCreateMemStorage(0);
    cvClearMemStorage(storage);

	int i;
	CvScalar colors[] = {
	{{0, 0, 255}}, {{0, 128, 255}},
	{{0, 255, 255}}, {{0, 255, 0}},
	{{255, 128, 0}}, {{255, 255, 0}},
	{{255, 0, 0}}, {{255, 0, 255}}
	};

	int key;
	int hige = 0;
	int sunglass = 0;
	int background = 0;
	int swap = 0;

	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("*----------------------------*\n");
	printf("|   コマンド一覧             |\n");
	printf("|                            |\n");
	printf("|   ひげ: h                  |\n");
	printf("|   サングラス: s            |\n");
	printf("|   顔入れ替え: w            |\n");
	printf("|   写真撮る: spacebar       |\n");
	printf("|   ヘルプ: c                |\n");
	printf("|   終了: q                  |\n");
	printf("|                            |\n");
	printf("*----------------------------*\n");

	while(1)
	{

		// キャプチャ画像の取得に失敗したら処理を中断
		img = cvQueryFrame(capture);
		if (img == NULL)	break;

		cvCvtColor(img, gray, CV_BGR2GRAY);
		cvEqualizeHist(gray, gray);
		
		/*顔の検出*/
		CvSeq *faces;
		faces = cvHaarDetectObjects(gray, face_cas, storage, 1.11, 4, 0);
		
		/*顔の位置に矩形を描画*/
		for (i = 0; i < (faces ? faces->total : 0); i++) {
			//printf("%d\n", i);
			CvRect *r = (CvRect *) cvGetSeqElem (faces, i);
			
			int x = r->x;
			int y = r->y;
			int h = r->height;
			int w = r->width;

			if(sunglass >= 1)
			{
				CvSeq *eyes;
				eyes = cvHaarDetectObjects(gray, eye_cas, storage, 1.11, 4, 0);
				for (int j = 0; j < (eyes ? eyes->total : 0); j++) {
					CvRect *re = (CvRect *) cvGetSeqElem (eyes, j);
					if(y < re->y && x < re->x && re->y < y + h && re->x < x + w){
						IplImage *prop = cvCreateImage(cvSize((int)(re->width * sg_scale_x), (int)(re->height * sg_scale_y)), IPL_DEPTH_8U, 3);
						if(sunglass == 1){
							cvResize(sunglass_img, prop, CV_INTER_NN);
						}
						else if(sunglass == 2) {
							cvResize(sunglass_img2, prop, CV_INTER_NN);
						}
						else if(sunglass == 3) {
							cvResize(sunglass_img3, prop, CV_INTER_NN);
						}
						if(sunglass==1 || sunglass==3){
							for(int width = 0; width < prop->width;width++)
							{
								for(int height = 0; height < prop->height;height++)
								{
									if(re->x + width<img->width || re->y + height<img->height){
										if(CV_IMAGE_ELEM(prop,uchar,height,width*3+0)<100 && CV_IMAGE_ELEM(prop,uchar,height,width*3+1)<100 && CV_IMAGE_ELEM(prop,uchar,height,width*3+2)<100)
										{
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+0)=CV_IMAGE_ELEM(prop,uchar,height,width*3+0);
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+1)=CV_IMAGE_ELEM(prop,uchar,height,width*3+1);
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+2)=CV_IMAGE_ELEM(prop,uchar,height,width*3+2);
										}
									}
								}
							}
						}
						else {
							for(int width = 0; width < prop->width;width++)
							{
								for(int height = 0; height < prop->height;height++)
								{
									if(re->x + width<img->width || re->y + height<img->height){
										if(CV_IMAGE_ELEM(prop,uchar,height,width*3+0)<100 && CV_IMAGE_ELEM(prop,uchar,height,width*3+1)<100 && CV_IMAGE_ELEM(prop,uchar,height,width*3+2)<100)
										{
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+0)=CV_IMAGE_ELEM(prop,uchar,height,width*3+0);
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+1)=CV_IMAGE_ELEM(prop,uchar,height,width*3+1);
											CV_IMAGE_ELEM(img,uchar,re->y + height,(re->x + (re->width/2 - prop->width/2) + width)*3+2)=CV_IMAGE_ELEM(prop,uchar,height,width*3+2);
										}
									}
								}
							}
						}
						cvReleaseImage(&prop);
					}
				}
			}
			
			if(hige >= 1)
			{
				CvSeq *nose;
				nose = cvHaarDetectObjects(gray, nose_cas, storage, 1.11, 4, 0);
				for (int k = 0; k < (nose ? nose->total : 0); k++) {
					CvRect *rn = (CvRect *) cvGetSeqElem (nose, k);
					if(y < rn->y && x < rn->x && rn->y < y + h && rn->x < x + w){
						IplImage *prop2 = NULL;
						if(hige==2) {
							prop2 = cvCreateImage(cvSize((int)(rn->width * 3), (int)(rn->height * hige_scale_y)), IPL_DEPTH_8U, 3);
						}
						else {
							prop2 = cvCreateImage(cvSize((int)(rn->width * hige_scale_x), (int)(rn->height * hige_scale_y)), IPL_DEPTH_8U, 3);
						}
						int scale=3;
						if(hige == 1){
							cvResize(hige_img, prop2, CV_INTER_NN);
						}
						else if(hige==2) {
							scale = 2;
							cvResize(hige_img2, prop2, CV_INTER_NN);
						}
						else if(hige==3) {
							cvResize(hige_img3, prop2, CV_INTER_NN);
						}

						for(int width = 0; width < prop2->width;width++)
						{
							for(int height = 0; height < prop2->height;height++)
							{
								if(rn->x + width<img->width || rn->y + height<img->height){
									if(CV_IMAGE_ELEM(prop2,uchar,height,width*3+0)<20 && CV_IMAGE_ELEM(prop2,uchar,height,width*3+1)<20 && CV_IMAGE_ELEM(prop2,uchar,height,width*3+2)<20)
									{
										CV_IMAGE_ELEM(img,uchar,rn->y + (rn->height/scale) + height,(rn->x + (rn->width/2 - prop2->width/2) + width)*3+0)=CV_IMAGE_ELEM(prop2,uchar,height,width*3+0);
										CV_IMAGE_ELEM(img,uchar,rn->y + (rn->height/scale) + height,(rn->x + (rn->width/2 - prop2->width/2) + width)*3+1)=CV_IMAGE_ELEM(prop2,uchar,height,width*3+1);
										CV_IMAGE_ELEM(img,uchar,rn->y + (rn->height/scale) + height,(rn->x + (rn->width/2 - prop2->width/2) + width)*3+2)=CV_IMAGE_ELEM(prop2,uchar,height,width*3+2);
									}
								}
							}
						}
						cvReleaseImage(&prop2);
					}
				}
			}
			//frame
			//cvRectangle(img, cvPoint(r->x,r->y), cvPoint(r->x+r->width,r->y+r->height), colors[i%8], 3, CV_AA);
		}

		if(swap == 1 && faces->total == 2) {
			CvRect *r = (CvRect *) cvGetSeqElem (faces, 0);
			CvRect *r_tmp = (CvRect *) cvGetSeqElem (faces, 1);
			
			IplImage *tmp = cvCreateImage(cvSize((int)(r->width), (int)(r->height)), IPL_DEPTH_8U, 3);
			IplImage *tmp2 = cvCreateImage(cvSize((int)(r_tmp->width), (int)(r_tmp->height)), IPL_DEPTH_8U, 3);
			IplImage *tmp3 = cvCreateImage(cvSize((int)(r_tmp->width), (int)(r_tmp->height)), IPL_DEPTH_8U, 3);
			IplImage *tmp4 = cvCreateImage(cvSize((int)(r->width), (int)(r->height)), IPL_DEPTH_8U, 3);

			for(int width = 0; width < tmp->width;width++)
			{
				for(int height = 0; height < tmp->height;height++)
				{
					CV_IMAGE_ELEM(tmp,uchar,height,(width)*3+0)=CV_IMAGE_ELEM(img,uchar,r->y + height,(r->x + width)*3+0);
					CV_IMAGE_ELEM(tmp,uchar,height,(width)*3+1)=CV_IMAGE_ELEM(img,uchar,r->y + height,(r->x + width)*3+1);
					CV_IMAGE_ELEM(tmp,uchar,height,(width)*3+2)=CV_IMAGE_ELEM(img,uchar,r->y + height,(r->x + width)*3+2);
				}
			}
			for(int width = 0; width < tmp3->width;width++)
			{
				for(int height = 0; height < tmp3->height;height++)
				{
					CV_IMAGE_ELEM(tmp3,uchar,height,(width)*3+0)=CV_IMAGE_ELEM(img,uchar,r_tmp->y + height,(r_tmp->x + width)*3+0);
					CV_IMAGE_ELEM(tmp3,uchar,height,(width)*3+1)=CV_IMAGE_ELEM(img,uchar,r_tmp->y + height,(r_tmp->x + width)*3+1);
					CV_IMAGE_ELEM(tmp3,uchar,height,(width)*3+2)=CV_IMAGE_ELEM(img,uchar,r_tmp->y + height,(r_tmp->x + width)*3+2);
				}
			}
			cvResize(tmp, tmp2, CV_INTER_NN);
			cvResize(tmp3, tmp4, CV_INTER_NN);
			for(int width = 0; width < tmp2->width;width++)
			{
				for(int height = 0; height < tmp2->height;height++)
				{
					CV_IMAGE_ELEM(img,uchar,r_tmp->y + (r_tmp->height/2 - tmp2->height/2) + height,(r_tmp->x + (r_tmp->width/2 - tmp2->width/2) + width)*3+0)=CV_IMAGE_ELEM(tmp2,uchar,height,width*3+0);
					CV_IMAGE_ELEM(img,uchar,r_tmp->y + (r_tmp->height/2 - tmp2->height/2) + height,(r_tmp->x + (r_tmp->width/2 - tmp2->width/2) + width)*3+1)=CV_IMAGE_ELEM(tmp2,uchar,height,width*3+1);
					CV_IMAGE_ELEM(img,uchar,r_tmp->y + (r_tmp->height/2 - tmp2->height/2) + height,(r_tmp->x + (r_tmp->width/2 - tmp2->width/2) + width)*3+2)=CV_IMAGE_ELEM(tmp2,uchar,height,width*3+2);
				
				}
			}
			for(int width = 0; width < tmp4->width;width++)
			{
				for(int height = 0; height < tmp4->height;height++)
				{
					CV_IMAGE_ELEM(img,uchar,r->y + (r->height/2 - tmp4->height/2) + height,(r->x + (r->width/2 - tmp4->width/2) + width)*3+0)=CV_IMAGE_ELEM(tmp4,uchar,height,width*3+0);
					CV_IMAGE_ELEM(img,uchar,r->y + (r->height/2 - tmp4->height/2) + height,(r->x + (r->width/2 - tmp4->width/2) + width)*3+1)=CV_IMAGE_ELEM(tmp4,uchar,height,width*3+1);
					CV_IMAGE_ELEM(img,uchar,r->y + (r->height/2 - tmp4->height/2) + height,(r->x + (r->width/2 - tmp4->width/2) + width)*3+2)=CV_IMAGE_ELEM(tmp4,uchar,height,width*3+2);
				}
			}
			cvReleaseImage(&tmp);
			cvReleaseImage(&tmp2);
			cvReleaseImage(&tmp3);
			cvReleaseImage(&tmp4);
		}
	
		cvNamedWindow("faces", CV_WINDOW_AUTOSIZE);
		cvShowImage("faces",img);
		key = cvWaitKey(1);
		if(key == 'h')
		{	
			if(hige==0){
				hige = 1;
				printf("ひげ: 1\n");
			}
			else if(hige==1) {
				hige = 2;
				printf("ひげ: 2\n");
			}
			else if(hige==2) {
				hige = 3;
				printf("ひげ: 3\n");
			}
			else {
				hige = 0;
				printf("ひげ: off\n");
			}
		}
		else if(key == 's')
		{
			if(sunglass==0) {
				sunglass = 1;
				printf("サングラス: 1\n");
			}
			else if(sunglass == 1){
				sunglass = 2;
				printf("サングラス: 2\n");
			}
			else if(sunglass == 2){
				sunglass = 3;
				printf("サングラス: 3\n");
			}
			else {
				sunglass = 0;
				printf("サングラス: off\n");
			}
		}
		else if(key == ' ')
		{
			printf("パシャ\n");
			cvSaveImage("saved_image.jpeg",img);
			for(int a = 0;a < img->height;a++)
			{
				for(int b = 0;b < img->width;b++)
				{
					CV_IMAGE_ELEM(img,uchar,a,b*3+0)=255;
					CV_IMAGE_ELEM(img,uchar,a,b*3+1)=255;
					CV_IMAGE_ELEM(img,uchar,a,b*3+2)=255;
				}
			}
			cvShowImage("faces", img);
			cvWaitKey(10);
			printf("\n");
			IplImage *saved_img = cvLoadImage("C:/Users/ejikken/Desktop/face2/BackgroundSubtraction/saved_image.jpeg", 1);
			cvShowImage("picture", saved_img);
			cvWaitKey(4000);
			cvDestroyWindow("picture");
			cvReleaseImage(&saved_img);
		}
		else if(key == 'w')
		{
			if(swap==0) {
				swap = 1;
				printf("顔入れ替え: on\n");
			}
			else{
				swap = 0;
				printf("顔入れ替え: off\n");
			}
		}
		else if(key == 'c')
		{
			printf("*----------------------------*\n");
			printf("|   コマンド一覧             |\n");
			printf("|                            |\n");
			printf("|   ひげ: h                  |\n");
			printf("|   サングラス: s            |\n");
			printf("|   顔入れ替え: w            |\n");
			printf("|   写真撮る: spacebar       |\n");
			printf("|   ヘルプ: c                |\n");
			printf("|   終了: q                  |\n");
			printf("|                            |\n");
			printf("*----------------------------*\n");
		}
		else if(key == 'q')
		{
			printf("終了\n");
			break;
		}
		
	}

    cvDestroyWindow("faces");
    cvReleaseImage(&img);
	cvReleaseImage(&sunglass_img);
	cvReleaseImage(&sunglass_img2);
	cvReleaseImage(&sunglass_img3);
	cvReleaseImage(&hige_img);
	cvReleaseImage(&hige_img2);
	cvReleaseImage(&hige_img3);

    cvReleaseImage(&gray);
	
    cvReleaseMemStorage(&storage);
		  
    return 0;
}