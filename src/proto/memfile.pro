/* memfile.c */
memfile_t *mf_open __ARGS((char_u *fname, int flags));
int mf_open_file __ARGS((memfile_t *mfp, char_u *fname));
void mf_close __ARGS((memfile_t *mfp, int del_file));
void mf_close_file __ARGS((buf_t *buf, int getlines));
bhdr_t *mf_new __ARGS((memfile_t *mfp, int negative, int page_count));
bhdr_t *mf_get __ARGS((memfile_t *mfp, blocknr_t nr, int page_count));
void mf_put __ARGS((memfile_t *mfp, bhdr_t *hp, int dirty, int infile));
void mf_free __ARGS((memfile_t *mfp, bhdr_t *hp));
int mf_sync __ARGS((memfile_t *mfp, int flags));
int mf_release_all __ARGS((void));
blocknr_t mf_trans_del __ARGS((memfile_t *mfp, blocknr_t old_nr));
void mf_set_ffname __ARGS((memfile_t *mfp));
void mf_fullname __ARGS((memfile_t *mfp));
int mf_need_trans __ARGS((memfile_t *mfp));
/* vim: set ft=c : */
