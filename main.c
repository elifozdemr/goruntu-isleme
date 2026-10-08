#include <stdio.h>
#include <stdlib.h>
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
}Pixel;
Pixel *pikseller;
char format[3];
int genislik,yukseklik,maxDeger;
void resimDosyasiniAc() {
    FILE *fptr;
    fptr=fopen("resim.ppm","rb");
    if(fptr==NULL) {
        perror("Dosya basarisiz\n");
        return;
    }
    printf("Dosya basariyla acildi\n");
    fclose(fptr);
}

void resimBilgileriniOku() {
    FILE *fptr;
    fptr=fopen("resim.ppm","rb");
    if(fptr==NULL) {
        perror("Dosya basarisiz\n");
        return;
    }
    fscanf(fptr,"%s",format);
    fscanf(fptr,"%d %d",&genislik,&yukseklik);
    fscanf(fptr,"%d",&maxDeger);
    fclose(fptr);
}
Pixel *pikselleriOku() {
    FILE *fptr;
    fptr=fopen("resim.ppm","rb");
    if(fptr==NULL) {
        perror("Dosya basarisiz\n");
        return NULL;
    }
    int pixelSayisi=genislik*yukseklik;
    pikseller = malloc(pixelSayisi*sizeof(Pixel));
    for(int i=0;i<pixelSayisi;i++) {
        fscanf(fptr,"%hhu %hhu %hhu",&pikseller[i].r,&pikseller[i].g,&pikseller[i].b);
    }
    fclose(fptr);
    return pikseller;
}
void grayscale() {
    unsigned char gri;
    int pixelSayisi=genislik*yukseklik;
    for(int i=0;i<pixelSayisi;i++) {
        gri=(pikseller[i].r+pikseller[i].g+pikseller[i].b)/3;
        pikseller[i].r=gri;
        pikseller[i].g=gri;
        pikseller[i].b=gri;
    }
}
void negatifEfekt() {
    int pixelSayisi=genislik*yukseklik;
    for(int i=0;i<pixelSayisi;i++) {
        pikseller[i].r=255-pikseller[i].r;
        pikseller[i].g=255-pikseller[i].g;
        pikseller[i].b=255-pikseller[i].b;
    }
}
void parlaklik() {
    int miktar;
    printf("Parlaklik miktarini giriniz: ");
    scanf("%d",&miktar);
    int pixelSayisi=genislik*yukseklik;
    for(int i=0;i<pixelSayisi;i++) {
        int r=pikseller[i].r+miktar;
        int g=pikseller[i].g+miktar;
        int b=pikseller[i].b+miktar;
        if (r>255) r=255;
        if (g>255) g=255;
        if (b>255) b=255;
        if (r<0) r=0;
        if (g<0) g=0;
        if (b<0) b=0;
        pikseller[i].r=r;
        pikseller[i].g=g;
        pikseller[i].b=b;
    }
}
void yatayFlip() {
    for (int x=0;x<genislik/2;x++) {
        for (int y=0;y<yukseklik;y++) {
            int solIndex= y * genislik + x;
            int sagIndex= y * genislik +(genislik-1-x);
            Pixel gecici=pikseller[solIndex];
            pikseller[solIndex]=pikseller[sagIndex];
            pikseller[sagIndex]=gecici;
        }
    }
}
void resmiKaydet() {
    FILE *fptr;
    fptr=fopen("yeniResim.ppm","w");
    if(fptr==NULL) {
        perror("Dosya basarisiz\n");
        return;
    }
    fprintf(fptr,"P3\n");
    fprintf(fptr,"%d %d\n",genislik,yukseklik);
    fprintf(fptr,"%d\n",maxDeger);
    int pixelSayisi=genislik*yukseklik;
    for(int i=0;i<pixelSayisi;i++) {
        fprintf(fptr,"%d %d %d\n",pikseller[i].r,pikseller[i].g,pikseller[i].b);
    }
    fclose(fptr);
}
void blur() {
    Pixel *kopya;
    int pixelSayisi=genislik*yukseklik;
    kopya = malloc(pixelSayisi * sizeof(Pixel));
    for(int i=0;i<pixelSayisi;i++) {
        kopya[i]=pikseller[i];
    }
    for (int y=1;y<yukseklik-1;y++) {
        for (int x=1;x<genislik-1;x++) {
            int toplamaR=0;
            int toplamaG=0;
            int toplamaB=0;
            for (int dy=-1;dy<=1;dy++) {
                for (int dx=-1;dx<=1;dx++) {
                    int index=(y+dy)*genislik+(x+dx);
                    toplamaR+=kopya[index].r;
                    toplamaG+=kopya[index].g;
                    toplamaB+=kopya[index].b;
                }
            }
            int index=y*genislik+x;
            pikseller[index].r=toplamaR/9;
            pikseller[index].g=toplamaG/9;
            pikseller[index].b=toplamaB/9;
        }
    }
    free(kopya);
}
void edgeDetection() {
    Pixel *kopya;
    int pixelSayisi=genislik*yukseklik;
    kopya = malloc(pixelSayisi * sizeof(Pixel));
    for(int i=0;i<pixelSayisi;i++) {
        kopya[i]=pikseller[i];
    }
    for (int y=1;y<yukseklik-1;y++) {
        for (int x=1;x<genislik-1;x++) {
            int gx=0;
            int gy=0;
            for (int dy=-1;dy<=1;dy++) {
                for (int dx=-1;dx<=1;dx++) {
                    int index=(y+dy)*genislik+(x+dx);
                    int gri=(kopya[index]).r+(kopya[index]).g+(kopya[index]).b/3;
                    int sobeIX[3][3]={
                            {-1,0,1},
                            {-2,0,2},
                            {-1,0,1},
                        };
                    int sobeIY[3][3]={
                        {-1,-2,-1},
                        {0,0,0},
                        {1,2,1},
                    };
                    gx +=gri*sobeIX[dy+1][dx+1];
                    gy +=gri*sobeIY[dy+1][dx+1];
                }
            }
            int kenar=abs(gx)+abs(gy);
            if (kenar>255)
                kenar=255;
            int index=y*genislik+x;
            pikseller[index].r=kenar;
            pikseller[index].g=kenar;
            pikseller[index].b=kenar;
        }
    }
    free(kopya);
}
int main() {
    int sec;
    do{
    printf("_______________________________________________________________\n");
    printf("                         Goruntu Isleme                        \n");
    printf("_______________________________________________________________\n");
    printf("1-Resim dosyasini acma\n");
    printf("2-Resim bilgilerini oku\n");
    printf("3-Pixelleri oku\n");
    printf("4-Resmi gri yapma\n");
    printf("5-Negatif yapma\n");
    printf("6-Parlaklik ayarlama\n");
    printf("7-Yatay-Dikey flip\n");
    printf("8-Resmi kaydet\n");
    printf("9-Resmi blurlama\n");
    printf("10-Edge detection yapma\n");
    printf("Bir secim yapiniz: ");
    scanf("%d",&sec);
    switch (sec) {
        case 1:resimDosyasiniAc();
            break;
        case 2:resimBilgileriniOku();
            break;
        case 3:pikseller=pikselleriOku();
            break;
        case 4:grayscale();
            break;
        case 5:negatifEfekt();
            break;
        case 6:parlaklik();
            break;
        case 7:yatayFlip();
            break;
        case 8:resmiKaydet();
            break;
        case 9:blur();
            break;
        case 10:edgeDetection();
            break;
    }
}while(sec!=10);
    free(pikseller);
    return 0;
}