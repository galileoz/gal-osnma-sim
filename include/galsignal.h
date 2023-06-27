
/**
 * @brief signal fonction definition
 * @file:   galsignal.h
 * @author: FDC
 *
 * Created on 8 février 2018, 13:59
 */

#ifndef SIGNAL_H
#define SIGNAL_H


void cnv_encd(long input_len, int *in_array, int *out_array);

void interleave(const int *in, int row, int col, int *out);

void add_sync(int *in, int *out);

void boc(short *boccode, short *code, int len, int m, int n);

void galcodegen_E1B(short *primaryCode_E1B, int prn);

void galcodegen_E1C(short *primaryCode_E1C, int prn);

#endif /* SIGNAL_H */
