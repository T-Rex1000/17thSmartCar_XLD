#include "image_handle.h"
#include <string.h>
#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a
//#define abs(a) a>0?a:(-1*a)
#define ROADW 80
#define ROADL 12
#define ROADR 81

int leftline[60],cenline[60],rightline[60];
int imagezip[60][94],threshold;
char over_flag=0;
float k;
char *contemp;
volatile uint8 pre=15,back=55;
volatile uint8 judge_width=92;
volatile uint8 C0=0,C1=20;

int leftloose,rightloose;

uint8 change_O=15;

uint8 T_count=4;

uint8 left_sud_up_x,left_sud_down_x,left_slow_down_x;
uint8 left_sud_up_y,left_sud_down_y,left_slow_down_y;
uint8 right_sud_up_x,right_sud_down_x,right_slow_down_x;
uint8 right_sud_up_y,right_sud_down_y,right_slow_down_y;

int type_Y=1,type_O=0,type_F=2,O_change_y=45,O_change_x=75;

uint8 white_0_l,white_0_r,white_1_l,white_1_r,straight_l,straight_r;

uint8 count_O=0;

uint8 cenblack_r,cenblack_l,allblack_r,allblack_l,cenblack,allblack;

uint8 start=1;

uint8 arc_l,arc_r;

void resetmark_road()
{
    left_sud_up_x=left_sud_down_x=left_slow_down_x=0;
    left_sud_up_y=left_sud_down_y=left_slow_down_y=0;
    right_sud_up_x=right_sud_down_x=right_slow_down_x=0;
    right_sud_up_y=right_sud_down_y=right_slow_down_y=0;

    leftloose=0;
    rightloose=0;

    white_0_l=0;white_0_r=0;white_1_l=0;white_1_r=0;

    cenblack_r=cenblack_l=allblack_r=allblack_l=cenblack=allblack=0;

    straight_l=straight_r=10;

    arc_l=arc_r=0;

}
#define GrayScale 256
uint16 Image_Width  = 94;
uint16 Image_Height = 60;
int X; uint16 Y;
int* data = imagezip[0];
int HistGram[GrayScale];
int threshold_1()
{
    for (Y = 0; Y < GrayScale; Y++)
    {
        HistGram[Y] = 0;
    }
    for (Y = 0; Y <Image_Height; Y++)
    {
        //Y=Image_Height;
        for (X = 0; X < Image_Width; X++)
        {
        HistGram[(int)data[Y*Image_Width + X]]++;
        }
    }

    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0;
    int16 MinValue=0, MaxValue=0;
    int Threshold = 0;


    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ;

    if (MaxValue == MinValue)
    {
        return MaxValue;
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;
    }

    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
          PixelBack = PixelBack + HistGram[Y];
          PixelFore = Amount - PixelBack;
          OmegaBack = (double)PixelBack / Amount;
          OmegaFore = (double)PixelFore / Amount;
          PixelIntegralBack += HistGram[Y] * Y;
          PixelIntegralFore = PixelIntegral - PixelIntegralBack;
          MicroBack = (double)PixelIntegralBack / PixelBack;
          MicroFore = (double)PixelIntegralFore / PixelFore;
          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
          if (Sigma > SigmaB)
          {
              SigmaB = Sigma;
              Threshold = Y;
          }
    }
    return Threshold;
}
int fb_image[60][3];
void img_conversion(int (*src)[94],int (*dst)[94],int val)
{
    int line=0,col=0;
    for(line=0;line<60;line++)
    {
       for(col=0;col<94;col++)
       {
          if(src[line][col]>val)  dst[line][col]=250;
          else dst[line][col]=0;
       }
    }
}

void find_borders()
{
    int16 i,j,temp_centere;
    for(i=0;i<60;i++)
    {
       for(j=0;j<3;j++)
       {
          fb_image[i][j]=0;
       }
    }
    temp_centere=94/2;
    for(int i=0;i<=30;i++)
    {
         if(imagezip[59][temp_centere+i]==250 || imagezip[59][temp_centere+i]==255)
         {
            temp_centere=temp_centere+i;
            break;
         }
         if(imagezip[59][temp_centere-i]==250 || imagezip[59][temp_centere-i]==255)
         {
            temp_centere=temp_centere-i;
            break;
         }
    }
    for(i=temp_centere;i>=0;i--)
    {
        if(imagezip[59][i] !=0 )
        {
            imagezip[59][i] =255;
        }
        else break;
    }
    if(i==-1)
    {
       fb_image[59][0] = 0;
    }
    else
    {
       fb_image[59][0] = i+1;
    }
    for(i=temp_centere; i<94; i++)
    {
        if(imagezip[59][i] !=0 )
        {
            imagezip[59][i] =255;
        }
        else break;
    }
    if(i == 94)
    {
        fb_image[59][2] = 93;
    }
    else
    {
        fb_image[59][2] = i-1;
    }

    for(i=59;i>=0;i--)
    {
        for(j=0;j<94;j++)
        {
          if( imagezip[i+1][j] == 255 )
          {
            if( imagezip[i][j]== 250 || imagezip[i][j] == 255 )
            {
                imagezip[i][j] = 255;
            }
            if( j-1 >=0 &&( imagezip[i][j-1] == 250 || imagezip[i][j-1] == 255 ))
            {
                imagezip[i][j-1] =255;
            }
            if( j+1<=159 &&(imagezip[i][j+1] == 250 || imagezip[i][j+1] == 255))
            {
                imagezip[i][j+1]= 255;
            }
          }
        }
        for(j=0;j<=93;j++)
        {
            if(imagezip[i][j] == 255)
            {
               break;
            }
        }
        if(j==94)
        {
          break;
        }
        for(;j>=0;j--)
        {
            if(imagezip[i][j] == 250 || imagezip[i][j] == 255)
            {
                imagezip[i][j] =255;
            }
            else
            {
              fb_image[i][0] = j+1;
              break;
            }
        }
        if(j==-1)
        {
          fb_image[i][0] = 0;
        }
        for(j=93;j>=0; j--)
        {
          if(imagezip[i][j] == 255)
          {
            break;
          }
        }
        if(j==-1)
        {
          break;
        }
        for(;j<=93;j++)
        {
          if(imagezip[i][j] == 250 || imagezip[i][j] == 255)
          {
              imagezip[i][j] = 255;
          }
          else
          {
            fb_image[i][2] = j-1;
            break;
          }
        }
        if(j==93)
        {
          fb_image[i][2] = 159;
        }
    }


#if 1
    if(i >= 3 )
    {
        temp_centere=94/2;
        for(int j=0;j<=30;j++)
        {
          if(imagezip[i][temp_centere+j]==250 || imagezip[i][temp_centere+j]==255)
          {
            temp_centere=temp_centere+j;
            break;
          }
          if(imagezip[i][temp_centere-j]==250 || imagezip[i][temp_centere-j]==255)
          {
            temp_centere=temp_centere-j;
            break;
          }
        }
        if( temp_centere != 94/2)
        {
            for(i=temp_centere;i>=3;i--)
            {
              for(j=0;j<93;j++)
              {
                if( imagezip[i+1][j] == 255 )
                {
                  if( imagezip[i][j]== 250 || imagezip[i][j] == 255)
                  {
                      imagezip[i][j] = 255;
                  }
                  if( j-1 >=0 &&( imagezip[i][j-1] == 250 || imagezip[i][j-1] == 255 ))
                  {
                      imagezip[i][j-1] =255;
                  }
                  if( j+1<=159 &&(imagezip[i][j+1] == 250 || imagezip[i][j+1] == 255))
                  {
                      imagezip[i][j+1]= 255;
                  }
               }
             }



              for(j=0;j<=93;j++)
              {
                if(imagezip[i][j] == 255)
                {
                  break;
                }
              }
              if(j==94)
              {
                break;
              }
              for(;j>=0;j--)
              {
                if(imagezip[i][j] == 250 || imagezip[i][j] == 255)
                {
                    imagezip[i][j] =255;
                }
                else
                {
                  fb_image[i][0] = j+1;
                  break;
                }
              }
              if(j==-1)
              {
                fb_image[i][0] = 0;
              }



              for(j=93;j>=0; j--)
              {
                if(imagezip[i][j] == 255)
                {
                  break;
                }
              }
              if(j==-1)
              {
                break;
              }
              for(;j<=93;j++)
              {
                if(imagezip[i][j] == 250 || imagezip[i][j] == 255)
                {
                    imagezip[i][j] = 255;
                }
                else
                {
                  fb_image[i][2] = j-1;
                  break;
                }
              }
              if(j==94)
              {
                fb_image[i][2] = 93;
              }


            }
        }
    }
#endif
}

