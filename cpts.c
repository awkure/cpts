#pragma GCC push_options

/* ------------------------------------------------------------- */

/*
 * Utils
 */

#if defined(CPTS_UTILS) || defined(CPTS_ALL)


int 
string_append (char ** from, const char * fmt, ...)
{
    char * r = NULL;
    char * b = NULL;

    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vsprintf(r, fmt, arg_ptr);

    sprintf(b, "%s", (*from == NULL ? "\0" : *from));

    char * n = (char *)calloc(strlen(b) + strlen(r) + 1, sizeof(char));

    strcat(n, b);
    strcat(n, r);

    if (*from) free(*from);
    *from = n;

    free(b);
    free(r);

    return 0;
}

inline void **
tail (void * l[])
{
    size_t s = sizeof(l[0]);
    free(l[0]);
    return l+s;
}

#endif // CPTS_UTILS || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Hashing
 */

#if defined(CPTS_MAP) || defined(CPTS_ALL)

size_t 
hash_fnv32 (void * d, size_t l) 
{
    char * p = (char *) d;
    int r = p4;
    
    for (int i = 0; i < l; ++i) {
        r ^= p[i];
        r *= p5;
        r ^= r >> 16;
    }
  
    return r;
}

inline size_t 
hash (size_t n) 
{
    return (n * p1) ^ ((n * p2) >> 16);
}

#endif // CPTS_HASH || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Basic map implementation
 */

#if defined(CPTS_MAP) || defined(CPTS_ALL)

inline void 
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
    Map * n = calloc(c, sizeof(Map)); 
    n->k = calloc(c, sizeof(size_t));
    n->v = malloc(c * sizeof(size_t));
    n->c = c;
    n->l = 0;
    // { .l = 0 
    //       , .c = c 
    //       , .k = calloc(c, sizeof(int))
    //       , .v = malloc(c * sizeof(void*)) };


  
    if (n->k == NULL || n->v == NULL) { free(n->k); free(n->v); return 0; }

    for (int i = 0 ; i < m->c ; ++i)
        if (m->k[i]) map_set(n, m->k[i], (void*) m->v[i]);

    free(m->k); 
    free(m->v);
    m = n;

    return 1;
}

