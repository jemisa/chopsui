#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "css.h"
#include "parser.h"
#include "util/list.h"
#include "util/unicode.h"
#include "util/errors.h"
#include "parse/parse.h"

stylesheet_t *css_parse(const char *source, errors_t **errs) {
	stylesheet_t *css = calloc(1, sizeof(stylesheet_t));
	css->rules = list_create();
	css->media_rules = list_create();
	css->keyframes = list_create();

	struct parser_state state = { 0 };
	state.errs = errs;
	state.data = css;

	while (*source) {
		uint32_t ch = utf8_decode(&source);
		css_parse_ch(css, &state, ch);
	}

	parser_cleanup(&state);

	return css;
}