void twoBJ()
{
    int i,j,k,temp_centere;

    temp_centere=94/2;
    for(int i=0;i<=30;i++)
    {
      if(imagezip[59][temp_centere+i]==255)
      {
        temp_centere=temp_centere+i;
        break;
      }
      if( imagezip[59][temp_centere-i]==255)
      {
        temp_centere=temp_centere-i;
        break;
      }
    }


    for(i=60-1;i>=0;i--)
    {
      for(j=temp_centere;j<94;j++)
      {
          if(imagezip[i][j]==0)
          {
            if(j==temp_centere)
            {
              return ;
            }
            fb_image[i][2]=j-1;
            break;
          }
          else
          {
              imagezip[i][j]=255;
          }

      }
      if(j==94)
      {
        fb_image[i][2]=94-1;
      }


      for(k=temp_centere;k>=0;k--)
      {
          if(imagezip[i][k]==0)
          {
            if(k==temp_centere)
            {
              return ;
            }
            fb_image[i][0]=k+1;
            break;
          }
          else
          {
              imagezip[i][k]=255;
          }

      }

      if(k==-1)
      {
        fb_image[i][0]=0;
      }

      temp_centere=(j+k)/2;

      fb_image[i][1]=(fb_image[i][0]+fb_image[i][2])/2;

      if((k==(94-1)/2&&j==(94-1)/2)||(k==0&& j==0))
      {
        break;
      }
      if( k == 59 && j == 59)
      {
        break;
      }

  }

}


uint8 GetOSTUThreshold(uint8 (*img)[94], uint16 start_row,uint16 end_row, uint16 start_col, uint16 end_col)
{
  int     threshold1 =0;
  int32 sum_gray   =0;
  int32 sum_pix_num=0;
  int32 pl_pix_num =0;
  int32 p2_pix_mum =0;
  int32 p1_sum_gray=0;
  float m1=0;
  float m2=0;
  float V=0;
  float variance=0;
  int i,j,k=0;
  uint16 MinValue=0,MaxValue=255;
  uint16  HistoGram[256]={0};

  for(i = 0; i < 256; i++)
    HistoGram[i] = 0;

  for(i = start_row; i < end_row; i++)
  {
      for(j = start_col; j < end_col; j++)
      {
          HistoGram[(int)img[i][j]]++;
      }
  }

  for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; ++MinValue) ;
  for (MaxValue = 255; MaxValue > MinValue && HistoGram[MaxValue] == 0; --MaxValue) ;

  for(k=MinValue;k<=MaxValue;k++)
  {
      sum_gray+=k*HistoGram[k];
      sum_pix_num+=HistoGram[k];
  }

  for(k=MinValue;k<=MaxValue;k++)
  {
      pl_pix_num+=HistoGram[k];
      p2_pix_mum=sum_pix_num-pl_pix_num;
      p1_sum_gray+=k*HistoGram[k];
      m1=(float)p1_sum_gray/pl_pix_num;
      m2=(float)(sum_gray-p1_sum_gray)/p2_pix_mum;

      V=pl_pix_num*p2_pix_mum*(m1-m2)*(m1-m2);

      if(V>variance)
      {
          variance=V;
          threshold1=k;
      }
  }

#if 1
  uint8 t1      = threshold1/2;
  uint8 t2      = threshold1+10;
  float rate    = 1.25;
  uint8 side_x  = 20;

  for(i=255;i>0;i--)
      HistoGram[i] =(unsigned short int)i;

  for(i=t1;i<t2;i++)
  {
      HistoGram[i] = (uint16)(i*rate);
      if(HistoGram[i]>t2) HistoGram[i] = t2;
  }

  for(i=0;i<MT9V03X_H;i++)
  {
      for(j=0;j<side_x;j++)
      {
          mt9v03x_image[i][j] = (unsigned char)HistoGram[mt9v03x_image[i][j]];
      }
  }

  for(i=0;i<MT9V03X_H;i++)
  {
      for(j=MT9V03X_W - 1 - side_x;j<MT9V03X_W;j++)
      {
          mt9v03x_image[i][j] = (uint8)HistoGram[mt9v03x_image[i][j]];
      }
  }
#endif
  return threshold1;
}


