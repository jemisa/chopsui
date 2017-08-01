#include <string.h>
#include <assert.h>
#include "test.h"
#include "node.h"
#include "sui.h"

char *test_name = "sui/sui_parse";

static int test_type() {
	errors_t *errs = NULL;
	sui_node_t *node = sui_parse("test", &errs);
	assert(!errs);
	assert(node && strcmp(node->type, "test") == 0);
	node_free(node);
	return 0;
}

static int test_class() {
	errors_t *errs = NULL;
	sui_node_t *node = sui_parse("test .foo", &errs);
	assert(!errs);
	assert(node && strcmp(node->type, "test") == 0);
	assert(node_has_class(node, "foo"));
	node_free(node);

	node = sui_parse("test .foo.bar .baz", NULL);
	assert(node && strcmp(node->type, "test") == 0);
	assert(node_has_class(node, "foo"));
	assert(node_has_class(node, "bar"));
	assert(node_has_class(node, "baz"));
	node_free(node);
	return 0;
}

static int test_id() {
	errors_t *errs = NULL;
	sui_node_t *node = sui_parse("test @foo", &errs);
	assert(!errs);
	assert(node && strcmp(node->type, "test") == 0);
	assert(strcmp(node->id, "foo") == 0);
	node_free(node);

	node = sui_parse("test @foo @bar", &errs);
	assert(errs);
	assert(errs->length == 1);
	assert(0 == strcmp(errs->items[0],
				"[1:15] Node cannot have two IDs"));
	node_free(node);
	errors_free(errs);
	return 0;
}

static int test_children() {
	errors_t *errs = NULL;
	sui_node_t *root = sui_parse(
			"test\n"
			"\tfoo\n"
			"\tbar\n"
			"\t\tbaz\n"
			"\t\t\tzab\n"
			"\trab", &errs);
	assert(!errs);

	assert(root && strcmp(root->type, "test") == 0);
	assert(root->children->length == 3);

	sui_node_t *n = root->children->items[0];
	assert(n && strcmp(n->type, "foo") == 0);

	n = root->children->items[1];
	assert(n && strcmp(n->type, "bar") == 0);
	assert(n->children->length == 1);

	n = n->children->items[0];
	assert(n && strcmp(n->type, "baz") == 0);
	assert(n->children->length == 1);

	n = n->children->items[0];
	assert(n && strcmp(n->type, "zab") == 0);

	n = root->children->items[2];
	assert(n && strcmp(n->type, "rab") == 0);

	node_free(root);
	return 0;
}

static int test_indentation_errors() {
	errors_t *errs = NULL;
	sui_node_t *root = sui_parse(
			"test\n"
			"  foo\n"
			"   bar", &errs);
	assert(errs);
	assert(errs->length == 1);
	assert(0 == strcmp(errs->items[0],
			"[3:4] Inconsistent indentation width is not permitted"));
	errors_free(errs);
	node_free(root);

	errs = NULL;
	root = sui_parse(
			"test\n"
			"\tfoo\n"
			"\t\t\tbar", &errs);
	assert(errs->length == 1);
	assert(0 == strcmp(errs->items[0],
			"[3:4] Multiple indents where one was expected"));
	errors_free(errs);
	node_free(root);

	return 0;
}

int test_main() {
	return test_type()
		|| test_class()
		|| test_id()
		|| test_children()
		|| test_indentation_errors()
	;
}