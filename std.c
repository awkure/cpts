#pragma GCC push_options

void * syscall1(
        intptr,
        void*
);

void * syscall2(
        intptr,
        void*,
        void*
);

void * syscall3(
        intptr,
        void*,
        void*,
        void*
);

void * syscall4(
        intptr,
        void*,
        void*,
        void*,
        void*
);

void * syscall5(
        intptr,
        void*,
        void*,
        void*,
        void*,
        void*
);

void * syscall6(
        intptr,
        void*,
        void*,
        void*,
        void*,
        void*,
        void*
);

#if 0
/* Pretty sad that C macros are not that extensible like in lisp */
#define INIT_SYSCALL(n,...) (void * syscall##n(intptr,##__VA_ARGS__))
#define SYSCALL(n,...) ((n < 6 && n < 0) ? \
        syscall##n(##__VA_ARGS__) : __mba(0))
#endif

#define __THROW __attribute__ ((__nothrow__ __LEAF))

#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))

#define __DEV(x) _Pragma (#x)
__DEV(message("Currently in development"))


#define __isascii(c)  (((c) & ~0x7f) == 0)
#define __toascii(c)  ((c)  &  0x7f)
#define __exctype(n) extern int n (int) __THROW


#define NULL ((void*)0) 
#define EOF  (-1)

#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#define STDERR_FILENO  2 

#define feof(p)     (((p)->flag & _EOF) != 0)
#define ferror(p)   (((p)->flag & _ERR) != 0)
#define fileno(p)   ((p)->fd)

#define O_RDONLY  0x0000
#define O_WRONLY  0x0001
#define O_RDWR    0x0002  
#define O_CREAT   0x0040
#define O_TRUNC   0x0200
#define O_APPEND  0x0400


#define getc(p) (--(p)->cnt >= 0 \
              ? (unsigned char) *(p)->ptr++ : _fillbuf(p))
#define putc(p) (--(p)->cnt >= 0 \
              ? *(p)->ptr++ = (x) : _flushbuf((x),p))

#define getchar()  getc(stdin)
#define putchar(c) putc((c), stdout)


enum _fflags {
      _READ  = 01
    , _WRITE = 02
    , _UNBUF = 04
    , _EOF   = 010
    , _ERR   = 020
};

typedef u16 * va_list;
#define va_start(l, p) (l = (((va_list)&p) + sizeof(p)))
#define va_arg(l, t) (*(t*)((l += sizeof(t)) - sizeof(t)))
#define va_end(l) (free(l))

#define NALLOC 1024

typedef union hdr {
    struct {
        union hdr * ptr;
        unsigned sz;
    } s;
    align x;
} header;

static header base;
static header * freep = NULL;


typedef struct _iobuf {
    intptr  cnt;
    char *  ptr;
    char * base;
    i16    flag;
    i32      fd;
} fd_t;


fd_t _iob[_NFILEMAX] = {
    { 0, (char*) 0, (char*) 0, _READ,           0 },
    { 0, (char*) 0, (char*) 0, _WRITE,          0 },
    { 0, (char*) 0, (char*) 0, _WRITE | _UNBUF, 0 }
};

#define stdin  (&_iob[STDIN_FILENO])
#define stdout (&_iob[STDOUT_FILENO])
#define stderr (&_iob[STDERR_FILENO])

#define strcpy(dst, src) memcpy(dst, src, strlen(src)+1)
#define strncpy memcpy

#define ZEROPAD  (1<<0)
#define SIGN     (1<<1)
#define PLUS     (1<<2)
#define SPACE    (1<<3)
#define LEFT     (1<<4)
#define SPECIAL  (1<<5) 
#define LARGE    (1<<6) 

#define CVTBUFSIZE  80

static char * digits   = "0123456789abcdefghijklmnopqrstuvwxyz";
static char * updigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

extern inline intptr write(int, void const *, uintptr);
extern inline intptr read(int, void *, intptr);
extern inline int open(char * const, u32, u32); 
extern inline void close(int);
extern inline void exit(int);
extern inline int creat(char * const, mode_t);
extern void atexit(void (*f)());
extern inline off_t lseek(u32, off_t, u32); 
extern inline time_t time(void *);

extern int ftruncate(int, off_t);

