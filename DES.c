#include "DES.h"

// 초기 전치 함수
static void IP(UBYTE p[8], UBYTE ip[8]);
// 역 초기 전치 함수
static void IPinv(UBYTE ip[8], UBYTE c[8]);
// f 함수
static void F(UBYTE R[4], UBYTE rk[6], UBYTE res[4]);
// 선형 치환 함수
static void P(UBYTE input[4], UBYTE output[4]);
// 비트 확장 함
static void E(UBYTE R[4], UBYTE ER[6]);

// S-Box 1~8
static UBYTE S1[64] = { 14, 0, 4, 15, 13, 7, 1, 3, 2, 14, 15, 2, 11, 13, 8, 1, 3, 10, 10, 6, 6, 12, 12, 11, 5, 9, 9, 5, 0, 3, 7, 8, 4, 15, 1, 12, 14, 8, 8, 2, 13, 4, 6, 9, 2, 1, 11, 7, 15, 5, 12, 11, 9, 3, 7, 14, 13, 10, 10, 0, 5, 6, 0, 13 };
static UBYTE S2[64] = { 15, 3, 1, 13, 8, 4, 14, 7, 6, 15, 11, 2, 3, 8, 4, 14, 9, 12, 7, 0, 2, 1, 13, 10, 12, 6, 0, 9, 5, 11, 10, 5, 0, 13, 14, 8, 7, 10, 11, 1, 10, 3, 4, 15, 13, 4, 1, 2, 5, 11, 8, 6, 12, 7, 6, 12, 9, 0, 3, 5, 2, 14, 15, 9 };
static UBYTE S3[64] = { 10, 13, 0, 7, 9, 0, 14, 9, 6, 3, 3, 4, 15, 6, 5, 10, 1, 2, 13, 8, 12, 5, 7, 14, 11, 12, 4, 11, 2, 15, 8, 1, 13, 1, 6, 10, 4, 13, 9, 0, 8, 6, 15, 9, 3, 8, 0, 7, 11, 4, 1, 15, 2, 14, 12, 3, 5, 11, 10, 5, 14, 2, 7, 12 };
static UBYTE S4[64] = { 7, 13, 13, 8, 14, 11, 3, 5, 0, 6, 6, 15, 9, 0, 10, 3, 1, 4, 2, 7, 8, 2, 5, 12, 11, 1, 12, 10, 4, 14, 15, 9, 10, 3, 6, 15, 9, 0, 0, 6, 12, 10, 11, 1, 7, 13, 13, 8, 15, 9, 1, 4, 3, 5, 14, 11, 5, 12, 2, 7, 8, 2, 4, 14 };
static UBYTE S5[64] = { 2, 14, 12, 11, 4, 2, 1, 12, 7, 4, 10, 7, 11, 13, 6, 1, 8, 5, 5, 0, 3, 15, 15, 10, 13, 3, 0, 9, 14, 8, 9, 6, 4, 11, 2, 8, 1, 12, 11, 7, 10, 1, 13, 14, 7, 2, 8, 13, 15, 6, 9, 15, 12, 0, 5, 9, 6, 10, 3, 4, 0, 5, 14, 3 };
static UBYTE S6[64] = { 12, 10, 1, 15, 10, 4, 15, 2, 9, 7, 2, 12, 6, 9, 8, 5, 0, 6, 13, 1, 3, 13, 4, 14, 14, 0, 7, 11, 5, 3, 11, 8, 9, 4, 14, 3, 15, 2, 5, 12, 2, 9, 8, 5, 12, 15, 3, 10, 7, 11, 0, 14, 4, 1, 10, 7, 1, 6, 13, 0, 11, 8, 6, 13 };
static UBYTE S7[64] = { 4, 13, 11, 0, 2, 11, 14, 7, 15, 4, 0, 9, 8, 1, 13, 10, 3, 14, 12, 3, 9, 5, 7, 12, 5, 2, 10, 15, 6, 8, 1, 6, 1, 6, 4, 11, 11, 13, 13, 8, 12, 1, 3, 4, 7, 10, 14, 7, 10, 9, 15, 5, 6, 0, 8, 15, 0, 14, 5, 2, 9, 3, 2, 12 };
static UBYTE S8[64] = { 13, 1, 2, 15, 8, 13, 4, 8, 6, 10, 15, 3, 11, 7, 1, 4, 10, 12, 9, 5, 3, 6, 14, 11, 5, 0, 0, 14, 12, 9, 7, 2, 7, 2, 11, 1, 4, 14, 1, 7, 9, 4, 12, 10, 14, 8, 2, 13, 0, 15, 6, 12, 10, 9, 13, 0, 15, 3, 3, 5, 5, 6, 8, 11 };

