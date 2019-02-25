#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
// #include <string.h>

// #include "std.c"
#include "cpts.h"

#define sym char * 



/* ------------------------------------------------------------- */

/*
 * Hashing
 */

#if defined(CPTS_MAP) || defined(CPTS_ALL)

int
hash (int n) 
{
    n *= 0x85ebca6b;
    n ^= n >> 16;
    return n;
}

#endif // CPTS_HASH || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Basic map implementation
 */

#if defined(CPTS_MAP) || defined(CPTS_ALL)

void 
map_free (Map * m)
{
    free(m->k); free(m->v);
    m->k = 0; m->v = 0; m->c = 0; m->l = 0;
}

int 
map_reserve (Map * m, int c) 
{
  c = MAX(16, c);
  Map n = { .l = 0 
            , .c = c 
            , .k = calloc(c, sizeof(int))
            , .v = malloc(c * sizeof(void*)) };
  
//   n.l = 0;
//   n.c = c;
//   n.k = calloc(c, sizeof(int));
//   n.v = malloc(c * sizeof(void*));
  
  if (!n.k || !n.v) { free(n.k); free(n.v); return 0; }

  for (int i = 0 ; i < m->c ; ++i)
      if (m->k[i]) map_set(&n, m->k[i], m->v[i]);

  free(m->k); 
  free(m->v);
  *m = n;

  return 1;
}

int 
map_set (Map * m, int k, void * v) 
{
    if (2*m->l >= m->c - 0) map_reserve(m, 2*m->c);
    if (  m->l >= m->c - 1) return 0;
  
    for (int j = 0, i = hash(k ? k : 1); j < m->c; ++j, ++i) {
        i &= m->c - 1;
        if (!m->k[i]) {
            m->k[i] = k;
            m->v[i] = v;
            ++m->l;
            return 1;
        } else if (m->k[i] == k) {
            if (v) m->v[i] = v;
            else {
                m->k[i] = 0;
                --m->l;
            }
            return 1;
        }
    }
    return 0;
}

void * 
map_get (Map * m, int k) 
{
    if (!m->c) return 0;
  
    for (int i = hash(k ? k : 1); ; ++i) {
        i &= m->c - 1;
        if (m->k[i] == k) return m->v[i];
        else if (!m->k[i]) break;
    }

    return 0;
}


#endif // CPTS_MAP || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Term introduction rules 
 */

#if defined(CPTS_INTRO) || defined(CPTS_ALL)

Term *
var (const char * n)
{
    // return &(struct Term) { &(struct Var) { .v = n } }; 
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Var;
    t->Var.v = n; 
    return t;
}

Term *
app (const Term * lhs , const Term * rhs)
{
    // return &(struct App) { .h = lhs , .b = rhs };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = App;
    t->App.h = lhs; t->App.b = rhs;
    return t;
}

Term *
lam (const char * n , const Term * h , const Term * b)
{
    // return &(struct Term) { &(const struct Lam) { .n = n , .h = h , .t = b } };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Lam;
    t->Lam.n = n; t->Lam.h = h; t->Lam.t = b;
    return t;
}

Term *
pi (const char * n , const Term * e , const Term * b)
{
    // return &(struct Term) { &(struct Pi) { .n = n , .e = e , .b = b } , Box };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Pi;
    t->Pi.n = n; t->Pi.e = e; t->Pi.b = b;
    return t;
}

Term * 
arr (const Term * e , const Term * b) 
{
    return pi("x", e, b);
}

Term *
sg (const char * n , const Term * l , const Term * r)
{
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Sg;
    t->Sg.n = n; t->Sg.l = l; t->Sg.r = r;
    return t;
}

Term * 
sort (const Set s) 
{
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = SO;
    t->SO.s.s = s;
    return t;
}

#endif // CPTS_HELPERS || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * λ-cube 
 */

#if defined(CPTS_LAMBDA_CUBE) || defined(CPTS_ALL)