extern char * strchr(char const *, int);
extern char * strrchr(char *, int);
extern char * strstr(char *, char *);
extern char * strcat(char * restrict, char * restrict);
extern size_t strcspn(char const *, char const *);
extern uintptr strlen(char const *);
extern uintptr strnlen(char const *, size_t);
static char * _getbase(char *, i16 *);
extern int strcmp(const char *, const char *);
static int skip_atoi(const char **);
static int _atob(uintptr *, char *, i16);
extern int atob(u32 *, char *, i16);
extern inline int isdigit(int); /* TODO: define via ctype */
extern inline int iscntrl(int); /* TODO: define via ctype */
extern inline int isspace(int); /* TODO: define via ctype */
extern char * strdup(char *);
extern int getline(char **, size_t *, fd_t *);


static int vsscanf(char *, char *, va_list);
extern int sscanf(char * const, char * const, ...);
static char * _number(char *, intptr, int, size_t, int, int);
static char * eaddr(char *, u8 *, size_t, int);
static char * iaddr(char *, u8 *, size_t, int);
static void cfltcvt(double, char *, char, int);
static void forcdecpt(char *);
static void cropzeros(char *);
static char * flt(char *, double, int, int, char, int);

extern char * ecvtbuf(double, int, int *, int *, char *); /* TODO: unimplemented */
extern char * fcvtbuf(double, int, int *, int *, char *); /* TODO: unimplemented */

int vsprintf(char *, const char *, va_list);
int sprintf(char *, const char *, ...);
int printf(const char *, ...);

extern void * mmap(void *, uintptr, int, int, int, u32);
extern int munmap(void *, size_t);
extern void * malloc(size_t);
extern void * calloc(size_t, size_t);
extern void memset(void *, u8, size_t);
extern void free(void *);
extern void memcpy(void *, void const *, size_t);
extern void memmove(void*, void const *, size_t);

static u8 _flushbuf(int c, fd_t *);
static u8 _fillbuf(fd_t *);

extern fd_t * fopen(char * const, char * const);
extern void fclose(fd_t *);
extern intptr fflush(fd_t *);
extern void stdfflush(void);



inline intptr __attribute__((nonnull(2)))
write(int fd, void const * d, uintptr b)
{
    return (uintptr)
        syscall3(
                SYS_WRITE,
                (void*)(intptr)fd,
                (void*)d,
                (void*)b
        );
}



inline intptr __attribute__((nonnull(2)))
read(int fd, void * d, intptr b)
{
    return (intptr)
        syscall3(
                SYS_READ,
                (void*)(intptr)fd,
                d,
                (void*)b
        );
}



inline int __attribute__((nonnull(1)))
open(char * const fn, u32 fl, mode_t m)
{
    return (int)(intptr)
        syscall3(
                SYS_OPEN,
                (void*)fn,
                (void*)(intptr)fl,
                (void*)(intptr)m
        );
}



inline int __attribute__((nonnull(1)))
creat(char * const fn, mode_t m)
{
    return open(fn, O_WRONLY|O_CREAT|O_TRUNC, m);
        /* syscall2(
                SYS_CREAT,
                (void*)fn,
                (void*)(uintptr)m
        ); */
}



inline void
close(int fd)
{
    syscall1(
            SYS_CLOSE,
            (void*)(intptr)fd
    );
}



inline void
exit(int st)
{
    syscall1(
            SYS_EXIT,
            (void*)(intptr)st
    );
}



void 
atexit(void (*f)())
{
    (void)(*f)();
    exit(0);
}



inline off_t
lseek(u32 fd, off_t off, u32 fr)
{
    return (off_t)
        syscall3(
                SYS_LSEEK,
                (void*)(uintptr)fd,
                (void*)(uintptr)off,
                (void*)(uintptr)fr
        );
}



inline time_t 
time(void * tloc)
{
    return (time_t) 
        syscall1(
                SYS_TIME,
                tloc
        );
}



int 
ftruncate(int fd, off_t l)
{
    /* TODO */
    (void)fd;
    (void)l;
    return 0;
}




char *
strchr(char const * s, int c)
{
    while (*s != (char) c)
        if (!*s++) 
            return NULL;
    return (char *) s;
}



char *
strrchr(char * s, int c)
{
    char * r = 0;
    do { 
        if (*s == (char) c)
            r = s;
    } while (*s++);
    return r;
}



char * __attribute__((nonnull(1,2)))
strstr(char * str, char * sstr)
{
    while (*str) {
        char * b = str;
        char * p = sstr;

        while (*str && *p && *str == *p)
            str++, p++;

        if (!*p)
            return b;

        str = b+1;
    }
    return NULL;
}