void image_draw_point(int x,int y,int color)
{
    imagezip[x][y]=color;
    if(x-1>=0) imagezip[y][x-1]=color;
    if(x-2>=0) imagezip[y][x-2]=color;
    if(x+1>=0) imagezip[y][x+1]=color;
    if(x+2>=0) imagezip[y][x+2]=color;
    if(y-1>=0) imagezip[y-1][x]=color;
    if(y-2>=0) imagezip[y-2][x]=color;
    if(y+1>=0) imagezip[y+1][x]=color;
    if(y+2>=0) imagezip[y+2][x]=color;

}

void image_draw_line(int x0,int y0,int x1,int y1)
{
    int y,x;
    if(y1-y0)
    {
        k=((x1-x0)*1.00/(y1-y0)*1.00)*1.00;
        for(y=y0;y>=y1;y--)
        {
            x=(uint8)(((y-y0)*k)+x0);
            imagezip[y][x]=-3;
        }
    }
    else
    {
        y=y0;
        //for(x=min(x0,x1);x<=max(x0,x1);x++)
        for(x=x0;x<=x1;x++)
        {
            imagezip[y][x]=-3;
        }
    }
}

void image_filter(int x,int y)
{
    if(imagezip[y][x+1]>threshold&&imagezip[y][x-1]>threshold)
    {
        imagezip[y][x]=255;
    }
    else if(imagezip[y][x+1]<threshold&&imagezip[y][x-1]<threshold)
    {
        imagezip[y][x]=0;
    }
    else if(imagezip[y+1][x]>threshold&&imagezip[y-1][x]>threshold)
    {
        imagezip[y][x]=255;
    }
    else if(imagezip[y+1][x]<threshold&&imagezip[y-1][x]<threshold)
    {
        imagezip[y][x]=0;
    }
    if(imagezip[y][x+2]>threshold&&imagezip[y][x-2]>threshold&&imagezip[y+2][x]>threshold&&imagezip[y-2][x]>threshold)
    {
        imagezip[y][x]=255;
        imagezip[y][x+2]=255;imagezip[y+2][x]=255;
        imagezip[y][x-2]=255;imagezip[y-2][x]=255;

        imagezip[y-2][x-2]=255;imagezip[y+1][x+2]=255;
        imagezip[y-2][x+2]=255;imagezip[y+1][x-2]=255;
    }
}
void image_zip()
{
    int i,j;
    for(j=0;j<60;j++)
    {
        for(i=0;i<94;i++)
        {
            imagezip[j][i]=mt9v03x_image[2*j][2*i];
        }
    }
}

void image_Bin()
{
    int i,j;
    for(j=0;j<60;j++)
    {
        for(i=0;i<94;i++)
        {
            if(imagezip[j][i]>=threshold)
                imagezip[j][i]=255;
            else
                imagezip[j][i]=0;

            if(i>=4&&i<=89&&j>=4&&j<=55) image_filter(i,j);
        }
    }

}

void threshold_judge(int x,int y,int range)
{
    int tempimage8[9],i,j,temp_min=imagezip[x][y],temp_max=0;

    if(x-range>=0&&y-range>=0)  tempimage8[0]=imagezip[y-range][x-range];else tempimage8[0]=imagezip[y-1][x-1];
    if(x-range>=0)              tempimage8[1]=imagezip[y][x-range];else tempimage8[1]=imagezip[y][x-1];
    if(x-range>=0&&y+range<=59) tempimage8[2]=imagezip[y+range][x-range];else tempimage8[2]=imagezip[y+1][x-1];

    if(y-range>=0)              tempimage8[3]=imagezip[y-range][x];else tempimage8[3]=imagezip[y-1][x];
                                tempimage8[4]=imagezip[y][x];
    if(y+range<=59)             tempimage8[5]=imagezip[y+range][x];else tempimage8[5]=imagezip[y+1][x];

    if(x+range<=93&&y-range>=0) tempimage8[6]=imagezip[y-range][x+range];else tempimage8[6]=imagezip[y-1][x+1];
    if(x+range<=93)             tempimage8[7]=imagezip[y][x+range];else tempimage8[7]=imagezip[y][x+1];
    if(x+range<=93&&y+range<=59)tempimage8[8]=imagezip[y+range][x+range];else tempimage8[8]=imagezip[y+1][x+1];
    /*
    for(i=0;i<8;i++)
    {
        for(j=i;j<8;j++)
        {
            if(tempimage8[j]>tempimage8[j+1])
            {
                tempimage8[j]+=tempimage8[j+1];
                tempimage8[j+1]=tempimage8[j]-tempimage8[j+1];
                tempimage8[j]-=tempimage8[j+1];
            }
        }
    }
    */
    for(i=0;i<9;i++)
    {
        if(temp_min>tempimage8[i])
            temp_min=tempimage8[i];
        if(temp_max<tempimage8[i])
            temp_max=tempimage8[i];
    }
    threshold=(temp_max-temp_min)/2+temp_min;
    i=x-range;j=y-range;
    for(;j<=y+range;j++)
    {
        for(;i<=x+range;i++)
        {
            if(imagezip[j][i]>=threshold)
                imagezip[j][i]=255;
            else
                imagezip[j][i]=0;
        }
    }
    /*
    for(i=0;i<9;i++)
    {
        threshold+=tempimage8[i];
    }
    threshold/=8;
    */
}


void sew_line(int x0,int y0,int x1,int y1)
{
    if(y1-y0) k=((x1-x0)*1.00/(y1-y0)*1.00)*1.00;
    int y;
    for(y=y0;y>=y1;y--)
    {
        if(x0<x1)
        {
            leftline[back-y]=(uint8)(((y-y0)*k)+x0);
            imagezip[y][leftline[back-y]]=-3;
        }
        if(x0>x1)
        {
            rightline[back-y]=(uint8)(((y-y0)*k)+x0);
            imagezip[y][rightline[back-y]]=-3;
        }
    }
}

