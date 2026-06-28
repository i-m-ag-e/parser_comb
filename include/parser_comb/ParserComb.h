/**
 * @defgroup primitives Core Primitives
 * @brief Core primitives for parsing, including character and string parsers.
 *
 * @defgroup combinators Combinators
 * @brief Combinators for combining parsers (e.g., sequencing, alternation,
 * mapping).
 */

#ifndef PARSER_COMB_H
#define PARSER_COMB_H

#include <parser_comb/Recursive.h>
#include <parser_comb/combinators/Alt.h>
#include <parser_comb/combinators/FMap.h>
#include <parser_comb/combinators/MapToValue.h>
#include <parser_comb/combinators/NotAhead.h>
#include <parser_comb/combinators/OptionalOr.h>
#include <parser_comb/combinators/Quantifiers.h>
#include <parser_comb/combinators/Sequence.h>
#include <parser_comb/combinators/Skip.h>
#include <parser_comb/primitives/Char.h>
#include <parser_comb/primitives/Core.h>
#include <parser_comb/primitives/String.h>
#include <parser_comb/utils/TupleUtils.h>
#include <parser_comb/utils/Whitespace.h>

#endif