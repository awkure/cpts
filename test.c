#include "cpts.h"

/* ------------------------------------------------------------- */

/*
 * Test suite
 */

#if defined(CPTS_TEST_INTRO) || defined(CPTS_ALL)

void 
test_var_intro (void)
{
    assert(var("x"));
}

void 
test_lam_intro (void) 
{
    assert(lam("x", var("x"), var("y")));
}

#endif

/* ------------------------------------------------------------- */

/*
 * Hashset tests
 */

#if defined(CPTS_TEST_INTRO) || defined(CPTS_ALL)

void 
test_hashset_stage_1 (void)
{
    char *missing = "missing";
    char *items[] = {"zero", "one", "two", "three", NULL};
    char *foo = "foo";
    size_t ii, nitems = 4;
    Hashset * set = hashset_new();

    if (set == NULL) {
        fprintf(stderr, "failed to create hashset instance\n");
        abort();
    }

    for (ii = 0; ii < nitems; ++ii) {
        hashset_insert(set, items[ii]);
    }

    for (ii = 0; ii < nitems; ++ii) {
        assert(hashset_is_contained(set, items[ii]));
    }

    assert(hashset_is_member(set, missing) == 0);

    assert(hashset_remove(set, items[1]) == 1);
    assert(hashset_num_items(set) == 3);
    assert(hashset_remove(set, items[1]) == 0);

    assert(hashset_add(set, foo) == 1);
    assert(hashset_add(set, foo) == 0);

    hashset_free(set);
}

#endif // CPTS_TEST_HASHSET || CPTS_ALL

/* ------------------------------------------------------------- */

/*
 * Typecheck
 */

#if defined(CPTS_TEST_ALPHA) || defined(CPTS_ALL)

void
test_alpha_eq_var (void)
{
    Term * x = var("x");
    // assert(strcmp(x->Lam.n, var("x")->Lam.n) == 0);
    assert(alpha_eq(x, var("x")));
}

void 
test_alpha_neq_var (void) 
{
    Term * x = var("x");
    // assert(strcmp(x->Lam.n, var("y")->Lam.n) != 0);
    assert(!alpha_eq(x, var("y")));
}

void 
test_alpha_eq_lam () 
{
    Term * l = lam("x", var("y"), var("x"));
    Term * r = lam("a", var("y"), var("a"));
    assert(eq(l, r));
}

void 
test_alpha_eq_app (void) 
{
    Term * l = app(lam("x", arr(var("y"), var("y")), var("x")), lam("x", var("y"), var("x")));
    Term * r = app(lam("a", arr(var("y"), var("y")), var("a")), lam("b", var("y"), var("b")));
    assert(alpha_eq(l,r));
}

void 
test_alpha_eq_pi (void)
{
    Term * l = pi("a", sort(Star), var("a"));
    Term * r = pi("b", sort(Star), var("b"));
    assert(alpha_eq(l, r));
}

void 
test_alpha_eq_sg (void)
{
    Term * l = sg("a", sort(Star), var("a"));
    Term * r = sg("b", sort(Star), var("b"));
    assert(alpha_eq(l, r));
}

#endif // CPTS_TEST_ALPHA || CPTS_ALL

/* ------------------------------------------------------------- */

/*
 * Run all tests
 */

#if defined(CPTS_TEST) || defined(CPTS_ALL)

void 
test (void)
{
    #if defined(CPTS_TEST_HASHSET) || defined(CPTS_ALL)
        test_hashset_stage_1();
    #endif // CPTS_TEST_HASHSET || CPTS_ALL

    #if defined(CPTS_TEST_INTRO) || defined(CPTS_ALL)
        test_lam_intro();    
    #endif // CPTS_TEST_INTRO || CPTS_ALL

    #if defined(CPTS_TEST_ALPHA) || defined(CPTS_ALL)
        test_alpha_eq_var();
        test_alpha_neq_var();
        test_alpha_eq_app();
        test_alpha_eq_pi();
        test_alpha_eq_sg();
    #endif // CPTS_TEST_ALPHA | CPTS_ALL
}

#endif // CPTS_TEST || CPTS_ALL