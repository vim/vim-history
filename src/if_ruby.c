/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * Ruby interface by Shugo Maeda.
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <stdio.h>
#include <string.h>

#include <ruby.h>

#undef EXTERN
#undef _
#include "vim.h"
#include "version.h"

#ifdef PROTO
/* Define these to be able to generate the function prototypes. */
# define VALUE int
#endif

static int ruby_initialized = 0;
static VALUE objtbl;

static VALUE mVIM;
static VALUE cBuffer;
static VALUE cWindow;
static VALUE eDeletedBufferError;
static VALUE eDeletedWindowError;

static void ensure_ruby_initialized();
static void error_print(int);
static void ruby_io_init();
static void ruby_vim_init();

void ex_ruby(exarg_t *eap)
{
    int state;

    ensure_ruby_initialized();
    rb_eval_string_protect((char *) eap->arg, &state);
    if (state) error_print(state);
}

void ex_rubydo(exarg_t *eap)
{
    int state;
    linenr_t i;

    ensure_ruby_initialized();
    if (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	return;
    for (i = eap->line1; i <= eap->line2; i++) {
	VALUE line, oldline;

	line = oldline = rb_str_new2(ml_get(i));
	rb_lastline_set(line);
	rb_eval_string_protect((char *) eap->arg, &state);
	if (state) {
	    error_print(state);
	    break;
	}
	line = rb_lastline_get();
	if (!NIL_P(line)) {
	    ml_replace(i, (char_u *) STR2CSTR(line), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); /* recompute syntax hl. for this line */
#endif
	}
    }
    adjust_cursor();
    update_curbuf(NOT_VALID);
}

void ex_rubyfile(exarg_t *eap)
{
    int state;

    ensure_ruby_initialized();
    rb_load_protect(rb_str_new2((char *) eap->arg), 0, &state);
    if (state) error_print(state);
}

void ruby_buffer_free(buf_t *buf)
{
    if (buf->ruby_ref) {
	rb_hash_aset(objtbl, rb_obj_id((VALUE) buf->ruby_ref), Qnil);
	RDATA(buf->ruby_ref)->data = NULL;
    }
}

void ruby_window_free(win_t *win)
{
    if (win->ruby_ref) {
	rb_hash_aset(objtbl, rb_obj_id((VALUE) win->ruby_ref), Qnil);
	RDATA(win->ruby_ref)->data = NULL;
    }
}

static void ensure_ruby_initialized()
{
    if (!ruby_initialized) {
	ruby_init();
	ruby_init_loadpath();
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
}

static void error_print(int state)
{
    extern VALUE ruby_errinfo;
    VALUE eclass;
    VALUE einfo;
    char buff[BUFSIZ];

#define TAG_RETURN	0x1
#define TAG_BREAK	0x2
#define TAG_NEXT	0x3
#define TAG_RETRY	0x4
#define TAG_REDO	0x5
#define TAG_RAISE	0x6
#define TAG_THROW	0x7
#define TAG_FATAL	0x8
#define TAG_MASK	0xf

    switch (state) {
    case TAG_RETURN:
	EMSG("unexpected return");
	break;
    case TAG_NEXT:
	EMSG("unexpected next");
	break;
    case TAG_BREAK:
	EMSG("unexpected break");
	break;
    case TAG_REDO:
	EMSG("unexpected redo");
	break;
    case TAG_RETRY:
	EMSG("retry outside of rescue clause");
	break;
    case TAG_RAISE:
    case TAG_FATAL:
	eclass = CLASS_OF(ruby_errinfo);
	einfo = rb_obj_as_string(ruby_errinfo);
	if (eclass == rb_eRuntimeError && RSTRING(einfo)->len == 0) {
	    EMSG("unhandled exception");
	}
	else {
	    VALUE epath;
	    char *p;

	    epath = rb_class_path(eclass);
	    snprintf(buff, BUFSIZ, "%s: %s",
		     RSTRING(epath)->ptr, RSTRING(einfo)->ptr);
	    p = strchr(buff, '\n');
	    if (p) *p = '\0';
	    EMSG(buff);
	}
	break;
    default:
	snprintf(buff, BUFSIZ, "unknown longjmp status %d", state);
	EMSG(buff);
	break;
    }
}

static VALUE vim_message(VALUE self, VALUE str)
{
    char *buff, *p;

    str = rb_obj_as_string(str);
    buff = ALLOCA_N(char, RSTRING(str)->len);
    strcpy(buff, RSTRING(str)->ptr);
    p = strchr(buff, '\n');
    if (p) *p = '\0';
    MSG(buff);
    return Qnil;
}

static VALUE vim_set_option(VALUE self, VALUE str)
{
    do_set((char_u *) STR2CSTR(str), FALSE);
    update_screen(NOT_VALID);
    return Qnil;
}

static VALUE vim_command(VALUE self, VALUE str)
{
    do_cmdline((char_u *) STR2CSTR(str), NULL, NULL, DOCMD_VERBOSE + DOCMD_NOWAIT);
    return Qnil;
}

static VALUE vim_evaluate(VALUE self, VALUE str)
{
    char_u *value = eval_to_string((char_u *) STR2CSTR(str), (char_u**) 0);
    if (value) {
	VALUE val = rb_str_new2(value);
	vim_free(value);
	return val;
    }
    else {
	return Qnil;
    }
}

static VALUE buffer_new(buf_t *buf)
{
    if (buf->ruby_ref) {
	return (VALUE) buf->ruby_ref;
    }
    else {
	VALUE obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
	buf->ruby_ref = (void *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	return obj;
    }
}

static buf_t *get_buf(VALUE obj)
{
    buf_t *buf;

    Data_Get_Struct(obj, buf_t, buf);
    if (buf == NULL)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    return buf;
}

static VALUE buffer_s_current()
{
    return buffer_new(curbuf);
}

static VALUE buffer_s_count()
{
    buf_t *b;
    int n = 0;

    for (b = firstbuf; b; b = b->b_next) n++;
    return INT2NUM(n);
}

static VALUE buffer_s_aref(VALUE self, VALUE num)
{
    buf_t *b;
    int n = NUM2INT(num);

    for (b = firstbuf; b; b = b->b_next, --n) {
	if (n == 0)
	    return buffer_new(b);
    }
    return Qnil;
}

static VALUE buffer_name(VALUE self)
{
    buf_t *buf = get_buf(self);

    return buf->b_ffname ? rb_str_new2(buf->b_ffname) : Qnil;
}

static VALUE buffer_number(VALUE self)
{
    buf_t *buf = get_buf(self);

    return INT2NUM(buf->b_fnum);
}

static VALUE buffer_count(VALUE self)
{
    buf_t *buf = get_buf(self);

    return INT2NUM(buf->b_ml.ml_line_count);
}

static VALUE buffer_aref(VALUE self, VALUE num)
{
    buf_t *buf = get_buf(self);
    long n = NUM2LONG(num);

    if (n > 0 && n <= buf->b_ml.ml_line_count) {
	char *line = ml_get_buf(buf, n, FALSE);
	return line ? rb_str_new2(line) : Qnil;
    }
    else {
	rb_raise(rb_eIndexError, "index %d out of buffer", n);
    }
}

static VALUE buffer_aset(VALUE self, VALUE num, VALUE str)
{
    buf_t *buf = get_buf(self);
    buf_t *savebuf = curbuf;
    char *line = STR2CSTR(str);
    long n = NUM2LONG(num);

    if (n > 0 && n <= buf->b_ml.ml_line_count && line != NULL) {
	curbuf = buf;
	if (u_savesub(n) == OK) {
	    ml_replace(n, (char_u *) line, TRUE);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(n); /* recompute syntax hl. for this line */
#endif
	}
	curbuf = savebuf;
	update_curbuf(NOT_VALID);
    }
    else {
	rb_raise(rb_eIndexError, "index %d out of buffer", n);
    }
    return str;
}

static VALUE buffer_delete(VALUE self, VALUE num)
{
    buf_t *buf = get_buf(self);
    buf_t *savebuf = curbuf;
    long n = NUM2LONG(num);

    if (n > 0 && n <= buf->b_ml.ml_line_count) {
	curbuf = buf;
	if (u_savedel(n, 1) == OK) {
	    mark_adjust(n, n, MAXLNUM, -1);
	    ml_delete(n, 0);
	    changed();
	}
	curbuf = savebuf;
	update_curbuf(NOT_VALID);
    }
    else {
	rb_raise(rb_eIndexError, "index %d out of buffer", n);
    }
    return Qnil;
}

static VALUE buffer_append(VALUE self, VALUE num, VALUE str)
{
    buf_t *buf = get_buf(self);
    buf_t *savebuf = curbuf;
    char *line = STR2CSTR(str);
    long n = NUM2LONG(num);

    if (n >= 0 && n <= buf->b_ml.ml_line_count && line != NULL) {
	curbuf = buf;
	if (u_inssub(n + 1) == OK) {
	    mark_adjust(n + 1, MAXLNUM, 1L, 0L);
	    ml_append(n, (char_u *) line, (colnr_t) 0, FALSE);
	    changed();
	}
	curbuf = savebuf;
	update_curbuf(NOT_VALID);
    }
    else {
	rb_raise(rb_eIndexError, "index %d out of buffer", n);
    }
    return str;
}

static VALUE window_new(win_t *win)
{
    if (win->ruby_ref) {
	return (VALUE) win->ruby_ref;
    }
    else {
	VALUE obj = Data_Wrap_Struct(cWindow, 0, 0, win);
	win->ruby_ref = (void *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	return obj;
    }
}

static win_t *get_win(VALUE obj)
{
    win_t *win;

    Data_Get_Struct(obj, win_t, win);
    if (win == NULL)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    return win;
}

static VALUE window_s_current()
{
    return window_new(curwin);
}

static VALUE window_s_count()
{
    win_t	*w;
    int n = 0;

    for (w = firstwin; w; w = w->w_next) n++;
    return INT2NUM(n);
}

static VALUE window_s_aref(VALUE self, VALUE num)
{
    win_t *w;
    int n = NUM2INT(num);

    for (w = firstwin; w != NULL; w = w->w_next, --n) {
	if (n == 0)
	    return window_new(w);
    }
    return Qnil;
}

static VALUE window_buffer(VALUE self)
{
    win_t *win = get_win(self);

    return buffer_new(win->w_buffer);
}

static VALUE window_height(VALUE self)
{
    win_t *win = get_win(self);

    return INT2NUM(win->w_height);
}

static VALUE window_set_height(VALUE self, VALUE height)
{
    win_t *win = get_win(self);
    win_t *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    return height;
}

static VALUE window_cursor(VALUE self)
{
    win_t *win = get_win(self);

    return rb_assoc_new(INT2NUM(win->w_cursor.lnum), INT2NUM(win->w_cursor.col));
}

static VALUE window_set_cursor(VALUE self, VALUE pos)
{
    VALUE lnum, col;
    win_t *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    if (RARRAY(pos)->len != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY(pos)->ptr[0];
    col = RARRAY(pos)->ptr[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    adjust_cursor();		    /* put cursor on an existing line */
    update_screen(NOT_VALID);
    return Qnil;
}

static VALUE f_p(int argc, VALUE *argv, VALUE self)
{
    int i;
    VALUE str = rb_str_new("", 0);

    for (i = 0; i < argc; i++) {
	if (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    MSG(RSTRING(str)->ptr);
    return Qnil;
}

static void ruby_io_init()
{
    extern VALUE rb_defout;

    rb_defout = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_defout, "write", vim_message, 1);
    rb_define_global_function("p", f_p, -1);
}

static void ruby_vim_init()
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    mVIM = rb_define_module("VIM");
    rb_define_const(mVIM, "VERSION_MAJOR", INT2NUM(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2NUM(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2NUM(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2NUM(VIM_VERSION_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_VERSION_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_VERSION_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_VERSION_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_VERSION_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_message, 1);
    rb_define_module_function(mVIM, "set_option", vim_set_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);

    eDeletedBufferError = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    eDeletedWindowError = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    cBuffer = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_method(cBuffer, "current", buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", buffer_s_count, 0);
    rb_define_singleton_method(cBuffer, "[]", buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", buffer_name, 0);
    rb_define_method(cBuffer, "number", buffer_number, 0);
    rb_define_method(cBuffer, "count", buffer_count, 0);
    rb_define_method(cBuffer, "length", buffer_count, 0);
    rb_define_method(cBuffer, "[]", buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", buffer_aset, 2);
    rb_define_method(cBuffer, "delete", buffer_delete, 1);
    rb_define_method(cBuffer, "append", buffer_append, 2);

    cWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cWindow, "current", window_s_current, 0);
    rb_define_singleton_method(cWindow, "count", window_s_count, 0);
    rb_define_singleton_method(cWindow, "[]", window_s_aref, 1);
    rb_define_method(cWindow, "buffer", window_buffer, 0);
    rb_define_method(cWindow, "height", window_height, 0);
    rb_define_method(cWindow, "height=", window_set_height, 1);
    rb_define_method(cWindow, "cursor", window_cursor, 0);
    rb_define_method(cWindow, "cursor=", window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current, 0);
    rb_define_virtual_variable("$curwin", window_s_current, 0);
}