Set * 
lift (Set s) 
{
    switch (s) { case Star : return (Set *)Box;
                 case Box  : return        NULL; }
}

char * 
show_set (Set * s) 
{
         if ( *s == Star ) return "Star" ;
    else if ( *s ==  Box ) return  "Box" ;
    else                   return "None" ;
}

#define LAMBDA_CUBE(name, ...)\
Set *\
name##_rule (Set s1, Set s2, Set rules[][3])\
{\
    for (int i = 0; i < sizeof(rules)/4 ; ++i) {\
        if (rules[i][0] == s1 && rules[i][1] == s2)\
            return &rules[i][2];\
    }\
    return NULL;\
}

LAMBDA_CUBE(STLC);

#endif // CPTS_LAMBDA_CUBE || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Typecheck
 */

#if defined(CPTS_TYPECHECK) || defined(CPTS_ALL)

Term *
type_check (Term * r, Map * con)
{
    switch (r->Kind) 
    {
        case Var : {
            break;
        }
        default : printf("Default case");
    }
}

#endif // CPTS_TYPECHECK || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Pretty printing
 */

#if defined(CPTS_SHOWCASE) || defined(CPTS_ALL)

const char * 
format (const char * pat, ...)
{
    char b[100]; // TODO : POSSIBLE OVERFLOW, CHANGE TO EXTERNAL CONST 

    va_list v;
    va_start(v, pat);

    vsnprintf(b, sizeof(b), pat, v);

    va_end(v);

    return b;
}

// TODO : refactor 
const char *
show_term (Term * t) 
{
    switch (t->Kind)
    {
        case Var : return format("var %s"         ,            t->Var.v                                                   ) ; break;
        case App : return format("%s $ %s"        , show_term( t->App.h  ) , show_term( t->App.b )                        ) ; break;
        case Lam : return format("\\{%s} %s. %s"  ,            t->Lam.n    , show_term( t->Lam.h ) , show_term( t->Lam.t )) ; break;
        case  Pi : return format("%s ->{%s} %s"   , show_term( t->Pi.e   ) ,            t->Pi.n    , show_term( t->Pi.b  )) ; break;
        case  Sg : return format("Sg{%s} [%s] %s" ,            t->Sg.n     , show_term( t->Sg.l  ) , show_term( t->Sg.r  )) ; break;
        // case  SO : return format("SO[%s]"         , show_set(  t->SO.s.s )                                                ) ; break;
        default  : return "[None]";
    }
}

#endif // CPTS_SHOWCASE || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Entry point
 */

void 
fatal (const char * fn, int l, const char * f, char * e) 
{
    char * ln;
    sprintf(ln, "%d", l);
    write(STDERR_FILENO, fn, SIZE(fn));
    write(STDERR_FILENO, ":", 1);
    write(STDERR_FILENO, ln, SIZE(ln));
    write(STDERR_FILENO, " ", 1);
    write(STDERR_FILENO, e, SIZE(e));
    exit(1);
}

int
main (void) 
{
    Set STLC_rules[2][3] = { { Star , Star , Box }
                           , { Box  , Star , Box } };

    printf("Done\n");

    char * s = show_set(STLC_rule(Box, Star, STLC_rules));
    printf("STLC_rules: %s\n", s ? s : "NULL");

    Term * t = var("b");
    Map * c =  (Map*)calloc(1, sizeof(Map));
    type_check(t, c);

    test_lam_intro();

    return 0;
}



/* ------------------------------------------------------------- */

/*
 * Tests
 */

#if defined(CPTS_TEST) || defined(CPTS_ALL)

void 
test_lam_intro () 
{
    Term * l = var("l");
    Term * r = var("r");
    Term * la = lam ("x", l , NULL);
    // ASSERT(strcmp(la->Lam.h->Var.v, 'l'));
    printf("%s\n", la->Lam.h->Var.v);
}

#endif // CPTS_TEST || CPTS_ALL