void cen_line(char dir)
{
    int i;
    if(dir==1)
    {
        for(i=2;i<=back-pre;i++)
        {
            cenline[i]=leftline[i]+(int)(rightline[1]-leftline[1])/2;
            if(cenline[i]>=0&&cenline[i]<=93) imagezip[back-i][cenline[i]]=0;
        }
    }
    else if(dir==-1)
    {
        for(i=2;i<=back-pre;i++)
        {
            cenline[i]=rightline[i]-(int)(rightline[1]-leftline[1])/2;
            if(cenline[i]>=0&&cenline[i]<=93) imagezip[back-i][cenline[i]]=0;
        }
    }
    else
    {
        for(i=0;i<=back-pre;i++)
        {
            cenline[i]=(int)(rightline[i]+leftline[i])/2;
        }
    }
}
/*
void blackline(int i)
{
    int cen=(leftline[i]+rightline[i])/2;
    for(int y=55-i;y>=5;y--,i++)
    {
        leftline[i]=cen;
        rightline[i]=cen;
        imagezip[y][cen]=-4;
    }
    if(cen>=57) black_r=1;
    else if(cen<=37) black_l=1;
    else black_r=black_l=1;

}
*/
void find_line()
{
    int cen=47;
    int xl,xr,y,i;
    int cenblack_r_flag=0,cenblack_l_flag=0;
    int cenblack_r_flag_done=1,cenblack_l_flag_done=1;
    int white_1_r_flag=1,white_1_l_flag=1;
    int white_road=0;
    for(y=back;y>=pre;y--)
    {
        i=back-y;
        xl=cen;
        xr=cen;
        cenblack_r_flag=0;
        cenblack_l_flag=0;
        cenblack_r_flag_done=1;
        cenblack_l_flag_done=1;
        white_road=0;
        while(xr<judge_width)
        {
            if(imagezip[y][xr]>threshold)
            {
                white_road=1;
                rightloose=1;
                if(cenblack_r_flag&&cenblack_r_flag_done)
                {
                    cenblack_r++;
                    cenblack_r_flag_done=0;
                }
            }
            if(imagezip[y][xr]<threshold&&!white_road)
            {
                cenblack_r_flag=1;
            }
            if(imagezip[y][xr]<threshold&&white_road)
            {
                imagezip[y][xr]=-2;
                imagezip[y][xr-1]=-2;
                rightloose=0;
                break;
            }
            xr++;
        }
        if(!white_road&&cenblack_r_flag)
        {
            allblack_r++;
            imagezip[y][xr]=-4;
            imagezip[y][xr-1]=-4;
            if(i>=1)
            {
                xr=rightline[i-1];
            }
            else
            {
                xr=93;
            }
        }
        if(rightloose&&xr>=judge_width)
        {
            xr=judge_width+1;
            imagezip[y][xr]=-2;
            imagezip[y][xr-1]=-2;
            if(!white_0_r)
            {
                white_0_r=y;
                white_1_r=y;
                white_1_r_flag=1;
            }
            else if(y>1&&white_1_r_flag)
            {
                if((imagezip[y-2][xr]>threshold||imagezip[y-2][xr-1]>threshold))
                    white_1_r=y;
                else
                    white_1_r_flag=0;
            }
        }
        rightline[i]=xr;
        //LEFT
        white_road=0;
        while(xl>93-judge_width)
        {
            if(imagezip[y][xl]>threshold)
            {
                white_road=1;
                leftloose=1;
                if(cenblack_l_flag&&cenblack_l_flag_done)
                {
                    cenblack_l++;
                    cenblack_l_flag_done=0;
                }
            }
            if(imagezip[y][xl]<threshold&&!white_road)
            {
                cenblack_l_flag=1;
            }
            if(imagezip[y][xl]<threshold&&white_road)
            {
                imagezip[y][xl]=-1;
                imagezip[y][xl+1]=-1;
                leftloose=0;
                break;
            }
            xl--;
        }
        if(!white_road&&cenblack_l_flag)
        {
            allblack_l++;
            imagezip[y][xl]=-4;
            imagezip[y][xl+1]=-4;
            if(i>=1)
            {
                xl=leftline[i-1];
            }
            else
            {
                xl=0;
            }
        }
        if(leftloose&&xl<=93-judge_width)
        {
            xl=93-judge_width-1;
            imagezip[y][xl]=-1;
            imagezip[y][xl+1]=-1;
            if(!white_0_l)
            {
                white_0_l=y;
                white_1_l=y;
                white_1_l_flag=1;
            }
            else if(y>1&&white_1_l_flag)
            {
                if((imagezip[y-2][xl]>threshold||imagezip[y-2][xl-1]>threshold))
                    white_1_l=y;
                else
                    white_1_l_flag=0;
            }
        }
        leftline[i]=xl;
        cen=(leftline[i]+rightline[i])/2;
        if(cenblack_r>=5)
        {
            allblack_l=allblack_r=0;
        }
        if(i>=1)
        {
            if(rightline[i-1]-rightline[i]>=0&&rightline[i-1]-rightline[i]<=5&&straight_r)
            {
                straight_r=1;
            }
            else
            {
                straight_r=0;
            }

            if(leftline[i]-leftline[i-1]>=0&&leftline[i]-leftline[i-1]<=5&&straight_l)
            {
                straight_l=1;
            }
            else
            {
                straight_l=0;
            }
        }
    }
}

void image_scope()
{

    for(int j=pre;j<=back;j++)
    {
        imagezip[j][93-judge_width]=-4;
        imagezip[j][judge_width]=-4;
    }

    for(int i=93-judge_width;i<judge_width;i++)
    {
        imagezip[pre][i]=-4;
        imagezip[back][i]=-4;
    }

}

