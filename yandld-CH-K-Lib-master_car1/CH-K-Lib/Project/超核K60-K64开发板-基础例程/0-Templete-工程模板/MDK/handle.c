void handle()
{
    int i=0,j,m;
    int o=0;
    double k,l;
for(j=0,k=0,l=0;j<10;j++)
        {
        if(gpHREF[0][j]!=255)
        {
            for(m=7;m>=0;m--)
            {
            if(((gpHREF[i][j]>>m) & 0x01)==0)
            {
               k=8*j+8-m;
               //printf("k=%f\r\n",k);
                goto out1;
            }
            }
        }
        }

        out1:for(j=9;j>=0;j--)
        {
            if(gpHREF[0][j]!=255)
        {
            for(m=0;m<8;m++)
            {
            if(((gpHREF[i][j]>>m) & 0x01)==0)
            {
               l=8*j+8-m-1;
               //printf("l=%f\r\n",l);
                goto out2;
            }
            }
    for(i=59,o=0;i>=0;i--)
    {

        }
        }
        out2:
        if(k==0)
        {
            break;
        }
        img[o++]=(k+l)/2;
    }
    carP=findP(60-i);
    l=2*carP*carP+9260;
    printf("carP=%f\r\nl=%f\r\n",carP,l);
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,l);
}