char * __attribute__((nonnull(1,2)))
strcat (char * dest, char * src)
{
    char * r = dest;
    while (*dest) ++dest;
    while (*dest++ = *src++);
    return r;
}


size_t __attribute__((nonnull(1,2)))
strcspn(char const * s1, char const * s2)
{
    size_t r = 0;
    
    while(*s1)
        if (strchr(s2, *s1)) return r;
        else s1++, r++;
    
    return r;
}



inline int
isdigit(int c)
{
    return (c>='0') && (c<='9');
}



inline int
iscntrl(int c)
{
    return strchr("\r\n\v\f\r\e\a\b", c) != NULL;
}



inline int
isspace(int c)
{
    return strchr(" \n\t\v\f\r", c) != NULL;
} 



#if 0
#define isdigit(c) __isctype((c), (1<<3)) 
#define isspace(c) __isctype((c), (1<<5)) 
#define iscntrl(c) __isctype((c), (1<<9)) 
#endif  



int 
getline(char ** s, size_t * n, fd_t * fp)
{
    char line[sizeof(char)];
    size_t l = *n, i = 0;
    int c = 0;;


    if (fp == NULL || n == NULL)
        return -1;

    if (ferror(fp) || feof(fp))
        return -1;

    while (--l > 0 && (c = getchar()) != EOF && c != '\n')
        line[i++] = c;

    if (c == '\n')
        line[i++] = c;

    line[i] = '\0';

    *s = line;
    //*n = i;

    return i;
}



int 
strcmp(const char * s1, const char * s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++, s2++;
    }

    return *(const u8 *)s1 - *(const u8 *)s2;
}



static int 
skip_atoi(const char ** s) 
{
    size_t i = 0;
    while (isdigit(**s)) 
        i = i*10 + *((*s)++) - '0';
    return i;
}