void find_line1()
{
    int xl=93-judge_width,xr=judge_width;
    int y,i;
    int white_flag=0;
    int road_flag=0;
    for(y=back;y>=pre;y--)
    {
        i=back-y;
        if(imagezip[y][xl]<threshold)
        {
            while(xl<judge_width)
            {
                if(imagezip[y][xl]>threshold)
                {
                    road_flag=1;
                    break;
                }
                xl++;
            }
        }
        else
        {
            while(xl>93-judge_width)
            {
                if(imagezip[y][xl]<threshold)
                {
                    road_flag=1;
                    break;
                }
                xl--;
            }
        }
        if(imagezip[y][xr]<threshold)
        {
            while(xr>93-judge_width)
            {
                if(imagezip[y][xr]>threshold)
                {
                    road_flag=1;
                    break;
                }
                xr--;
            }
        }
        else
        {
            while(xr<judge_width)
            {
                if(imagezip[y][xr]<threshold)
                {
                    road_flag=1;
                    break;
                }
                xr++;
            }
        }
        if(!road_flag)
        {
            if(xl>=judge_width)
            {
                xl=ROADL;
            }
            if(xr<=93-judge_width)
            {
                xr=ROADR;
            }
        }
        if(i>=1)
        {
            if((leftline[i-1]-xl>1||xl-leftline[i-1]>2))
            {
                straight_l--;
                if(straight_l>5)
                {
                    straight_l=0;
                }
            }
            /*
            else if(xl<=93-judge_width)
            {
                straight_l--;
                if(straight_l>5)
                {
                    straight_l=0;
                }
            }
            */
            if((xr-rightline[i-1]>1||rightline[i-1]-xr>2))
            {
                straight_r--;
                if(straight_r>5)
                {
                    straight_r=0;
                }
            }
            /*
            else if(xr>=judge_width)
            {
                straight_r--;
                if(straight_r>5)
                {
                    straight_r=0;
                }
            }
            */
        }
        leftline[i]=xl;
        rightline[i]=xr;
        imagezip[y][xl]=-1;
        imagezip[y][xl+1]=-1;
        imagezip[y][xr]=-2;
        imagezip[y][xr-1]=-2;
    }
}
int line_judge(int line[back-pre])
{
    int x0=line[5],y0=back-5,x1=line[back-pre-5],y1=pre+5;
    float k=(x1-x0)*1.00/(y1-y0)*1.00;
    int i=0;
    int straight=0,arc_left=0,arc_right=0;
    for(i=1;i<back-pre;i++)
    {
        if(line[i]<=(x0+k*(back-i-y0)+2)&&line[i]>=(x0+k*(back-i-y0)-2))
        {
            straight++;
        }
        else if(line[i]<(x0+k*(back-i-y0)-2))
        {
            arc_left++;
        }
        else if(line[i]>(x0+k*(back-i-y0)+2))
        {
            arc_right++;
        }
    }
    if(straight>=3)
        return 1;
    else if(arc_left>=3)
        return 2;
    else if(arc_right>=3)
        return 3;
}
void line_handle()
{
    int i=0;
    int lean_l_0=0,lean_r_0=0,lean_l_1=0,lean_r_1=0;
    int slow_l=1,slow_r=1;
    for(i=3;i<back-pre;i++)
    {
        lean_l_0=leftline[i]-leftline[i-2];
        lean_l_1=leftline[i+2]-leftline[i];

        lean_r_0=rightline[i-2]-rightline[i];
        lean_r_1=rightline[i]-rightline[i+2];

        if(abs(lean_l_1)>abs(lean_l_0)&&lean_l_0-lean_l_1>5&&!left_sud_down_y)
        {
            left_sud_down_x=leftline[i];
            left_sud_down_y=back-i;
        }
        if(abs(lean_l_1)<abs(lean_l_0)&&lean_l_0-lean_l_1>5&&!left_sud_up_y)
        {
            left_sud_up_x=leftline[i];
            left_sud_up_y=back-i;
        }
        if(abs(lean_r_1)>abs(lean_r_0)&&lean_r_0-lean_r_1>5&&!right_sud_down_y)
        {
            right_sud_down_x=rightline[i];
            right_sud_down_y=back-i;
        }
        if(abs(lean_r_1)<abs(lean_r_0)&&lean_r_0-lean_r_1>5&&!right_sud_up_y)
        {
            right_sud_up_x=rightline[i];
            right_sud_up_y=back-i;
        }
        //arc
        //if((lean_r_1-lean_r_0)>0&&(lean_r_1-lean_r_0)<5)

        if(leftline[i]-leftline[i-2]>0&&leftline[i]-leftline[i-2]<5)
        {
            arc_l++;
        }
        //if((lean_l_1-lean_l_0)>0&&(lean_l_1-lean_l_0)<5)
        if(rightline[i-2]-rightline[i]>0&&rightline[i-2]-rightline[i]<5)
        {
            arc_r++;
        }
        //
        if(leftline[i]<=93-judge_width)
        {
            if(!white_0_l)
            {
                white_0_l=back-i;
                white_1_l=back-i;
            }
            if(white_0_l&&(leftline[i+1]<=93-judge_width))
            {
                white_1_l--;
            }
        }
        if(white_1_l&&i>=back-white_1_l&&(leftline[i]>left_slow_down_x)&&slow_l)
        {
            left_slow_down_x=leftline[i];
            left_slow_down_y=back-i;
        }
        else if(white_1_l&&i>=back-white_1_l&&(leftline[i]<left_slow_down_x)&&slow_l)
        {
            slow_l=0;
        }

        if(rightline[i]>=judge_width)
        {
            if(!white_0_r)
            {
                white_0_r=back-i;
                white_1_r=back-i;
            }
            if(white_0_r&&(rightline[i+1]>=judge_width))
            {
                white_1_r--;
            }
        }
        if(white_1_r&&i>=back-white_1_r&&(rightline[i]<right_slow_down_x)&&slow_r)
        {
            right_slow_down_x=rightline[i];
            right_slow_down_y=back-i;
        }
        else if(white_1_r&&i>=back-white_1_r&&(rightline[i]>right_slow_down_x))
        {
            slow_r=0;
        }
        if(rightline[i]<=93-judge_width||leftline[i]>=judge_width)//全黑
        {
            /*
            if(i>=5)
            {
                rightline[i]=rightline[i-1]-(rightline[i-5]-rightline[i-1])/4;
                leftline[i]=leftline[i-1]-(leftline[i-5]-leftline[i-1])/4;
                if(rightline[i]>=0&&rightline[i]<=93) imagezip[back-i][rightline[i]]=-2;
                if(leftline[i]>=0&&leftline[i]<=93) imagezip[back-i][leftline[i]]=-1;
            }
            else
            {
                rightline[i]=92;
                leftline[i]=1;
                imagezip[back-i][rightline[i]]=-2;
                imagezip[back-i][leftline[i]]=-1;
            }
            */
            if(i>=1)
            {
                rightline[i]=rightline[i-1];
                leftline[i]=leftline[i-1];
                if(rightline[i]>=0&&rightline[i]<=93) imagezip[back-i][rightline[i]]=-2;
                if(leftline[i]>=0&&leftline[i]<=93) imagezip[back-i][leftline[i]]=-1;
            }
            else
            {
                rightline[i]=judge_width;
                leftline[i]=93-judge_width;
                imagezip[back-i][rightline[i]]=-2;
                imagezip[back-i][leftline[i]]=-1;
            }
            allblack++;
        }
        else if(imagezip[back-i][leftline[i]+(rightline[i]-leftline[i])/2]<threshold)
        {
            cenblack++;
            imagezip[back-i][leftline[i]+(rightline[i]-leftline[i])/2]=-4;
        }
    }
    //image_draw_point(right_sud_up_x,right_sud_up_y,-3);
    //image_draw_point(right_sud_down_x,right_sud_down_y,-4);
    //image_draw_point(left_sud_up_x,left_sud_up_y,-3);
    //image_draw_point(left_sud_down_x,left_sud_down_y,-4);
}