// DES 암호화 함수 구현
void DES_enc(UBYTE p[8], UBYTE k[7], UBYTE c[8])
{
	int i;
	UBYTE sk[16][6] = {0x0, }, ip[8], L[4], R[4], res[4], temp[4];
	int round;

	key_gen(k, sk); 

	IP(p, ip);

	for(i=0; i<4; ++i)
	{
	L[i] = ip[i];
	R[i] = ip[4+i];
	}

	for (round=0; round<15; round++)
	{
	F(R, sk[round], res);

	for(i=0; i<4; ++i)
	{
		temp[i] = L[i];
		L[i] = R[i];
		R[i] = temp[i]^res[i];
	}
	} // 15라운드 끝

	// 16라운드
	F(R, sk[round], res);

	for(i=0; i<4; ++i)
	{
	L[i] = L[i]^res[i];
	}

	for(i=0; i<4; ++i)
	{
	ip[i] = L[i];
	ip[i+4] = R[i];
	}

	IPinv(ip, c);
}

// DES 복호화 함수 구현
void DES_dec(UBYTE c[8], UBYTE k[7], UBYTE p[8])
{
	int i;
	UBYTE sk[16][6] = {0x0, }, ip[8], L[4], R[4], res[4], temp[4];
	int round;

	key_gen(k, sk); 

	IP(c, ip);

	for(i=0; i<4; ++i)
	{
	L[i] = ip[i];
	R[i] = ip[4+i];
	}

	for (round=0; round<15; round++)
	{
	F(R, sk[15-round], res);

	for(i=0; i<4; ++i)
	{
		temp[i] = L[i];
		L[i] = R[i];
		R[i] = temp[i]^res[i];
	}
	} // 15라운드 끝

	// 16라운드
	F(R, sk[0], res);

	for(i=0; i<4; ++i)
	{
	L[i] = L[i]^res[i];
	}

	for(i=0; i<4; ++i)
	{
	ip[i] = L[i];
	ip[i+4] = R[i];
	}

	IPinv(ip, p);



}


// 초기 전치 함수 구현
static void IP(UBYTE p[8], UBYTE ip[8])
{
	int i;

	for(i=0; i<8; ++i)
	ip[i] = 0x0;

	ip[0] |= ((p[7] >> 6) & 1) << 7; 
	ip[0] |= ((p[6] >> 6) & 1) << 6;
	ip[0] |= ((p[5] >> 6) & 1) << 5;
	ip[0] |= ((p[4] >> 6) & 1) << 4;
	ip[0] |= ((p[3] >> 6) & 1) << 3;
	ip[0] |= ((p[2] >> 6) & 1) << 2;
	ip[0] |= ((p[1] >> 6) & 1) << 1;
	ip[0] |= ((p[0] >> 6) & 1);
	ip[1] |= ((p[7] >> 4) & 1) << 7;
	ip[1] |= ((p[6] >> 4) & 1) << 6;
	ip[1] |= ((p[5] >> 4) & 1) << 5;
	ip[1] |= ((p[4] >> 4) & 1) << 4;
	ip[1] |= ((p[3] >> 4) & 1) << 3;
	ip[1] |= ((p[2] >> 4) & 1) << 2;
	ip[1] |= ((p[1] >> 4) & 1) << 1;
	ip[1] |= ((p[0] >> 4) & 1);
	ip[2] |= ((p[7] >> 2) & 1) << 7;
	ip[2] |= ((p[6] >> 2) & 1) << 6;
	ip[2] |= ((p[5] >> 2) & 1) << 5;
	ip[2] |= ((p[4] >> 2) & 1) << 4;
	ip[2] |= ((p[3] >> 2) & 1) << 3;
	ip[2] |= ((p[2] >> 2) & 1) << 2;
	ip[2] |= ((p[1] >> 2) & 1) << 1;
	ip[2] |= ((p[0] >> 2) & 1);
	ip[3] |= (p[7] & 1) << 7;
	ip[3] |= (p[6] & 1) << 6;
	ip[3] |= (p[5] & 1) << 5;
	ip[3] |= (p[4] & 1) << 4;
	ip[3] |= (p[3] & 1) << 3;
	ip[3] |= (p[2] & 1) << 2;
	ip[3] |= (p[1] & 1) << 1;
	ip[3] |= (p[0] & 1);
	ip[4] |= ((p[7] >> 7) & 1) << 7;
	ip[4] |= ((p[6] >> 7) & 1) << 6;
	ip[4] |= ((p[5] >> 7) & 1) << 5;
	ip[4] |= ((p[4] >> 7) & 1) << 4;
	ip[4] |= ((p[3] >> 7) & 1) << 3;
	ip[4] |= ((p[2] >> 7) & 1) << 2;
	ip[4] |= ((p[1] >> 7) & 1) << 1;
	ip[4] |= ((p[0] >> 7) & 1);
	ip[5] |= ((p[7] >> 5) & 1) << 7;
	ip[5] |= ((p[6] >> 5) & 1) << 6;
	ip[5] |= ((p[5] >> 5) & 1) << 5;
	ip[5] |= ((p[4] >> 5) & 1) << 4;
	ip[5] |= ((p[3] >> 5) & 1) << 3;
	ip[5] |= ((p[2] >> 5) & 1) << 2;
	ip[5] |= ((p[1] >> 5) & 1) << 1;
	ip[5] |= ((p[0] >> 5) & 1);
	ip[6] |= ((p[7] >> 3) & 1) << 7;
	ip[6] |= ((p[6] >> 3) & 1) << 6;
	ip[6] |= ((p[5] >> 3) & 1) << 5;
	ip[6] |= ((p[4] >> 3) & 1) << 4;
	ip[6] |= ((p[3] >> 3) & 1) << 3;
	ip[6] |= ((p[2] >> 3) & 1) << 2;
	ip[6] |= ((p[1] >> 3) & 1) << 1;
	ip[6] |= ((p[0] >> 3) & 1);
	ip[7] |= ((p[7] >> 1) & 1) << 7;
	ip[7] |= ((p[6] >> 1) & 1) << 6;
	ip[7] |= ((p[5] >> 1) & 1) << 5;
	ip[7] |= ((p[4] >> 1) & 1) << 4;
	ip[7] |= ((p[3] >> 1) & 1) << 3;
	ip[7] |= ((p[2] >> 1) & 1) << 2;
	ip[7] |= ((p[1] >> 1) & 1) << 1;
	ip[7] |= ((p[0] >> 1) & 1);
}