static char * 
_number (
        char * str, 
        intptr n, 
        int _base, 
        size_t siz, 
        int pr, 
        int t ) 
{
    char c, sign, buf[66];
    char * dig = digits;
    int i;

    if (t & LARGE)  
        dig = updigits;
    if (t & LEFT) 
        t &= ~ZEROPAD;
    if (_base < 2 || _base > 36) 
        return 0;

    c = (t & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (t & SIGN) {
        if (n < 0) {
            sign = '-';
            n = -n;
            siz--;
        } else if (t & PLUS) {
            sign = '+';
            siz--;
        } else if (t & SPACE) {
            sign = ' ';
            siz--;
        }
    }

    if (t & SPECIAL) {
        if (_base == 16) {
            siz -= 2;
        } else if (_base == 8) {
            siz--;
        }
    }

    i = 0;

    if (n == 0) {
        buf[i++] = '0';
    } else {
        while (n != 0) {
            buf[i++] = dig[((uintptr) n) % (u32) _base];
            n = ((uintptr) n) / (u32) _base;
        }
    }

    if (i > pr) 
        pr = i;
    
    siz -= pr;
    
    if (!(t & (ZEROPAD | LEFT))) 
        while (siz-- > 0) 
            *str++ = ' ';
    
    if (sign) 
        *str++ = sign;

    if (t & SPECIAL) {
        if (_base == 8) {
            *str++ = '0';
        } else if (_base == 16) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }

    if (!(t & LEFT)) 
        while (siz-- > 0) 
            *str++ = c;
    
    while (i < pr--) 
        *str++ = '0';
    
    while (i-- > 0) 
        *str++ = buf[i];
    
    while (siz-- > 0) 
        *str++ = ' ';

    return str;
}



static char *
eaddr (
        char * str, 
        u8 * addr, 
        size_t siz, 
        int t ) 
{
    char buf[24];
    char * dig = digits;
    uintptr i, len;


    if (t & LARGE)  
        dig = updigits;
    
    len = 0;
    
    for (i = 0; i < 6; i++) {
        if (i != 0) 
            buf[len++] = ':';
        buf[len++] = dig[addr[i] >> 4];
        buf[len++] = dig[addr[i] & 0x0F];
    }

    if (!(t & LEFT)) 
        while (len < siz--) 
            *str++ = ' ';
    
    for (i = 0; i < len; ++i) 
        *str++ = buf[i];
    
    while (len < siz--) 
        *str++ = ' ';

    return str;
}



static char *
iaddr (
        char * str, 
        u8 * addr, 
        size_t siz, 
        int t ) 
{
    char buf[24];
    int i, n, len;

    len = 0;
    for (i = 0; i < 4; i++) {
        if (i != 0) 
            buf[len++] = '.';
        
        n = addr[i];

        if (n == 0) {
            buf[len++] = digits[0];
        } else {
            if (n >= 100) {
                buf[len++] = digits[n / 100];
                n = n % 100;
                buf[len++] = digits[n / 10];
                n = n % 10;
            } else if (n >= 10) {
                buf[len++] = digits[n / 10];
                n = n % 10;
            }

            buf[len++] = digits[n];
        }
    }

    if (!(t & LEFT)) 
        while ((size_t)len < siz--) 
            *str++ = ' ';
    
    for (i = 0; i < len; ++i) 
        *str++ = buf[i];
    
    while ((size_t)len < siz--) 
        *str++ = ' ';

    return str;
}



static void 
cfltcvt(
        double val, 
        char * buf, 
        char fmt, 
        int pr ) 
{
    int decpt, sign, exp, pos;
    char * digs = NULL;
    char cvtbuf[CVTBUFSIZE];
    int capexp = 0;
    int mag;

    if (fmt == 'G' || fmt == 'E') {
        capexp = 1;
        fmt += 'a' - 'A';
    }

    if (fmt == 'g') {
        digs = ecvtbuf(val, pr, &decpt, &sign, cvtbuf);
        mag = decpt - 1;
        if (mag < -4  ||  mag > pr - 1) {
            fmt = 'e';
            pr -= 1;
        } else {
            fmt = 'f';
            pr -= decpt;
        }
    }

    if (fmt == 'e') {
        digs = ecvtbuf(val, pr + 1, &decpt, &sign, cvtbuf);

        if (sign) 
            *buf++ = '-';
        
        *buf++ = *digs;
        
        if (pr > 0) 
            *buf++ = '.';
        
        memcpy(buf, digs + 1, pr);
        buf += pr;
        *buf++ = capexp ? 'E' : 'e';

        if (decpt == 0) {
            if (val == 0.0) {
                exp = 0;
            } else {
                exp = -1;
            }
        } else {
            exp = decpt - 1;
        }

        if (exp < 0) {
            *buf++ = '-';
            exp = -exp;
        } else {
            *buf++ = '+';
        }

        buf[2] = (exp % 10) + '0';
        exp = exp / 10;
        buf[1] = (exp % 10) + '0';
        exp = exp / 10;
        buf[0] = (exp % 10) + '0';
        buf += 3;
    } else if (fmt == 'f') {
        digs = fcvtbuf(val, pr, &decpt, &sign, cvtbuf);
        
        if (sign) 
            *buf++ = '-';
        
        if (*digs) {
            if (decpt <= 0) {
                *buf++ = '0';
                *buf++ = '.';
                
                for (pos = 0; pos < -decpt; pos++) 
                    *buf++ = '0';
                
                while (*digs) 
                    *buf++ = *digs++;
            } else {
                pos = 0;
                while (*digs) {
                    if (pos++ == decpt) 
                        *buf++ = '.';
                    *buf++ = *digs++;
                }
            }
        } else {
            *buf++ = '0';
            if (pr > 0) {
                *buf++ = '.';
                for (pos = 0; pos < pr; pos++) 
                    *buf++ = '0';
            }
        }
    }

    *buf = '\0';
}



static void 
forcdecpt(char * buf) 
{
    while (*buf) {
        if (*buf == '.') 
            return;
        
        if (*buf == 'e' || *buf == 'E') 
            break;
        
        buf++;
    }

    if (*buf) {
        int n = strlen(buf);
        while (n > 0) {
            buf[n + 1] = buf[n];
            n--;
        }

        *buf = '.';
    } else {
        *buf++ = '.';
        *buf = '\0';
    }
}



static void 
cropzeros(char * buf) 
{
    char *stop;

    while (*buf && *buf != '.') 
        buf++;
    
    if (*buf++) {
        while (*buf && *buf != 'e' && *buf != 'E') 
            buf++;
        
        stop = buf--;
        
        while (*buf == '0') 
            buf--;
        
        if (*buf == '.') 
            buf--;
        
        while ((*++buf = *stop++))
            ;
    }
}



static char *
flt(
        char * str, 
        double num, 
        int siz, 
        int pr, 
        char fmt, 
        int flags ) 
{
    char cvtbuf[CVTBUFSIZE];
    char c, sign;
    int n, i;

    if (flags & LEFT) flags &= ~ZEROPAD;

    c = (flags & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (flags & SIGN) {
        if (num < 0.0) {
            sign = '-';
            num = -num;
            siz--;
        } else if (flags & PLUS) {
            sign = '+';
            siz--;
        } else if (flags & SPACE) {
            sign = ' ';
            siz--;
        }
    }

    if (pr < 0) {
        pr = 6; 
    } else if (pr == 0 && fmt == 'g') {
        pr = 1; 
    }

    cfltcvt(num, cvtbuf, fmt, pr);

    if ((flags & SPECIAL) && pr == 0) 
        forcdecpt(cvtbuf);

    if (fmt == 'g' && !(flags & SPECIAL)) 
        cropzeros(cvtbuf);

    n = strlen(cvtbuf);

    siz -= n;
    
    if (!(flags & (ZEROPAD | LEFT))) 
        while (siz-- > 0) 
            *str++ = ' ';
    
    if (sign) 
        *str++ = sign;
    
    if (!(flags & LEFT)) 
        while (siz-- > 0) 
            *str++ = c;
    
    for (i = 0; i < n; i++) 
        *str++ = cvtbuf[i];
    
    while (siz-- > 0) 
        *str++ = ' ';

    return str;
}



int 
vsprintf(char * buf, const char * fmt, va_list args) 
{
    int len;
    uintptr n;
    int i, _base;
    char *str;
    char *s;

    int flags, fw, pr, qua;

    for (str = buf; *fmt; fmt++) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        flags = 0;
repeat:
        fmt++; 
        switch (*fmt) {
            case '-': flags |= LEFT;    goto repeat;
            case '+': flags |= PLUS;    goto repeat;
            case ' ': flags |= SPACE;   goto repeat;
            case '#': flags |= SPECIAL; goto repeat;
            case '0': flags |= ZEROPAD; goto repeat;
        }

        fw = -1;
        if (isdigit(*fmt)) {
            fw = skip_atoi(&fmt);
        } else if (*fmt == '*') {
            fmt++;
            fw = va_arg(args, int);
            if (fw < 0) {
                fw = -fw;
                flags |= LEFT;
            }
        }

        pr = -1;
        if (*fmt == '.') {
            ++fmt;    
            if (isdigit(*fmt)) {
                pr = skip_atoi(&fmt);
            } else if (*fmt == '*') {
                ++fmt;
                pr = va_arg(args, int);
            }
            if (pr < 0) pr = 0;
        }

        qua = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qua = *fmt;
            fmt++;
        }

        _base = 10;

        switch (*fmt) {
            case 'c':
                if (!(flags & LEFT)) 
                    while (--fw > 0) 
                        *str++ = ' ';
                
                *str++ = (unsigned char) va_arg(args, int);
                
                while (--fw > 0) 
                    *str++ = ' ';
                
                continue;

            case 's':
                s = va_arg(args, char *);
                
                if (!s) 
                    s = "<NULL>";
                
                len = strnlen(s, pr);
                
                if (!(flags & LEFT)) 
                    while (len < fw--) 
                        *str++ = ' ';
                
                for (i = 0; i < len; ++i) 
                    *str++ = *s++;
                
                while (len < fw--) 
                    *str++ = ' ';
                
                continue;

            case 'p':
                if (fw == -1) {
                    fw = 2 * sizeof(void *);
                    flags |= ZEROPAD;
                }
                
                str = _number(str, (unsigned long) va_arg(args, void *), 16, fw, pr, flags);
                
                continue;

            case 'n':
                if (qua == 'l') {
                    long *ip = va_arg(args, long *);
                    *ip = (str - buf);
                } else {
                    int *ip = va_arg(args, int *);
                    *ip = (str - buf);
                }
                continue;

            case 'A':
            case 'a':
                if (*fmt == 'A')
                    flags |= LARGE;
                if (qua == 'l') {
                    str = eaddr(str, va_arg(args, unsigned char *), fw, flags);
                } else {
                    str = iaddr(str, va_arg(args, unsigned char *), fw, flags);
                }
                continue;

            case 'o':
                _base = 8;
                break;

            case 'X':
            case 'x':
                if (*fmt == 'X')
                    flags |= LARGE;
                _base = 16;
                break;

            case 'd':
            case 'i':
                flags |= SIGN;

            case 'u':
                break;


            case 'E':
            case 'G':
            case 'e':
            case 'f':
            case 'g':
                str = flt(str, va_arg(args, double), fw, pr, *fmt, flags | SIGN);
                continue;


            default:
                if (*fmt != '%') *str++ = '%';
                if (*fmt) {
                    *str++ = *fmt;
                } else {
                    --fmt;
                }
                continue;
        }

        if (qua == 'l') { 
            n = va_arg(args, unsigned long);
        } else if (qua == 'h') {
            if (flags & SIGN) {
                n = va_arg(args, short);
            } else {
                n = va_arg(args, unsigned short);
            }
        } else if (flags & SIGN) {
            n = va_arg(args, int);
        } else {
            n = va_arg(args, unsigned int);
        }

        str = _number(str, n, _base, fw, pr, flags);
    }

    *str = '\0';
    return str - buf;
}