int 
map_set (Map * m, int k, const void const * v) 
{
    if (2*m->l >= m->c - 0) map_reserve(m, 2*m->c);
    if (  m->l >= m->c - 1) return 0;
  
    size_t vv = (size_t) v;

    for (int j = 0, i = hash(k ? k : 1); j < m->c; ++j, ++i) {
        i &= m->c - 1;
        if (!m->k[i]) {
            m->k[i] = k;
            m->v[i] = vv;
            ++m->l;
            return 1;
        } else if (m->k[i] == k) {
            if (vv) m->v[i] = vv;
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
map_get (Map * m, const char * const s) 
{
    if (!m->c) return 0;
  
    int k = hash_fnv32((char *) s, SIZE(s));
    for (int i = k ? k : 1; ; ++i) {
        i &= m->c - 1;
        if (m->k[i] == k) return (void *) m->v[i];
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

    return s;
}

inline void 
hashset_free (Hashset * s)
{
    if (s) free(s->i);
    free(s);
}

int 
hashset_append (Hashset * s, void * v) 
{
    size_t vv = (size_t) v;
    if (vv == 0 || vv == 1) return -1;

    size_t idx = s->m & hash(vv); // (p1 * v);

    while (s->i[idx] != 0 && s->i[idx] != 1) 
        if (s->i[idx] == vv) return 0;
        else idx = s->m & (idx + p2);

    s->l++;
    if (s->i[idx] == 1) s->d--;
    s->i[idx] = vv;

    return 1;
}

// TODO : reimplement
void * 
hashset_get (Hashset * s, void * i) 
{
    size_t vv = (size_t) i;
    if (vv == 0 || vv == 1) return NULL;

    size_t idx = s->m & hash(vv);

    while (s->i[idx] != 0 && s->i[idx] != 1)
        if (s->i[idx] == vv) return (void*) vv;

    return NULL;

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

int // TODO 
hashset_union (Hashset * t, Hashset * s)
{
    
    if (t == NULL || t->i == NULL
    ||  s == NULL || s->i == NULL ) return -1;

    // size_t idx = s->m & hash(v); // (p1 * v);

    // while (s->i[idx] != 0 && s->i[idx] != 1) 
    //     if (s->i[idx] == v) return 0;
    //     else idx = s->m & (idx + p2);

    // s->l++;
    // if (s->i[idx] == 1) s->d--;
    // s->i[idx] = v;

    // return 1;
    int r;

    reharsh(t);

    return r;
}

int 
hashset_remove (Hashset * s, void * v)
{
    size_t idx = s->m & hash((size_t)v);

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
hashset_is_contained (Hashset * s, void * v) 
{
    size_t idx = s->m & hash((size_t)v);

    while (s->i[idx] != 0) 
        if (s->i[idx] == (size_t)v) return 1;
        else idx = s->m & (idx + p2);

    return 0;
}

#endif // CPTS_SET || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Term introduction rules 
 */

#if defined(CPTS_INTRO) || defined(CPTS_ALL)

Term * __attribute__((const))
var (const char * n)
{
    // return &(struct Term) { &(struct Var) { .v = n } }; 
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Var;
    t->Var.v = n; 
    return t;
}

Term * __attribute__((const))
app (const Term * lhs , const Term * rhs)
{
    // return &(struct App) { .h = lhs , .b = rhs };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = App;
    t->App.h = lhs; t->App.b = rhs;
    return t;
}

Term * __attribute__((const))
lam (const char * n , const Term * h , const Term * b)
{
    // return &(struct Term) { &(const struct Lam) { .n = n , .h = h , .t = b } };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Lam;
    t->Lam.n = n; t->Lam.h = h; t->Lam.t = b;
    return t;
}

Term * __attribute__((const))
pi (const char * n , const Term * e , const Term * b)
{
    // return &(struct Term) { &(struct Pi) { .n = n , .e = e , .b = b } , Box };
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Pi;
    t->Pi.n = n; t->Pi.e = e; t->Pi.b = b;
    return t;
}

Term * __attribute__((const))
arr (const Term * e , const Term * b) 
{
    return pi("x", e, b);
}

Term * __attribute__((const))
sg (const char * n , const Term * l , const Term * r)
{
    Term * t = (Term*)calloc(1, sizeof(Term));
    t->Kind = Sg;
    t->Sg.n = n; t->Sg.l = l; t->Sg.r = r;
    return t;
}

Term * __attribute__((const))
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


Set * __attribute__((pure))
lift (Set s) 
{
    switch (s) { case Star : return (Set *)Box;
                 case Box  : return        NULL; }
}

char * 
show_set (Set s) 
{
         if ( s == Star ) return "Star" ;
    else if ( s ==  Box ) return  "Box" ;
    else                  return "None" ;
}

// for (int i = 0; i < sizeof(rules)/4 ; ++i) { // TODO : possible bug in sizeof 
#define LAMBDA_CUBE(name, ...)\
Set *\
name##_rule (Set s1, Set s2, Rules rules)\
{\
    for (int i = 0; i < rules.s/4; ++i)\
        if (rules.r[i][0] == s1 && rules.r[i][1] == s2)\
            return &rules.r[i][2];\
    return NULL;\
}

LAMBDA_CUBE(STLC);

#endif // CPTS_LAMBDA_CUBE || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Typecheck
 */

#if defined(CPTS_TYPECHECK) || defined(CPTS_ALL)

const Term * __attribute__((hot))
type_check (const Term * t, Map * con, PTS * ts)
{
    switch (t->Kind) 
    {
        case Var : { 
            const Term * v = map_get(con, t->Var.v);
            
            if (v != NULL) DIE(format("Unbound variable %s", t->Var.v));

            return v;
        }

        case App : {
            const Term * l = whnf(type_check(t->App.h, con, ts));

            if (l->Kind != Pi) DIE(format("Expression of type %s cannot be applied", show_term(l)));

            const Term * r = type_check(t->App.b, con, ts);

            if (!beta_eq(r, l->Pi.e)) DIE(format("%s cannot be substituted with %s", show_term(l->Pi.e), show_term(r)));

            return subst(l->Pi.b, l->Pi.n, r);
        }

        case Lam : {
            type_check(t->Lam.h, con, ts);

            Map * nc = con;
            map_set(nc, hash_fnv32((char *) t->Lam.n, SIZE(t->Lam.n)), (Term *) t->Lam.h);

            const Term * i = type_check(t->Lam.t, nc, ts);

            return pi(t->Lam.n, t->Lam.h, i);
        }

        case Pi : {
            const Term * dom = whnf(type_check(t->Pi.e, con, ts));
            if (dom->Kind != SO) DIE(format("Uninhabited domain of pi type provided: %s", show_term(dom)));

            Map * nc = con;
            map_set(con, hash_fnv32((char *) t->Pi.n, SIZE(t->Pi.n)), dom); 

            const Term const * codom = whnf(type_check(t->Pi.b, con, ts));
            if (codom->Kind != SO) DIE(format("Uninhabited codomain of pi type provided: %s", show_term(codom)));

            Set * ns = ts->rule(dom->SO.s.s, codom->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", t->SO.s.s));

            return sort(*ns);
        }

        case Sg : {
            const Term const * dom = whnf(type_check(t->Sg.l, con, ts));
            if (dom->Kind != SO) DIE(format("Uninhabited domain of pi type provided: %s", show_term(dom))); 

            Map * nc = con;
            map_set(con, hash_fnv32((char *) t->Sg.n, SIZE(t->Sg.n)), dom); 

            const Term const * codom = whnf(type_check(t->Sg.r, con, ts));
            if (codom->Kind != SO) DIE(format("Uninhabited codomain of pi type provided: %s", show_term(codom)));

            Set * ns = ts->rule(dom->SO.s.s, codom->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", t->SO.s.s));

            return sort(*ns);
        }

        case SO : {
            Set * ns = ts->lift(t->SO.s.s);
            if (ns == NULL) DIE(format("Reached omega at %s", (char *) t->SO.s.s)); 
            else return sort(*ns);
        }
    }
}

const Term * __attribute__((hot))
subst (const Term * t, const char * from, const Term * to)
{
    switch (t->Kind) 
    {
        case Var : return strcmp(t->Var.v, from) ? t : to;
        
        case App : return app(t->App.h, subst(t->App.b, from, to));
        
        case Lam : {
            if (strcmp(t->Lam.n, from)) 
                return lam(t->Lam.n, subst(t->Lam.h, from, to), t->Lam.t);
            
            if (hashset_get(free_vars(to), (void *) t->Lam.n) == NULL) 
                return lam(t->Lam.n, subst(t->Lam.h, from, to), subst(t->Lam.t, from, to));

            char * unused = (char *) t->Lam.n;
            string_append(&unused, "'");

            for (;;) { 
                Hashset * used = free_vars(t->Lam.t);
                hashset_union(used, free_vars(to));
                
                if (hashset_is_contained(used, unused)) string_append(&unused, "'");
                else {
                    Term * r = lam(unused, subst(t, t->Lam.n, var(unused)), subst(t->Lam.h, t->Lam.n, var(unused)));
                    return subst(r, from, to);
                }
            }
        }

        case  Pi : {
            if (strcmp(t->Pi.n, from) != 0) 
                return pi(t->Pi.n, subst(t->Pi.e, from, to), t->Pi.b);

            if (!hashset_is_contained(free_vars(to), (void *) t->Pi.n))
                return pi(t->Pi.n, subst(t->Pi.e, from, to), subst(t->Pi.b, from, to));

            char * unused = (char *) t->Pi.n;
            string_append(&unused, "'");

            for (;;) {
                Hashset * used = free_vars(t->Pi.b);
                hashset_union(used, free_vars(to));

                if (hashset_is_contained(used, unused)) string_append(&unused, "'");
                else {
                    Term * r = pi(unused, subst(t, t->Pi.n, var(unused)), subst(t->Pi.e, t->Pi.n, var(unused)));
                    return subst(r, from, to);
                }
            }
        }
        
        case  Sg : {
            if (strcmp(t->Pi.n, from) != 0) 
                return pi(t->Pi.n, subst(t->Pi.e, from, to), t->Pi.b);

            if (!hashset_is_contained(free_vars(to), (void *) t->Pi.n))
                return pi(t->Pi.n, subst(t->Pi.e, from, to), subst(t->Pi.b, from, to));

            char * unused = (char *) t->Pi.n; 
            string_append(&unused, "'");

            for (;;) {
                Hashset * used = free_vars(t->Pi.b);
                hashset_union(used, free_vars(to));

                if (hashset_is_contained(used, unused)) string_append(&unused, "'");
                else {
                    Term * r = pi(unused, subst(t, t->Pi.n, var(unused)), subst(t->Pi.e, t->Pi.n, var(unused)));
                    return subst(r, from, to);
                }
            }
        }

        case  SO : return t;
    }

}

Hashset * __attribute__((hot))
free_vars (const Term * t)
{
    Hashset * s = NULL;

    switch (t->Kind) {

        case Var : hashset_insert(s, (void *) t->Var.v);

        case App : { 
            hashset_union(s, free_vars(t->App.h));
            hashset_union(s, free_vars(t->App.b));
            break;
        }

        case Lam : {
            Hashset * b = free_vars(t->Lam.t);
            hashset_remove(s, (void *) t->Lam.n);
            hashset_union(s, b);
            hashset_union(s, free_vars(t->Lam.h));
            break;
        }

        case  Pi : {
            Hashset * b = free_vars(t->Pi.b);
            hashset_remove(s, (void *) t->Pi.n);
            hashset_union(s, b);
            hashset_union(s, free_vars(t->Pi.e));
            break;
        }

        case  Sg : {
            Hashset * b = free_vars(t->Sg.r);
            hashset_remove(s, (void *) t->Sg.n);
            hashset_union(s, b);
            hashset_union(s, free_vars(t->Sg.l));
            break;
        }

        case  SO : break;
    }

    return s;
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
        case Var : return (b->Kind == Var) ? strcmp(a->Var.v, b->Var.v) == 0                              : false ;
        case App : return (b->Kind == App) ? alpha_eq(a->App.h, b->App.h) && alpha_eq(a->App.b, b->App.b) : false ;
        case Lam : return (b->Kind == Lam) ? strcmp(a->Lam.n, b->Lam.n) == 0 
                                          && alpha_eq(a->Lam.h, b->Lam.h) 
                                          && alpha_eq(a->Lam.t, subst(b->Lam.t, a->Lam.n, a->Lam.t)) : false; // TODO : possible bug 
        case  Pi : return (b->Kind ==  Pi)                                                                        ; 
        case  SO : return (b->Kind ==  SO) ? a->SO.s.s == b->SO.s.s                                       : false ;
        default  : return                                                                                   false ;
    }
}

inline two
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

const Term * __attribute__((hot))
fold (Term * s[], const Term * t, Term * (*f)(const Term *, const Term *))
{
    return s == NULL ? t : f(s[0], fold((Term **) tail((void **)s), t, f));
}

inline const Term * 
whnf (const Term * t)
{   
    Term * s[] = {0}; // malloc(sizeof(Term)*sizeof(Term)+1);   
    return spine_whnf(t, s);
}

const Term * __attribute__((hot))
spine_whnf (const Term * t, Term * s[]) 
{
    switch (t->Kind) {
        case App : {
            Term ** u = s;
            u[SIZE(*s)+1] = (Term *) t->App.b; 
            return spine_whnf(t->App.h, u);
        }

        case Lam : {
            if (s != NULL) {
                Term ** u = s;
                Term  * b = s[SIZE(*s)];
                free(s[SIZE(*s)]);
                return spine_whnf(subst(t->Lam.t, t->Lam.n, b), u);
            }
        }

        default : return fold(s, t, app); 
    }
}

const Term * __attribute__((__gnu_inline__))
nf (const Term * t)
{
    Term * s[] = {0};
    return spine_nf(t, s);
}

const Term * __attribute__((hot)) 
spine_nf (const Term * t, Term * s[]) 
{
    switch (t->Kind) {
        case App : {
            Term ** u = s;
            u[SIZE(*s)+1] = (Term *) t->App.b;
            return spine_whnf(t->App.h, u);
        }

        case Lam : {
            if (s == NULL) return lam(t->Lam.n, nf(t->Lam.h), nf(t->Lam.t));
            else {
                Term ** u = s;
                Term  * b = s[SIZE(*s)];
                free(s[SIZE(*s)]);
                return spine_whnf(subst(t->Lam.t, t->Lam.n, b), u);
            }
        }

        case  Pi : return fold(s, pi(t->Pi.n, nf(t->Pi.e), nf(t->Pi.b)), app); // TODO : \ e b -> app(e, nf(b))
    
        default : return fold(s, t, app); // TODO : \ e b -> app(e, nf(b))
    }
}

#endif // CPTS_NORMALISATION || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Pretty printing
 */

#if defined(CPTS_SHOWCASE) || defined(CPTS_ALL)

char * __attribute__((nonnull (1)))
format (const char * pat, ...)
{
    char * b = calloc(MAX_FORMAT_SIZE+1, sizeof(char));

    va_list v;
    va_start(v, pat);

    vsprintf(b, pat, v);

    va_end(v);

    return b;
}

const char *
show_term (const Term * t) 
{
    switch (t->Kind)
    {
        case Var : return format("var %s"         ,            t->Var.v                                                   ) ; break ;
        case App : return format("%s $ %s"        , show_term( t->App.h  ) , show_term( t->App.b )                        ) ; break ;
        case Lam : return format("\\{%s} %s. %s"  ,            t->Lam.n    , show_term( t->Lam.h ) , show_term( t->Lam.t )) ; break ;
        case  Pi : return format("%s ->{%s} %s"   , show_term( t->Pi.e   ) ,            t->Pi.n    , show_term( t->Pi.b  )) ; break ;
        case  Sg : return format("Sg{%s} [%s] %s" ,            t->Sg.n     , show_term( t->Sg.l  ) , show_term( t->Sg.r  )) ; break ;
        case  SO : return format("SO[%s]"         , show_set(  t->SO.s.s )                                                ) ; break ;
        default  : return "[None]";
    }
}

#endif // CPTS_SHOWCASE || CPTS_ALL



/* ------------------------------------------------------------- */

/*
 * Entry point
 */

void __attribute__((nonnull (1,3,4)))
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
    test();
    return 0;
}

#pragma GCC pop_options