// 역 초기 전치 함수 구현
static void IPinv(UBYTE ip[8], UBYTE c[8])
{
	int i;
	
	for(i=0; i<8; ++i)
	c[i] = 0x0;
	
	c[0] |= ((ip[4] ) & 1) << 7;
	c[0] |= ((ip[0] ) & 1) << 6;
	c[0] |= ((ip[5] ) & 1) << 5;
	c[0] |= ((ip[1] ) & 1) << 4;
	c[0] |= ((ip[6] ) & 1) << 3;
	c[0] |= ((ip[2] ) & 1) << 2;
	c[0] |= ((ip[7] ) & 1) << 1;
	c[0] |= ((ip[3] ) & 1);
	c[1] |= ((ip[4] >> 1) & 1) << 7;
	c[1] |= ((ip[0] >> 1) & 1) << 6;
	c[1] |= ((ip[5] >> 1) & 1) << 5;
	c[1] |= ((ip[1] >> 1) & 1) << 4;
	c[1] |= ((ip[6] >> 1) & 1) << 3;
	c[1] |= ((ip[2] >> 1) & 1) << 2;
	c[1] |= ((ip[7] >> 1) & 1) << 1;
	c[1] |= ((ip[3] >> 1) & 1);
	c[2] |= ((ip[4] >> 2) & 1) << 7;
	c[2] |= ((ip[0] >> 2) & 1) << 6;
	c[2] |= ((ip[5] >> 2) & 1) << 5;
	c[2] |= ((ip[1] >> 2) & 1) << 4;
	c[2] |= ((ip[6] >> 2) & 1) << 3;
	c[2] |= ((ip[2] >> 2) & 1) << 2;
	c[2] |= ((ip[7] >> 2) & 1) << 1;
	c[2] |= ((ip[3] >> 2) & 1);
	c[3] |= ((ip[4] >> 3) & 1) << 7;
	c[3] |= ((ip[0] >> 3) & 1) << 6;
	c[3] |= ((ip[5] >> 3) & 1) << 5;
	c[3] |= ((ip[1] >> 3) & 1) << 4;
	c[3] |= ((ip[6] >> 3) & 1) << 3;
	c[3] |= ((ip[2] >> 3) & 1) << 2;
	c[3] |= ((ip[7] >> 3) & 1) << 1;
	c[3] |= ((ip[3] >> 3) & 1);
	c[4] |= ((ip[4] >> 4) & 1) << 7;
	c[4] |= ((ip[0] >> 4) & 1) << 6;
	c[4] |= ((ip[5] >> 4) & 1) << 5;
	c[4] |= ((ip[1] >> 4) & 1) << 4;
	c[4] |= ((ip[6] >> 4) & 1) << 3;
	c[4] |= ((ip[2] >> 4) & 1) << 2;
	c[4] |= ((ip[7] >> 4) & 1) << 1;
	c[4] |= ((ip[3] >> 4) & 1);
	c[5] |= ((ip[4] >> 5) & 1) << 7;
	c[5] |= ((ip[0] >> 5) & 1) << 6;
	c[5] |= ((ip[5] >> 5) & 1) << 5;
	c[5] |= ((ip[1] >> 5) & 1) << 4;
	c[5] |= ((ip[6] >> 5) & 1) << 3;
	c[5] |= ((ip[2] >> 5) & 1) << 2;
	c[5] |= ((ip[7] >> 5) & 1) << 1;
	c[5] |= ((ip[3] >> 5) & 1);
	c[6] |= ((ip[4] >> 6) & 1) << 7;
	c[6] |= ((ip[0] >> 6) & 1) << 6;
	c[6] |= ((ip[5] >> 6) & 1) << 5;
	c[6] |= ((ip[1] >> 6) & 1) << 4;
	c[6] |= ((ip[6] >> 6) & 1) << 3;
	c[6] |= ((ip[2] >> 6) & 1) << 2;
	c[6] |= ((ip[7] >> 6) & 1) << 1;
	c[6] |= ((ip[3] >> 6) & 1);
	c[7] |= ((ip[4] >> 7) & 1) << 7;
	c[7] |= ((ip[0] >> 7) & 1) << 6;
	c[7] |= ((ip[5] >> 7) & 1) << 5;
	c[7] |= ((ip[1] >> 7) & 1) << 4;
	c[7] |= ((ip[6] >> 7) & 1) << 3;
	c[7] |= ((ip[2] >> 7) & 1) << 2;
	c[7] |= ((ip[7] >> 7) & 1) << 1;
	c[7] |= ((ip[3] >> 7) & 1);
}