int zebra_line()
{
    int i=0,j=0,ju=0,jd=0,jus=0,jds=0;
    int count=0;
    if((white_0_l-white_1_l)>(white_0_r-white_1_r))
    {
        j=(int)((white_0_l+white_1_l)*1.00/2-(white_0_l-white_1_l)*1.00/6);
        i=93-rightline[back-j];
    }
    else
    {
        j=(int)((white_0_r+white_1_r)*1.00/2-(white_0_r-white_1_r)*1.00/6);
        i=leftline[back-j];
    }
    ju=j-5;//abs(white_0_l-white_1_l)/4;
    jd=j+5;//abs(white_0_l-white_1_l)/4;
    jus=j-8;//abs(white_0_l-white_1_l)/4;
    jds=j+8;//abs(white_0_l-white_1_l)/4;
    for(;i<83;i++)
    {
        if(imagezip[j][i]>threshold&&imagezip[j][i+1]<threshold)
        {
            count++;
            i+=2;
        }
        /*
        if(imagezip[ju][i]>threshold&&imagezip[ju][i+1]<threshold)
        {
            count++;
            i+=2;
        }
        if(imagezip[jd][i]>threshold&&imagezip[jd][i+1]<threshold)
        {
            count++;
            i+=2;
        }
        if(imagezip[jus][i]>threshold&&imagezip[jus][i+1]<threshold)
        {
            count++;
            i+=2;
        }
        if(imagezip[jds][i]>threshold&&imagezip[jds][i+1]<threshold)
        {
            count++;
            i+=2;
        }
        */
        imagezip[j][i]=-1;
        if(count>=5)
            return count;
    }
    if(count>=6)
        return count;
    else
        return 0;
}

void cross()
{
    int y1=min(white_1_l,white_1_r)-5;
    if(y1<pre) y1=pre;
    if(y1>back) y1=back;
    sew_line(leftline[0],back,left_sud_up_x,y1);
    sew_line(rightline[0],back,right_sud_up_x,y1);
    //sew_line(leftline[0],back,leftline[back-y1],y1);
    //sew_line(rightline[0],back,rightline[back-y1],y1);
}
void road_Y()
{
    if(type_Y==3||type_Y==2)//right
    {
        /*
        for(X=rightline[back-pre-2]-2;X>5;X--)
        {
            if(imagezip[pre+2][X]<threshold)
                break;
        }
        */
        sew_line(leftline[0],back,87,5+pre);
    }
    else if(type_Y==5||type_Y==4)//left
    {
        /*
        for(X=leftline[back-pre-2]+2;X<90;X++)
        {
            if(imagezip[pre+2][X]<threshold)
                break;
        }
        */
        sew_line(rightline[0],back,7,5+pre);
    }

}

void road_O()
{
    int goal_line=30;
    int max_x=0,max_y=0;
    static int count_O=1;
    //if(type_O==2||type_O==-2)
        /*
    if(rightline[change_O]-left[change_O]>=ROADW)
    {
        if(count_O==0)
            count_O=1;
    }
    else if(rightline[change_O]-left[change_O]<ROADW)
    {
        if(count_O==1)
            count_O=2;
    }
    */
    if(type_O==2)
    {
        rightline[C1]=leftline[C1]+64;
        //sew_line(rightline[0],back,47,pre);
        //sew_line(leftline[0],back,47,pre);
    }
    else if(type_O==-2)
    {
        leftline[C1]=rightline[C1]-64;
        //sew_line(leftline[0],back,47,pre);
        //sew_line(rightline[0],back,47,pre);
    }
    /*
    if(type_O==2)
    {
        max_x=93;
        for(int i=back-white_1_r;i<back-pre;i++)
        {
            if(rightline[i]<max_x)
            {
                max_x=rightline[i];
                max_y=back-i;
            }
        }
        if(max_x>=judge_width-2)
        {
            max_x=leftline[back/2]+(47-leftline[back/2])*2;
            max_y=back/2;
        }
        if(rightline[0]<=judge_width-2) sew_line(rightline[0],back,max_x,max_y);
        else sew_line(ROADR,back,max_x,max_y);
    }
    else if(type_O==-2)
    {
        max_x=0;
        for(int i=back-white_1_l;i<back-pre;i++)
        {
            if(leftline[i]>max_x)
            {
                max_x=leftline[i];
                max_y=back-i;
            }
        }
        if(max_x<=93-judge_width+2)
        {
            max_x=rightline[back/2]-(rightline[back/2]-47)*2;
            max_y=back/2;
        }
        if(leftline[0]<=93-judge_width+2) sew_line(leftline[0],back,max_x,max_y);
        else sew_line(ROADL,back,max_x,max_y);
    }
*/
    if(type_O==3)
    {
        sew_line(leftline[0],back,90,1);
        //sew_line(leftline[0],back,right_sud_up_x,right_sud_up_y);
    }
    else if(type_O==-3)
    {
        sew_line(rightline[0],back,3,1);
        //sew_line(rightline[0],back,left_sud_up_x,left_sud_up_y);
    }
    else if(type_O==1||type_O==-1)
    {
        count_O=0;
        if(type_O==1)
        {
            max_x=93;
            for(int i=back-white_1_r;i<back-pre;i++)
            {
                if(rightline[i]<max_x)
                {
                    max_x=rightline[i];
                    max_y=back-i;
                }
            }
            if(max_x>=judge_width-2)
            {
                max_x=leftline[back/2]+(47-leftline[back/2])*2;
                max_y=back/2;
            }
            if(rightline[0]<=judge_width-2) sew_line(rightline[0],back,max_x,max_y);
            else sew_line(ROADR,back,max_x,max_y);
        }
        else
        {
            max_x=0;
            for(int i=back-white_1_l;i<back-pre;i++)
            {
                if(leftline[i]>max_x)
                {
                    max_x=leftline[i];
                    max_y=back-i;
                }
            }
            if(max_x<=93-judge_width+2)
            {
                max_x=rightline[back/2]-(rightline[back/2]-47)*2;
                max_y=back/2;
            }
            if(leftline[0]<=93-judge_width+2) sew_line(leftline[0],back,max_x,max_y);
            else sew_line(ROADL,back,max_x,max_y);
        }
    }
}

void road_T()
{
    /*
    if(type_O==1)
    {
        sew_line(rightline[0],back,93-judge_width,pre);
    }
    else if(type_O==-1)
    {
        sew_line(leftline[0],back,judge_width,pre);
    }
    else if(type_O==4)
    {
        sew_line(leftline[0],back,judge_width,pre);
    }
    else if(type_O==-4)
    {
        sew_line(rightline[0],back,93-judge_width,pre);
    }
    else
    {
        sew_line(leftline[0],back,judge_width,pre);
    }
    */
    if(T_count==3)
    {
        sew_line(leftline[0],back,judge_width,pre);
    }
    else if(T_count==2)
    {
        sew_line(leftline[0],back,judge_width,pre);
    }
    else if(T_count==1)
    {
        sew_line(rightline[0],back,93-judge_width,pre);
    }
}

