#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cpts.h"

/* ------------------------------------------------------------- */

/*
 * Test suite
 */

#if defined(CPTS_TEST) || defined(CPTS_ALL)

void 
stage_1 (void)
{
    test_hashset_stage_1();

    test_lam_intro();    
}

void 
test_lam_intro () 
{
    Term *  l = var("l");
    Term *  r = var("r");
    Term * la = lam ("x", l , NULL);
    // ASSERT(strcmp(la->Lam.h->Var.v, 'l'));
    printf("%s\n", la->Lam.h->Var.v);
}

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
    // assert(hashset_is_member(set, missing) == 0);

    // assert(hashset_remove(set, items[1]) == 1);
    // assert(hashset_num_items(set) == 3);
    // assert(hashset_remove(set, items[1]) == 0);

    // assert(hashset_add(set, foo) == 1);
    // assert(hashset_add(set, foo) == 0);

    hashset_free(set);
}

#endif // CPTS_TEST || CPTS_ALL