int 
sprintf(char * buf, const char * fmt, ...) 
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = vsprintf(buf, fmt, args);
    va_end(args);

    return n;
}



int __attribute__((nonnull(1)))
printf(const char * fmt, ...) 
{
    va_list a;
    int r;

    va_start(a, fmt);
    r = vfprintf(stdout, fmt, a);
    va_end(a);

    return r;
}


static char *
_getbase(char * p, i16 * bp)
{
    if (p[0] == '0') {
        switch (p[1]) {
            case 'x': 
                *bp = 16;
                break;
            case 't':
            case 'n':
                *bp = 10;
                break;
            case '0':
                *bp = 10;
                return p;
        }
        return (p + 2);
    }
    *bp = 10;
    return p;
}



static int 
_atob(uintptr * vp, char * p, i16 bas)
{
    uintptr val, v1, v2;
    char * q, tmp[20];
    int digit;

    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
        bas = 16;
    }

    if (bas == 16 && (q = strchr(p, '.')) != 0) {
        if ((size_t)(q - p) > sizeof(tmp) - 1)
            return 0;

        strncpy(tmp, p, q - p);
        tmp[q-p] = '\0';
        
        if (!_atob (&v1, tmp, 16))
			return 0;

		q++;

		if (strchr (q, '.'))
			return 0;

		if (!_atob (&v2, q, 16))
			return 0;
		
        *vp = (v1 << 16) + v2;
		
        return 1;
	}

	val = *vp = 0;
	for (; *p; p++) {
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			digit = *p - 'A' + 10;
		else
			return 0;

		if (digit >= bas)
			return 0;

		val *= bas;
		val += digit;
	}

	*vp = val;
	return 1;
}



