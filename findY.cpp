#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

IplImage* image = 0;
IplImage* dst = 0;

// для хранения каналов RGB
IplImage* rgb = 0;
IplImage* r_plane = 0;
IplImage* g_plane = 0;
IplImage* b_plane = 0;
// для хранения каналов RGB после преобразования
IplImage* r_range = 0;
IplImage* g_range = 0;
IplImage* b_range = 0;
// для хранения суммарной картинки
IplImage* rgb_and = 0;

int Rmin = 0;
int Rmax = 256;

int Gmin = 0;
int Gmax = 256;

int Bmin = 0;
int Bmax = 256;

int RGBmax = 256;

// функции-обработчики ползунка
//
void myTrackbarRmin(int pos) {
        Rmin = pos;
        cvInRangeS(r_plane, cvScalar(Rmin), cvScalar(Rmax), r_range);
}

void myTrackbarRmax(int pos) {
        Rmax = pos;
        cvInRangeS(r_plane, cvScalar(Rmin), cvScalar(Rmax), r_range);
}

void myTrackbarGmin(int pos) {
        Gmin = pos;
        cvInRangeS(g_plane, cvScalar(Gmin), cvScalar(Gmax), g_range);
}

void myTrackbarGmax(int pos) {
        Gmax = pos;
        cvInRangeS(g_plane, cvScalar(Gmin), cvScalar(Gmax), g_range);
}

void myTrackbarBmin(int pos) {
        Bmin = pos;
        cvInRangeS(b_plane, cvScalar(Bmin), cvScalar(Bmax), b_range);
}

void myTrackbarBmax(int pos) {
        Bmax = pos;
        cvInRangeS(b_plane, cvScalar(Bmin), cvScalar(Bmax), b_range);
}

int main(int argc, char* argv[])
{

    int x = 0, y = 0;

    char* filename = argc == 2 ? argv[1] : "D://room.jpg";
    image = cvLoadImage(filename,1);

    printf("[i] image: %s\n", filename);
    assert( image != 0 );

    // создаём картинки
    rgb = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
    r_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    g_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    b_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    r_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    g_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    b_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    rgb_and = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
    //  копируем
    cvCopyImage(image, rgb);
    // разбиваем на отельные каналы
    cvSplit( rgb, b_plane, g_plane, r_plane, 0 );

    // определяем минимальное и максимальное значение
    // у каналов HSV
    double framemin=0;
    double framemax=0;

    cvMinMaxLoc(r_plane, &framemin, &framemax);
    Rmin = framemin;
    Rmax = framemax;
    cvMinMaxLoc(g_plane, &framemin, &framemax);
    Gmin = framemin;
    Gmax = framemax;
    cvMinMaxLoc(b_plane, &framemin, &framemax);
    Bmin = framemin;
    Bmax = framemax;

    // окна для отображения картинки
    cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("R",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("G",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("B",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("R range",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("G range",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("B range",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("rgb and",CV_WINDOW_AUTOSIZE);

    cvCreateTrackbar("Rmin", "R range", &Rmin, RGBmax, myTrackbarRmin);
    cvCreateTrackbar("Rmax", "R range", &Rmax, RGBmax, myTrackbarRmax);
    cvCreateTrackbar("Gmin", "G range", &Gmin, RGBmax, myTrackbarGmin);
    cvCreateTrackbar("Gmax", "G range", &Gmax, RGBmax, myTrackbarGmax);
    cvCreateTrackbar("Bmin", "B range", &Gmin, RGBmax, myTrackbarBmin);
    cvCreateTrackbar("Bmax", "B range", &Gmax, RGBmax, myTrackbarBmax);

    while(true)
	{
        // показываем картинку
        cvShowImage("original",image);

        // показываем слои
        cvShowImage( "R", r_plane );
        cvShowImage( "G", g_plane );
        cvShowImage( "B", b_plane );

        // показываем результат порогового преобразования
        cvShowImage( "R range", r_range );
        cvShowImage( "G range", g_range );
        cvShowImage( "B range", b_range );

        // складываем 
        cvAnd(r_range, g_range, rgb_and);
        cvAnd(rgb_and, b_range, rgb_and);

	for( int i = 0; i < rgb_and->height/2; i++ )	//  Делю высоту на 2, т.к. мой угол в верхней половине
	{
		uchar* ptr = (uchar*) (rgb_and->imageData + i * rgb_and->widthStep); // Получаем указатель на начало строки 'y' 
		for( int j = 0; j < rgb_and->width/2; j++ )	  // Делю ширину на 2, т.к. мой угол в левой половине
		{
			if (i <= 15)
				ptr[1*j + 1] = 0;		// Устанваливаем S в 0
			else 
				if (ptr[1*j + 1] == 255)
				{
					x = j;
					y = i;
				}
		}
	}

        // показываем результат
        cvShowImage( "rgb and", rgb_and );

        char c = cvWaitKey(33);
        if (c == 13)		// если нажат Enter - выводим результат
	{ 
        	break;
        }
    }
        
    cout << "koordinati x = "<< x << " y = "<< y << endl;
	
    system("pause");

    // освобождаем ресурсы
    cvReleaseImage(& image);
    cvReleaseImage(&rgb);
    cvReleaseImage(&r_plane);
    cvReleaseImage(&g_plane);
    cvReleaseImage(&b_plane);
    cvReleaseImage(&r_range);
    cvReleaseImage(&g_range);
    cvReleaseImage(&b_range);
    cvReleaseImage(&rgb_and);
    // удаляем окна
    cvDestroyAllWindows();
    return 0;
}
