#include "tests.h"
#include "texture.h"

START_TEST (test_texture_ref)
{
	struct hyd_tex *t1 = hyd_tex_create("t1");
	ck_assert_int_eq(*t1->ref_count, 0);
	struct hyd_tex *ref1 = hyd_tex_copy(t1);
	ck_assert_int_eq(*t1->ref_count, 1);
	struct hyd_tex *ref2 = hyd_tex_copy(t1);
	ck_assert_int_eq(*t1->ref_count, 2);

	ck_assert_str_eq(ref1->name, "t1");
	ck_assert_str_eq(ref2->name, "t1");
	ck_assert_int_eq(*ref1->ref_count, 2);
	ck_assert_int_eq(*ref2->ref_count, 2);

	hyd_tex_destroy(ref1);
	ck_assert_int_eq(*t1->ref_count, 1);
	hyd_tex_destroy(ref2);
	ck_assert_int_eq(*t1->ref_count, 0);
	
	hyd_tex_destroy(t1);
}
END_TEST

Suite *texture_suite(void)
{
	Suite *s = suite_create("Texture");

	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_texture_ref);

	suite_add_tcase(s, tc_core);

	return s;
}