int
atob(u32 * vp, char * p, i16 bas)
{
    uintptr v;

    if (bas == 0)
        p = _getbase(p, &bas);
    
    if (_atob (&v, p, bas)) {
        *vp = v;
        return 1;
    }

    return 0;
}



static int
vsscanf(char * buf, char * s, va_list ap)
{
    int count, noassign, _base = 10, lflag;
    u32 width;
    const char * tc;
    char * t, tmp[sizeof(char)];

    count = noassign = width = lflag = 0;
    
    while (*s && *buf) {
	    while (isspace (*s))
	        s++;
	    if (*s == '%') {
	        s++;
	        for (; *s; s++) {
		        if (strchr ("dibouxcsefg%", *s))
		            break;
		
                if (*s == '*')
		            noassign = 1;
		        else if (*s == 'l' || *s == 'L')
		            lflag = 1;
		        else if (*s >= '1' && *s <= '9') {
		            for (tc = s; isdigit (*s); s++)
                        ;
		            strncpy (tmp, tc, s - tc);
		            tmp[s - tc] = '\0';
		            atob(&width, tmp, 10);
		            s--;
		        }
	        }
	    
            if (*s == 's') {
		        while (isspace (*buf))
		            buf++;
		
                if (!width)
		            width = strcspn (buf, " \t\n\r\f\v");
		
                if (!noassign) {
		            strncpy (t = va_arg (ap, char *), buf, width);
		            t[width] = '\0';
		        }
		
                buf += width;
	        } else if (*s == 'c') {
		    
                if (!width)
		            width = 1;
		        
                if (!noassign) {
		            strncpy (t = va_arg (ap, char *), buf, width);
		            t[width] = '\0';
		        }
		
                buf += width;
	        } else if (strchr ("dobxu", *s)) {
		        while (isspace (*buf))
		            buf++;
		        
                if (*s == 'd' || *s == 'u')
		            _base = 10;
		        else if (*s == 'x')
		            _base = 16;
		        else if (*s == 'o')
		            _base = 8;
		        else if (*s == 'b')
		            _base = 2;
		    
                if (!width) {
		            if (isspace (*(s + 1)) || *(s + 1) == 0)
			            width = strcspn (buf, " \t\n\r\f\v");
		            else
			            width = strchr (buf, *(s + 1)) - buf;
		        }
		
                strncpy (tmp, buf, width);
		        tmp[width] = '\0';
		        buf += width;
		
                if (!noassign)
		            atob (va_arg (ap, u32 *), tmp, _base);
	        }
	    
            if (!noassign)
		        count++;
	    
            width = noassign = lflag = 0;
	        s++;
        } else {
	        while (isspace (*buf))
		        buf++;
	    
            if (*s != *buf)
		        break;
	        else
		        s++, buf++;
	    }
    }
    return (count);
}



