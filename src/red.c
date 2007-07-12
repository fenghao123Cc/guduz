/* 
   Simple generic RED gateway simulator
     by Mehmet A. Suzen
     January 2005, Famagusta, Cyprus
     October 2005,  Minor Modifications
     June 2007,  Minor Modifications 
     Famagusta, Cyprus
     (c) 2005-2006-2007
     General Public License 
      GPL  
#    Copyright (C) 2006  Mehmet Suzen <mehmet.suzen@physics.org>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Prototypes                        */
/*************************************/
int main();
void get_randoms(int *numbers, int upper);
void get_bin_randoms(int *numbers2, int upper);
int read_network(int *hostrate);
/*************************************/

int main()
{
   FILE *fp1,*fp2,*fp3;
   int i,j,q,numhost,count,minth,maxth,time,simtime,m,qtime,*q_time;
   int *nran,*nran2,*hostrate,**hostdrops,zero,rcount;
   char line1[100];
   double wq,avg,maxp,pa,pb,*avg_time;  
    zero=0;
    numhost=0;
    simtime=100;
    wq=0.002;
    minth=5;
    maxth=15;
    maxp=0.02;
    printf(" \n"); 
    printf("          Simple genericRED gateway simulator \n"); 
    printf("                      M. Suzen (c) 2005 -2007 \n"); 
    printf(" \n"); 
    printf(" \n"); 
    printf(" \n"); 
    printf(" \n"); 
    printf(" \n"); 
    hostrate=(int *) malloc(1000*sizeof(int));
    printf("  allocated hostrate \n"); 
    numhost=read_network(hostrate);
    printf("  read network file !\n"); 
     avg_time=(double *) malloc(simtime*sizeof(double));
     q_time=(int *) malloc(simtime*sizeof(int));
     nran=(int *) malloc((simtime+1)*(numhost+1)*sizeof(int));
     nran2=(int *) malloc((numhost+1)*sizeof(int));
     hostdrops=(int **) malloc((simtime+3)*sizeof(int));
     for(i=0;i<=simtime;i++){ 
      hostdrops[i]=(int *) malloc((numhost+3)*sizeof(int));
     }
    printf("  allocated other stuff !! !\n"); 
   for(i=0;i<numhost;i++) {
     printf("%d th hostrate = %d  packets/unit time\n",i,hostrate[i]); 
    }

     /* initilize hostdrops and avg_time */
      for(time=0; time < simtime; time++) {
           avg_time[time]=0;
         for(i=0;i<numhost;i++) {
           hostdrops[time][i]=0; 
          }
       }
     /* core RED algorithm */
          avg=0;
          count=-1;
          qtime=0;
       fp3=fopen("traffic.ntw","w");
           /* Find current queue size or hosts sending packet */
       rcount=0;
           /* randomly pick which hosts are sending packets*/
            get_bin_randoms(nran,(numhost+1)*(simtime+1)); 
	     printf("core RED Algorithm \n");
      for(time=0;time<simtime; time++) {
	     printf("core RED Algorithm time =%d \r",time); 
            q=0;
              sprintf(line1,"%d ",time);
            for(i=0;i<numhost;i++) {
                   nran2[i]=nran[rcount];
                    /* printf("count nran2=%d\n",nran2[i]); */
               if(nran[rcount] == 1) {
                  q=q+hostrate[i];
                  sprintf(line1,"%s%d ",line1,hostrate[i]);
                 } else {
                  sprintf(line1,"%s%d ",line1,zero);
               }
                rcount++;
            }
              fprintf(fp3,"%s\n",line1);
               q_time[time]=q;
               /* printf("Current time = %d QUEUE q=%d\n",time,q); */
             /* loop over each host sending packet */
            for(i=0;i<numhost;i++) {
               /* printf("here nran2=%d\n",nran2[i]); */
               if(nran2[i] == 1) {
                    for(j=0;j<hostrate[i];j++) {
                          if(q != 0) {
                             avg=(1-wq)*avg+wq*q;
                             avg_time[time]=avg;
                        /*   printf("ZERO q= %d time=%d avg_time=%f\n",q,time,avg_time[time]);  */
                            } else {
                             m=time-qtime;
                             avg=pow((1-wq),m)*avg;
                             avg_time[time]=avg;
                         /*printf("time=%d avg_time=%f\n",time,avg_time[time]); */
                           }
                          /*printf("time=%d avg_time=%f\n",time,avg_time[time]); */
                          if(minth <= avg && avg < maxth ) {
                             count++; 
                      /* printf("maxp=%f avg=%f minth=%d maxth=%d\n",maxp,avg,minth,maxth); */
                             pb=maxp*(avg-minth)/(maxth-minth);
                             pa=pb/(1-count*pb);
                                   /* printf("count=%d pb=%f pa=%f \n",count,pb,pa); */
                              if(pa >= 0.015) { 
                               hostdrops[time][i]++; 
                               /* printf("PA dropping: i %d time %d value %d \n",i,time,hostdrops[time][i]); */
                               count=0;
                              }
                           }
                           if(maxth <= avg) {
                               hostdrops[time][i]++; 
                              /* printf("dropping: i %d time %d value %d \n",i,time,hostdrops[time][i]); */
                               count=0;
                             } else {
                               count=-1;
                           } 
                           if(q ==0) {
                              qtime=time;
                           }
                     }
                }  
             }
        }
    fclose(fp3);
    /* Report Host Drops  */
    fp1=fopen("hostdrops.ntw","w");
      for(time=0; time < simtime; time++) {
           sprintf(line1,"%5d",time);
         for(i=0;i<numhost;i++) {
                /* printf("hostdrop=%d \n",hostdrops[time][i]); */
           sprintf(line1,"%s %5d",line1,hostdrops[time][i]);
          }
                /* printf("C line1=%s \n",line1); */
           fprintf(fp1,"%s\n",line1);
       } 
    fclose(fp1);
    /* Report average and current queue size */
    fp2=fopen("queues.ntw","w");
      for(time=0; time < simtime; time++) {
       fprintf(fp2,"%d  %f  %d\n",time,avg_time[time],q_time[time]);
       /* printf("time=%d avg_time=%f\n",time,avg_time[time]);  */
      }
    fclose(fp2);
    /* Free Dynamic Arrays */
   free(nran);
   free(hostrate);
   free(hostdrops);
   free(avg_time);
   free(q_time);
	     printf("\n \n  _ _ _ RED Algorithm  finished \n"); 
    exit(0);
}
 /**********************************************************
    RED simulator  Functions                                     
  **********************************************************/
