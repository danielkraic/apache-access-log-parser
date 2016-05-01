#pragma once

#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>

template <typename Iterator>
bool parseLine(Iterator first, Iterator last)
{
    using boost::spirit::qi::lexeme;
    using boost::spirit::qi::char_;
    using boost::spirit::qi::uint_;
    
    std::cout << std::string(first, last) << std::endl;

    std::string host, logname, user, time, request, status, transferred_bytes, referer, useragent;

    auto lex_ip_addr = lexeme[+char_("0-9.")];
    auto lex_string  = lexeme[+char_("a-zA-Z0-9._-")];
    auto lex_number  = lexeme[+char_("0-9-")];
    auto lex_time    = lexeme[('[' >> *~char_("]") >> ']')];
    auto lex_qstring = lexeme[('"' >> *~char_("\"") >> '"')];

    bool r = boost::spirit::qi::phrase_parse(
            // source:
            first,
            last,

            // grammar:
            lex_ip_addr >> // host
            lex_string >>  // logname
            lex_string >>  // username
            lex_time >>    // timestamp
            lex_qstring >> // request
            lex_number >>  // status
            lex_number >>  // transfered bytes
            lex_qstring >> // referer
            lex_qstring    // useragent
            ,

            // delimiter:
            boost::spirit::ascii::space,

            // stored values:
            host,
            logname,
            user,
            time,
            request,
            status,
            transferred_bytes,
            referer,
            useragent
    );

    if (r) {
        std::cout << "parse OK\n";
    } else {
        std::cout << "parse FAIL\n";
    }

    std::cout << "host: " << host << "\n";
    std::cout << "name: " << logname << "\n";
    std::cout << "user: " << user << "\n";
    std::cout << "time: " << time << "\n";
    std::cout << "reqs: " << request << "\n";
    std::cout << "stat: " << status << "\n";
    std::cout << "byts: " << transferred_bytes << "\n";
    std::cout << "refr: " << referer << "\n";
    std::cout << "usag: " << useragent << "\n";
}