void road_F()
{
    if(type_F==1)
    {

        leftline[C1]=rightline[C1]-64;
        /*
        if(straight_l)
            sew_line(rightline[1],54,rightline[55-white_1_r+5],white_1_r-5);
        else if(straight_r)
            sew_line(leftline[1],54,leftline[55-white_1_l+5],white_1_l-5);
            */
    }
    else if(type_F==0)
    {
        sew_line(leftline[1],54,judge_width,pre);
        /*
        if(white_0_)
            sew_line(rightline[1],54,93-judge_width,pre);
        else if(straight_l)
            sew_line(leftline[1],54,judge_width,pre);
            */
    }

}
int zebra_count=3;
void determine_element()
{
    //char count_O=0;
    /*
    if(right_sud_down_y&&right_sud_up_y&&left_sud_down_y&&left_sud_up_y)//(!allblack&&cenblack<=2&&(white_0_l-white_1_l>=10)&&(white_0_r-white_1_r>=10))
    {
        if(contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L')
            contemp[0]='+';
    }
    else
    */
    if(!allblack&&!cenblack&&!white_1_l&&!white_1_r&&!white_0_l&&!white_0_r)
    {
        contemp="|";
    }
    else if((allblack>2&&(white_0_l-white_1_l>=10)&&(white_0_r-white_1_r>=10)))
    {
        if(contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L')
        {
            T_count--;
            contemp="T     ";
        }
        /*
        if((type_O==2||type_O==-2)||(type_O==3||type_O==-3))
        {
            if(type_O==2&&(abs(right_sud_down_y-left_sud_down_y)<10))
            {
                type_O=1;
                type_Y=1;
            }
            if(type_O==-2&&(abs(right_sud_down_y-left_sud_down_y)<10))
            {
                type_O=-1;
                type_Y=1;
            }
            if(type_O==3) type_O=4;
            if(type_O==-3) type_O=-4;
        }
        else if(start)
        {
            contemp="TStart";
            start--;
        }
        */
    }

    else if(!allblack&&(cenblack>7)&&abs(white_0_l-white_0_r)<4&&white_0_l&&white_0_r&&(white_0_l-white_1_l>=10)&&(white_0_r-white_1_r>=10))
    {
        if(contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L')
        {
//            if(type_Y==4)
//            {
//                type_Y=1;
//                contemp="Y1    ";
//            }
//            else
            //{
                type_Y++;
                if(type_Y==2)contemp="Y1    ";
                if(type_Y==3)contemp="Y2    ";
                if(type_Y==4)contemp="Y3    ";
                if(type_Y==5)contemp="Y4    ";
            //}
        }
    }
    //else if((!cenblack&&!allblack&&(straight_l^straight_r)&&((white_0_l<back-10)&&(white_0_r<back-10))&&(white_1_l>5^white_1_r>5)&&(((white_0_l-white_1_l>=10)&&(white_0_l-white_1_l<=45))^((white_0_r-white_1_r>=10)&&(white_0_r-white_1_r<=45))))/*&&white_1_l>pre&&white_1_r>pre*/)
    else if((!cenblack&&!allblack&&(right_sud_down_y||left_sud_down_y||right_sud_up_y||left_sud_up_y)&&(straight_l^straight_r)&&(white_1_l>5^white_1_r>5)&&(((white_0_l-white_1_l>=10)&&(white_0_l-white_1_l<=50))^((white_0_r-white_1_r>=10)&&(white_0_r-white_1_r<=50)))))
    {//左右只有一边直线&&无全黑&&存在至少一个拐点&&白边长度只有一边在10到45/50之间
        if(!zebra_line())
        {
            if((contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L'||contemp[0]=='t')&&(type_O==2||type_O==-2||type_O==1||type_O==-1||type_O==0)&&abs((rightline[change_O]-leftline[change_O])-ROADW)>10&&(right_sud_down_y||left_sud_down_y))//&&rightline[change_O]-leftline[change_O]>ROADW
            {//
                if(straight_l)
                {
                    contemp="O1    ";
                    type_O=2;
                }
                else
                {
                    contemp="O1    ";
                    type_O=-2;
                }
            }
            /*
            else if((contemp[0]=='o')&&(type_O==2||type_O==-2)&&abs((rightline[change_O]-leftline[change_O])-ROADW)<10&&(right_sud_up_y||left_sud_up_y))//&&rightline[change_O]-leftline[change_O]<=ROADW
            {
                if(straight_l)
                {
                    contemp="O2R   ";
                    //type_O=3;
                }
                else
                {
                    contemp="O2L   ";
                    //type_O=-3;
                }
            }
            else if((contemp[0]=='t')&&(type_O==4||type_O==-4)&&abs((rightline[change_O]-leftline[change_O])-ROADW)>10&&(right_sud_up_y||left_sud_up_y))//&&rightline[change_O]-leftline[change_O]>ROADW
            {
                if(straight_l)
                {
                    contemp="O4    ";
                    type_O=1;
                }
                else if(straight_r)
                {
                    contemp="O4    ";
                    type_O=-1;
                }

            }
            */
        }
        else
        {
            if(contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L'||contemp[0]=='t'||contemp[0]=='o'||contemp[0]=='O')
            {
//                if(T_count==3&&type_Y==1)
//                    type_F=1;
//                if(T_count==1&&type_Y==5)
//                    type_F=0;
                type_F--;
                contemp="F    ";
                if(type_Y==5)
                    type_F=0;
            }
        }
    }
    else if(((white_0_l-white_1_l)>=20^(white_0_r-white_1_r)>=20)&&(allblack))//||((white_1_l<=pre&&white_1_l!=0)^(white_1_r<=pre&&white_1_r!=0)))
    {
        if(((white_0_l-white_1_l)>(white_0_r-white_1_r)||white_1_r==0)&&(arc_r-arc_l>20))
            contemp="L    ";
        else if(((white_0_l-white_1_l)<(white_0_r-white_1_r)||white_1_l==0)&&(arc_l-arc_r>20))//&&(contemp[0]!='Y'||(contemp[0]=='Y'&&(type_Y==2||type_Y==3)))
            contemp="R    ";
    }
    else if(allblack>25&&(white_0_l-white_1_l)<15&&(white_0_r-white_1_r)<15)
    {
        if(contemp[0]=='f')
            contemp="Game";
    }
    if(zebra_line()&&(contemp[0]=='O'||contemp[0]=='o'))
    {
        contemp="F";
        type_F--;
    }
    /*
    if(((type_Y==5&&T_count==1)||(T_count==3&&type_Y==1)))
    {
        zebra_count=1;
    }
    else
        zebra_count=0;
    if(zebra_count)
    {
        if(zebra_line())
        {
            if(contemp[0]=='|'||contemp[0]=='R'||contemp[0]=='L'||contemp[0]=='O'||contemp[0]=='o'||contemp[0]=='t')
                contemp="f2";
            type_F--;
        }

    }
    */
}
void element_handle()
{
    if(contemp[0]=='Y'&&white_0_l>=back-C1-5&&white_0_r>=back-C1-5)
    {
        contemp="y";
        Pidautocontrol->Kp=6;
    }
    else if(contemp[0]=='T')
    {
        if(T_count==3&&allblack>10)
        {
            contemp="t";
            Pidautocontrol->Kp=3;
        }
        else if(T_count==2&&(white_0_l>=back-C1-10||white_0_r>=back-C1-10))
        {
            contemp="t";
            Pidautocontrol->Kp=6;
        }
        else if(T_count==1)
        {
            contemp="t";
            Pidautocontrol->Kp=7;
        }
    }
    else if(contemp[0]=='F')//&&((white_0_l>=back-C1+15||white_0_r>=back-C1+15)||(white_0_r-white_1_r)>7||(white_0_l-white_1_l)>7))
    {
        contemp="f";
        Pidautocontrol->Kp=5;
        button=1;
    }
    else if(contemp[0]=='O'&&(white_0_l>=back-C1-15||white_0_r>=back-C1-15))
    {
        contemp="o";
        Pidautocontrol->Kp=6;
        //button=1;
    }
    else if(contemp[0]=='|')
    {
        Pidautocontrol->Kp=3;
        button=0;
    }
    else if(contemp[0]=='G')
    {
        button=1;
    }

    if(contemp[0]=='y')
    {
        road_Y();
    }
    else if(contemp[0]=='t')
    {
        road_T();
        if(!allblack&&T_count==2)
        {
            contemp[0]=='F';
        }
        if(!allblack&&T_count==1)
        {
            contemp[0]=='O';
        }
    }
    else if(contemp[0]=='o')
    {
        if(type_Y==5)
            road_F();
        else
            road_O();
    }
    else if(contemp[0]=='f')
    {
        road_F();
    }
}

void cenline_correct()
{
    int i=1;
    for(i=2;i<back-pre;i++)
    {
        if(abs(abs(cenline[i]-cenline[i-1])-abs(cenline[i-1]-cenline[i-2]))>2)
        {
            cenline[i]=cenline[i-1];
            imagezip[back-i][cenline[i]]=-4;
        }
    }
}

void duty_control()
{
    duty_goal=750+(cenline[C0]-cenline[C1])*duty_k;
    if(duty_goal>820) duty_goal=820;
    else if(duty_goal<680) duty_goal=680;
}

void OSTU_0()
{
    if(threshold_ad>0)
    {
        threshold=threshold_ad;
    }
    else
    {
        threshold=GetOSTUThreshold(imagezip,1,92,1,58)+20;
    }
    image_Bin();
}
void OSTU_1()
{
    if(threshold_ad==0)
    {
        threshold=threshold_1()-20;
        img_conversion(imagezip,imagezip,threshold);
        find_borders();
        twoBJ();
    }
    else
    {
        threshold=threshold_ad;
        image_Bin();
    }
}
void toral_control()
{
    resetmark_road();

    image_zip();
    //OSTU_0();
    OSTU_1();
    find_line1();
    line_handle();
    image_scope();
    /*
    if(line_judge(&leftline)==1)
        straight_l=1;
    else if(line_judge(&leftline)==3)
        arc_l=1;
    if(line_judge(&rightline)==1)
        straight_l=1;
    else if(line_judge(&rightline)==2)
        arc_r=1;
    */
    //if((((white_0_l>=back-20)||(white_0_r>=back-20))&&((white_0_l-white_1_l>=15)||(white_0_r-white_1_r>=15)))||(!white_0_l&&!white_0_r))
    determine_element();
    element_handle();

    if(contemp[0]=='R'&&allblack>=6)
    {
        cen_line(1);
        Pidautocontrol->Kp=7;
    }
    else if(contemp[0]=='L'&&allblack>=6)
    {
        cen_line(-1);
        Pidautocontrol->Kp=7;
    }
    else
    {
        cen_line(0);
        //cenline_correct();
    }
    //contemp[6]='\0';
     /*
     int cencount=0;
     for(int i=0;i<50;i++)
     {
         if(cenline[i]==46)
             cencount++;
     }
     */
     if(C0==0)
         cenline[C0]=47;


     //lcd_showint32(0,7,cenblack,3);
     //lcd_showint32(50,7,allblack,3);

     //lcd_showint32(0,7,white_0_l-white_1_l,3);
     //lcd_showint32(50,7,white_0_r-white_1_r,3);

     lcd_showint32(0,8,arc_l,3);
     lcd_showint32(30,8,arc_l,3);
     lcd_showint32(0,9,rightline[C1]-leftline[C1],3);
     //lcd_showint32(30,9,type_O,3);
     //lcd_showint32(90,9,zebra_line(),3);

     lcd_showstr(60,5,contemp);

     //image_draw_point(right_slow_down_x,right_slow_down_y,-4);

     image_draw_line(90,white_0_r,90,white_1_r);
     image_draw_line(3,white_0_l,3,white_1_l);

     image_draw_line(cenline[C0]+1,back-C0,cenline[C1]+1,back-C1);
     image_draw_line(cenline[C0]-1,back-C0,cenline[C1]-1,back-C1);
     image_draw_line(cenline[C0],back-C0,cenline[C1],back-C1);
     //image_draw_line(47-ROADW/2,back-C1,47+ROADW/2,back-C1);
     duty_control();
     lcd_displayimage032(imagezip[0],94,60);
     /*
     lcd_showstr(82,6,"O:");lcd_showint32(95,6,type_O,1);
     lcd_showstr(82,7,"Y:");lcd_showint32(95,7,type_Y,1);
     lcd_showstr(82,8,"S:");lcd_showint32(95,8,start,1);
     lcd_showstr(82,9,"P:");lcd_showint32(95,9,type_P,1);
     */
}

