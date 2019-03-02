#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
// #include <string.h>

// #include "std.c"
#include "cpts.h"
#include "test.c"

/* ------------------------------------------------------------- */

/*
 * Hashing
 */

#if defined(CPTS_MAP) || defined(CPTS_ALL)

size_t
hash (size_t n) 
{
    // n *= 0xdeadbeef;
    // n ^= n >> 16;
    return (n * p1) >> 16;
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
    free(m->k); 
    free(m->v);
    free(m);
}

int 
map_reserve (Map * m, int c) 
{
  c = MAX(16, c);
  Map n = { .l = 0 
          , .c = c 
          , .k = calloc(c, sizeof(int))
          , .v = malloc(c * sizeof(void*)) };
  
  if (n.k == NULL || n.v == NULL) { free(n.k); free(n.v); return 0; }

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
 * Set implementation
 */

#if defined(CPTS_SET) || defined(CPTS_ALL)

Hashset * 
hashset_new (void)
{
    Hashset * s = calloc(1, sizeof(Hashset));
    if (s == NULL) return NULL;

    s->b = 3;
    s->c = (size_t)(1 << s->b);
    s->m = s->c - 1;
    s->i = calloc(s->c, sizeof(size_t));
    s->l = 0;
    s->d = 0;

    // Hashset s = { .b = 3
    //             , .c = (size_t)(1<<s.b) 
    //             , .m = s.c - 1 
    //             , .i = calloc(s.c, sizeof(size_t)) 
    //             , .l = 0
    //             , .d = 0 };

    // if (s.i == NULL) { hashset_free(&s); return NULL; }

    // return &s;
    return s;
}

void 
hashset_free (Hashset * s)
{
    if (s) free(s->i);
    free(s);
}

int 
hashset_append (Hashset * s, void * i) 
{
    size_t v = (size_t)i;
    if (v == 0 || v == 1) return -1;

    size_t idx = s->m & hash(v); // (p1 * v);

    while (s->i[idx] != 0 && s->i[idx] != 1) 
        if (s->i[idx] == v) return 0;
        else idx = s->m & (idx + p2);

    s->l++;
    if (s->i[idx] == 1) s->d--;
    s->i[idx] = v;

    return 1;
}

static void 
reharsh (Hashset * s) 
{
    if (s->l + s->d >= (double)s->c * 0.85) {
        size_t * bi = s->i;
        size_t   bc = s->c; 
        s->b++;
        s->c = (size_t)(1 << s->b);
        s->m = s->c - 1;
        s->i = calloc(s->c, sizeof(size_t));
        s->l = 0;
        s->d = 0;
        for (size_t idx = 0; idx < bc; ++idx) hashset_append(s, (void *)bi[idx]);
        free(bi);
    }
}

int 
hashset_insert (Hashset * s, void * i)
{
    int r = hashset_append(s, i);
    reharsh(s);
    return r;
}

int 
hashset_remove (Hashset * s, void * i)
{
    size_t v = (size_t)i;
    size_t idx = s->m & hash(v);

    while (s->i[idx] != 0) 
        if (s->i[idx] != 0) {
            s->i[idx] = 1;
            s->l--;
            s->d++;
            return 1;
        } else idx = s->m & (idx + p2);

    return 0;
}

int 
hashset_is_contained (Hashset * s, void * i) 
{
    size_t v = (size_t)i;
    size_t idx = s->m & hash(v);

    while (s->i[idx] != 0) 
        if (s->i[idx] == v) return 1;
        else idx = s->m & (idx + p2);

    return 0;
}

#endif // CPTS_SET || CPTS_ALL



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
type_check (Term * t, Map * con, PTS * ts)
{
    switch (t->Kind) 
    {
        case Var : { 
            Term * v = map_get(con, &t->Var.v);
            
            if (v != NULL) DIE(format("Unbound variable %s", t->Var.v));

            return v;
        }

        case App : {
            Term * l = whnf(type_check(t->App.h, con, ts));

            if (l->Kind != Pi) DIE(format("Expression of type %s cannot be applied", l)); // TODO : print bug 

            Term * r = type_check(t->App.b, con, ts);

            if (!beta_eq(r, l->Pi.e)) DIE(format("%s cannot be substituted with %s", l->Pi.e, r)); // TODO : print bug 

            return subst(l->Pi.b, l->Pi.n, r);
        }

        case Lam : {
            type_check(t->Lam.h, con, ts);

            Map * nc = con;
            map_set(nc, hash(t->Lam.n), t->Lam.h);

            Term * i = type_check(t->Lam.t, nc, ts);

            return pi(t->Lam.n, t->Lam.h, i);
        }

        case Pi : {
            Term * dom = whnf(type_check(t->Pi.e, con, ts));
            if (dom->Kind != SO) DIE(format("Uninhabited domain of pi type provided: %s", dom)); // TODO : print

            Map * nc = con;
            map_set(con, hash(t->Pi.n), dom); 

            Term * codom = whnf(type_check(t->Pi.b, con, ts));
            if (codom->Kind != SO) DIE(format("Uninhabited codomain of pi type provided: %s", codom)); // TODO : coprint

            Set * ns = ts->rule(dom->SO.s.s, codom->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", t->SO.s.s));

            return ns;
        }

        case Sg : {
            Term * dom = whnf(type_check(t->Sg.l, con, ts));
            if (dom->Kind != SO) DIE(format("Uninhabited domain of pi type provided: %s", dom)); // TODO : print

            Map * nc = con;
            map_set(con, hash(t->Sg.n), dom); 

            Term * codom = whnf(type_check(t->Sg.r, con, ts));
            if (codom->Kind != SO) DIE(format("Uninhabited codomain of pi type provided: %s", codom)); // TODO : coprint

            Set * ns = ts->rule(dom->SO.s.s, codom->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", t->SO.s.s));

            return ns;
        }

        case SO : {
            Set * ns = ts->lift(t->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", t->SO.s.s)); else return ns;
        }
    }
}

Term * 
subst (const Term * t, const char * from, const Term * to)
{
    switch (t->Kind) 
    {
        case Var : return (t->Var.v == from) ? to : t; // TODO : string equality here 
        
        case App : return app(t->App.h, subst(t->App.b, from, to));
        
        case Lam : {
            if (strcmp(t->Lam.n, from)) 
                return lam(t->Lam.n, subst(t->Lam.h, from, to), t->Lam.t);
            
            if (map_get(free_vars(to), t->Lam.n) == NULL) 
                return lam(t->Lam.n, subst(t->Lam.h, from, to), subst(t->Lam.t, from, to));

            const char * unused = t->Lam.n;

            // for (;;) { // TODO 
            // }

        }

        case  Pi : return NULL;
        case  Sg : return NULL;
        case  SO : return NULL;
    }

}

Map * 
free_vars (const Term * t)
{
    return NULL;
}

#endif // CPTS_TYPECHECK || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Equality
 */

#if defined(CPTS_EQUALITY) || defined(CPTS_ALL)

two 
alpha_eq (const Term * a, const Term * b) 
{
    // switch ((a->Kind, b->Kind)) 
    // {
    //     (Pi,Lam) : return false;
    //     default : return false;
    // }
    switch (a->Kind) 
    {
        case Var : switch (b->Kind) { case Var : return a->Var.v == b->Var.v 
                                    ;  default : return false; }
        case App : switch (b->Kind) { case App : return alpha_eq(a->App.h, b->App.h) && alpha_eq(a->App.b, b->App.b)
                                    ;  default : return false; }
        case Lam : switch (b->Kind) { case Lam : return true // TODO 
                                    ;  default : return false; }
        case  Pi : switch (b->Kind) { case  Pi : return true // TODO 
                                    ;  default : return false; }
        case  SO : switch (b->Kind) { case  SO : return a->SO.s.s == b->SO.s.s 
                                    ;  default : return false; }
         default : return false;
    }
}

two 
beta_eq (const Term * a, const Term * b) 
{
    return alpha_eq(nf(a), nf(b));
}


#endif // CPTS_EQUALITY || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Normalisation
 */

#if defined(CPTS_NORMALISATION) || defined(CPTS_ALL)

Term * 
whnf (Term * t)
{
    return NULL;
}

Term * 
spine (Term * t, Term ** s) 
{
    return NULL;
}

Term * 
nf (Term * t)
{
    return NULL;
}

#endif // CPTS_NORMALISATION || CPTS_ALL



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
        case Var : return format("var %s"         ,            t->Var.v                                                   ) ; break ;
        case App : return format("%s $ %s"        , show_term( t->App.h  ) , show_term( t->App.b )                        ) ; break ;
        case Lam : return format("\\{%s} %s. %s"  ,            t->Lam.n    , show_term( t->Lam.h ) , show_term( t->Lam.t )) ; break ;
        case  Pi : return format("%s ->{%s} %s"   , show_term( t->Pi.e   ) ,            t->Pi.n    , show_term( t->Pi.b  )) ; break ;
        case  Sg : return format("Sg{%s} [%s] %s" ,            t->Sg.n     , show_term( t->Sg.l  ) , show_term( t->Sg.r  )) ; break ;
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
    Map  * c = (Map*)calloc(1, sizeof(Map));
    type_check(t, c, NULL); // TODO : BUG HERE 

    test_lam_intro();
    stage_1();

    return 0;
}