// f 함수 구현
static void F(UBYTE R[4], UBYTE rk[6], UBYTE res[4])
{
	int i;
	UBYTE ER[6], S_in[8], S_out[8], P_in[4];

	// expansion
	E(R, ER);

	// xor
	for(i=0; i<6; ++i)
	ER[i] ^= rk[i];

	// S-box computation
	S_in[0] = (ER[0]&0xfc)>>2;
	S_in[1] = ((ER[0]&0x03)<<4) | ((ER[1] & 0xf0)>>4);
	S_in[2] = ((ER[1]&0x0f)<<2) | ((ER[2] & 0xc0)>>6);
	S_in[3] = ER[2] & 0x3f;
	S_in[4] = (ER[3]&0xfc)>>2;
	S_in[5] = ((ER[3]&0x03)<<4) | ((ER[4] & 0xf0)>>4);
	S_in[6] = ((ER[4]&0x0f)<<2) | ((ER[5] & 0xc0)>>6);
	S_in[7] = ER[5] & 0x3f;

	S_out[0] = S1[S_in[0]];
	S_out[1] = S2[S_in[1]];
	S_out[2] = S3[S_in[2]];
	S_out[3] = S4[S_in[3]];
	S_out[4] = S5[S_in[4]];
	S_out[5] = S6[S_in[5]];
	S_out[6] = S7[S_in[6]];
	S_out[7] = S8[S_in[7]];

	// P computation
	for(i=0; i<4; ++i)
	P_in[i]	= (S_out[2*i]<<4) | S_out[2*i+1];

	P(P_in, res);
}

