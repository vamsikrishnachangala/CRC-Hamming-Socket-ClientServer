#ifndef ENCDEC_H
#define ENCDEC_H

#define READ_END 0
#define WRITE_END 1

char* ascii_to_binary(char c);

char* int_to_binary(int num);

char binary_to_ascii(char* binary);

char* add_odd_parity(char* binary);

char* remove_odd_parity(char* binary);

char* read_file(char* filename, off_t* offset,int buffer_size);

void inpf_to_binf(char* file);

void binf_to_outf();

void outf_to_chck();

void chck_to_done();

#endif /* ENCDEC_H */
