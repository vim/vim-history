/* memfile.c */
struct memfile *mf_open __PARMS((unsigned char *fname, int new, int fail_nofile));
int mf_open_file __PARMS((struct memfile *mfp, unsigned char *fname));
void mf_close __PARMS((struct memfile *mfp, int delete));
struct block_hdr *mf_new __PARMS((struct memfile *mfp, int negative, int page_count));
struct block_hdr *mf_get __PARMS((struct memfile *mfp, long nr, int page_count));
void mf_put __PARMS((struct memfile *mfp, struct block_hdr *hp, int dirty, int infile));
void mf_free __PARMS((struct memfile *mfp, struct block_hdr *hp));
int mf_sync __PARMS((struct memfile *mfp, int all, int check_char));
int mf_release_all __PARMS((void));
long mf_trans_del __PARMS((struct memfile *mfp, long old));
void mf_fullname __PARMS((struct memfile *mfp));
int mf_need_trans __PARMS((struct memfile *mfp));
void mf_statistics __PARMS((void));