// 선형 치환 함수 구현
static void P(UBYTE input[4], UBYTE output[4]) 
{
	int i;
	for(i=0; i<4; i++)
	output[i]=0;

	output[0] |=(input[1] & 1) << 7;
	output[0] |=((input[0]>>1) & 1) << 6;
	output[0] |=((input[2]>>4) & 1) << 5;
	output[0] |=((input[2]>>3) & 1) << 4;
	output[0] |=((input[3]>>3) & 1) << 3;
	output[0] |=((input[1]>>4) & 1) << 2;
	output[0] |=((input[3]>>4) & 1) << 1;
	output[0] |=((input[2]>>7) & 1);

	output[1] |=((input[0]>>7) & 1) << 7;
	output[1] |=((input[1]>>1) & 1) << 6;
	output[1] |=((input[2]>>1) & 1) << 5;
	output[1] |=((input[3]>>6) & 1) << 4;
	output[1] |=((input[0]>>3) & 1) << 3;
	output[1] |=((input[2]>>6) & 1) << 2;
	output[1] |=((input[3]>>1) & 1) << 1;
	output[1] |=((input[1]>>6) & 1);

	output[2] |=((input[0]>>6) & 1) << 7;
	output[2] |=(input[0] & 1) << 6;
	output[2] |=(input[2] & 1) << 5;
	output[2] |=((input[1]>>2) & 1) << 4;
	output[2] |=((input[3])	& 1) << 3;
	output[2] |=((input[3]>>5) & 1) << 2;
	output[2] |=((input[0]>>5) & 1) << 1;
	output[2] |=((input[1]>>7) & 1);

	output[3] |=((input[2]>>5) & 1) << 7;
	output[3] |=((input[1]>>3) & 1) << 6;
	output[3] |=((input[3]>>2) & 1) << 5;
	output[3] |=((input[0]>>2) & 1) << 4;
	output[3] |=((input[2]>>2) & 1) << 3;
	output[3] |=((input[1]>>5) & 1) << 2;
	output[3] |=((input[0]>>4) & 1) << 1;
	output[3] |=((input[3]>>7) & 1);
}

// 비트 확장 함수 구현
static void E(UBYTE R[4], UBYTE ER[6])
{
	int i;
	for(i=0; i<6; i++)
	ER[i]=0;

	ER[0] |=((R[3])	& 1) << 7;
	ER[0] |=((R[0]>>7) & 1) << 6;
	ER[0] |=((R[0]>>6) & 1) << 5;
	ER[0] |=((R[0]>>5) & 1) << 4;
	ER[0] |=((R[0]>>4) & 1) << 3;
	ER[0] |=((R[0]>>3) & 1) << 2;
	ER[0] |=((R[0]>>4) & 1) << 1;
	ER[0] |=((R[0]>>3) & 1);

	ER[1] |=((R[0]>>2) & 1) << 7;
	ER[1] |=((R[0]>>1) & 1) << 6;
	ER[1] |=((R[0])	& 1) << 5;
	ER[1] |=((R[1]>>7) & 1) << 4;
	ER[1] |=((R[0])	& 1) << 3;
	ER[1] |=((R[1]>>7) & 1) << 2;
	ER[1] |=((R[1]>>6) & 1) << 1;
	ER[1] |=((R[1]>>5) & 1);

	ER[2] |=((R[1]>>4) & 1) << 7;
	ER[2] |=((R[1]>>3) & 1) << 6;
	ER[2] |=((R[1]>>4) & 1) << 5;
	ER[2] |=((R[1]>>3) & 1) << 4;
	ER[2] |=((R[1]>>2) & 1) << 3;
	ER[2] |=((R[1]>>1) & 1) << 2;
	ER[2] |=((R[1])	& 1) << 1;
	ER[2] |=((R[2]>>7) & 1);

	ER[3] |=((R[1])	& 1) << 7;
	ER[3] |=((R[2]>>7) & 1) << 6;
	ER[3] |=((R[2]>>6) & 1) << 5;
	ER[3] |=((R[2]>>5) & 1) << 4;
	ER[3] |=((R[2]>>4) & 1) << 3;
	ER[3] |=((R[2]>>3) & 1) << 2;
	ER[3] |=((R[2]>>4) & 1) << 1;
	ER[3] |=((R[2]>>3) & 1);

	ER[4] |=((R[2]>>2) & 1) << 7;
	ER[4] |=((R[2]>>1) & 1) << 6;
	ER[4] |=((R[2]	 ) & 1) << 5;
	ER[4] |=((R[3]>>7) & 1) << 4;
	ER[4] |=((R[2]	 ) & 1) << 3;
	ER[4] |=((R[3]>>7) & 1) << 2;
	ER[4] |=((R[3]>>6) & 1) << 1;
	ER[4] |=((R[3]>>5) & 1);

	ER[5] |=((R[3]>>4) & 1) << 7;
	ER[5] |=((R[3]>>3) & 1) << 6;
	ER[5] |=((R[3]>>4) & 1) << 5;
	ER[5] |=((R[3]>>3) & 1) << 4;
	ER[5] |=((R[3]>>2) & 1) << 3;
	ER[5] |=((R[3]>>1) & 1) << 2;
	ER[5] |=(R[3] & 1) << 1;
	ER[5] |=((R[0]>>7) & 1);
}