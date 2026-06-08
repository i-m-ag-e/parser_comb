//
// Created by akul on 8/9/25.
//

#include <parser_comb/CType.h>
#include <parser_comb/ParserComb.h>

#include <iostream>

#include "parser_comb/combinators/Quantifiers.h"
#include "parser_comb/combinators/Sequence.h"
#include "parser_comb/primitives/Char.h"

int main(int, char* argv[]) {
    // const std::string pattern = argv[1];
    // const std::string str = argv[2];
    // const comb::Regex re(pattern);
    // std::cout << "Testing \"" << str << "\" against \"" << pattern << "\"\n";
    // std::cout << re.print_ast() << std::endl;
    // re.print_code();

    // auto match_res = re.match(str);
    // if (match_res) {
    //     print_match(*match_res);
    // } else {
    //     std::cout << "No MATCH\n";
    // }
    // std::cout << '\n';

    // auto search_res = re.search(str);
    // if (search_res) {
    //     print_match(*search_res);
    // } else {
    //     std::cout << "No SEARCH\n";
    // }
    // std::cout << '\n';
    auto p = comb::skip_while1(comb::satisfy(comb::isdigit),
                               [](char c) { return c <= '5'; });
    auto res = p.parse("1234567(yo)bye");
    if (res) {
        // std::cout << '(' << res->first << ", " << res->second << ')' << '\n';

        // std::cout << "((" << std::get<0>(res->first) << ", "
        //           << std::get<1>(res->first)
        //           //    << ", " << std::get<2>(res->first)
        //           << "), " << res->second << ')' << '\n';

        // std::cout << "([ ";
        // for (auto const& e : res->first) {
        //     std::cout << e << " ";
        // }
        // std::cout << "], " << res->second << ')' << '\n';

        std::cout << '(' << ", " << res->second << ')' << '\n';
    } else {
        std::cout << "fuck\n";
    }
}