int 
sscanf (char * const b, char * const fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    int i = vsscanf(b, fmt, ap);
    va_end(ap);

    return i;
}



void *
mmap(
    void * addr,
    uintptr nb,
    int prot,
    int fl,
    int fd,
    u32 off )
{
    return  
        syscall6(
                SYS_MMAP,
                addr,
                (void*)nb,
                (void*)(intptr)prot,
                (void*)(intptr)fl,
                (void*)(intptr)fd,
                (void*)(intptr)off
        );
}



inline int __attribute__((nonnull(1)))
munmap(void * addr, size_t nb)
{
    return (int)(intptr)
        syscall2(
                SYS_MUNMAP,
                addr,
                (void*)nb
        );

}



void *
malloc(size_t nb)
{
    header * p, * pr;
    size_t nunits;

    nunits = (nb+sizeof(header)-1)/sizeof(header)+1;
    if ((pr = freep) == NULL) {
        base.s.ptr = freep = pr = &base;
        base.s.sz = 0;
    }

    for (p = pr->s.ptr; ; pr = p, p = p->s.ptr) {
        if (p->s.sz >= nunits) { 
            if (p->s.sz == nunits) 
                pr->s.ptr = p->s.ptr;
            else {
                p->s.sz -= nunits;
                p += p->s.sz;
                p->s.sz = nunits;
            }
            freep = pr;
            return (void *)(p+1);
        }

        if (p == freep) 
            return NULL;
    }
}



void *
calloc(size_t nmemb, size_t sz)
{
    void * r = malloc(nmemb * sz);

    if (r)
        memset(r, 0x00, nmemb * sz);

    return r;
}



void * __attribute__((nonnull(1)))
realloc(void * p, size_t nl)
{
    if (nl == 0) {
        free(p);
        return NULL;
    } else {
        void * np = calloc(1,nl);
        if (np) {
            memcpy(np, p, sizeof(p));
            free(p);
        }
        return np;
    }
}



void
memset(void * dst, u8 v, size_t nb)
{
    uintptr i;

    if (nb % sizeof(size_t) == 0) {
        size_t * dst_ch = (size_t*)dst;
        size_t ch;
        u8 * ch_raw = (u8*)&ch;

        for (i = 0; i < sizeof(size_t); ++i) ch_raw[i] = v;
        for (i = 0; i < nb / sizeof(size_t); ++i) dst_ch[i] = ch;
    } else {
        u8 * dst_b = (u8*)dst;
        
        for (i = 0; i < nb; ++i)
            dst_b[i] = v;
    }
}



