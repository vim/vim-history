/* vim: set sw=4 sts=4 : */
/*
 * uvim.h
 *
 * "class uvim" specification
 *
 * When       Who       What
 * 1999-??-?? Anon      Original
 * 2001-08-04 W.Briscoe Added dual C/C++ capability. Wrote STANDALONE demo.
 */

#ifndef GVIM_H
#define GVIM_H

#if 0
/*
 * As vim currently only provides an IDispatch interface, It seems a mistake to
 * include if_ole.h W.Briscoe 2001-08-04
 *
 * if_ole.h, created with VC5, is portable to VC4 C with definitions below.
 * It is not portable to VC4 C++.
 */
#if !defined(BEGIN_INTERFACE)
# define BEGIN_INTERFACE
# define END_INTERFACE
#endif

/* VC4 does not process #if 0 in finding dependencies */
/* #include "if_ole.h" o* This is a VC5 file */
#endif /* 0 */

#if defined(__cplusplus)
class uvim
{
    IDispatch *m_pDisp;
    public:
    uvim();
    ~uvim();
    void SendKeys(const char *sKeys); /* sKeys is nul-terminated */
    char *Eval(const char *sExpr); /* Caller frees return value. */
    void SetForeground();
    unsigned GetHwnd();
};

#else

typedef struct uvim uvim;

struct uvim
{
    IDispatch *m_pDisp;
    void (*SendKeys)(uvim *this, const char *sKeys);
						 /* sKeys is nul-terminated */
    char *(*Eval)(uvim *this, const char *sExpr);
					      /* Caller frees return value. */
    void (*SetForeground)(uvim *this);
    unsigned (*GetHwnd)(uvim *this);
};

uvim touvim(void);
void unuvim(uvim *o);
#endif /* defined(__cplusplus) */

#endif /* ndef GVIM_H */
