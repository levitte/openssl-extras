/* This is unlicensed, see UNLICENSE and http://unlicense.org/ */

const BIO_METHOD *make_btrace(void);
void free_btrace(BIO_METHOD * /* btrace */);
void set_btrace_output_bio(BIO * /* bio */, BIO * /* trace */);
