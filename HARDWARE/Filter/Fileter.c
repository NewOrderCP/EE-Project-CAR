#include "stm32f10x.h"
#include "Fileter.h"

#define n_Sample	20


void Filter (u16 DatBuf[])
{
	unsigned char i;
	
  int sum0=0;
	int sum1=0;
	int sum2=0;
	int sum3=0;
	
	static u16 DatCh0[n_Sample];
	static u16 DatCh1[n_Sample];
	static u16 DatCh2[n_Sample];
	static u16 DatCh3[n_Sample];
	
  for(i=0;i<n_Sample;i++)
  {
    DatCh0[i]=DatCh0[i+1];
    sum0 += DatCh0[i];
		
		DatCh1[i]=DatCh1[i+1];
    sum1 += DatCh1[i];
		
		DatCh2[i]=DatCh2[i+1]; 
    sum2 += DatCh2[i];
		
		DatCh3[i]=DatCh3[i+1]; 
    sum3 += DatCh3[i];
  }
	
	DatCh0[n_Sample - 1] = DatBuf[0];
	sum0 += DatCh0[n_Sample - 1];
	
	DatCh1[n_Sample - 1] = DatBuf[1];
	sum1 += DatCh1[n_Sample - 1];
	
	DatCh2[n_Sample - 1] = DatBuf[2];
	sum2 += DatCh2[n_Sample - 1];
	
	DatCh3[n_Sample - 1] = DatBuf[3];
	sum3 += DatCh3[n_Sample - 1];
	
  DatBuf[0] = sum0/n_Sample;
	DatBuf[1] = sum1/n_Sample;
	DatBuf[2] = sum2/n_Sample;
	DatBuf[3] = sum3/n_Sample;
}

  
	
//	
//	
//void Filter (u16 DatBuf[])
//{
//	int i;
//	int DatSumCh0 = 0;
//	int DatSumCh1 = 0;
////	int DatSumCh2 = 0;
////	int DatSumCh3 = 0;
//	
//	static u16 DatCh0[n_Sample];
//	static u16 DatCh1[n_Sample];
////	static u16 DatCh2[n_Sample];
////	static u16 DatCh3[n_Sample];
//	
//	for (i = 0; i < n_Sample; i++)
//	{
//		DatCh0[i-1] = DatCh0[i];
//		DatSumCh0 += DatCh0[i] * i;
//		
//		DatCh1[i-1] = DatCh1[i];
//		DatSumCh1 += DatCh1[i] * i;
//		
////		DatCh2[i-1] = DatCh2[i];
////		DatSumCh2 += DatCh2[i] * i;
////		
////		DatCh3[i-1] = DatCh3[i];
////		DatSumCh3 += DatCh3[i] * i;
//	}
//	
//	DatCh0[n_Sample-1] = DatBuf[0];
//  DatSumCh0 += DatBuf[0] * n_Sample;
//	
//	DatCh1[n_Sample-1] = DatBuf[1];
//  DatSumCh1 += DatBuf[1] * n_Sample;
//	
////	DatCh2[n_Sample-1] = DatBuf[2];
////  DatSumCh2 += DatBuf[2] * n_Sample;
////	
////	DatCh3[n_Sample-1] = DatBuf[3];
////  DatSumCh3 += DatBuf[3] * n_Sample;
//	
//	DatBuf[0] = (DatSumCh0 / (n_Sample));
//	DatBuf[1] = (DatSumCh1 / (n_Sample));
////	DatBuf[2] = (DatSumCh2 / (11*n_Sample/2.0));
////	DatBuf[3] = (DatSumCh3 / (11*n_Sample/2.0));
//}
