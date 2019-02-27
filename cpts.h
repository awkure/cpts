#pragma once 

#define _VERSION "0.0.0.1"

__BEGIN_DECLS

#define CPTS_ALL // Road to nostd

#ifdef  CPTS_MAP
#define CPTS_HASH
#endif 

#if defined(CPTS_HASH) || defined(CPTS_ALL)

#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))

#define __st(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __mba(a) BUILD_BUG_ON_ZERO(__st((void *)&(a), &(a)[0]))

#define __GLUE(a,b) a ## b

#define SIZE(a) (sizeof(a) / sizeof(a[0]) + __mba(a))
#define SIZEA(a) ((int)sizeof(a) / sizeof(a)[0] + __mba(a))
#define MAX(a,b) ({ __auto_type _a = (a); __auto_type _b = (b); (_a > _b) ? _a : _b; })

#define DIE(err) (fatal(__FILE__, __LINE__, __func__, err))
#define ASSERT(e) __ASSERT(e,__LINE__) 
#define __ASSERT(e, line) typedef char __GLUE(assertion_failed_, line) [2*!!(e)-1];

#endif // CPTS_HASH 

// pub trait PureTypeSystem: Clone + Debug {
//     type Sort: Copy + Clone + Debug + Display + Eq + Hash;
//     fn axiom(sort: Self::Sort) -> Option<Self::Sort>;
//     fn rule(s1: Self::Sort, s2: Self::Sort) -> Option<Self::Sort>;
// }

#if defined(CPTS_MAP) || defined(CPTS_ALL)

typedef struct { int   * k
               ; void ** v
               ; int     c 
               ; int     l
               ; } Map;

extern void   map_free    (Map *               );
extern int    map_reserve (Map * , int         );
extern int    map_set     (Map * , int , void *);
extern void * map_get     (Map * , int         );

#endif 

typedef enum { false, true } two;

typedef enum { Star , Box } Set;

typedef struct { void * s ; } Sort; // TODO : DANGER

typedef struct { Set s
               ; Set * (* lift) (const Set)
               ; Set * (* rule) (const Set , const Set) ; } PTS;

enum TKind { Var, App, Lam, Pi, Sg, SO };

typedef struct Term 
{ enum TKind Kind
; union { struct Var { const char        * v                                                 ; } Var
        ; struct App { const struct Term * h ; const struct Term * b                         ; } App
        ; struct Lam { const char        * n ; const struct Term * h ; const struct Term * t ; } Lam
        ; struct  Pi { const char        * n ; const struct Term * e ; const struct Term * b ; }  Pi
        ; struct  Sg { const char        * n ; const struct Term * l ; const struct Term * r ; }  Sg
        ; struct  SO { PTS                 s                                                 ; }  SO ; }; } Term;

extern Term *  var (const char *                              );
extern Term *  app (const Term * , const Term *               );
extern Term *  lam (const char * , const Term * , const Term *);
extern Term *   pi (const char * , const Term * , const Term *);
extern Term *  arr (const Term * , const Term *               );
extern Term *   sg (const char * , const Term * , const Term *);
extern Term * sort (const Set                                 );

extern Term * subst (const Term *, const char * from, const Term *);

extern two alpha_eq (const Term *, const Term *);
extern two  beta_eq (const Term *, const Term *);

extern Term * whnf (Term *);
extern Term *   nf (Term *);

extern void fatal (const char * fn, int l, const char * f, char * e);

#if defined(CPTS_TEST) || defined(CPTS_ALL)

extern void test_lam_intro ();

#endif // CPTS_TEST || CPTS_ALL

__END_DECLS