## Homomorphic filter

Homomorphic filters are filters that operate in the frequency domain, based on the separation of reflectance and illumance and using a modified Gaussian filter for regulating illumination in scene.

The result below shows the application of a homomorphic filter, with source code displayed as well.

![Homomorphic](results/result_homomorphic.gif)

#### homomorphic.cpp
```c++
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define RADIUS 100

using namespace cv;
using namespace std;

float gl = 0;
int gl_slider = 0;
int gl_slider_max = 100;

float gh = 0;
int gh_slider = 50;
int gh_slider_max = 100;

float d0 = 0;
int d0_slider = 50;
int d0_slider_max = 100;

float c = 0;
int c_slider = 5;
int c_slider_max = 100;

char TrackbarName[50];

Mat imaginaryInput, complexImage, multsp;
Mat padded, filter, mag;
Mat_<float> realInput, zeros;
vector<Mat> planos;

// valor do ruido
float mean;

// guarda tecla capturada
char key;
Mat image, imagegray, tmp; 
// valores ideais dos tamanhos da imagem
// para calculo da DFT
int dft_M, dft_N;

char *filename;

// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image ){
    Mat tmp2, A, B, C, D;

    // se a imagem tiver tamanho impar, recorta a regiao para
    // evitar cópias de tamanho desigual
    image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
    int cx = image.cols/2;
    int cy = image.rows/2;

    // reorganiza os quadrantes da transformada
    // A B   ->  D C
    // C D       B A
    A = image(Rect(0, 0, cx, cy));
    B = image(Rect(cx, 0, cx, cy));
    C = image(Rect(0, cy, cx, cy));
    D = image(Rect(cx, cy, cx, cy));

    // A <-> D
    A.copyTo(tmp2);  D.copyTo(A);  tmp2.copyTo(D);

    // C <-> B
    C.copyTo(tmp2);  B.copyTo(C);  tmp2.copyTo(B);
}

void on_trackbar_homomorphic(int, void*) {
    gl = (float) gl_slider / 100.0;
    gh = (float) gh_slider / 100.0;
    d0 = 25.0 * d0_slider / 100.0;
    c  = (float) c_slider  / 100.0;

    cout << "gl = " << gl << endl;
    cout << "gh = " << gh << endl;
    cout << "d0 = " << d0 << endl;
    cout << "c = "  << c  << endl;

    image = imread(filename);
    cvtColor(image, imagegray, CV_BGR2GRAY);
    imshow("original", imagegray);

    // realiza o padding da imagem
    copyMakeBorder(imagegray, padded, 0,
                   dft_M - image.rows, 0,
                   dft_N - image.cols,
                   BORDER_CONSTANT, Scalar::all(0));

    // limpa o array de matrizes que vao compor a
    // imagem complexa
    planos.clear();
    // cria a compoente real
    realInput = Mat_<float>(padded);
    // insere as duas componentes no array de matrizes
    planos.push_back(realInput);
    planos.push_back(zeros);

    // combina o array de matrizes em uma unica
    // componente complexa
    merge(planos, complexImage);

    // calcula o dft
    dft(complexImage, complexImage);

    // realiza a troca de quadrantes
    deslocaDFT(complexImage);
    
    // filtro homomorfico
    for(int i=0; i < tmp.rows; i++){
        for(int j=0; j < tmp.cols; j++){
            float d2 = (i-dft_M/2)*(i-dft_M/2)+(j-dft_N/2)*(j-dft_N/2);
            //cout << "d2 = " << d2 << endl;
            tmp.at<float> (i,j) = (gh-gl)*(1.0 - (float)exp(-(c*d2/(d0*d0)))) + gl;
        }
    }

    // cria a matriz com as componentes do filtro e junta
    // ambas em uma matriz multicanal complexa
    Mat comps[]= {tmp, tmp};
    merge(comps, 2, filter);
    
    // aplica o filtro frequencial
    mulSpectrums(complexImage,filter,complexImage,0);

    // troca novamente os quadrantes
    deslocaDFT(complexImage);

    // calcula a DFT inversa
    idft(complexImage, complexImage);

    // limpa o array de planos
    planos.clear();

    // separa as partes real e imaginaria da
    // imagem filtrada
    split(complexImage, planos);

    // normaliza a parte real para exibicao
    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
    imshow("filtrada", planos[0]);

}

int main(int argc, char** argv){
    
    namedWindow("original", WINDOW_NORMAL);
    namedWindow("filtrada", WINDOW_NORMAL);

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <img_path>" << endl;
        return 1;
    }

    filename = argv[1];
    image = imread(filename);

    // identifica os tamanhos otimos para
    // calculo do FFT
    dft_M = getOptimalDFTSize(image.rows);
    dft_N = getOptimalDFTSize(image.cols);

    // realiza o padding da imagem
    copyMakeBorder(image, padded, 0,
                   dft_M - image.rows, 0,
                   dft_N - image.cols,
                   BORDER_CONSTANT, Scalar::all(0));

    // parte imaginaria da matriz complexa (preenchida com zeros)
    zeros = Mat_<float>::zeros(padded.size());

    // prepara a matriz complexa para ser preenchida
    complexImage = Mat(padded.size(), CV_32FC2, Scalar(0));

    // a função de transferência (filtro frequencial) deve ter o
    // mesmo tamanho e tipo da matriz complexa
    filter = complexImage.clone();

    // cria uma matriz temporária para criar as componentes real
    // e imaginaria do filtro ideal
    tmp = Mat(dft_M, dft_N, CV_32F);

    
    // Inicializar trackbars
    sprintf( TrackbarName, "gamma_l" );
    createTrackbar( TrackbarName, "filtrada",
                    &gl_slider,
                    gl_slider_max,
                    on_trackbar_homomorphic );
    
    sprintf( TrackbarName, "gamma_h" );
    createTrackbar( TrackbarName, "filtrada",
                    &gh_slider,
                    gh_slider_max,
                    on_trackbar_homomorphic );
    
    sprintf( TrackbarName, "d_zero" );
    createTrackbar( TrackbarName, "filtrada",
                    &d0_slider,
                    d0_slider_max,
                    on_trackbar_homomorphic );
    
    sprintf( TrackbarName, "c" );
    createTrackbar( TrackbarName, "filtrada",
                    &c_slider,
                    c_slider_max,
                    on_trackbar_homomorphic );

    on_trackbar_homomorphic(100, NULL);

    while (1) {
        key = (char) waitKey(10);
        if( key == 27 ) break; // esc pressed!
    }

    return 0;
}
```
