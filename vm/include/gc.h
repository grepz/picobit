#ifndef PICOBIT_GC_H
#define PICOBIT_GC_H

/* TODO explain what each tag means, with 1-2 mark bits.
   Currently, they're described in IFL paper. */
#define GC_TAG_0_LEFT   (1<<5)
#define GC_TAG_1_LEFT   (2<<5)
#define GC_TAG_UNMARKED (0<<5)

/* Number of object fields of objects in ram */
#ifdef LESS_MACROS
uint8 HAS_2_OBJECT_FIELDS(uint16 visit)
{
	return (RAM_PAIR_P(visit) || RAM_CONTINUATION_P(visit));
}

#ifdef CONFIG_BIGNUM_LONG
uint8 HAS_1_OBJECT_FIELD(uint16 visit)
{
	return (RAM_COMPOSITE_P(visit) || RAM_CLOSURE_P(visit) || RAM_BIGNUM_P(visit));
}
#else
uint8 HAS_1_OBJECT_FIELD(uint16 visit)
{
	return (RAM_COMPOSITE_P(visit) || RAM_CLOSURE_P(visit));
}
#endif

#else
#define HAS_2_OBJECT_FIELDS(visit) (RAM_PAIR_P(visit) || RAM_CONTINUATION_P(visit))
#ifdef CONFIG_BIGNUM_LONG
#define HAS_1_OBJECT_FIELD(visit)  (RAM_COMPOSITE_P(visit) \
				    || RAM_CLOSURE_P(visit) || RAM_BIGNUM_P(visit))
#else
#define HAS_1_OBJECT_FIELD(visit)  (RAM_COMPOSITE_P(visit) || RAM_CLOSURE_P(visit))
#endif
#endif
// all composites except pairs and continuations have 1 object field

/* List of unused cells. */
obj free_list;

/*
 * First unused cell in vector space.
 * It points into vector space using whole-RAM addressing;
 * its value should be over MAX_RAM_ENCODING.
 */
obj free_vec_pointer;

#ifdef CONFIG_GC_DEBUG
extern int max_live;
#endif

#endif