void
free(void * ap)
{
    header * bp, * p;

    bp = (header *)ap - 1; 
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; 

    if (bp + bp->s.sz == p->s.ptr) { 
        bp->s.sz += p->s.ptr->s.sz;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else 
        bp->s.ptr = p->s.ptr;

    if (p + p->s.sz == bp) {
        p->s.sz += bp->s.sz;
        p->s.ptr = bp->s.ptr;
    } else 
        p->s.ptr = bp;

    freep = p;
}



void
memcpy(void * dst, void const * src, size_t nb)
{
    size_t i;
    u8 * dst_b;
    const u8 * src_b;

    if (nb / sizeof(intptr)) {
        intptr * dst_ch = (intptr*)dst;
        const intptr * src_ch = (intptr*)src;

        for (i = 0; i < nb / sizeof(intptr); ++i) dst_ch[i] = src_ch[i];

        nb %= sizeof(intptr);
        dst = &dst_ch[i];
        src = &src_ch[i];
    }

    dst_b = (u8*)dst;
    src_b = (u8*)src;

    for (i = 0; i < nb; ++i)
        dst_b[i] = src_b[i];
}



void __attribute__((nonnull(1,2)))
memmove(void * d, void const * s, size_t n)
{
    char * cs = (char*)s;
    char * cd = (char*)d;
    char * bf = calloc(1,n);

    for (size_t i = 0; i < n; ++i) bf[i] = cs[i];
    for (size_t i = 0; i < n; ++i) cd[i] = bf[i];

    free(bf);
}



static u8 __attribute__((nonnull(1)))
_fillbuf(fd_t * fp)
{
    intptr bs;

    if ((fp->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;

    bs = (fp->flag & _UNBUF) ? 1 : BUFSIZ;
    if (fp->base == NULL) 
        if ((fp->base = (char *) calloc(1, bs)) == NULL)
            return EOF; 

    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, bs);

    if (--fp->cnt < 0) {
        fp->flag |= fp->cnt == -1 ? _EOF : _ERR;
        fp->cnt = 0;
        return EOF;
    }

    return (u8) *fp->ptr++;
}



static u8 __attribute__((nonnull(2)))
_flushbuf(int c, fd_t * f)
{
    intptr wr, bs;

    if ((f->flag & (_WRITE|_EOF|_ERR)) != _WRITE)
        return EOF;

    if (f->base == NULL && ((f->flag & _UNBUF) == 0)) {
        if ((f->base = calloc(1, BUFSIZ)) == NULL)
            f->flag |= _UNBUF;
        else {
            f->ptr = f->base;
            f->cnt = BUFSIZ - 1;
        }
    }

    if (f->flag & _UNBUF) {
        f->ptr = f->base = NULL;
        f->cnt = 0;
        if (c == EOF)
            return EOF;
        wr = write(f->fd, &c, 1);
        bs = 1;
    } else {
        if (c != EOF)
            //f->ptr = uc;
            *(f)->ptr++ = c;
        bs = (int) (f->ptr - f->base);
        wr = write(f->fd, f->base, bs);
        f->ptr = f->base;
        f->cnt = BUFSIZ - 1;
    }

    if (wr == bs)
        return c;
    else {
        f->flag |= _ERR;
        return EOF;
    }
}



fd_t * __attribute__((nonnull(1,2)))
fopen(char * const fn, char * const m)
{
    int fd = 1;
    fd_t * fp;

    if (*m != 'r' && *m != 'w' && *m != 'a')
        return NULL;

    for (fp = _iob; fp < _iob + _NFILEMAX; fp++)
        if ((fp->flag & (_READ | _WRITE)) == 0)
            break;  

    if (fp >= _iob + _NFILEMAX)
        return NULL;

    if (*m == 'w')
        fd = creat(fn, 0666);
    else if (*m == 'a') { 
        if ((fd = open(fn, O_WRONLY, 0)) == -1)
            fd = creat(fn, 0666);
        /* lseek(fd, 0L, 2); */
    } else
        fd = open(fn, O_RDONLY, 0);
    if (fd == -1)       
        return NULL;

    fp->fd   = fd;
    fp->cnt  = 0;
    fp->base = NULL;
    fp->flag = (*m == 'r') ? _READ : _WRITE;

    return fp;
}



void __attribute__((nonnull(1)))
fclose(fd_t * f)
{
    int fd;

    fd = f->fd;
    fflush(f); 

    f->cnt = 0;
    f->ptr = NULL;

    if (f->base != NULL)
        free(f->base);

    f->base = NULL;
    f->flag = 0;
    f->fd   = -1;

    return close(fd);
}



intptr 
fflush(fd_t * f)
{
    intptr r;
    
    if (f == NULL) {
        for (u16 i = 0; i < _NFILEMAX; ++i)
            if ((_iob[i].flag & _WRITE) && (fflush(&_iob[i]) == -1))
                r = -1;
    } else {
        if ((f->flag & _WRITE) == 0)
            return -1;

        _flushbuf(EOF, f);

        if (f->flag & _ERR)
            r = -1;
    }

    return r;
}



void
stdfflush(void)
{
    fflush(stdout);
    fflush(stderr);
}



uintptr __attribute__((pure))
strlen(char const * s)
{
    char const * p;
    for (p = s; *p; ++p);
    return p - s;
}



uintptr
strnlen(char const * s, size_t nb)
{
    char const * p;
    for (p = s; *p != '\0' && nb--; ++p);
    return p - s;
}



char *
strdup(char * s)
{
    char * st, * p;
    int l = 0;

    while (s[l]) ++l;

    st = calloc(1,l+1);
    p  = st;

    while (*s) *p++ = *s++;
    *p = '\0';

    return st;
}

#pragma GCC pop_options