#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

void SubBytes(unsigned char *dst, unsigned char *src);
void invSubBytes(unsigned char *dst, unsigned char *src);
void ShiftRows(unsigned char *dst, unsigned char *src);
void invShiftRows(unsigned char *dst, unsigned char *src);
void MixColumns(unsigned char *dst, unsigned char *src);
void invMixColumns(unsigned char *dst, unsigned char *src);
void AddRoundKey(unsigned char *dst, unsigned char *src, unsigned char *key);
void KeyExpansion(unsigned char *w, unsigned char *init_key);
void Transpose(unsigned char *src);

#endif