/* Get random squence over upper*/
void get_randoms(int *numbers, int upper) {
   int i;
   /*srand( (unsigned int) time( NULL ) );*/
      for(i =upper-1; i > 0; i -= 1 ){
        numbers[i] = rand() % i;
        /* printf("numbers=%d i=%d \n",numbers[i],i);*/
       }
}
/* Get random squence of 1s and 0s of uppers*/
void get_bin_randoms(int *numbers2, int upper) {
   int i,where;
   /*srand( (unsigned int)time( NULL ) );*/
      for(i =0; i < upper; i++ ){
        where=rand();  
            /* printf("where = %d\n",where) */; 
        numbers2[i] = where % 2;
       }
}
/* Read Network Data */
/* First line number of hosts */
/* other lines packets/sec perline */ 
int read_network(int *hostrate) {
    FILE *fp;
    int i,numhost=0;
    char line[100];

     i=0; 
    printf(" network file \n");
     /* while(feof(fp) == 0) {*/
    fp=fopen("network.ntw","r");
     while( fgets(line,10,fp) != NULL) { 
            /* printf(" \n *** hell line is =%s \n",line); */
        if(i == 0) {
           /* realloc(hostrate,2*sizeof(int));  */
           /* printf(" network hostrate realloc\n"); */
           sscanf(line,"%3d",&numhost); 
           /* printf("numhost=%d \n",numhost);  */
          } else {
           /* printf("other then that =%s \n",line); */
           /*realloc(hostrate,2*sizeof(int)+(i+1)*sizeof(int));  */
           sscanf(line,"%3d",&hostrate[i]);
           /* printf("hostrate i=%d rate=%d\n",i,hostrate[i]);  */
        }
           i++;
            /* printf("line increment i=%d numhost=%d \n",i,numhost);   */
      }
            /* printf("line out\n");  */
       if(numhost != (i-2)) {
          printf("missing host in network.ntw ; numhost=%d total lines=%d   \n",numhost,i); 
          exit(0);
        }
   fclose(fp);
        printf(" network file read completed \n");
  return numhost;
}
